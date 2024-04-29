#pragma once

#define LIB_DEFINE_START(lib, ver, mod, verMajor, verMinor)                                                                                                    \
  {                                                                                                                                                            \
    auto libInfo        = std::make_unique<Symbols::SymbolExport>(lib, ver, mod, verMajor, verMinor);                                                          \
    libInfo->symbolsMap = {

#define LIB_FUNC(sym, func)                                                                                                                                    \
  {                                                                                                                                                            \
    sym, {                                                                                                                                                     \
      sym, #func, reinterpret_cast<uint64_t>(func), Symbols::SymbolType::Func                                                                                  \
    }                                                                                                                                                          \
  }

#define LIB_OBJECT(sym, obj)                                                                                                                                   \
  {                                                                                                                                                            \
    sym, {                                                                                                                                                     \
      sym, #obj, reinterpret_cast<uint64_t>(obj), Symbols::SymbolType::Object                                                                                  \
    }                                                                                                                                                          \
  }

#define LIB_DEFINE_END()                                                                                                                                       \
  }                                                                                                                                                            \
  ;                                                                                                                                                            \
  accessRuntimeLinker().addExport(std::move(libInfo));                                                                                                         \
  }