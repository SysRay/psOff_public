# How to libSceNgs2

I used "We Are Doomed" as a reference, to see how things work.

First of all game should create `SceNgs2Handle` objects for `Ngs2System`, `Ngs2SamplerRack` and `Ngs2MasteringRack`: \
\> `sceNgs2SystemCreate[WithAllocator]` + `sceNgs2RackCreate[WithAllocator]`. \
I think passed allocator should be used to allocate all the structures Ngs2 use, but I'm not sure.

Then the game parses the audio waveform file using  `sceNgs2ParseWaveform[Data/File/User]` and then pushes it to `VoceHandle`:

Somewhere in the game's code:
```c++
const SceNgs2SamplerVoiceWaveformBlocksParam param {
  .header.lsize = sizeof(param),
  .header.next = nextParam,
  .header.id = SCE_NGS2_SAMPLER_VOICE_ADD_WAVEFORM_BLOCKS,
  .flags = samplerFlags,
  .data = waveData /*???*/,
  .numBlocks = waveInfoNum,
  .aBlock = waveInfoBlocks
};
sceNgs2VoiceControl(samplerVoiceHandle, &param);
```

Game main thread:
```c++

static int my_ngs2_alloc(SceNgs2ContextBufferInfo* cbi) {
  void* mem = malloc(cbi->hostBufferSize);
  if (mem == nullptr) return Err::EMPTY_BUFFER;
  cbi->hostBuffer = mem;
  return Ok;
}

static int my_ngs2_free(SceNgs2ContextBufferInfo* cbi) {
  free(cbi->hostBuffer);
  return Ok;
}

static void SceNgs2RenderFunc(SomeAudioInfoStruct* param) {
  SceNgs2RenderBufferInfo rbi {
    .bufferPtr = param->audioOutBuffer,
    .bufferSize = param->audioOutBufferSize,
    .waveType = SceNgs2WaveFormType::PCM_F32LITTLE,
    .channelsCount = SceNgs2ChannelsCount::CH_7_1,
  };
  sceNgs2SystemRender(param->system, &rbi, 1 /*number of buffers*/);
}

SceNgs2BufferAllocator allocator = {
  .allocHandler = my_ngs2_alloc,
  .freeHandler = my_ngs2_free,
  .userData = nullptr
};

SomeAudioInfoStruct param = {
  .render = SceNgs2RenderFunc
};

sceNgs2SystemCreateWithAllocator(nullptr /*System options, most games won't use it*/, &allocator, &param.system);
sceNgs2RackCreateWithAllocator(param.system, 0x3000, nullptr, &allocator, &param.masteringRack);
sceNgs2RackCreateWithAllocator(param.system, 0x1000, nullptr, &allocator, &param.samplerRack);
sceNgs2RackGetVoiceHandle(param.masteringRack, 0, &param.masteringVoiceHandle)
// Configure voice handle...
someThreadSpawnFunction(&threadEntry, &param);
```

Game audio thread:
```c++
static int threadEntry(SomeAudioInfoStruct* param) {
  int aHandle = sceAudioOutOpen(...);
  if (aHandle < 0) return 0;

  do {
    param->render(param);
    sceAudioOutOutput(aHandle, param->audioOutBuffer);
  } while (param->audioThreadActive);

  sceAudioOutOutput(aHandle, nullptr);
  sceAudioOutClose(aHandle);
}
```

Scheme:
```
               #############################################
               #               We Are Doomed               #
               #############################################
                                /\                  ||
                                ||                  ||
        Control, rendering etc. ||                  ||
                                ||                  ||
                                \/                  || audioOutBuffer from sceNgs2SystemRender,
             ##############################         || the game should send it to libSceAudioOut
             #         libSceNgs2         #         ||
             ##############################         ||
                                                    ||
                                                    ||
                                                    \/
                                            ##################
                                            # libSceAudioOut #
                                            ##################
```
