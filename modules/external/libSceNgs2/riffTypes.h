#pragma once
#include <stdint.h>

struct RiffHeader {
  unsigned char chunkID[4]; // RIFF
  uint32_t      chunkSize;
} __attribute__((packed));

struct RiffWaveHeader {
  unsigned char chunkID[4]; // RIFF
  uint32_t      chunkSize;
  unsigned char riffType[4]; // WAVE
} __attribute__((packed));

struct RiffFormatHeader {
  unsigned char chunkID[4]; // RIFF
  uint32_t      chunkSize;
  uint16_t      audioFormat;
  uint16_t      numChannels;
  uint32_t      sampleRate;  // Sampling Frequency in Hz
  uint32_t      avgByteRate; // ~bytes per second
  uint16_t      frameSize;   // BlockAlign
  uint16_t      bitsPerSample;
} __attribute__((packed));