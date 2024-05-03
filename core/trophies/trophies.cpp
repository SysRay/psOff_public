#define __APICALL_EXTERN
#include "trophies.h"
#undef __APICALL_EXTERN

#include "core/fileManager/fileManager.h"
#include "modules_include/system_param.h"
#include "tools/config_emu/config_emu.h"

#include <boost/scoped_ptr.hpp>
#include <fstream>
#include <openssl/evp.h>
#include <xml3all.h>

#define TR_AES_BLOCK_SIZE 16
#undef min // We don't need it there

class Trophies: public ITrophies {
  struct usr_context {
    struct trophy {
      uint32_t id;
      uint32_t re; // reserved
      uint64_t ts;
    };

    bool     created;
    uint32_t label;

    std::vector<struct trophy> trophies;
  } m_ctx[4];

  struct trp_header {
    uint32_t magic; // should be 0xDCA24D00
    uint32_t version;
    uint64_t tfile_size; // size of trp file
    uint32_t entry_num;  // num of entries
    uint32_t entry_size; // size of entry
    uint32_t dev_flag;   // 1: dev
    uint8_t  digest[20]; // sha hash
    uint32_t key_index;
    char     padding[44];
  };

  struct trp_entry {
    char     name[32];
    uint64_t pos;
    uint64_t len;
    uint32_t flag;
    char     padding[12];
  };

