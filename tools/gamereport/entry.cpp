#define __APICALL_EXTERN
#include "gamereport.h"
#undef __APICALL_EXTERN

#include "config_emu.h"
#include "git_ver.h"
#include "logging.h"
#include "third_party/nlohmann/json.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/url.hpp>
#include <format>
#include <shellapi.h>
#include <unordered_map>

LOG_DEFINE_MODULE(GameReport)

namespace {
struct GitHubIssue {
  int         id;
  std::string status;
  std::string descr;
};

static std::unordered_map<std::string, const char*> descrs = {
    {"audio", "* audio issues (sound is missing, choppy or playing incorrectly);\n"},
    {"graphics", "* graphics issues (visual artifacts, low framerate, black screen);\n"},
    {"input", "* input issues (gamepad/keyboard won't work, inpust lag is present);\n"},
    {"savedata", "* savedata issues (the game won't save your progress);\n"},
    {"video", "* video issues (ingame videos are decoded incorrectly or not played at all);\n"},
    {"missing-symbol", "* missing symbol (the game needs functions that we have not yet implemented);\n"},
    {"nvidia-specific", "* nvidia specific (the game has known issues on NVIDIA cards);\n"},
};

static std::unordered_map<std::string, const char*> statuses = {
    {"status-nothing", "nothing (the game don't initialize properly, not loading atall and/or crashing the emulator)"},
    {"status-intro", "intro (the game display image but don't make it past the menus)"},
    {"status-ingame", "igname (the game can't be either finished, have serious glitches or insufficient performance)"},
    {"status-playable", "playable (the game can be finished with playable performance and no game breaking glitches)"},
};

static const char* retrieve_label_status(std::string& label) {
  auto it = statuses.find(label);
  if (it != statuses.end()) return it->second;
  return nullptr;
};

static const char* retrieve_label_description(std::string& label) {
  auto it = descrs.find(label);
  if (it != descrs.end()) return it->second;
  return nullptr;
}

static int find_issue(const char* title_id, GitHubIssue* issue) {
  LOG_USE_MODULE(GameReport);
  using namespace boost::asio;
  using namespace boost::beast;
  using json = nlohmann::json;

  boost::urls::url link("https://api.github.com/search/issues");

  auto params = link.params();
  params.append({"q", std::format("repo:{} is:issue {} in:title", GAMEREPORT_REPO_NAME, title_id)});
  link.normalize();

  io_service svc;

  ssl::context                 ctx(ssl::context::sslv23_client);
  ssl::stream<ip::tcp::socket> sock = {svc, ctx};
  try {
    ip::tcp::resolver resolver(svc);
    auto              resolved = resolver.resolve(link.host(), link.scheme());
    boost::asio::connect(sock.lowest_layer(), resolved);
    sock.handshake(ssl::stream_base::handshake_type::client);
  } catch (boost::system::system_error& ex) {
    LOG_ERR(L"%S", ex.what());
    return -1;
  }

  http::request<http::empty_body> req {http::verb::get, link, 11};
  req.set(http::field::host, link.host());
  req.set(http::field::user_agent, "psOff-http/1.0");
  req.set("X-GitHub-Api-Version", "2022-11-28");
  // req.set(http::field::authorization, "Bearer @GIT_TOKEN@");
  http::write(sock, req);

  http::response<http::string_body> res;
  flat_buffer                       buf;
  http::read(sock, buf, res);

  if (res[http::field::content_type].contains("application/json") && res.result_int() == 200) {
    try {
      json jresp = json::parse(res.body());
      if (jresp["total_count"] < 1) return -1;
      auto& jissue  = jresp["items"][0];
      auto& jlabels = jissue["labels"];

      std::string tempstr;
      for (auto it: jlabels) {
        it["name"].get_to(tempstr);

        if (auto gstat = retrieve_label_status(tempstr)) issue->status = gstat;
        if (auto descr = retrieve_label_description(tempstr)) issue->descr += descr;
      }
      if (auto len = issue->descr.length()) issue->descr.erase(len - 2);
      jissue["number"].get_to(issue->id);
    } catch (json::exception& ex) {
      LOG_ERR(L"find_issue => jsonparse error: %S", ex.what());
      return -1;
    }
  } else {
    LOG_ERR(L"GitHub request error: %S", res.body().c_str());
    return -1;
  }

  return 0;
}
} // namespace

