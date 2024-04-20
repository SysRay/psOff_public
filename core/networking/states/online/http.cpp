#include "../online.h"
#include "logging.h"

#include <Windows.h>
#include <boost/asio.hpp>
#include <boost/url.hpp>

LOG_DEFINE_MODULE(OnlineNetCore_http);

using namespace boost::asio;

namespace {
constexpr size_t ARRAY_LENGTH = 128;

template <typename T, size_t size>
class HttpContainer {
  T* m_data[size] = {nullptr};

  public:
  int lookupAvailableIdx(T* action) {
    int  i;
    bool found = false;
    for (i = 1; i < size; i++) {
      if (!m_data[i]) {
        m_data[i] = action;

        break;
      }
    }
    if (found == false) {
      delete action;
      return 0;
    }
    return i;
  }

  int testId(int id) {
    if (id < 1 || id >= size || m_data[id] == nullptr) return Err::HTTP_BAD_ID;

    return Ok;
  }

  void remove(int id) {
    delete m_data[id];
    m_data[id] = nullptr;
  }

  T* getId(int id) { return m_data[id]; }
};

static io_service        svc;
static ip::tcp::resolver resolver(svc);

struct HttpClient {
  int      libnetMemId;
  int      libsslCtxId;
  size_t   poolSize;
  uint32_t connectTimeout;

  HttpClient(int libnetMemId, int libsslCtxId, size_t poolSize): libnetMemId(libnetMemId), libsslCtxId(libsslCtxId), poolSize(poolSize) {}
};

struct HttpTemplate {
  HttpClient* parentClient;
  const char* userAgent;
  int         httpVer;
  int         isAutoProxyConf;

  HttpTemplate(HttpClient* parentClient, const char* userAgent, int httpVer, int isAutoProxyConf)
      : parentClient(parentClient), userAgent(userAgent), httpVer(httpVer), isAutoProxyConf(isAutoProxyConf) {}
};

struct HttpConnection {
  HttpTemplate* parentTemplate;
  const char*   serverName;
  const char*   scheme;
  uint16_t      port;
  int           isEnableKeepalive;

  bool connected = false;
  bool shouldFreeStrings;

  ip::tcp::resolver::query*   query = nullptr;
  ip::tcp::resolver::iterator endpoint_iterator;
  ip::tcp::socket*            socket = nullptr;

  HttpConnection(HttpTemplate* parentTemplate, const char* serverName, const char* scheme, uint16_t port, int isEnableKeepalive, bool shouldFreeStrings)
      : parentTemplate(parentTemplate),
        serverName(serverName),
        scheme(scheme),
        port(port),
        isEnableKeepalive(isEnableKeepalive),
        shouldFreeStrings(shouldFreeStrings) {}
};

struct HttpResponse {
  int         statusCode;
  uint32_t    contentLength;
  const char* body = nullptr;
};

struct HttpRequest {
  HttpConnection* parentConnection;
  int             method;
  const char*     path;
  uint64_t        contentLength;
  const void*     postData = nullptr;
  size_t          size;
  HttpResponse*   lastResponse = nullptr;

  bool shouldFreePath;

  HttpRequest(HttpConnection* parentConnection, int method, const char* path, uint64_t contentLength, bool shouldFreePath)
      : parentConnection(parentConnection), method(method), path(path), contentLength(contentLength), shouldFreePath(shouldFreePath) {}
};

struct HttpRequestParams {
  HttpConnection* connection;
  HttpRequest*    request;
  uint32_t        connectTimeout;
  const char*     userAgent;
  int             httpVer;
  int             isAutoProxyConf;
  const char*     serverName;
  const char*     scheme;
  uint16_t        port;
  int             isEnableKeepalive;
  int             method;
  const char*     path;
  uint64_t        contentLength;
  const void*     postData; // will be assigned to nullptr
  size_t          size;

