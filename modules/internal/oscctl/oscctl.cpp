#include "oscctl.h"

class OscCtl: public IOscCtl {
  Params m_params {};

  public:
  OscCtl() {}

  ~OscCtl() {}

  bool setEnterLabel(SceImeEnterLabel lab) {
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

  Params* getParams() { return &m_params; };
};

IOscCtl& accessOscCtl() {
  static OscCtl ih;
  return ih;
}