  static bool caseequal(char a, char b) { return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b)); }

  bool        m_bKeySet                  = false;
  bool        m_bIgnoreMissingLocale     = false;
  uint8_t     m_trkey[TR_AES_BLOCK_SIZE] = {};
  std::string m_localizedTrophyFile      = {};
  std::mutex  m_mutexParse;

  static ErrCodes XML_parse(const char* mem, trp_context* ctx) {
    XML3::XML xml(mem, strlen(mem));

    { // xml parser
      auto& rootel = xml.GetRootElement();
      if (rootel.GetElementName() == "trophyconf") {
        // Prepare trophyset info structure to save some data
        if (!ctx->itrop.cancelled) {
          ctx->itrop.data.title_name.clear();
          ctx->itrop.data.title_detail.clear();
          ctx->itrop.data.trophyset_version.clear();
        }

        for (auto& chel: rootel.GetChildren()) {
          auto& cheln = chel->GetElementName();
          if (!ctx->entry.cancelled && cheln == "trophy") {
            ctx->entry.data.id    = -1;
            ctx->entry.data.group = -1;
            ctx->entry.data.type  = 0xFF;
            ctx->entry.data.name.clear();
            ctx->entry.data.detail.clear();

            for (auto& chvar: chel->GetVariables()) {
              auto& vname = chvar->GetName();
              if (vname == "id") {
                ctx->entry.data.id = chvar->GetValueInt(-1);
              } else if (vname == "hidden") {
                ctx->entry.data.hidden = chvar->GetValue() == "yes";
              } else if (vname == "ttype") {
                ctx->entry.data.type = chvar->GetValue().at(0);
              } else if (vname == "gid") {
                ctx->entry.data.group = chvar->GetValueInt(-1);
              }
            }

            // There is no `name` and `detail` fields if we read TROPCONF.ESFM
            if (ctx->lightweight) continue;

            for (auto& chch: chel->GetChildren()) {
              auto& cname = chch->GetElementName();
              if (cname == "name") {
                ctx->entry.data.name.assign(chch->GetContent());
              } else if (cname == "detail") {
                ctx->entry.data.detail.assign(chch->GetContent());
              }
            }

            ctx->entry.cancelled = ctx->entry.func(&ctx->entry.data);
          } else if (!ctx->group.cancelled && cheln == "group") {
            for (auto& chvar: chel->GetVariables()) {
              if (ctx->group.cancelled) break;
              auto& vname = chvar->GetName();
              if (vname == "id") {
                ctx->group.data.id = chvar->GetValueInt(-1);
              }

              // There is no `name` and `detail` fields if we read TROPCONF.ESFM
              if (ctx->lightweight) continue;

              for (auto& chch: chel->GetChildren()) {
                auto& cname = chch->GetElementName();
                if (cname == "name") {
                  ctx->group.data.name.assign(chch->GetContent());
                } else if (cname == "detail") {
                  ctx->group.data.detail.assign(chch->GetContent());
                }
              }

              ctx->group.cancelled = ctx->group.func(&ctx->group.data);
            }
          }
          if (!ctx->itrop.cancelled) {
            if (cheln == "trophyset-version") {
              ctx->itrop.data.trophyset_version.assign(chel->GetContent());
            } else if (cheln == "title-name") {
              ctx->itrop.data.title_name.assign(chel->GetContent());
            } else if (cheln == "title-detail") {
              ctx->itrop.data.title_detail.assign(chel->GetContent());
            } else if (cheln == "trophy") {
              ++ctx->itrop.data.trophy_count;
            }
          }
        }

        // Pass the final trophyset info to itrop callback
        if (!ctx->itrop.cancelled) ctx->itrop.func(&ctx->itrop.data);

        // We already parsed trophy info, we don't need more of it
        ctx->entry.cancelled = true;
        ctx->group.cancelled = true;
        ctx->itrop.cancelled = true;
      } // element: trophyconf
    }   // xml parser

    return ErrCodes::CONTINUE; // todo: check xml errors
  }

  ErrCodes TRP_readentry(const trp_entry& ent, trp_entry& dent, std::ifstream& trfile, trp_context* ctx) {
    if (!ctx->pngim.cancelled) {
      static std::string_view ext(".png");
      std::string_view        name(ent.name);
      if (std::equal(ext.rbegin(), ext.rend(), name.rbegin(), caseequal)) { // Test trp file extension
        if (((ent.flag >> 24) & 0x03) == 0) {
          ctx->pngim.data.pngsize = ent.len;
          ctx->pngim.data.pngname.assign(ent.name);
          ctx->pngim.data.pngdata = new char[ent.len]; // Developer should free this memory manually
          trfile.seekg(ent.pos);
          if (trfile.read((char*)ctx->pngim.data.pngdata, ent.len)) {
            ctx->pngim.cancelled = ctx->pngim.func(&ctx->pngim.data);
            return ErrCodes::CONTINUE;
          }

          return ErrCodes::IO_FAIL;
        } else {
          // Is this even possible?
          return ErrCodes::NOT_IMPLEMENTED;
        }
      }
    }

    if (!ctx->group.cancelled || !ctx->entry.cancelled || !ctx->itrop.cancelled) {
      static std::string_view ext(".esfm");
      std::string_view        name(ent.name);
      if (!std::equal(ext.rbegin(), ext.rend(), name.rbegin(), caseequal)) return ErrCodes::CONTINUE;
      if ((ent.len % 16) != 0) return ErrCodes::INVALID_AES;
      if (ctx->lightweight) {
        static std::string_view lwfile("tropconf.esfm");
        if (!std::equal(lwfile.begin(), lwfile.end(), name.begin(), name.end(), caseequal)) return ErrCodes::CONTINUE;
      } else if (!m_bIgnoreMissingLocale) {
        static std::string_view dfile("trop.esfm");
        if (m_localizedTrophyFile.length() == 0) {
          // No localized trophy needed, using the English one
          if (!std::equal(name.begin(), name.end(), dfile.begin(), dfile.end(), caseequal)) return ErrCodes::CONTINUE;
        } else {
          // Trying to find localized trophy
          if (!std::equal(name.begin(), name.end(), m_localizedTrophyFile.begin(), m_localizedTrophyFile.end(), caseequal)) {
            if (std::equal(name.begin(), name.end(), dfile.begin(), dfile.end(), caseequal)) dent = ent;
            return ErrCodes::CONTINUE;
          }
        }
      }

      boost::scoped_ptr<char> mem(new char[ent.len]);
      trfile.seekg(ent.pos); // Seek to file position

      if (((ent.flag >> 24) & 0x03) == 0) {
        if (!trfile.read(mem.get(), ent.len)) return ErrCodes::IO_FAIL;
      } else {
        static constexpr int32_t IV_SIZE           = TR_AES_BLOCK_SIZE;
        static constexpr int32_t ENC_SCE_SIGN_SIZE = TR_AES_BLOCK_SIZE * 3; // 384 encrypted bits is just enough to find interesting for us string

        uint8_t d_iv[TR_AES_BLOCK_SIZE];
        uint8_t kg_iv[TR_AES_BLOCK_SIZE];
        uint8_t enc_xmlh[ENC_SCE_SIGN_SIZE];
        ::memset(kg_iv, 0, TR_AES_BLOCK_SIZE);

        if (!trfile.read((char*)d_iv, TR_AES_BLOCK_SIZE)) return ErrCodes::IO_FAIL;
        if (!trfile.read((char*)enc_xmlh, ENC_SCE_SIGN_SIZE)) return ErrCodes::IO_FAIL;

        const auto trydecrypt = [this, &mem, &ent, d_iv, kg_iv, enc_xmlh, &trfile](uint32_t npid) -> bool {
          uint8_t outbuffer[512];
          uint8_t inbuffer[512];

          ::memset(outbuffer, 0, 512);
          ::memset(inbuffer, 0, 512);
          ::sprintf_s((char*)inbuffer, sizeof(inbuffer), "NPWR%05d_00", npid);

          int outlen;

          //  Key creation context
          {
            EVP_CIPHER_CTX* key_ctx = EVP_CIPHER_CTX_new();
            if (!EVP_EncryptInit(key_ctx, EVP_aes_128_cbc(), m_trkey, kg_iv)) {
              EVP_CIPHER_CTX_free(key_ctx);
              return false;
            }
            if (!EVP_EncryptUpdate(key_ctx, outbuffer, &outlen, inbuffer, TR_AES_BLOCK_SIZE)) {
              EVP_CIPHER_CTX_free(key_ctx);
              return false;
            }
            /**
             * Cipher finalizing is not really necessary there,
             * since we use only 16 bytes encrypted by the update function above
             */
            EVP_CIPHER_CTX_free(key_ctx);
          }
          //- Key creation context

          //  Data decipher context
          EVP_CIPHER_CTX* data_ctx = EVP_CIPHER_CTX_new();
          {
            if (!EVP_DecryptInit(data_ctx, EVP_aes_128_cbc(), outbuffer, d_iv)) {
              EVP_CIPHER_CTX_free(data_ctx);
              return false;
            }
            if (!EVP_DecryptUpdate(data_ctx, outbuffer, &outlen, enc_xmlh, ENC_SCE_SIGN_SIZE)) {
              EVP_CIPHER_CTX_free(data_ctx);
              return false;
            }
            if (::_strnicmp((char*)outbuffer, "<!--Sce-Np-Trophy-Signature", 27) != 0) {
              EVP_CIPHER_CTX_free(data_ctx);
              return false;
            }

            // We found valid NPID, now we can continue our thing
            ::memcpy(mem.get(), outbuffer, outlen);
            char* mem_off = mem.get() + outlen;
            // Seeking to unread encrypted data position (skip Init Vector + Signature Comkment Part)
            trfile.seekg(ent.pos + TR_AES_BLOCK_SIZE + ENC_SCE_SIGN_SIZE);

            size_t copied;
            while ((copied = size_t(mem_off - mem.get())) < ent.len) {
              size_t len = std::min(ent.len - copied, sizeof(inbuffer));
              // Reading the rest of AES data block by block
              if (!trfile.read((char*)inbuffer, len)) {
                EVP_CIPHER_CTX_free(data_ctx);
                return false;
              }
              if (!EVP_DecryptUpdate(data_ctx, (uint8_t*)mem_off, &outlen, inbuffer, len)) {
                EVP_CIPHER_CTX_free(data_ctx);
                return false;
              }

              mem_off += outlen;
            }

            EVP_CIPHER_CTX_free(data_ctx);
            *mem_off = '\0'; // Finally
          }
          //- Data decipher context

          return true;
        }; // lambda: trydecrypt

        int  npid;
        bool success   = false;
        auto npid_path = accessFileManager().getGameFilesDir() / ".npcommid";

        {
          // Trying to obtain cached NPID for this title and use it for trophies decrypting
          std::ifstream npid_f(npid_path);
          if (npid_f.is_open()) {
            npid_f >> npid;
            success = trydecrypt(npid);
          }
        }

        if (!success) { // We failed to decrypt xml with saved NPID, now we search it
          for (uint32_t n = 0; n < 99999; n++) {
            trfile.seekg(ent.pos + TR_AES_BLOCK_SIZE);
            if ((success = trydecrypt(n)) == true) {
              npid = n;
              break;
            }
          }

          if (success) { // NPID found, saving it to cache file
            std::ofstream npid_f(npid_path, std::ios::out);
            if (npid_f.is_open()) {
              npid_f << npid;
            }
          }
        }

        if (!success) return ErrCodes::DECRYPT;
      }

      return XML_parse(mem.get(), ctx);
    } // group & trophy callbacks

    return ErrCodes::CONTINUE;
  }

  public:
  Trophies() {
    auto [lock, jData] = accessConfig()->accessModule(ConfigModFlag::GENERAL);
    std::string hexdata;

    if (getJsonParam(jData, "trophyKey", hexdata) && hexdata.length() > 31) {
      if (hexdata.starts_with("0x")) hexdata.erase(0, 2);
      if (hexdata.find_first_not_of("0123456789abcdefABCDEF") != hexdata.npos) return;
      m_bKeySet = true;
      for (uint32_t i = 0; i < 32; i += 2) {
        auto byte      = hexdata.substr(i, 2);
        m_trkey[i / 2] = (uint8_t)std::strtol(byte.c_str(), nullptr, 16);
      }
    }

    {
      SystemParamLang systemlang;

      try {
        (*jData)["systemlang"].get_to(systemlang);
      } catch (const json::exception& e) {
        systemlang = SystemParamLang::EnglishUS;
      }

      if (systemlang != SystemParamLang::EnglishUS) m_localizedTrophyFile = std::format("TROP_{:02}.ESFM", (uint32_t)systemlang);
    }
  }

  ErrCodes parseTRP(trp_context* ctx) final {
    if (!m_bKeySet) return ErrCodes::NO_KEY_SET;
    if (ctx == nullptr) return ErrCodes::INVALID_CONTEXT;
    // We don't want every callback to be cancelled before we even begin
    ctx->group.cancelled = (ctx->group.func == nullptr);
    ctx->entry.cancelled = (ctx->entry.func == nullptr);
    ctx->pngim.cancelled = (ctx->pngim.func == nullptr);
    ctx->itrop.cancelled = (ctx->itrop.func == nullptr);
    if (ctx->cancelled()) return ErrCodes::NO_CALLBACKS;
    if (ctx->lightweight && !ctx->itrop.cancelled) return ErrCodes::NO_ITROP;

    std::unique_lock lock(m_mutexParse);
    m_bIgnoreMissingLocale = false;

    auto mpath = accessFileManager().getMappedPath("/app0/sce_sys/trophy/trophy00.trp");
    if (!mpath.has_value()) return ErrCodes::NO_TROPHIES;

    std::ifstream trfile(mpath->c_str(), std::ios::in | std::ios::binary);

    if (trfile.is_open()) {
      trp_header hdr;

      if (!trfile.read((char*)&hdr, sizeof(hdr))) return ErrCodes::IO_FAIL;
      if (hdr.magic != 0x004DA2DC) return ErrCodes::INVALID_MAGIC;
      if (hdr.version != 0x03000000) return ErrCodes::INVALID_VERSION;

      /**
       * This file's numbers encoded in big-endian for some twisted reason.
       * Probably because of format history.
       */
      hdr.entry_num  = _byteswap_ulong(hdr.entry_num);
      hdr.entry_size = _byteswap_ulong(hdr.entry_size);
      if (hdr.entry_size != sizeof(trp_entry)) return ErrCodes::INVALID_ENTSIZE;

      trp_entry ent, dent = {0};
      for (uint32_t i = 0; i < hdr.entry_num; ++i) {
        if (ctx->cancelled()) return ErrCodes::SUCCESS;

        trfile.seekg(sizeof(trp_header) + (sizeof(trp_entry) * i));
        if (!trfile.read((char*)&ent, sizeof(trp_entry))) return ErrCodes::IO_FAIL;

        ent.pos = _byteswap_uint64(ent.pos);
        ent.len = _byteswap_uint64(ent.len);

        auto ret = TRP_readentry(ent, dent, trfile, ctx);
        if (ret != ErrCodes::CONTINUE) return ret;
      } // entries loop

      // Group or trophy callback is not cancelled yet, looks like we missed localized esfm file, trying to use the default one
      if (!ctx->group.cancelled || !ctx->entry.cancelled || !ctx->itrop.cancelled) {
        m_bIgnoreMissingLocale = true;
        if (dent.len != 0) {
          auto ret = TRP_readentry(dent, dent, trfile, ctx);
          if (ret != ErrCodes::CONTINUE) return ret;
        }
      }

      return ErrCodes::SUCCESS;
    } // trfile.is_open()

    return ErrCodes::NO_TROPHIES;
  } // parseTRP()

  bool createContext(int32_t userId, uint32_t label) final {}

  bool getProgress(int32_t userId, uint32_t progress[4]) final {}

  bool unlockTrophy(int32_t userId, int32_t trophyId) final {}

  bool resetUserInfo(int32_t userId) final {}
};

ITrophies& accessTrophies() {
  static Trophies ti;
  return ti;
}