  HttpRequestParams(HttpRequest* from) {
    connection        = from->parentConnection;
    request           = from;
    connectTimeout    = from->parentConnection->parentTemplate->parentClient->connectTimeout;
    userAgent         = from->parentConnection->parentTemplate->userAgent;
    httpVer           = from->parentConnection->parentTemplate->httpVer;
    isAutoProxyConf   = from->parentConnection->parentTemplate->isAutoProxyConf;
    serverName        = from->parentConnection->serverName;
    scheme            = from->parentConnection->scheme;
    port              = from->parentConnection->port;
    isEnableKeepalive = from->parentConnection->isEnableKeepalive;
    method            = from->method;
    path              = from->path;
    contentLength     = from->contentLength;
    postData          = from->postData;
    size              = from->size;
  }
};

static HttpContainer<HttpClient, ARRAY_LENGTH>     g_client;
static HttpContainer<HttpTemplate, ARRAY_LENGTH>   g_template;
static HttpContainer<HttpConnection, ARRAY_LENGTH> g_connection;
static HttpContainer<HttpRequest, ARRAY_LENGTH>    g_request;

#define GET_LAST_RESPONSE                                                                                                                                      \
  HttpRequest*  request  = g_request.getId(reqId);                                                                                                             \
  HttpResponse* response = request->lastResponse;                                                                                                              \
  if (response == nullptr) return Err::HTTP_SEND_REQUIRED;

static void deleteResponse(HttpResponse* response) {
  if (response->body != nullptr) {
    delete response->body;
  }
  delete response;
}

static int httpMethodStringToInt(const char* method) {
  if (::_stricmp(method, "get") == 0) {
    return SCE_HTTP_GET;
  } else if (::_stricmp(method, "post") == 0) {
    return SCE_HTTP_POST;
  }
  LOG_USE_MODULE(OnlineNetCore_http);
  LOG_TRACE(L"unsupported http method: %s", method);

  return -1;
}

static int32_t performHttpRequest(HttpRequestParams* request, HttpResponse* response) {
  LOG_USE_MODULE(OnlineNetCore_http);

  HttpConnection* connection = request->connection;
  try {
    if (!connection->connected) {
      connection->query             = new ip::tcp::resolver::query(request->serverName, std::to_string(request->port));
      connection->endpoint_iterator = resolver.resolve(*connection->query);
      connection->socket            = new ip::tcp::socket(svc);
      boost::asio::connect(*connection->socket, connection->endpoint_iterator);
      connection->connected = true;

      if (std::strcmp(connection->scheme, "https") == 0) {
        LOG_TRACE(L"detected an attempt to connect via https, it's not supported yet");

        return Err::HTTP_SSL_ERROR;
      }
      if (std::strcmp(connection->scheme, "http") != 0) {
        LOG_TRACE(L"unknown scheme: %s://", connection->scheme);

        return Err::HTTP_BAD_SCHEME;
      }
    }
    if (request->request->lastResponse != nullptr) {
      deleteResponse(request->request->lastResponse);
    }
    boost::asio::streambuf buffer;
    std::ostream           bufferStream(&buffer);

    if (request->method == SCE_HTTP_GET) {
      bufferStream << "GET ";
    } else if (request->method == SCE_HTTP_POST) {
      bufferStream << "POST ";
    } else {
      LOG_TRACE(L"unsupported request method code (%d), pretending we've failed to connect", request->method);

      return Err::HTTP_FAILURE;
    }
    bufferStream << request->path;
    if (request->httpVer == 1)
      bufferStream << "HTTP/1.0";
    else
      bufferStream << "HTTP/1.1";
    bufferStream << "\r\n";
    if (request->httpVer != 1 /* means HTTP 1.1 */) {
      bufferStream << "Host: " << request->serverName << "\r\n";
      bufferStream << "User-Agent: " << request->userAgent << "\r\n";
      bufferStream << "Accept: */*\r\n";
      bufferStream << "Connection: " << (request->isEnableKeepalive ? "keep-alive" : "close") << "\r\n";
    }
    if (request->postData != nullptr) {
      bufferStream << "Content-Length: " << request->contentLength << "\r\n\r\n";
      for (size_t i = 0; i < request->size; i++) {
        bufferStream << ((char*)request->postData)[i];
      }
    }
    boost::asio::write(*connection->socket, buffer);

    boost::asio::streambuf responseBuffer;
    boost::asio::read_until(*connection->socket, responseBuffer, "\r\n");
    std::istream responseBufferStream(&responseBuffer);

    std::string httpVersion;
    int         statusCode;
    std::string statusDescription;
    responseBufferStream >> httpVersion;
    responseBufferStream >> statusCode;
    std::getline(responseBufferStream, statusDescription);

    boost::asio::read_until(*connection->socket, responseBuffer, "\r\n\r\n");

    bool        foundContentLengthHeader = false;
    uint32_t    contentLength;
    std::string header;
    while (std::getline(responseBufferStream, header) && header != "\r") {
      if (header.rfind("Content-Length: ", 0) == 0) { // todo: remove case-sensitive check
        std::string contentLengthUnparsed = header.substr(16);
        contentLength                     = std::stoi(contentLengthUnparsed);
        foundContentLengthHeader          = true;

        break;
      }
    }
    if (!foundContentLengthHeader) {
      LOG_TRACE(L"failed to find \"Content-Length\" header in the response");

      return Err::HTTP_FAILURE;
    }
    bool tooLow;
    if ((tooLow = contentLength < 1) || contentLength > 1610612736) {
      LOG_TRACE(L"bad content length: %s", (tooLow ? "less than 1 byte" : "more than 1.5 GiB"));

      return Err::HTTP_FAILURE;
    }
    size_t currentIdx = 0;
    char*  body;
    try {
      body = new char[contentLength];
    } catch (std::bad_alloc&) {
      LOG_TRACE(L"failed to allocate %d bytes", contentLength);

      return Err::HTTP_FAILURE;
    }
    response->statusCode    = statusCode;
    response->contentLength = contentLength;
    response->body          = body;

    boost::system::error_code error;
    while (boost::asio::read(*connection->socket, responseBuffer, boost::asio::transfer_at_least(1), error)) {
      std::streamsize available = responseBuffer.in_avail();
      char            c;
      for (std::streamsize i = 0; i < available; i++) {
        responseBufferStream >> c;
        body[currentIdx++] = c;
      }
    }
    if (error != boost::asio::error::eof) throw boost::system::system_error(error);

    return Ok;
  } catch (const boost::exception& e) {
    LOG_TRACE(L"caught a boost exception while performing an http request");

    return Err::HTTP_FAILURE;
  } catch (const std::exception& e) {
    LOG_TRACE(L"caught an std::exception while performing an http request");

    return Err::HTTP_FAILURE;
  }
}
} // namespace

