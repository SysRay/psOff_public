#pragma once
#include "../formats/ISymbols.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

std::pair<void*, std::unique_ptr<Symbols::SymbolExport>> loadModule(const char* name, const wchar_t* filepath, int libVersion);

void unloadModule(void* handle);
