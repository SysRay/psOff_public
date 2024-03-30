#include <boost/uuid/detail/sha1.hpp>
#include <filesystem>
#include <fstream>
#include <vector>
#include <windows.h>

std::string encode(const uint8_t* in) {
  std::string out;
  out.resize(12);

  auto pOut = out.data();

  constexpr const char* tab = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-"};

  for (auto n = 8 / 3; n--;) {
    *pOut++ = tab[(in[0] & 0xfc) >> 2];
    *pOut++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
    *pOut++ = tab[((in[2] & 0xc0) >> 6) + ((in[1] & 0x0f) << 2)];
    *pOut++ = tab[in[2] & 0x3f];
    in += 3;
  }

  *pOut++ = tab[(in[0] & 0xfc) >> 2];
  *pOut++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
  *pOut++ = tab[(in[1] & 0x0f) << 2];
  *pOut++ = '=';

  *pOut = '\0';
  return out;
}

std::string name2nid(std::string_view name) {
  boost::uuids::detail::sha1 sha1;
  sha1.process_bytes((unsigned char*)name.data(), name.size());
  sha1.process_bytes("\x51\x8d\x64\xa6\x35\xde\xd8\xc1\xE6\xB0\x39\xB1\xC3\xE5\x52\x30", 16);

  unsigned hash[5];
  sha1.get_digest(hash);

  uint64_t const digest = ((uint64_t)hash[0] << 32u) | hash[1];
  return encode((uint8_t*)&digest);
}