int OnlineNet::httpInit(int libnetMemId, int libsslCtxId, size_t poolSize) {
  auto const handle = g_client.lookupAvailableIdx(new HttpClient(libnetMemId, libsslCtxId, poolSize));
  if (handle == 0) return Err::HTTP_OUT_OF_MEMORY;

  LOG_USE_MODULE(OnlineNetCore_http);
  LOG_TRACE(L"new http client (id: %d, memId: %d, sslCtxId: %d, poolSize: %d)", handle, libnetMemId, libsslCtxId, poolSize);

  return handle;
}

int OnlineNet::httpTerm(int libhttpCtxId) {
  if (auto ret = g_client.testId(libhttpCtxId)) return ret;

  g_client.remove(libhttpCtxId);

  LOG_USE_MODULE(OnlineNetCore_http);
  LOG_TRACE(L"http client removed (id: %d)", libhttpCtxId);

  return Ok;
}

int OnlineNet::httpGetMemoryPoolStats(int libhttpCtxId, SceHttpMemoryPoolStats* currentStat) {
  if (auto ret = g_client.testId(libhttpCtxId)) return ret;

  currentStat->currentInuseSize = 16384; // todo (?)
  currentStat->maxInuseSize     = 131072;
  currentStat->poolSize         = g_client.getId(libhttpCtxId)->poolSize;

  LOG_USE_MODULE(OnlineNetCore_http);
  LOG_TRACE(L"memory pool stats were requested (client id: %d)", libhttpCtxId);

  return Ok;
}

