#include "moduleLoader.h"

#include "logging.h"

#include <boost/beast/core/detail/base64.hpp>
#include <boost/functional/hash.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <vector>
#include <windows.h>

#undef min

LOG_DEFINE_MODULE(ModuleLoader);

std::pair<void*, std::unique_ptr<Symbols::SymbolExport>> loadModule(const char* libName, const wchar_t* filepath, int libVersion) {
  LOG_USE_MODULE(ModuleLoader);

  HMODULE hModule = LoadLibraryW(filepath);
  if (hModule == NULL) {
    LOG_ERR(L"Couldn't load library %S, err:%d", filepath, GetLastError());
    return {};
  }

  auto const imageDosHeader = (PIMAGE_DOS_HEADER)hModule;
  if (imageDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
    LOG_ERR(L"Wrong image signature %x for %S", imageDosHeader->e_magic, filepath);
    return {};
  }

  auto const imageNtHeaders = (PIMAGE_NT_HEADERS)((unsigned char*)imageDosHeader + imageDosHeader->e_lfanew);
  if (imageNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
    LOG_ERR(L"Wrong PE signature %x for %S", imageDosHeader->e_magic, filepath);
    return {};
  }

  PIMAGE_OPTIONAL_HEADER  imageOptionalHeader      = (PIMAGE_OPTIONAL_HEADER)&imageNtHeaders->OptionalHeader;
  PIMAGE_DATA_DIRECTORY   imageExportDataDirectory = &(imageOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
  PIMAGE_EXPORT_DIRECTORY imageExportDirectory     = (PIMAGE_EXPORT_DIRECTORY)((unsigned char*)hModule + imageExportDataDirectory->VirtualAddress);

  DWORD numberOfNames = imageExportDirectory->NumberOfNames;

  PDWORD exportAddressTable     = (PDWORD)((unsigned char*)hModule + imageExportDirectory->AddressOfFunctions);
  PWORD  nameOrdinalsPointer    = (PWORD)((unsigned char*)hModule + imageExportDirectory->AddressOfNameOrdinals);
  PDWORD exportNamePointerTable = (PDWORD)((unsigned char*)hModule + imageExportDirectory->AddressOfNames);

  auto libInfo = std::make_unique<Symbols::SymbolExport>(libName, libVersion, "", 1, 0);

  for (size_t n = 0; n < numberOfNames; n++) {
    auto name = std::string_view((const char*)((unsigned char*)hModule + exportNamePointerTable[n]));
    if (name.ends_with("=")) {
      std::string nid(name.data(), 0, name.size() - 1);
      auto const  pFunc = (uint64_t)((unsigned char*)hModule + exportAddressTable[nameOrdinalsPointer[n]]);

      libInfo->symbolsMap.emplace(std::make_pair(nid, Symbols::SymbolExport::Symbol {nid, {}, pFunc, Symbols::SymbolType::Func}));
    } else if (name == "MODULE_NAME") {
      auto const val     = *(char const**)((unsigned char*)hModule + exportAddressTable[nameOrdinalsPointer[n]]);
      libInfo->modulName = val;
    }
  }

  if (libInfo->modulName.empty()) {
    LOG_CRIT(L"No Module name in %S", libName);
  }
  return {(void*)hModule, std::move(libInfo)};
}

void unloadModule(void* handle) {
  FreeLibrary((HMODULE)handle);
}
