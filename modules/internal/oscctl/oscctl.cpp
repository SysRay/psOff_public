#define __APICALL_EXTERN
#include "oscctl.h"
#undef __APICALL_EXTERN

class OscCtl: public IOscCtl {
  Params m_params {};

  public:
  OscCtl() {}

  ~OscCtl() {}

  bool setEnterLabel(SceImeEnterLabel lab) final {
    const char* label = nullptr;

    switch (lab) {
      case SceImeEnterLabel::DEFAULT: label = "Enter"; break;
      case SceImeEnterLabel::SEND: label = "Send"; break;
      case SceImeEnterLabel::SEARCH: label = "Search"; break;
      case SceImeEnterLabel::GO: label = "Go"; break;
    }

    if (label) {
      std::string_view(label).copy(m_params.enterLabel, sizeof(m_params.enterLabel));
      return true;
    }

    *m_params.enterLabel = '\0';
    return false;
  }

  bool setBuffer(wchar_t* buff, size_t buffsz) final {
    if (buffsz == 0) return false;
    m_params.internal_buffer = (char*)malloc(buffsz);
    m_params.buffer          = buff;
    m_params.buffersz        = buffsz;
    return true;
  }

  bool run() final {
    if (m_params.status != IOscCtl::Status::NONE) return false;
    m_params.status = IOscCtl::Status::HIDDEN;
    return true;
  }

  bool show() final {
    if (m_params.status != IOscCtl::Status::HIDDEN) return false;
    m_params.status = IOscCtl::Status::SHOWN;
    return true;
  }

  void destroy() final {
    if (m_params.status == IOscCtl::Status::NONE) return;
    m_params.status = IOscCtl::Status::NONE;
    if (m_params.internal_buffer) {
      free(m_params.internal_buffer);
      m_params.internal_buffer = nullptr;
    }
    m_params.buffer      = nullptr;
    m_params.buffersz    = 0;
    *m_params.enterLabel = '\0';
    *m_params.title      = '\0';
    *m_params.details    = '\0';
  }

  Params* getParams() { return &m_params; };
};

IOscCtl& accessOscCtl() {
  static OscCtl ih;
  return ih;
}