int OnlineNet::httpCreateTemplate(int libhttpCtxId, const char* userAgent, int httpVer, int isAutoProxyConf) {
  if (auto ret = g_client.testId(libhttpCtxId)) return ret;

  HttpClient* client = g_client.getId(libhttpCtxId);

  auto const handle = g_template.lookupAvailableIdx(new HttpTemplate(client, userAgent, httpVer, isAutoProxyConf));
  if (handle == 0) return Err::HTTP_OUT_OF_MEMORY;

  LOG_USE_MODULE(OnlineNetCore_http);
  LOG_TRACE(L"new http template (id: %d, client id: %d)", handle, libhttpCtxId);

  return handle;
}

int OnlineNet::httpDeleteTemplate(int tmplId) {
  if (auto ret = g_template.testId(tmplId)) return ret;

  g_template.remove(tmplId);

  LOG_USE_MODULE(OnlineNetCore_http);
  LOG_TRACE(L"http template removed (id: %d)", tmplId);

  return Ok;
}

static int sceHttpCreateConnection(int tmplId, const char* serverName, const char* scheme, uint16_t port, int isEnableKeepalive, bool shouldFreeStrings) {
  if (auto ret = g_template.testId(tmplId)) return ret;

  HttpTemplate* httpTemplate = g_template.getId(tmplId);
  auto const    handle = g_connection.lookupAvailableIdx(new HttpConnection(httpTemplate, serverName, scheme, port, isEnableKeepalive, shouldFreeStrings));

  LOG_USE_MODULE(OnlineNetCore_http);
  LOG_TRACE(L"new http connection (id: %d, template id: %d)", handle, tmplId);

  return handle;
}

int OnlineNet::httpCreateConnection(int tmplId, const char* serverName, const char* scheme, uint16_t port, int isEnableKeepalive) {
  return sceHttpCreateConnection(tmplId, serverName, scheme, port, isEnableKeepalive, false);
}

int OnlineNet::httpCreateConnectionWithURL(int tmplId, const char* url, int isEnableKeepalive) {
  boost::urls::url link(url);
  uint16_t         port;

  bool isNotHttp;
  if ((isNotHttp = std::strcmp(link.scheme().data(), "http") != 0) && std::strcmp(link.scheme().data(), "https") != 0) {

    return Err::HTTP_BAD_SCHEME;
  }
  if (link.has_port()) {
    port = link.port_number();
  } else {
    if (isNotHttp)
      port = 443;
    else
      port = 80;
  }

  int result = sceHttpCreateConnection(tmplId, link.host().data(), link.scheme().data(), port, isEnableKeepalive, true);

  return result;
}

int OnlineNet::httpDeleteConnection(int connId) {
  if (auto ret = g_connection.testId(connId)) return ret;

  HttpConnection* connection = g_connection.getId(connId);
  if (connection->query != nullptr) {
    delete connection->query;
  }
  if (connection->socket != nullptr) {
    connection->socket->close();

    delete connection->socket;
  }
  if (connection->shouldFreeStrings) {
    delete connection->scheme;
    delete connection->serverName;
  }
  g_connection.remove(connId);

  LOG_USE_MODULE(OnlineNetCore_http);
  LOG_TRACE(L"http connection removed (id: %d)", connId);

  return Ok;
}

static int sceHttpCreateRequest(int connId, int method, const char* path, uint64_t contentLength, bool shouldFreePath) {
  if (auto ret = g_connection.testId(connId)) return ret;

  HttpConnection* httpConnection = g_connection.getId(connId);

  auto handle = g_request.lookupAvailableIdx(new HttpRequest(httpConnection, method, path, contentLength, shouldFreePath));

  LOG_USE_MODULE(OnlineNetCore_http);
  LOG_TRACE(L"new http request (id: %d, connection id: %d)", handle, connId);

  return Ok;
}

int OnlineNet::httpCreateRequest(int connId, int method, const char* path, uint64_t contentLength) {
  return sceHttpCreateRequest(connId, method, path, contentLength, false);
}

int OnlineNet::httpCreateRequest2(int connId, const char* method, const char* path, uint64_t contentLength) {
  return httpCreateRequest(connId, httpMethodStringToInt(method), path, contentLength);
}

