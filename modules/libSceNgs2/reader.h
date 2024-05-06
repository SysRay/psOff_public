#pragma once

class Reader {
  void* m_pimpl;
  bool  m_isInit = false;

  public:
  Reader();
  ~Reader();

  bool init(void const* data, size_t size);

  bool getAudio(void* buffer, size_t bufferSize);
};