class GameReport: public IGameReport {
  public:
  GameReport();
  virtual ~GameReport() = default;

  virtual void ShowReportWindow(const Info& info) final;
};

GameReport::GameReport() {}

void GameReport::ShowReportWindow(const Info& info) {
  LOG_USE_MODULE(GameReport);
  if (m_bEnabled == false) {
    LOG_WARN(L"[gamereport] Reporting is disabled!");
    return;
  }

  const char* message;

  switch (info.type) {
    case USER:
      message = "Do you want to file report about the game you running?\n"
                "If you press \"Yes\", your default browser will be opened.\n\n"
                "You must have a GitHub account to create an issue!";
      break;

    case EXCEPTION:
    case MISSING_SYMBOL:
      message = "We believe your emulator just crashed! Do you want to file a game report?\n"
                "If you press \"Yes\", your default browser will be opened.\n\n"
                "You must have a GitHub account to create an issue!";
      break;

    default: break;
  }

  SDL_MessageBoxButtonData btns[2] {
      {.flags = 0, .buttonid = 1, .text = "Yes"},
      {.flags = 0, .buttonid = 2, .text = "No"},
  };

  SDL_MessageBoxData mbd {
      .flags      = SDL_MESSAGEBOX_INFORMATION,
      .window     = info.wnd,
      .title      = "File a game report",
      .message    = message,
      .numbuttons = 2,
      .buttons    = btns,
  };

  int btn = -1;

  if (SDL_ShowMessageBox(&mbd, &btn) != 0) {
    LOG_ERR(L"[gamereport] Failed to generate SDL MessageBox: %S", SDL_GetError());
    return;
  }

  if (btn != 1) return;

  GitHubIssue issue {
      .id = -1,
  };

  std::string issue_msg;

  if (find_issue(info.title_id, &issue) == 0) {
    issue_msg   = std::format("Looks like we already know about issue(-s) in this game!\n\n"
                                "Game status: {}\nPossible issues:\n{}\n\n"
                                "Do you want to open issue's page?",
                              issue.status, issue.descr);
    mbd.message = issue_msg.c_str();

    if (SDL_ShowMessageBox(&mbd, &btn) != 0) {
      LOG_ERR(L"[gamereport] Failed to generate SDL MessageBox: %S", SDL_GetError());
      return;
    }

    if (btn != 1) return;
  }

  try {
    std::string str;
    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::LOGGING);
    (*jData)["sink"].get_to(str);
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });

    if (str != "filebin") {
      mbd.title      = "Logging warning";
      mbd.message    = "You might not be able to provide a log for issue you about to open.\n"
                       "Your logger configuration might be incorrect, since \"sink\" is not equals to \"FileBin\".\n"
                       "Creating issues without log file is not recommended.";
      mbd.flags      = SDL_MESSAGEBOX_WARNING;
      mbd.numbuttons = 2;
      btns[0].text   = "OK";
      btns[1].text   = "Cancel";

      if (SDL_ShowMessageBox(&mbd, &btn) != 0) {
        LOG_ERR(L"[gamereport] Failed to generate SDL MessageBox: %S", SDL_GetError());
        return;
      }

      if (btn != 1) return;
    }
  } catch (const json::exception& ex) {
    LOG_ERR(L"Something off with the logging configuration: %S", ex.what());
  }

  boost::urls::url link("https://github.com/");

  if (issue.id == -1) {
    link.set_path(std::format("/{}/issues/new", GAMEREPORT_REPO_NAME));
    auto params = link.params();
    params.append({"template", "game_report.yml"});
    switch (info.type) {
      case EXCEPTION:
        params.append({"what-happened", info.add.ex->what()}); // todo: add stack trace?
        break;
      case MISSING_SYMBOL:
        params.append({"what-happened", info.add.message}); // todo: some formatting?
        break;

      default: break;
    }
    params.append({"title", std::format("[{}]: {}", info.title_id, info.title)});
    params.append({"game-version", info.app_ver});
    params.append({"lib-version", git::CommitSHA1().data()});
  } else {
    link.set_path(std::format("/{}/issues/{}", GAMEREPORT_REPO_NAME, issue.id));
  }

  ShellExecuteA(nullptr, nullptr, link.normalize().c_str(), nullptr, nullptr, SW_SHOW);
}

IGameReport& accessGameReport() {
  static GameReport obj;
  return obj;
}