int OnlineNet::httpCreateRequestWithURL(int connId, int method, const char* url, uint64_t contentLength) {
  boost::urls::url link(url);
  if (link.has_password()) {
    LOG_USE_MODULE(OnlineNetCore_http);
    LOG_TRACE(L"urls containing credentials are not supported");

    return Err::HTTP_BAD_PARAM;
  }

  int result = sceHttpCreateRequest(connId, method, link.path().data(), contentLength, true);

  return result;
}

int OnlineNet::httpCreateRequestWithURL2(int connId, const char* method, const char* url, uint64_t contentLength) {
  return httpCreateRequestWithURL(connId, httpMethodStringToInt(method), url, contentLength);
}

int OnlineNet::httpDeleteRequest(int reqId) {
  if (auto ret = g_request.testId(reqId)) return ret;

  HttpRequest* request = g_request.getId(reqId);
  if (request->lastResponse != nullptr) {
    deleteResponse(request->lastResponse);
  }
  if (request->shouldFreePath) {
    delete request->path;
  }
  g_request.remove(reqId);

  LOG_USE_MODULE(OnlineNetCore_http);
  LOG_TRACE(L"http request removed (id: %d)", reqId);

  return Ok;
}

int OnlineNet::httpSetRequestContentLength(int id, uint64_t contentLength) {
  int reqId = id; // (?)
  if (auto ret = g_request.testId(reqId)) return ret;

  g_request.getId(reqId)->contentLength = contentLength;

  return Ok;
}

int OnlineNet::httpSetChunkedTransferEnabled(int id, int isEnable) {
  return Ok;
}

int OnlineNet::httpSetInflateGZIPEnabled(int id, int isEnable) {
  return Ok;
}

int OnlineNet::httpSendRequest(int reqId, const void* postData, size_t size) {
  if (auto ret = g_request.testId(reqId)) return ret;

  HttpRequest* request          = g_request.getId(reqId);
  request->postData             = postData;
  request->size                 = size;
  HttpRequestParams* fullParams = new HttpRequestParams(request);
  HttpResponse*      response   = new HttpResponse();
  int32_t            result     = performHttpRequest(fullParams, response);
  delete fullParams;
  if (result == Ok) {
    request->lastResponse = response;
  } else {
    deleteResponse(response);
  }

  return result;
}

int OnlineNet::httpAbortRequest(int reqId) {
  return Ok;
}

int OnlineNet::httpGetResponseContentLength(int reqId, int* result, uint64_t* contentLength) {
  if (auto ret = g_request.testId(reqId)) return ret;

  GET_LAST_RESPONSE;
  *result        = 0; // Content-Length is guaranteed to exist, otherwise performHttpRequest would have failed
  *contentLength = response->contentLength;

  return Ok;
}

int OnlineNet::httpGetStatusCode(int reqId, int* statusCode) {
  if (auto ret = g_request.testId(reqId)) return ret;

  GET_LAST_RESPONSE;
  *statusCode = response->statusCode;

  return Ok;
}

int OnlineNet::httpGetAllResponseHeaders(int reqId, char** header, size_t* headerSize) {
  *headerSize = 0;

  return Ok;
}

int OnlineNet::httpReadData(int reqId, void* data, size_t size) {
  if (auto ret = g_request.testId(reqId)) return ret;

  GET_LAST_RESPONSE;
  size_t finalSize = min(size, response->contentLength);
  memcpy(data, response->body, finalSize);

  return Ok;
}

int OnlineNet::httpAddRequestHeader(int id, const char* name, const char* value, uint32_t mode) {
  return Ok;
}

int OnlineNet::httpRemoveRequestHeader(int id, const char* name) {
  return Ok;
}

int OnlineNet::httpParseResponseHeader(const char* header, size_t headerLen, const char* fieldStr, const char** fieldValue, size_t* valueLen) {
  return Ok;
}

int OnlineNet::httpParseStatusLine(const char* statusLine, size_t lineLen, int* httpMajorVer, int* httpMinorVer, int* responseCode, const char** reasonPhrase,
                                   size_t* phraseLen) {
  return Ok;
}