std::string hexId2nid(std::string_view hexId) {
  union bytes {
    uint8_t  c[8];
    uint64_t l;
  } dst, src;

  uint64_t hash = strtoull(hexId.data(), 0, 16);

  src.l = hash;

  // big <-> little conversion
  dst.c[0] = src.c[7];
  dst.c[1] = src.c[6];
  dst.c[2] = src.c[5];
  dst.c[3] = src.c[4];
  dst.c[4] = src.c[3];
  dst.c[5] = src.c[2];
  dst.c[6] = src.c[1];
  dst.c[7] = src.c[0];

  return encode(dst.c);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Wrong argument count: filepath, symbolFolder\n");
    return -1;
  }

  std::ifstream file;
  file.open(argv[1], std::ios::binary);
  if (file.fail()) {
    printf("Couldn't read %s\n", argv[1]);
    return -1;
  }

  // Read DOS HEADER
  _IMAGE_DOS_HEADER imageDosHeader;
  if (file.read((char*)&imageDosHeader, sizeof(_IMAGE_DOS_HEADER)).fail()) {
    printf("Couldn't read dos header\n");
    return -2;
  }

  if (imageDosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
    printf("File has no DOS Header\n");
    return -1;
  }

  // Read NT HEADER
  _IMAGE_NT_HEADERS imageNtHeaders;
  file.seekg(imageDosHeader.e_lfanew);

  if (file.read((char*)&imageNtHeaders, sizeof(_IMAGE_NT_HEADERS)).fail()) {
    printf("Couldn't read NT header\n");
    return -2;
  }

  // Read sections -> find .rdata base in file
  int64_t  vaddr_rdata = 0;
  uint64_t paddr_rdata = 0;

  std::vector<uint8_t> rdata;

  auto const numSections = imageNtHeaders.FileHeader.NumberOfSections;

  {
    int64_t curOffset = imageDosHeader.e_lfanew + sizeof(_IMAGE_NT_HEADERS) + 16;
    file.seekg(curOffset, std::ios_base::beg);
  }

  for (size_t n = 0; n < numSections; ++n) {
    _IMAGE_SECTION_HEADER header;
    if (file.read((char*)&header, IMAGE_SIZEOF_SECTION_HEADER).fail()) {
      printf("Couldn't read NT header\n");
      return -2;
    }
    if (std::string((char*)header.Name) == ".rdata") {
      vaddr_rdata = header.VirtualAddress;
      paddr_rdata = header.PointerToRawData;

      rdata.resize(header.SizeOfRawData);

      file.seekg(paddr_rdata, std::ios_base::beg);
      if (file.read((char*)rdata.data(), rdata.size()).fail()) {
        printf("Couldn't read rdata header\n");
        return -2;
      }
      break;
    }
  }

  if (vaddr_rdata == 0) {
    printf("Couldn't find .rdata\n");
    return -2;
  }
  // -

  // Additional Header
  PIMAGE_OPTIONAL_HEADER imageOptionalHeader      = (PIMAGE_OPTIONAL_HEADER)&imageNtHeaders.OptionalHeader;
  PIMAGE_DATA_DIRECTORY  imageExportDataDirectory = &(imageOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);

  PIMAGE_EXPORT_DIRECTORY imageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(rdata.data() + imageExportDataDirectory->VirtualAddress - vaddr_rdata);

  DWORD  numNames               = imageExportDirectory->NumberOfNames;
  PDWORD exportNamePointerTable = (PDWORD)(rdata.data() + imageExportDirectory->AddressOfNames - vaddr_rdata);
  PDWORD exportAddressTable     = (PDWORD)(rdata.data() + imageExportDirectory->AddressOfFunctions - vaddr_rdata);
  // -

  // ### Read all symbol names, make nids, store with new offset
  // Read all and change to nids

  std::filesystem::create_directory(argv[2]);
  std::ofstream fDumpSymbols((std::filesystem::path(argv[2]) / (std::filesystem::path(argv[1]).stem().string() + ".txt")).c_str(), std::ios::trunc);

  auto funcLogSymbol = [&fDumpSymbols](std::string_view nid, std::string_view name) { fDumpSymbols << nid << '\t' << name << '\n'; };

  std::vector<std::string> symbols(numNames);
  bool                     hasModuleName = false;
  for (size_t n = 0; n < numNames; n++) {
    auto name = std::string_view((const char*)(rdata.data() + exportNamePointerTable[n] - vaddr_rdata));
    // Handle mangled name
    if (name.starts_with('?')) {
      name = name.substr(1, name.find_first_of('@') - 1);
    }
    // -

    if (name.starts_with("sce") || name.starts_with("_sce")) {
      // Change to nids
      symbols[n] = name2nid(name);
      funcLogSymbol(symbols[n], name);
    } else if (name.starts_with("__hex_")) {
      // Change to nids
      symbols[n] = hexId2nid(name.substr(6));
      funcLogSymbol(symbols[n], "");
    } else if (name.starts_with("__sce_")) {
      // Change to nids
      symbols[n] = name2nid(name.substr(6));
      funcLogSymbol(symbols[n], name.substr(6));
    } else {
      symbols[n] = std::string(name);
      if (!hasModuleName && name == "MODULE_NAME") {
        hasModuleName = true;
      }
    }
  }

  if (!hasModuleName) {
    printf("MODULE_NAME not found\n");
    return -2;
  }

  const char* tableEndAddr = 0;
  {
    auto name    = std::string_view((const char*)(rdata.data() + exportNamePointerTable[numNames - 1] - vaddr_rdata));
    tableEndAddr = name.data() + name.size();
  }

  // Store
  DWORD curOffset = exportNamePointerTable[0] - vaddr_rdata;
  for (size_t n = 0; n < numNames; n++) {
    // Create new offsets
    exportNamePointerTable[n] = curOffset + vaddr_rdata;

    // store name
    auto pDest = (char*)(rdata.data() + curOffset);
    memcpy(pDest, symbols[n].data(), 1 + symbols[n].size());
    curOffset += 1 + symbols[n].size();
  }

  if ((char*)rdata.data() + curOffset > tableEndAddr) {
    size_t const diff = ((char*)rdata.data() + curOffset) - tableEndAddr;
    printf("New Table is > old by %llu\nSimply add a dummy function", diff);
    return -2;
  }
  for (char* begin = (char*)rdata.data() + curOffset; begin < tableEndAddr; ++begin) {
    *begin = 0;
  }

  std::fstream outFile;
  outFile.open(argv[1], std::ios::out | std::ios::in | std::ios::binary);
  if (outFile.fail()) {
    printf("Couldn't open file for out, %s\n", argv[1]);
    return -1;
  }

  outFile.seekp(paddr_rdata, std::ios_base::beg);
  if (outFile.write((char*)rdata.data(), rdata.size()).fail()) {
    printf("Couldn't write data\n");
    return -2;
  }
  return 0;
}