int OnlineNet::httpSetResponseHeaderMaxSize(int id, size_t headerSize) {
  return Ok;
}

int OnlineNet::httpSetAuthInfoCallback(int id, SceHttpAuthInfoCallback cbfunc, void* userArg) {
  return Ok;
}

int OnlineNet::httpSetAuthEnabled(int id, int isEnable) {
  return Ok;
}

int OnlineNet::httpGetAuthEnabled(int id, int* isEnable) {
  return Ok;
}

int OnlineNet::httpAuthCacheFlush(int libhttpCtxId) {
  return Ok;
}

int OnlineNet::httpSetRedirectCallback(int id, SceHttpRedirectCallback cbfunc, void* userArg) {
  return Ok;
}

int OnlineNet::httpSetAutoRedirect(int id, int isEnable) {
  return Ok;
}

int OnlineNet::httpGetAutoRedirect(int id, int* isEnable) {
  return Ok;
}

int OnlineNet::httpRedirectCacheFlush(int libhttpCtxId) {
  return Ok;
}

int OnlineNet::httpSetResolveTimeOut(int id, uint32_t usec) {
  return Ok;
}

int OnlineNet::httpSetResolveRetry(int id, int retry) {
  return Ok;
}

int OnlineNet::httpSetConnectTimeOut(int id, uint32_t usec) {
  return Ok;
}

int OnlineNet::httpSetSendTimeOut(int id, uint32_t usec) {
  return Err::SEND_TIMEOUT;
}

int OnlineNet::httpSetRecvTimeOut(int id, uint32_t usec) {
  return Ok;
}

int OnlineNet::httpSetRequestStatusCallback(int id, SceHttpRequestStatusCallback cbfunc, void* userArg) {
  return Ok;
}

int OnlineNet::httpGetLastErrno(int reqId, int* errNum) {
  return Ok;
}

int OnlineNet::httpSetNonblock(int id, int isEnable) {
  return Ok;
}

int OnlineNet::httpGetNonblock(int id, int* isEnable) {
  return Ok;
}

int OnlineNet::httpTrySetNonblock(int id, int isEnable) {
  return Ok;
}

int OnlineNet::httpTryGetNonblock(int id, int* isEnable) {
  return Ok;
}

int OnlineNet::httpCreateEpoll(int libhttpCtxId, SceHttpEpollHandle* eh) {
  return Ok;
}

int OnlineNet::httpSetEpoll(int id, SceHttpEpollHandle eh, void* userArg) {
  return Ok;
}

int OnlineNet::httpUnsetEpoll(int id) {
  return Ok;
}

int OnlineNet::httpGetEpoll(int id, SceHttpEpollHandle* eh, void** userArg) {
  return Ok;
}

int OnlineNet::httpDestroyEpoll(int libhttpCtxId, SceHttpEpollHandle eh) {
  return Ok;
}

int OnlineNet::httpWaitRequest(SceHttpEpollHandle eh, SceHttpNBEvent* nbev, int maxevents, int timeout) {
  return Ok;
}

int OnlineNet::httpAbortWaitRequest(SceHttpEpollHandle eh) {
  return Ok;
}

// HTTPS
int OnlineNet::httpsLoadCert(int libhttpCtxId, int caCertNum, const SceSslData** caList, const SceSslData* cert, const SceSslData* privKey) {
  return Ok;
}

int OnlineNet::httpsUnloadCert(int libhttpCtxId) {
  return Ok;
}

int OnlineNet::httpsEnableOption(int id, uint32_t sslFlags) {
  return Ok;
}

int OnlineNet::httpsDisableOption(int id, uint32_t sslFlags) {
  return Ok;
}

int OnlineNet::httpsGetSslError(int id, int* errNum, uint32_t* detail) {
  return Ok;
}

int OnlineNet::httpsSetSslCallback(int id, SceHttpsCallback cbfunc, void* userArg) {
  return Ok;
}

int OnlineNet::httpsSetSslVersion(int id, SceSslVersion version) {
  return Ok;
}
