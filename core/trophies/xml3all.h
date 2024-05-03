/**
 * @file xml3all.h
 * @author Michael Chourdakis
 * @brief A single header simple and portable C++11 XML parser
 * @date 04/09/2018
 *
 * @copyright Copyright (c) 2018
 *
 * This source code is licensed under The Code Project Open License (CPOL)
 */
#ifdef _MSC_VER
#pragma warning(disable : 4290)
#pragma warning(disable : 4789)
#endif
#define CMSG_SIGNED_ENCODE_INFO_HAS_CMS_FIELDS
#define CMSG_SIGNER_ENCODE_INFO_HAS_CMS_FIELDS
#include <algorithm>
#include <cctype>
#include <functional>
#include <memory>
#include <regex>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

#pragma once

#ifndef _XML3ALL_H
#define _XML3ALL_H

namespace XML3 {

using namespace std;

#define XML3_VERSION               0x1
#define XML3_VERSION_REVISION_DATE "01-06-2015"

typedef struct {
  int  VersionHigh;
  int  VersionLow;
  char RDate[20];
} XML_VERSION_INFO;

enum class XML_ERROR { OK = 0L, INVALIDARG = -1, OPENFAILED = -2, SAVEFAILED = -3, INVALIDIDX = -4 };

enum class XML_PARSE {
  OK           = 0L,
  OPENFAILED   = -1,
  HDRERROR     = -2,
  VARERROR     = -3,
  COMMENTERROR = -4,
};

// #undef XX

// BXML
class BXML {
  private:
  vector<char> d;

  public:
  BXML(size_t s = 0);
  bool operator==(const BXML& b2);

  void ToB(string& s);
  void FromB(const char* ba);
  operator const char*() const;
  operator char*();
  const char* p() const;
  char*       p();
  size_t      size() const;

  vector<char>& GetD() { return d; }

  void clear();
  void reset();

  void Ensure(size_t news);
  void Resize(size_t news);
  void AddResize(size_t More);
};

string Char2Base64(const char* Z, size_t s, bool = true);
void   Base64ToChar(const char* Z, size_t s, BXML& out);
string Decode(const char* src);
string Format(const char* f, ...);
string Trim(const char* src, int m = 2);
string Encode(const char* src);
void   JsonParser(class XMLElement* root, const char* txt);

// XMLU Class, converts utf input to wide char and vice versa
class XMLU {
  public:
  string  tu;
  wstring wu;
  XMLU(const char* x);
  XMLU(const wchar_t* x);

  const wchar_t* wc();
  const char*    bc();
  operator const wchar_t*();
  operator const char*();
};

// xstring class, same as string with unicode part
class xstring: public string {
  private:
  mutable std::wstring wxx;

  public:
  xstring(): string() {}

  xstring(const char* t): string(t) {}

  xstring(const string& t): string(t) {}

  xstring(const xstring& t): string(t) {}

  xstring(xstring&& t): string(std::forward<xstring>(t)) {}

  xstring& operator=(const char* t) {
    string::operator=(t);
    return *this;
  }

  xstring& operator=(const string& t) {
    string::operator=(t);
    return *this;
  }

  xstring& operator=(const xstring& t) {
    string::operator=(t.c_str());
    return *this;
  }

  // Extra  stuff now
#ifdef _MSC_VER
  xstring(const char* f, ...) {
    va_list args;
    va_start(args, f);

    int len = _vscprintf(f, args) // _vscprintf doesn't count
              + 100;              // terminating '\0'
    if (len < 8192) len = 8192;

    char* b = new char[len];
#ifdef _MSC_VER
    vsprintf_s(b, len, f, args);
#else
    vsprintf(b, f, args);
#endif
    assign(b);
    delete[] b;
    va_end(args);
  }

  xstring(const wchar_t* f, ...) {
    va_list args;
    va_start(args, f);

    int len = _vscwprintf(f, args) // _vscprintf doesn't count
              + 100;               // terminating '\0'
    if (len < 8192) len = 8192;

    wchar_t* b = new wchar_t[len];
    vswprintf_s(b, len, f, args);
    operator=(b);
    delete[] b;
    va_end(args);
  }

  xstring& operator=(const wchar_t* t) {
    assign(XMLU(t));
    return *this;
  }

  xstring& operator=(const wstring& t) {
    operator=(t.c_str());
    return *this;
  }

  const wchar_t* w_str() const {
    wxx.assign(XMLU(c_str()).wu.c_str());
    return wxx.c_str();
  }
#endif
};

class XML;

class XMLId {
  private:
  unsigned long long ts = 0;

  public:
  XMLId();
  XMLId(unsigned long long a);
  XMLId(const XMLId& a);
  void operator=(unsigned long long a);

  bool               operator==(const XMLId& a);
  void               operator=(const XMLId& a);
  unsigned long long g() const;
};

class XMLSerialization {
  public:
  bool   NoEnc       = false;
  size_t deep        = 0;
  bool   ExcludeSelf = false;
  bool   NoCRLF      = false;
  bool   Canonical   = false;
};

class XMLContent {
  protected:
  string v;
  size_t ep = 0;

  public:
  friend class XMLElement;
  XMLContent();
  XMLContent(size_t ElementPosition, const char* ht);
  XMLContent(size_t ElementPosition, const wchar_t* ht);

  size_t MemoryUsage();

  // Operators
  bool operator==(const XMLContent& t) const;
  operator const string&() const;

  static std::string trim(const std::string& str, const std::string& whitespace = " \r\n\t");

  // Sets
  const string& SetFormattedValue(const char* fmt, ...);

  template <typename T>
  const string& SetFormattedValue(const char* fmt, T ty) {
    unique_ptr<char> t(new char[10000]);
#ifdef _MSC_VER
    sprintf_s(t.get(), 10000, fmt, ty);
#else
    sprintf(t.get(), fmt, ty);
#endif
    SetValue(t.get());
    return v;
  }

  const string& SetValueDouble(double v5) { return SetFormattedValue<double>("%f", v5); }

  const string& SetValueFloat(float v5) { return SetFormattedValue<float>("%f", v5); }

  const string& SetValueInt(int v5) { return SetFormattedValue<int>("%i", v5); }

  const string& SetValueUInt(unsigned int v5) { return SetFormattedValue<unsigned int>("%u", v5); }

  const string& SetValueLongLong(long long v5) { return SetFormattedValue<long long>("%lli", v5); }

  const string& SetValueULongLong(unsigned long long v5) { return SetFormattedValue<long long>("%llu", v5); }

  XMLContent&   operator=(const char* s);
  XMLContent&   operator=(const string& s);
  const string& SetBinaryValue(const char* data, size_t len);

  void SetValue(const char* VV);
  void SetWideValue(const wchar_t* VV);
  void SetValue(const string& VV);
  void SetValue(const wstring& VV);
  void Clear();
  void SetEP(size_t epp);

  // Gets
  BXML GetBinaryValue() const;

  template <typename T>
  T GetFormattedValue(const char* fmt, const T def) const {
    T x = def;
#ifdef _MSC_VER
    sscanf_s(v.c_str(), fmt, &x);
#else
    sscanf(v.c_str(), fmt, &x);
#endif
    return x;
  }

  int GetValueInt(int def = 0) { return GetFormattedValue<int>("%i", def); }

  unsigned int GetValueUInt(unsigned int def = 0) { return GetFormattedValue<unsigned int>("%u", def); }

  long long GetValueLongLong(long long def = 0) { return GetFormattedValue<long long>("%lli", def); }

  unsigned long long GetValueULongLong(unsigned long long def = 0) { return GetFormattedValue<unsigned long long>("%llu", def); }

  float GetValueFloat(float def = 0) { return GetFormattedValue<float>("%f", def); }

  double GetValueDouble(double def = 0) { return GetFormattedValue<double>("%lf", def); }

  const string& GetValue() const;
  wstring       GetWideValue(int CP = CP_UTF8) const;
  string        GetValueDefault(const char* def) const;
  size_t        GetEP() const;

  // Serialization
  virtual string Serialize(XMLSerialization* srz = 0) const;
};

class XMLVariable: public XMLContent {
  private:
  string n;
  bool   tmp = false;

  public:
  friend class XMLElement;
  explicit XMLVariable();
  explicit XMLVariable(const char* nn, const char* vv, bool Temp = false);
  //	explicit XMLVariable(const XMLVariable& h);
  //	XMLVariable& operator =(const XMLVariable& h);
  //	XMLVariable& operator =(const std::initializer_list<string>& s);

  const string& SetName(const char* VN);
  const string& SetName(const string& VN);
  void          Clear();
  XMLVariable&  operator=(const char* s);
  XMLVariable&  operator=(const string& s);
  XMLVariable&  operator=(unsigned long long j);
  XMLVariable&  operator=(signed long long j);

  // Compare
  bool operator<(const XMLVariable& x) const;
  bool operator==(const XMLVariable& x) const;
  bool operator==(const char* x) const;
  // Gets
  const string& GetName() const;
  bool          IsNS() const;
  string        GetNS() const;

  // Memory usage
  size_t MemoryUsage() const;

  // Serialization
  virtual string Serialize(XMLSerialization* srz = 0) const;
};

typedef XMLVariable XMLAttribute;

class XMLCData: public XMLContent {
  public:
  XMLCData(size_t ElementPosition = 0, const char* ht = "");
  XMLCData(size_t ElementPosition, const wchar_t* ht);

  // Serialization
  virtual string Serialize(XMLSerialization* srz = 0) const;
};

class XMLDocType: public XMLContent {
  public:
  XMLDocType(const char* ht = "");

  XMLDocType(const wchar_t* ht);

  // Serialization
  virtual string Serialize(XMLSerialization* srz = 0) const;
};

class XMLComment: public XMLContent {
  public:
  friend class XML;

  XMLComment(size_t ElementPosition = 0, const char* ht = "");
  XMLComment(size_t ElementPosition, const wchar_t* ht);

  // Serialization
  virtual string Serialize(XMLSerialization* srz = 0) const;
};

class XMLHeader {
  private:
  XMLVariable version;
  XMLVariable encoding;
  XMLVariable standalone;

  public:
  XMLHeader();

  XMLVariable& GetVersion() { return version; }

  XMLVariable& GetEncoding() { return encoding; }

  XMLVariable& GetStandalone() { return standalone; }

  const XMLVariable& GetVersion() const { return version; }

  const XMLVariable& GetEncoding() const { return encoding; }

  const XMLVariable& GetStandalone() const { return standalone; }

  // Serialization
  string Serialize() const;

  void Default();
};

class XMLElement {
  private:
  string                          el = "e";
  vector<shared_ptr<XMLElement>>  children;
  vector<shared_ptr<XMLVariable>> variables;
  vector<shared_ptr<XMLContent>>  contents;
  vector<shared_ptr<XMLComment>>  comments;
  vector<shared_ptr<XMLCData>>    cdatas;
  unsigned long long              param  = 0;
  XMLId                           parent = 0;
  XMLId                           id;
  XMLElement*                     par = 0;

  static void CloneMirror(XMLElement& to, const XMLElement& from);

  public:
  XMLElement();
  XMLElement(const char*);
  XMLElement(const XMLElement&);
  XMLElement(XMLElement&&);

  XMLElement Mirror() const;

  const vector<shared_ptr<XMLComment>>& GetComments() const { return comments; }

  const vector<shared_ptr<XMLElement>>& GetChildren() const { return children; }

  const vector<shared_ptr<XMLVariable>>& GetVariables() const { return variables; }

  const vector<shared_ptr<XMLCData>>& GetCDatas() const { return cdatas; }

  const vector<shared_ptr<XMLContent>>& GetContents() const { return contents; }

  vector<shared_ptr<XMLComment>>& GetComments() { return comments; }

  vector<shared_ptr<XMLElement>>& GetChildren() { return children; }

  vector<shared_ptr<XMLVariable>>& GetVariables() { return variables; }

  vector<shared_ptr<XMLCData>>& GetCDatas() { return cdatas; }

  vector<shared_ptr<XMLContent>>& GetContents() { return contents; }

  size_t GetChildrenNum() const { return children.size(); }

  class it {
    const XMLElement* e;
    size_t            idx = 0;

public:
    it(const XMLElement* ee, size_t i = 0) {
      e   = ee;
      idx = i;
    }

    it& operator++() {
      idx++;
      return *this;
    }

    it& operator--() {
      idx--;
      return *this;
    }

    bool operator==(it i2) {
      if (i2.e == e && i2.idx == idx) return true;
      return false;
    }

    bool operator!=(it i2) { return !operator==(i2); }

    XMLElement& operator*() { return *(e->GetChildren()[idx]); }
  };

  it begin() const { return it(this); }

  it end() const { return it(this, GetChildrenNum()); }

  // Operators
  bool        operator==(const XMLElement& t) const;
  bool        operator<(const XMLElement& x) const;
  XMLElement& operator=(const char*);
  XMLElement& operator=(const XMLElement& t);

  // Gets
  const XMLElement& operator[](size_t idx) const;
  XMLElement&       operator[](size_t idx);
  XMLElement&       operator[](const char* elm);

  const string& v(size_t idx) const;
  wstring       wv(const char* nn);
  const string& v(const char* nn);
  string        vd(const char* nn, const char* def = 0);
  string        vd(const char* nn, const char* def = 0) const;

  string             Content() const;
  string             GetContent() const;
  XMLVariable&       vv(const char* nn);
  unsigned long long GetElementParam() const;
  const string&      GetElementName() const;
  void               GetAllChildren(vector<shared_ptr<XMLElement>>& ch) const;
  //		shared_ptr<XMLElement> GetParent(shared_ptr<XMLElement> r) const;
  XMLElement* GetParent(XMLElement* r) const;
  size_t      GetElementIndex(const XMLElement& e) const;

  // Sets
  void SetElementName(const char* x);
  void SetElementName(const wchar_t* x);
  void SetElementParam(unsigned long long p);

  XML_ERROR    MoveElement(size_t i, size_t y);
  XMLVariable& SetValue(const char* vn, const char* vp);
  XMLContent&  SetContent(const char* vp);

  // Find
  shared_ptr<XMLElement>  FindElementZ(const char* n, bool ForceCreate = false);
  shared_ptr<XMLVariable> FindVariableZ(const char* n, bool ForceCreate = false, const char* defv = "", size_t* pidx = 0);
  shared_ptr<XMLVariable> FindVariable(const char* n) const;

  // Inserts
  shared_ptr<XMLElement> InsertElement(size_t y, const XMLElement& x);
  shared_ptr<XMLElement> InsertElement(size_t y, XMLElement&& x);
  XMLElement&            AddElement(const XMLElement& c);
  XMLElement&            AddElement(XMLElement&& c);
  XMLElement&            AddElement(const char* n = "");
  shared_ptr<XMLElement> AddElement2(const char* n = "");
  void                   AddElements(const std::initializer_list<string>& s);
  void                   SetVariables(const std::initializer_list<string>& s);
  XMLVariable&           AddVariable(const char* vn = "n", const char* vv = "v", size_t p = -1);
  XMLVariable&           AddVariable(const XMLVariable& v, size_t p = -1);
  XMLContent&            AddContent(const XMLContent& co, size_t pos = -1);
  XMLContent&            AddContent(const char* pv, size_t ep, size_t p = -1);
  XMLComment&            AddComment(const char* pv, size_t ep, size_t p = -1);
  XMLCData&              AddCData(const char* pv, size_t ep, size_t p = -1);

  // Removals
  size_t RemoveAllElements();
  size_t RemoveElement(size_t i);
  size_t RemoveElement(XMLElement* p);
  size_t RemoveElementByName(const char* n);

  shared_ptr<XMLElement> RemoveElementAndKeep(size_t i);
  void                   RemoveDuplicateNamespaces(const char* vn = 0);

  void clear();

  // Variables
  size_t RemoveAllVariables();

  size_t RemoveVariable(size_t i);
  bool   RemoveVariableZ(const char* n);

  shared_ptr<XMLVariable> RemoveVariableAndKeep(size_t i);

  string EorE(const string& s, bool N) const;
  string Serialize(XMLSerialization* srz = 0) const;

  void Serialize(string& v, XMLSerialization* srz = 0) const;
};

class XML {
  private:
  bool                           UnicodeFile = false;
  string                         fname;
  XMLHeader                      hdr;
  XMLDocType                     doctype;
  vector<shared_ptr<XMLComment>> hdrcomments;
  XMLElement                     root;

  public:
  string GetFName() { return fname; }

  // Constructors
  XML();
  XML(const char* file);
  XML(const wchar_t* file);
  XML(const char* mem, size_t l);

  // Copy/Move
  XML(const XML&);
  void operator=(const XML&);
  XML(XML&&);
  void operator=(XML&&);

  void operator=(const char* d);

  // Savers
  size_t    SaveFP(FILE* fp) const;
  XML_ERROR Save() const;
  XML_ERROR Save(const char* f, XMLSerialization* se = 0) const;
  XML_ERROR Save(const wchar_t* f, XMLSerialization* se = 0) const;

  // Loaders
  XML_PARSE ParseFile(FILE* fp);
  XML_PARSE Load(const char* f);

  XML_PARSE Load(const wchar_t* f);
  XML_PARSE Parse(const char* m, size_t len);

  // Gets
  XMLElement& GetRootElement();
  XMLHeader&  GetHeader();
  size_t      MemoryUsage();

  // Sets
  void SetRootElement(XMLElement& newroot);
  void SetHeader(const XMLHeader& h);

  void Clear();
  void Version(XML_VERSION_INFO* x);

  // ser
  string Serialize(XMLSerialization* srz = 0) const;
};

// Debugging class for profiling
#ifdef _MSC_VER
class TICKCOUNT {
  public:
  unsigned long long var;
  unsigned long long a;
  unsigned long long freq = 0;
  string             fn;

  TICKCOUNT(const char* ffn) {
    var = 0;
    a   = 0;
    fn  = ffn;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&a);
  }

  ~TICKCOUNT() { r(); }

  void r() {
    unsigned long long b = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&b);
    var = (b - a);
    var *= 1000;
    var /= freq;
    char f[1000];
    sprintf_s(f, 1000, "%03llu %s\r\n", var, fn.c_str());
    OutputDebugStringA(f);
  }
};
#else
#define _strnicmp strncasecmp

inline int _vscprintf(const char* format, va_list argptr) {
  return (vsnprintf(0, 0, format, argptr));
}

#endif

static unsigned long long jtic = 1;

inline unsigned long long Tick() {
  return ++jtic;
}

inline string Char2Base64(const char* Z, size_t s, bool CRLF) {
  if (!s) return "";
  DWORD da  = 0;
  DWORD flg = CRYPT_STRING_BASE64;
  ;
  if (!CRLF) flg |= CRYPT_STRING_NOCRLF;
  CryptBinaryToString((const BYTE*)Z, (DWORD)s, flg, 0, &da);
  da += 100;
  unique_ptr<char> out(new char[da]);
  CryptBinaryToStringA((const BYTE*)Z, (DWORD)s, flg, out.get(), &da);
  return out.get();
}

inline BXML::BXML(size_t s) {
  d.resize(s);
  clear();
}

inline bool BXML::operator==(const BXML& b2) {
  if (size() != b2.size()) return false;
  if (memcmp(p(), b2.p(), size()) != 0) return false;
  return true;
}

inline void BXML::ToB(string& s) {
  s = Char2Base64(p(), d.size());
}

inline void BXML::FromB(const char* ba) {
  Base64ToChar(ba, strlen(ba), *this);
}

inline BXML::operator const char*() const {
  return p();
}

inline BXML::operator char*() {
  return p();
}

inline const char* BXML::p() const {
  return &d[0];
}

inline char* BXML::p() {
  if (d.size() == 0) return 0;
  return &d[0];
}

inline size_t BXML::size() const {
  return d.size();
}

inline void BXML::clear() {
  if (d.size() == 0) return;
  memset(p(), 0, d.size());
}

inline void BXML::reset() {
  d.clear();
}

inline void BXML::Ensure(size_t news) {
  if (news > d.size()) Resize(news);
}

inline void BXML::Resize(size_t news) {
  d.resize(news);
}

inline void BXML::AddResize(size_t More) {
  d.resize(d.size() + More);
}

inline void Base64ToChar(const char* Z, size_t s, BXML& out) {
  DWORD dw = 0;
  CryptStringToBinaryA(Z, (DWORD)s, CRYPT_STRING_BASE64, 0, &dw, 0, 0);
  out.Resize(dw);
  CryptStringToBinaryA(Z, (DWORD)s, CRYPT_STRING_BASE64, (BYTE*)out.p(), &dw, 0, 0);
}

inline string Decode(const char* src) {
  string trg;
  if (!src) return trg;
  if (strchr(src, '&') == 0) return src;
  // vector<string> t = {"&amp;","&","&apos;","'","&quot;","\"","&gt;",">","&lt;","<"};
  vector<string> t = {"&amp;", "&", "&quot;", "\"", "&gt;", ">", "&lt;", "<"};
  std::string    s = src;
  for (size_t i = 0; i < t.size() / 2; i++) {
    std::regex re(t[i * 2].c_str());
    s = std::regex_replace(s, re, t[i * 2 + 1]);
  }
  return s;
}

inline string Format(const char* f, ...) {
  va_list args;
  va_start(args, f);

  int len = _vscprintf(f, args) // _vscprintf doesn't count
            + 100;              // terminating '\0'
  if (len < 8192) len = 8192;

  std::unique_ptr<char> b(new char[len]);
#ifdef _MSC_VER
  vsprintf_s(b.get(), len, f, args);
#else
  vsprintf(b.get(), f, args);
#endif
  string a = b.get();
  va_end(args);
  return a;
}

inline std::string& ltrim(std::string& str) {
  auto it2 = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
  str.erase(str.begin(), it2);
  return str;
}

inline std::string& rtrim(std::string& str) {
  auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
  str.erase(it1.base(), str.end());
  return str;
}

inline string Trim(const char* src, int m) {
  string s = src;
  if (m == 0) return ltrim(s);
  if (m == 1) return rtrim(s);
  return ltrim(rtrim(s));
  /*	if (m == 0 || m == 2)
                  s.erase(s.begin(),std::find_if(s.begin(),s.end(),std::not1(std::ptr_fun<int,int>(std::isspace))));
          if (m == 1 || m == 2)
                  s.erase(std::find_if(s.rbegin(),s.rend(),std::not1(std::ptr_fun<int,int>(std::isspace))).base(),s.end());
          return s;
          */
}

inline string Encode(const char* src) {
  string trg;
  if (!src) return trg;
  size_t Y = strlen(src);
  for (size_t i = 0; i < Y; i++) {
    if (src[i] == '&' && src[i + 1] != '#') {
      trg += "&amp;";
      continue;
    }
    if (src[i] == '>') {
      trg += "&gt;";
      continue;
    }
    if (src[i] == '<') {
      trg += "&lt;";
      continue;
    }
    if (src[i] == '\"') {
      trg += "&quot;";
      continue;
    }
    /*		if (src[i] == '\'')
                            {
                            trg += "&apos;";
                            continue;
                            }
                            */
    trg += src[i];
  }
  return trg;
}

inline void strreplace(std::string& str, const std::string& from, const std::string& to) {
  if (from.empty()) return;
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
  }
}

// Not Fully working ...
inline void JsonParser(XMLElement* root, const char* txt) {
  XMLElement* sroot = root;
  // replace \" with &quot;
  std::string f = txt;
  strreplace(f, "\\\"", "___quot___");
  txt       = f.c_str();
  size_t ts = strlen(txt);

  string LastData;

  //	XMLElement* FirstRoot = root;

  int KeyTypeParse = 0;
  // 0 Begin
  // 1 End
  // 2 : found, begin new
  // 3 Variable Name
  // 4 Variable Value

  XMLVariable cv;
  bool        NextBraceArray = false;

  for (size_t j = 0; j < ts; j++) {
    char c = txt[j];
    if (c == ' ' || c == '\r' || c == '\n' || c == '\t') continue;

    if (c == '{') {
      // Begin XMLElement
      if (LastData.empty()) LastData = "json";

      root = &root->AddElement(LastData.c_str());

      continue;
    }
    if (c == '}') {
      // End XMLElement
      XMLElement* nr = root->GetParent(sroot);
      if (!nr) // End JSon parsing
        break;
      unsigned long long lp = root->GetElementParam();
      if (lp != 1) {
        root = nr;
      } else {
        // Same, we must add another one with same name
        LastData = root->GetElementName();
        root     = nr;
      }
      continue;
    }
    if (c == ']') {
      // End array
      XMLElement* nr = root->GetParent(sroot);
      if (!nr) // End JSon parsing
        break;
      root = nr;
      continue;
    }
    if (c == '\"') {
      if (KeyTypeParse == 0) {
        // Start key
        LastData.clear();
        KeyTypeParse = 1;
        continue;
      }
      if (KeyTypeParse == 1) {
        // End Key
        KeyTypeParse = 2;
        continue;
      }
    }
    if (c == ':' && KeyTypeParse == 2) {
      // It's something new

      //			int BreakType = 0;
      bool InQ       = false;
      bool FinishedQ = false;
      for (size_t jj = j + 1; jj < ts; jj++) {
        char cc = txt[jj];
        if (cc == ' ' || cc == '\r' || cc == '\n' || cc == '\t') continue;

        if (cc == '\"') {
          InQ = !InQ;
          if (InQ == false) FinishedQ = true;
          continue;
        }
        if (InQ) continue;

        if (cc == '[') {
          // Array
          NextBraceArray = true;
          continue;
        }

        if (cc == '{') {
          // Element
          XMLElement& bb = root->AddElement(LastData.c_str());
          if (NextBraceArray) bb.SetElementParam(1);
          NextBraceArray = false;
          root           = &bb;
          KeyTypeParse   = 0;
          j              = jj;
          break;
        }

        if (cc == ',' || FinishedQ) {
          // Variable
          cv.SetName(LastData.c_str());

          // From j + 1 to jj, value
          string vval;
          for (size_t k = j + 1; k < jj; k++) {
            char ccc = txt[k];
            if (ccc == ' ' || ccc == '\r' || ccc == '\n' || ccc == '\t') continue;
            if (ccc == '\"') continue;
            vval.append(txt + k, 1);
          }
          cv.SetValue(vval);

          KeyTypeParse = 0;
          j            = jj;
          if (cc != ',') j = jj - 1;
          FinishedQ = false;

          root->AddVariable(cv);
          break;
        }
      }
      continue;
    }
    if (KeyTypeParse == 1) {
      // In key
      LastData.append(txt + j, 1);

      continue;
    }
  }
}

inline XMLU::XMLU(const char* x) {
  tu                     = x;
  size_t              si = strlen(x) * 4 + 1000;
  unique_ptr<wchar_t> ws(new wchar_t[si]);
#ifdef _WIN32
  MultiByteToWideChar(CP_UTF8, 0, tu.c_str(), -1, ws.get(), (int)si);
#endif
  wu = ws.get();
}

inline XMLU::XMLU(const wchar_t* x) {
  wu                  = x;
  size_t           si = wcslen(x) * 4 + 1000;
  unique_ptr<char> ws(new char[si]);
#ifdef _WIN32
  WideCharToMultiByte(CP_UTF8, 0, wu.c_str(), -1, ws.get(), (int)si, 0, 0);
#endif
  tu = ws.get();
}

inline const wchar_t* XMLU::wc() {
  return wu.c_str();
}

inline const char* XMLU::bc() {
  return tu.c_str();
}

inline XMLU::operator const wchar_t*() {
  return wu.c_str();
}

inline XMLU::operator const char*() {
  return tu.c_str();
}

inline XMLId::XMLId() {
  ts = Tick();
}

inline XMLId::XMLId(unsigned long long a) {
  operator=(a);
}

inline XMLId::XMLId(const XMLId& a) {
  operator=(a);
}

inline void XMLId::operator=(unsigned long long a) {
  ts = a;
}

inline bool XMLId::operator==(const XMLId& a) {
  if (ts == a.ts) return true;
  return false;
}

inline void XMLId::operator=(const XMLId& a) {
  ts = a.g();
}

inline unsigned long long XMLId::g() const {
  return ts;
}

inline XMLContent::XMLContent() {
  ep = (size_t)-1;
  v.clear();
}

inline XMLContent::XMLContent(size_t ElementPosition, const char* ht) {
  ep = ElementPosition;
  v.clear();
  if (ht) v = ht;
}

inline XMLContent::XMLContent(size_t ElementPosition, const wchar_t* ht) {
  ep = ElementPosition;
  v.clear();
  if (ht) v = XMLU(ht);
}

inline size_t XMLContent::MemoryUsage() {
  size_t m = 0;
  // Our size
  m += sizeof(*this);
  m += v.length();
  m += sizeof(v);
  return m;
}

// Operators
inline bool XMLContent::operator==(const XMLContent& t) const {
  if (v != t.v) return false;
  return true;
}

inline XMLContent::operator const string&() const {
  return v;
}

// Sets
inline const string& XMLContent::SetFormattedValue(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  unique_ptr<char> data(new char[10000]);
#ifdef _MSC_VER
  vsprintf_s(data.get(), 10000, fmt, args);
#else
  vsprintf(data.get(), fmt, args);
#endif
  SetValue(data.get());
  va_end(args);
  return v;
}

inline XMLContent& XMLContent::operator=(const char* s) {
  if (s)
    v = s;
  else
    v.clear();
  return *this;
}

inline XMLContent& XMLContent::operator=(const string& s) {
  v = s;
  return *this;
}

inline const string& XMLContent::SetBinaryValue(const char* data, size_t len) {
  if (!len || !data) return v;
  v = Char2Base64(data, len);
  return v;
}

inline void XMLContent::SetValue(const char* VV) {
  if (!VV) {
    v.clear();
    return;
  }
  v = VV;
}

inline void XMLContent::SetWideValue(const wchar_t* VV) {
  if (!VV) {
    v.clear();
    return;
  }
  v = XMLU(VV);
}

inline void XMLContent::SetValue(const string& VV) {
  v = VV;
}

inline void XMLContent::SetValue(const wstring& VV) {
  v = XMLU(VV.c_str());
}

inline void XMLContent::Clear() {
  v.clear();
}

inline void XMLContent::SetEP(size_t epp) {
  ep = epp;
}

// Gets
inline BXML XMLContent::GetBinaryValue() const {
  size_t s           = v.length();
  size_t output_size = ((s * 4) / 3) + (s / 96) + 6 + 1000;
  BXML   b;
  if (s == 0) return b;
  b.Resize(output_size);
  Base64ToChar(v.c_str(), s, b);
  return b;
}

inline const string& XMLContent::GetValue() const {
  return v;
}

inline wstring XMLContent::GetWideValue(int CP) const {
  const char*     s  = v.c_str();
  int             sz = MultiByteToWideChar(CP, 0, s, -1, 0, 0);
  vector<wchar_t> d(sz + 100);
  MultiByteToWideChar(CP, 0, s, -1, d.data(), sz + 100);
  return d.data();
}

inline string XMLContent::GetValueDefault(const char* s) const {
  if (v.length()) return v;
  return s;
}

inline size_t XMLContent::GetEP() const {
  return ep;
}

inline std::string XMLContent::trim(const std::string& str, const std::string& whitespace) {
  const auto strBegin = str.find_first_not_of(whitespace);
  if (strBegin == std::string::npos) return ""; // no content

  const auto strEnd   = str.find_last_not_of(whitespace);
  const auto strRange = strEnd - strBegin + 1;

  string res = str.substr(strBegin, strRange);
  return res;
}

// Serialization
inline string XMLContent::Serialize(XMLSerialization* srz) const {
  XMLSerialization defsrz;
  if (!srz) srz = &defsrz;

  if (srz->NoEnc) return Format("%s", v.c_str());
  return Format("%s", Encode(v.c_str()).c_str());
}

inline XMLVariable::XMLVariable(): XMLContent((size_t)-1, "") {
  n = "v";
}

inline XMLVariable::XMLVariable(const char* nn, const char* vv, bool Temp): XMLContent((size_t)-1, vv) {
  if (nn) n = nn;
  tmp = Temp;
}

/*
        XMLVariable::XMLVariable(const XMLVariable& h) : XMLContent((size_t)-1,(const char*)0)
                {
                operator =(h);
                }
        XMLVariable& XMLVariable :: operator =(const XMLVariable& h)
                {
                Clear();
                tmp = h.tmp;
                v = h.v;
                n = h.n;
                return *this;
                }
*/
/*
        XMLVariable& XMLVariable :: operator =(const std::initializer_list<string>& s)
                {
                if (s.size() == 1)
                        v = s.begin()->c_str();
                if (s.size() == 2)
                        {
                        n = s.begin()->c_str();
                        v = (s.begin() + 1)->c_str();
                        }
                return *this;
                }

*/
inline const string& XMLVariable::SetName(const char* VN) {
  if (!VN) return n;
  n = VN;
  n.erase(n.find_last_not_of(" \n\r\t") + 1);
  return n;
}

inline const string& XMLVariable::SetName(const string& VN) {
  n = VN;
  n.erase(n.find_last_not_of(" \n\r\t") + 1);
  return n;
}

inline void XMLVariable::Clear() {
  n.clear();
  v.clear();
}

inline XMLVariable& XMLVariable::operator=(const char* s) {
  if (s)
    v = s;
  else
    v.clear();
  return *this;
}

inline XMLVariable& XMLVariable::operator=(const string& s) {
  v = s;
  return *this;
}

inline XMLVariable& XMLVariable::operator=(unsigned long long j) {
  char jh[100] = {0};
  sprintf_s(jh, 100, "%llu", j);
  v = jh;
  return *this;
}

inline XMLVariable& XMLVariable::operator=(signed long long j) {
  char jh[100] = {0};
  sprintf_s(jh, 100, "%lli", j);
  v = jh;
  return *this;
}

// Compare
inline bool XMLVariable::operator<(const XMLVariable& x) const {
  if (n > x.n) return false;
  return true;
}

inline bool XMLVariable::operator==(const XMLVariable& x) const {
  if (strcmp(n.c_str(), x.n.c_str()) != 0) return false;
  if (strcmp(v.c_str(), x.v.c_str()) != 0) return false;
  return true;
}

inline bool XMLVariable::operator==(const char* x) const {
  if (strcmp(v.c_str(), x) != 0) return false;
  return true;
}

// Gets
inline const string& XMLVariable::GetName() const {
  return n;
}

inline bool XMLVariable::IsNS() const {
  if (strchr(n.c_str(), ':')) return true;
  return false;
}

inline string XMLVariable::GetNS() const {
  return ""; //*
}

// Memory usage
inline size_t XMLVariable::MemoryUsage() const {
  size_t m = 0;

  // Our size
  m += sizeof(*this);

  // Variable size
  m += n.length() + v.length();

  return m;
}

// Serialization
inline string XMLVariable::Serialize(XMLSerialization* srz) const {
  XMLSerialization defsrz;
  if (!srz) srz = &defsrz;
  if (srz->NoEnc) return Format("%s=\"%s\"", n.c_str(), v.c_str());
  return Format("%s=\"%s\"", Encode(n.c_str()).c_str(), Encode(v.c_str()).c_str());
}

inline XMLCData::XMLCData(size_t ElementPosition, const char* ht): XMLContent(ElementPosition, ht) {}

inline XMLCData::XMLCData(size_t ElementPosition, const wchar_t* ht): XMLContent(ElementPosition, ht) {}

// Serialization
inline string XMLCData::Serialize(XMLSerialization* srz) const {
  XMLSerialization defsrz;
  if (!srz) srz = &defsrz;
  return Format("<![CDATA[%s]]>", v.c_str());
}

inline XMLDocType::XMLDocType(const char* ht): XMLContent(0, ht) {}

inline XMLDocType::XMLDocType(const wchar_t* ht): XMLContent(0, ht) {}

// Serialization
inline string XMLDocType::Serialize(XMLSerialization* srz) const {
  XMLSerialization defsrz;
  if (!srz) srz = &defsrz;
  return Format("<!DOCTYPE %s>", v.c_str());
}

inline XMLComment::XMLComment(size_t ElementPosition, const char* ht): XMLContent(ElementPosition, ht) {}

inline XMLComment::XMLComment(size_t ElementPosition, const wchar_t* ht): XMLContent(ElementPosition, ht) {}

// Serialization
inline string XMLComment::Serialize(XMLSerialization* srz) const {
  XMLSerialization defsrz;
  if (!srz) srz = &defsrz;
  if (srz->Canonical) return Format("<!-- %s -->", trim(v).c_str());
  return Format("<!--%s-->", v.c_str());
}

// Serialization
inline string XMLHeader::Serialize() const {
  return Format("<?xml version=\"%s\" encoding=\"%s\" standalone=\"%s\" ?>", version.GetValue().c_str(), encoding.GetValue().c_str(),
                standalone.GetValue().c_str());
}

inline void XMLHeader::Default() {
  version.SetName("version");
  encoding.SetName("encoding");
  standalone.SetName("standalone");

  version    = "1.0";
  encoding   = "UTF-8";
  standalone = "yes";
}

inline XMLHeader::XMLHeader() {
  Default();
}

inline XMLElement::XMLElement() {
  children.reserve(100);
  variables.reserve(20);
}

// Operators
inline bool XMLElement::operator==(const XMLElement& t) const {
  if (param != t.param) return false;
  if (el != t.el) return false;

  if (children.size() != t.children.size()) return false;
  if (variables.size() != t.variables.size()) return false;
  if (contents.size() != t.contents.size()) return false;
  if (cdatas.size() != t.cdatas.size()) return false;
  if (comments.size() != t.comments.size()) return false;

  for (size_t i = 0; i < children.size(); i++) {
    auto a = children[i];
    auto b = t.children[i];
    if (!(a == b)) return false;
  }

  for (size_t i = 0; i < variables.size(); i++) {
    auto a = variables[i];
    auto b = t.variables[i];
    if (!(a == b)) return false;
  }

  for (size_t i = 0; i < contents.size(); i++) {
    auto a = contents[i];
    auto b = t.contents[i];
    if (!(a == b)) return false;
  }

  for (size_t i = 0; i < comments.size(); i++) {
    auto a = comments[i];
    auto b = t.comments[i];
    if (!(a == b)) return false;
  }

  for (size_t i = 0; i < cdatas.size(); i++) {
    auto a = cdatas[i];
    auto b = t.cdatas[i];
    if (!(a == b)) return false;
  }

  return true;
}

inline bool XMLElement::operator<(const XMLElement& x) const {
  // Compare names
  if (el > x.el) return false;
  return true;
}

inline XMLElement& XMLElement ::operator=(const XMLElement& t) {
  operator=(t.Serialize().c_str());
  return *this;
}

inline XMLElement& XMLElement::operator=(const char* xx) {
  if (!xx) {
    clear();
    return *this;
  }
  if (xx[0] != '<') {
    SetElementName(xx);
    return *this;
  }

  XML x(xx, strlen(xx));
  CloneMirror(*this, x.GetRootElement());
  return *this;
}

inline XMLElement XMLElement::Mirror() const {
  XMLElement t;
  CloneMirror(t, *this);
  return t;
}

inline XMLElement::XMLElement(const char* t) {
  operator=(t);
}

inline void XMLElement::CloneMirror(XMLElement& to, const XMLElement& from) {
  to.children  = from.children;
  to.variables = from.variables;
  to.comments  = from.comments;
  to.contents  = from.contents;
  to.cdatas    = from.cdatas;
  to.el        = from.el;
  to.param     = from.param;
  to.parent    = from.parent;
  to.id        = from.id;
}

inline XMLElement::XMLElement(const XMLElement& from) {
  el    = from.el;
  param = from.param;

  // cdatas
  cdatas.reserve(from.cdatas.size());
  for (auto a: from.cdatas) {
    shared_ptr<XMLCData> c = make_shared<XMLCData>(XMLCData(a->ep, a->v.c_str()));
    cdatas.push_back(c);
  }

  // comments
  comments.reserve(from.comments.size());
  for (auto a: from.comments) {
    shared_ptr<XMLComment> c = make_shared<XMLComment>(XMLComment(a->ep, a->v.c_str()));
    comments.push_back(c);
  }

  // contents
  contents.reserve(from.contents.size());
  for (auto a: from.contents) {
    shared_ptr<XMLContent> c = make_shared<XMLContent>(XMLContent(a->ep, a->v.c_str()));
    contents.push_back(c);
  }

  // vars
  variables.reserve(from.variables.size());
  for (auto a: from.variables) {
    shared_ptr<XMLVariable> c = make_shared<XMLVariable>(XMLVariable(a->n.c_str(), a->v.c_str()));
    variables.push_back(c);
  }

  // children
  children.reserve(from.children.size());
  for (auto a: from.children) {
    shared_ptr<XMLElement> c = make_shared<XMLElement>(XMLElement());
    c->operator=((XMLElement&)*a.get());
    children.push_back(c);
  }

  //			operator=(from.Serialize().c_str());
}

inline XMLElement::XMLElement(XMLElement&& from) {
  el        = from.el;
  param     = from.param;
  cdatas    = from.cdatas;
  comments  = from.comments;
  contents  = from.contents;
  variables = from.variables;
  children  = from.children;
}

// Gets
inline const XMLElement& XMLElement::operator[](size_t idx) const {
  if (idx >= children.size()) throw XML_ERROR::INVALIDIDX;
  return *children[idx];
}

inline XMLElement& XMLElement::operator[](size_t idx) {
  if (idx >= children.size()) throw XML_ERROR::INVALIDIDX;
  return *children[idx];
}

inline XMLElement& XMLElement::operator[](const char* elm) {
  if (elm == 0) {
    if (children.size() == 0) throw XML_ERROR::INVALIDIDX;
    return *children[0];
  }
  shared_ptr<XMLElement> e = FindElementZ(elm, true);
  return *e;
}

inline string XMLElement ::vd(const char* nn, const char* def) {
  string k;
  if (!nn) {
    if (variables.size() == 0) return (def ? def : "");
    k = variables[0]->GetValue();
  } else {
    shared_ptr<XMLVariable> v = FindVariableZ(nn, true);
    k                         = v->GetValue();
  }
  if (k.empty() && def) k = def;
  return k;
}

inline string XMLElement::vd(const char* nn, const char* def) const {
  string k;
  if (!nn) {
    if (variables.size() == 0) return (def ? def : "");
    k = variables[0]->GetValue();
  } else {
    shared_ptr<XMLVariable> v = FindVariable(nn);
    if (v) k = v->GetValue();
  }
  if (k.empty() && def) k = def;
  return k;
}

inline const string& XMLElement::v(size_t idx) const {
  if (idx >= variables.size()) throw XML_ERROR::INVALIDIDX;
  return variables[idx]->GetValue();
}

inline const string& XMLElement::v(const char* nn) {
  if (!nn) {
    if (variables.size() == 0) throw XML_ERROR::INVALIDIDX;
    return variables[0]->GetValue();
  }
  shared_ptr<XMLVariable> v = FindVariableZ(nn, true);
  return v->GetValue();
}

inline wstring XMLElement::wv(const char* nn) {
  if (!nn) {
    if (variables.size() == 0) throw XML_ERROR::INVALIDIDX;
    return variables[0]->GetWideValue();
  }
  shared_ptr<XMLVariable> v = FindVariableZ(nn, true);
  return v->GetWideValue();
}

inline string XMLElement::Content() const {
  if (contents.empty()) return "";
  return contents[0]->GetValue().c_str();
}

inline string XMLElement::GetContent() const {
  return Content();
}

inline XMLVariable& XMLElement::vv(const char* nn) {
  shared_ptr<XMLVariable> v = FindVariableZ(nn, true);
  return *v;
}

inline unsigned long long XMLElement::GetElementParam() const {
  return param;
}

inline const string& XMLElement::GetElementName() const {
  return el;
}

inline void XMLElement::GetAllChildren(vector<shared_ptr<XMLElement>>& ch) const {
  for (auto& a: children) {
    ch.push_back(a);
    a->GetAllChildren(ch);
  }
}

/*	inline shared_ptr<XMLElement> XMLElement::GetParent(shared_ptr<XMLElement> r) const
                {
                if (r->par)
                        return r->par;
                if (r->id == parent)
                        return r;
                for (auto a : r->children)
                        {
                        if (a->id == parent)
                                return a;
                        }
                return 0;
                }
*/

inline XMLElement* XMLElement::GetParent(XMLElement* r) const {
  if (par) return par;
  if (r->id == parent) return r;
  vector<shared_ptr<XMLElement>> ch;
  r->GetAllChildren(ch);
  for (auto a: ch) {
    if (a->id == parent) return a.get();
  }
  return 0;
}

inline size_t XMLElement::GetElementIndex(const XMLElement& e) const {
  for (size_t i = 0; i < children.size(); i++) {
    auto& exl = children[i];
    if (exl.get() == &e) return i;
  }
  return (size_t)-1;
}

// Sets
inline void XMLElement::SetElementName(const char* x) {
  el.clear();
  if (x) el = x;
}

inline void XMLElement::SetElementName(const wchar_t* x) {
  el.clear();
  if (!x) return;
  XMLU wh(x);
  el = wh;
}

inline XMLContent& XMLElement::SetContent(const char* vp) {
  if (contents.size() == 0)
    return AddContent(vp, 0);
  else {
    GetContents()[0]->SetValue(vp);
    return *GetContents()[0];
  }
}

inline void XMLElement::SetElementParam(unsigned long long p) {
  param = p;
}

inline XMLVariable& XMLElement::SetValue(const char* vn, const char* vp) {
  auto& a = vv(vn);
  a       = vp;
  return a;
}

inline XML_ERROR XMLElement::MoveElement(size_t i, size_t y) {
  if (i >= children.size() || y >= children.size()) return XML_ERROR::INVALIDARG;
  shared_ptr<XMLElement> x = children[i];
  children.erase(children.begin() + i);
  children.insert(children.begin() + y, x);
  return XML_ERROR::OK;
}

// Find
inline shared_ptr<XMLElement> XMLElement::FindElementZ(const char* n, bool ForceCreate) {
  if (!n) return 0;
  for (size_t i = 0; i < children.size(); i++) {
    shared_ptr<XMLElement>& cc = children[i];
    const string&           cn = cc->GetElementName();
    if (strcmp(cn.c_str(), n) == 0) return cc;
  }
  if (ForceCreate == 0) return 0;
  XMLElement& vv = AddElement();
  vv.SetElementName(n);
  return FindElementZ(vv.GetElementName().c_str());
}

inline shared_ptr<XMLVariable> XMLElement::FindVariable(const char* n) const {
  if (!n) return 0;
  for (size_t i = 0; i < variables.size(); i++) {
    const shared_ptr<XMLVariable>& cc = variables[i];
    const string&                  cn = cc->GetName();
    if (strcmp(cn.c_str(), n) == 0) return cc;
  }
  return 0;
}

inline shared_ptr<XMLVariable> XMLElement::FindVariableZ(const char* n, bool ForceCreate, const char* defv, size_t* pidx) {
  if (!n) return 0;
  for (size_t i = 0; i < variables.size(); i++) {
    shared_ptr<XMLVariable>& cc = variables[i];
    const string&            cn = cc->GetName();
    if (strcmp(cn.c_str(), n) == 0) {
      if (pidx) *pidx = i;
      return cc;
    }
  }
  if (ForceCreate == 0) return 0;
  XMLVariable& vv = AddVariable(n, defv);
  return FindVariableZ(vv.GetName().c_str());
}

// Inserts
inline shared_ptr<XMLElement> XMLElement::InsertElement(size_t y, const XMLElement& x) {
  if (y >= children.size()) y = children.size();
  shared_ptr<XMLElement> xx = make_shared<XMLElement>(XMLElement(x));
  children.insert(children.begin() + y, xx);
  children[y]->parent = id;
  children[y]->par    = this;
  return children[y];
}

inline shared_ptr<XMLElement> XMLElement::InsertElement(size_t y, XMLElement&& x) {
  if (y >= children.size()) y = children.size();
  shared_ptr<XMLElement> xx = make_shared<XMLElement>(XMLElement(std::forward<XMLElement>(x)));
  children.insert(children.begin() + y, xx);
  children[y]->parent = id;
  children[y]->par    = this;
  return children[y];
}

inline XMLElement& XMLElement::AddElement(const XMLElement& c) {
  return *InsertElement((size_t)-1, c);
}

inline XMLElement& XMLElement::AddElement(XMLElement&& c) {
  return *InsertElement((size_t)-1, std::forward<XMLElement>(c));
}

inline XMLElement& XMLElement::AddElement(const char* n) {
  XMLElement c = n;
  return *InsertElement((size_t)-1, std::forward<XMLElement>(c));
}

inline shared_ptr<XMLElement> XMLElement::AddElement2(const char* n) {
  XMLElement c = n;
  return InsertElement((size_t)-1, std::forward<XMLElement>(c));
}

inline void XMLElement::AddElements(const std::initializer_list<string>& s) {
  for (auto& a: s) {
    XMLElement c = a.c_str();
    InsertElement((size_t)-1, c);
  }
}

inline void XMLElement::SetVariables(const std::initializer_list<string>& s) {
  if (s.size() % 2) return;
  for (size_t i = 0; i < s.size(); i += 2) {
    auto a         = s.begin() + i;
    auto b         = s.begin() + i + 1;
    vv(a->c_str()) = b->c_str();
  }
}

inline XMLVariable& XMLElement::AddVariable(const XMLVariable& vv, size_t p) {
  if (p == (size_t)-1) p = (size_t)variables.size();
  shared_ptr<XMLVariable> v = make_shared<XMLVariable>(XMLVariable(vv));
  variables.insert(variables.begin() + p, v);
  return *variables[p];
}

inline XMLVariable& XMLElement::AddVariable(const char* vn, const char* vv, size_t p) {
  if (p == (size_t)-1) p = (size_t)variables.size();
  shared_ptr<XMLVariable> v = make_shared<XMLVariable>(XMLVariable(vn, vv));
  variables.insert(variables.begin() + p, v);
  return *variables[p];
}

inline XMLContent& XMLElement::AddContent(const XMLContent& co, size_t pos) {
  if (pos == (size_t)-1) pos = (size_t)contents.size();
  shared_ptr<XMLContent> v = make_shared<XMLContent>(co);
  contents.insert(contents.begin() + pos, v);
  return *contents[pos];
}

inline XMLContent& XMLElement::AddContent(const char* pv, size_t ep, size_t p) {
  if (p == (size_t)-1) p = (size_t)contents.size();
  shared_ptr<XMLContent> v = make_shared<XMLContent>(XMLContent(ep, pv));
  contents.insert(contents.begin() + p, v);
  return *contents[p];
}

inline XMLComment& XMLElement::AddComment(const char* pv, size_t ep, size_t p) {
  if (p == (size_t)-1) p = (size_t)comments.size();
  shared_ptr<XMLComment> v = make_shared<XMLComment>(XMLComment(ep, pv));
  comments.insert(comments.begin() + p, v);
  return *comments[p];
}

inline XMLCData& XMLElement::AddCData(const char* pv, size_t ep, size_t p) {
  if (p == (size_t)-1) p = (size_t)cdatas.size();
  shared_ptr<XMLCData> v = make_shared<XMLCData>(XMLCData(ep, pv));
  cdatas.insert(cdatas.begin() + p, v);
  return *cdatas[p];
}

// Removals
inline size_t XMLElement::RemoveAllElements() {
  children.clear();
  return 0;
}

inline size_t XMLElement::RemoveElement(size_t i) {
  if (i >= children.size()) return (size_t)-1;
  children.erase(children.begin() + i);
  return children.size();
}

inline size_t XMLElement::RemoveElementByName(const char* n) {
  for (size_t i = 0; i < children.size(); i++) {
    if (strcmp(children[i]->GetElementName().c_str(), n) == 0) {
      children.erase(children.begin() + i);
      return i;
    }
  }
  return (size_t)-1;
}

inline void XMLElement::RemoveDuplicateNamespaces(const char* vn) {
  if (vn) {
    RemoveVariableZ(vn);
    for (auto& ch: children)
      ch->RemoveDuplicateNamespaces(vn);
    return;
  }

  for (size_t i = 0; i < variables.size(); i++) {
    shared_ptr<XMLVariable>& cc = variables[i];
    if (cc->IsNS()) {
      for (auto& ch: children)
        ch->RemoveDuplicateNamespaces(cc->GetName().c_str());
    }
  }

  for (auto& ch: children)
    ch->RemoveDuplicateNamespaces(0);
}

inline size_t XMLElement::RemoveElement(XMLElement* p) {
  for (size_t i = 0; i < children.size(); i++) {
    if (children[i].get() == p) {
      children.erase(children.begin() + i);
      break;
    }
  }
  return children.size();
}

inline shared_ptr<XMLElement> XMLElement::RemoveElementAndKeep(size_t i) {
  if (i >= children.size()) throw XML_ERROR::INVALIDARG;
  auto X = children[i];
  RemoveElement(i);
  return X;
}

inline void XMLElement::clear() {
  variables.clear();
  children.clear();
  cdatas.clear();
  comments.clear();
  contents.clear();
  param = 0;
  el    = "e";
}

// Variables
inline size_t XMLElement::RemoveAllVariables() {
  variables.clear();
  return 0;
}

inline size_t XMLElement::RemoveVariable(size_t i) {
  if (i >= variables.size()) return variables.size();
  variables.erase(variables.begin() + i);
  return variables.size();
}

inline bool XMLElement::RemoveVariableZ(const char* n) {
  size_t idx = 0;
  auto   v   = FindVariableZ(n, false, "", &idx);
  if (!v) return false;
  RemoveVariable(idx);
  return true;
}

inline shared_ptr<XMLVariable> XMLElement::RemoveVariableAndKeep(size_t i) {
  if (i >= variables.size()) throw XML_ERROR::INVALIDARG;
  auto v = variables[i];
  variables.erase(variables.begin() + i);
  return v;
}

inline string XMLElement::EorE(const string& s, bool N) const {
  if (N) return s;
  return Encode(s.c_str());
}

inline string XMLElement::Serialize(XMLSerialization* srz) const {
  string v;
  Serialize(v, srz);
  return v;
}

inline void XMLElement::Serialize(string& v, XMLSerialization* srz) const {
  XMLSerialization defsrz;
  if (!srz) srz = &defsrz;

  string padd;
  if (!srz->ExcludeSelf) {
    if (srz->Canonical) {
      /*	for (size_t i = 0; i < srz->deep; i++)
              {
                      padd += "   ";
              }*/
    } else if (srz->NoCRLF == false) {
      for (size_t i = 0; i < srz->deep; i++) {
        padd += "\t";
      }
    }

    // <n
    if (variables.empty() && children.empty() && comments.empty() && contents.empty() && cdatas.empty()) {
      if (srz->Canonical)
        v += Format("%s<%s></%s>", padd.c_str(), EorE(el, srz->NoEnc).c_str(), EorE(el, srz->NoEnc).c_str());
      else
        v += Format(srz->NoCRLF ? "%s<%s />" : "%s<%s />\r\n", padd.c_str(), EorE(el, srz->NoEnc).c_str());
      return;
    }

    v += Format("%s<%s", padd.c_str(), EorE(el, srz->NoEnc).c_str());
    if (!variables.empty()) {
      vector<shared_ptr<XMLVariable>> v2;
      if (srz->Canonical) {
        v2 = variables;
        std::sort(v2.begin(), v2.end(), [](std::shared_ptr<XML3::XMLVariable> e1, std::shared_ptr<XML3::XMLVariable> e2) -> bool {
          if (strncmp(e1->GetName().c_str(), "xmlns:", 6) == 0 && strncmp(e2->GetName().c_str(), "xmlns:", 6) != 0) return true;
          if (strncmp(e2->GetName().c_str(), "xmlns:", 6) == 0 && strncmp(e1->GetName().c_str(), "xmlns:", 6) != 0) return false;
          if (e1->GetName() < e2->GetName()) {
            return true;
          }
          return 0;
        });
      }
      const vector<shared_ptr<XMLVariable>>& v3 = srz->Canonical ? v2 : variables;

      for (auto a: v3) {
        v += " ";
        v += a->Serialize(srz);
      }
    }

    if (children.empty() && comments.empty() && contents.empty() && cdatas.empty() && !srz->Canonical) {
      v += srz->NoCRLF ? "/>" : "/>\r\n";
      return;
    }
    if (srz->Canonical)
      v += ">";
    else
      v += srz->NoCRLF ? ">" : ">\r\n";
  }

  auto ac = [&](vector<shared_ptr<XMLContent>>& cx, size_t B, size_t B2) {
    for (auto& a: contents) {
      if (a->GetEP() == B || a->GetEP() == B2) cx.push_back(a);
    }
    for (auto& a: comments) {
      if (a->GetEP() == B || a->GetEP() == B2) cx.push_back(a);
    }
    for (auto& a: cdatas) {
      if (a->GetEP() == B || a->GetEP() == B2) cx.push_back(a);
    }
  };

  // Stuff
  for (size_t i = 0; i < children.size(); i++) {
    auto& a = children[i];

    // Before items
    vector<shared_ptr<XMLContent>> cx;
    ac(cx, i, i);
    for (auto& n: cx) {
      string e = XMLContent::trim(n->Serialize(srz));
      if (srz->Canonical) {
        v += e;
        // v += "\n";
      } else
        v += Format(srz->NoCRLF ? "%s%s" : "%s%s\r\n", padd.c_str(), e.c_str());
    }

    XMLSerialization s2 = *srz;
    s2.deep             = srz->deep + 1;
    a->Serialize(v, &s2);
  }

  // After items
  vector<shared_ptr<XMLContent>> cx;
  ac(cx, (size_t)-1, children.size());
  for (auto& n: cx) {
    string e = XMLContent::trim(n->Serialize(srz));
    if (srz->Canonical) {
      v += e;
      //	v += "\n";
    } else
      v += Format(srz->NoCRLF ? "%s%s" : "%s%s\r\n", padd.c_str(), e.c_str());
  }

  if (srz->Canonical)
    v += Format("%s</%s>", padd.c_str(), EorE(el, srz->NoEnc).c_str());
  else {
    if (!srz->ExcludeSelf) v += Format(srz->NoCRLF ? "%s</%s>" : "%s</%s>\r\n", padd.c_str(), EorE(el, srz->NoEnc).c_str());
  }
  return;
}

// Constructors
inline XML::XML() {}

inline XML::XML(const char* file) {
  UnicodeFile = false;
  fname       = file;
  Load(file);
}

inline XML::XML(const wchar_t* file) {
  UnicodeFile = true;
  fname       = XMLU(file);
  Load(file);
}

inline XML::XML(const char* mem, size_t l) {
  UnicodeFile = false;
  Parse(mem, l);
}

inline void XML::operator=(const char* d) {
  UnicodeFile = false;
  Parse(d, strlen(d));
}

inline XML::XML(const XML& x) {
  operator=(x);
}

inline void XML::operator=(const XML& x) {
  UnicodeFile = x.UnicodeFile;
  fname       = x.fname;
  hdr         = x.hdr;
  doctype     = x.doctype;

  // comments
  hdrcomments.reserve(x.hdrcomments.size());
  for (auto a: x.hdrcomments) {
    shared_ptr<XMLComment> c = make_shared<XMLComment>(XMLComment(a->ep, a->v.c_str()));
    hdrcomments.push_back(c);
  }

  // Root
  root = x.root;
}

inline XML::XML(XML&& x) {
  operator=(std::forward<XML>(x));
}

inline void XML::operator=(XML&& x) {
  UnicodeFile = x.UnicodeFile;
  fname       = x.fname;
  hdr         = x.hdr;
  doctype     = x.doctype;
  hdrcomments = x.hdrcomments;
  root        = x.GetRootElement().Mirror();
}

// Savers
inline size_t XML::SaveFP(FILE* fp) const {
  string s  = Serialize();
  size_t sz = fwrite(s.c_str(), 1, s.length(), fp);
  fclose(fp);
  return sz;
}

inline XML_ERROR XML::Save() const {
  if (UnicodeFile) return Save(XMLU(fname.c_str()).operator const wchar_t*());
  return Save(fname.c_str());
}

inline XML_ERROR XML::Save(const char* f, XMLSerialization* se) const {
  if (!f) f = fname.c_str();
  FILE* fp = 0;
  fopen_s(&fp, f, "wb");
  if (!fp) return XML_ERROR::SAVEFAILED;
  string s = Serialize(se);
  if (hdr.GetEncoding() == "UTF-16") {
    fwrite("\xFE\xFF", 1, 3, fp);
    wstring s2 = XMLU(s.c_str()).operator const wchar_t*();
    size_t  sz = fwrite(s2.data(), 1, s.length() * 2, fp);
    fclose(fp);
    if (sz != s.length() * 2) return XML_ERROR::SAVEFAILED;
  } else {
    fwrite("\xEF\xBB\xBF", 1, 3, fp);
    size_t sz = fwrite(s.c_str(), 1, s.length(), fp);
    fclose(fp);
    if (sz != s.length()) return XML_ERROR::SAVEFAILED;
  }

  return XML_ERROR::OK;
}

inline XML_ERROR XML::Save(const wchar_t* f, XMLSerialization* se) const {
#ifdef _MSC_VER
  XMLU  wf(fname.c_str());
  FILE* fp = 0;
  _wfopen_s(&fp, f ? f : wf.operator const wchar_t*(), L"wb");
  if (!fp) return XML_ERROR::SAVEFAILED;
  string s = Serialize(se);
  if (hdr.GetEncoding() == "UTF-16") {
    fwrite("\xFE\xFF", 1, 3, fp);
    wstring s2 = XMLU(s.c_str()).operator const wchar_t*();
    size_t  sz = fwrite(s2.data(), 1, s.length() * 2, fp);
    fclose(fp);
    if (sz != s.length() * 2) return XML_ERROR::SAVEFAILED;
  } else {
    fwrite("\xEF\xBB\xBF", 1, 3, fp);
    size_t sz = fwrite(s.c_str(), 1, s.length(), fp);
    fclose(fp);
    if (sz != s.length()) return XML_ERROR::SAVEFAILED;
  }

  return XML_ERROR::OK;
#else
  return XML_ERROR::SAVEFAILED;
#endif
}

// Loaders
inline XML_PARSE XML::ParseFile(FILE* fp) {
  fseek(fp, 0, SEEK_END);
  size_t S = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  BXML b;
  b.Resize(S + 1);
  fread(b.p(), 1, S, fp);
  fclose(fp);

  // Check if this XML we loaded is a unicode XML
#ifdef _WIN32
  unsigned char a1 = b.p()[0];
  unsigned char a2 = b.p()[1];
  unsigned char a3 = b.p()[2];
  if (a1 == 0xFF && a2 == 0xFE) {
    BXML b2;
    b2.Resize(S * 4 + 32);
    WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)b.p(), (int)-1, b2.p(), (int)(S * 4 + 32), 0, 0);
    b = b2;
  }
  // UTF-8 BOM?
  // fwrite("\xEF\xBB\xBF",1,3,fp);
  //
  if (a1 == 0xEF && a2 == 0xBB && a3 == 0xBF) {
    BXML b2;
    b2.Resize(S);
    memcpy(b2.p(), b.p() + 3, S - 3);

    b = b2;
  }
#endif

  // b is the text
  const char* bb = (const char*)b.p();
  return Parse(bb, strlen(bb));
}

inline XML_PARSE XML::Load(const char* f) {
  Clear();
  FILE* fp = 0;
  fopen_s(&fp, f, "rb");
  if (!fp) return XML_PARSE::OPENFAILED;
  return ParseFile(fp);
}

inline XML_PARSE XML::Load(const wchar_t* f) {
#ifdef _MSC_VER
  Clear();
  FILE* fp = 0;
  _wfopen_s(&fp, f, L"rb");
  if (!fp) return XML_PARSE::OPENFAILED;
  return ParseFile(fp);
#else
  return XML_PARSE::OPENFAILED;
#endif
}

inline XML_PARSE XML::Parse(const char* m, size_t len) {
  Clear();
  //		TICKCOUNT c("Parse");
  hdrcomments.reserve(3);

  enum class PARSEELEMENT {
    STARTHDR,
    ENDHDR,
    STARTDOCTYPE,
    ENDDOCTYPE,
    STARTVAR,
    ENDVAR,
    STARTCOMMENT,
    ENDCOMMENT,
    STARTCD,
    ENDCD,
    STARTEL,
    ENDEL,
    ENDDOCUMENT,
    STARTQ,
    ENDQ,
  };

  int    InCData   = 0;
  int    InComment = 0;
  int    InHeader  = 0;
  int    InQ       = 0;
  string t1;
  t1.reserve(100000);
  XMLElement* l                  = 0;
  int         OutsideElementLine = 1;
  int         InVar              = 0; // 1 got =, 2 got ",
  int         InDocType          = 0;
  XMLVariable cv;
  XMLCData    cd;
  XMLComment  comm;
  bool        PendingElName        = false;
  bool        PendingElNameClosing = false;

  auto ParseFunc = [&](PARSEELEMENT pe) -> XML_PARSE {
    // TICKCOUNT c("ParseFunc");
    XML_PARSE e = XML_PARSE::OK;

#ifdef _DEBUG
    // string adsb = Serialize();
#endif

    if (pe == PARSEELEMENT::STARTHDR) {
      if (InHeader > 0) return XML_PARSE::HDRERROR;
      InHeader = 1;
    }
    if (pe == PARSEELEMENT::STARTEL) {
      if (!l)
        l = &root;
      else {
        //					XMLElement ne;
        XMLElement& ne2 = l->AddElement("el");
        l               = &ne2;
      }
      PendingElName = true;
    }
    if (pe == PARSEELEMENT::ENDEL) {
      if (l) {
        l = l->GetParent(&root);
      }
    }

    if (pe == PARSEELEMENT::ENDHDR) {
      if (InHeader != 1) return XML_PARSE::HDRERROR;
      InHeader = 2;
    }
    if (pe == PARSEELEMENT::STARTDOCTYPE) {
      if (InDocType > 0) return XML_PARSE::HDRERROR;
      InDocType = 1;
    }
    if (pe == PARSEELEMENT::ENDDOCTYPE) {
      if (InDocType != 1) return XML_PARSE::HDRERROR;
      InDocType = 2;
      doctype.SetValue(Decode(t1.c_str()).c_str());
      t1.clear();
    }
    if (pe == PARSEELEMENT::STARTCOMMENT) {
      if (InComment != 0) return XML_PARSE::COMMENTERROR;
      InComment = 1;
    }
    if (pe == PARSEELEMENT::STARTCD) {
      if (InCData != 0 || !l) return XML_PARSE::COMMENTERROR;
      InCData = 1;
    }

    if (pe == PARSEELEMENT::ENDCOMMENT) {
      if (InComment != 1) return XML_PARSE::COMMENTERROR;
      InComment = 0;
      if (l) comm.SetEP(l->GetChildren().size());
      comm.SetValue(Decode(t1.c_str()).c_str());
      t1.clear();
      if (l)
        l->AddComment(comm.GetValue().c_str(), comm.GetEP());
      else
        hdrcomments.push_back(make_shared<XMLComment>(XMLComment(comm)));
    }

    if (pe == PARSEELEMENT::ENDCD) {
      if (InCData != 1 || !l) return XML_PARSE::COMMENTERROR;
      InCData = 0;
      cd.SetEP(l->GetChildren().size());
      cd.SetValue(Decode(t1.c_str()).c_str());
      t1.clear();
      l->AddCData(cd.GetValue().c_str(), cd.GetEP());
    }
    // Header var
    if (pe == PARSEELEMENT::STARTVAR && InHeader == 1) {
      if (InVar != 0) return XML_PARSE::VARERROR;
      cv.Clear();
      InVar = 1;
      cv.SetName(Decode(t1.c_str()).c_str());
      t1.clear();
    }
    if (pe == PARSEELEMENT::ENDVAR && InHeader == 1) {
      if (InVar != 2) return XML_PARSE::VARERROR;
      InVar = 0;
      cv.SetValue(Decode(t1.c_str()).c_str());
      t1.clear();
      if (cv.GetName() == "version")
        hdr.GetVersion() = cv;
      else if (cv.GetName() == "encoding")
        hdr.GetEncoding() = cv;
      else if (cv.GetName() == "standalone")
        hdr.GetStandalone() = cv;
      else
        return e; // ignore it... return XML_PARSE::HDRERROR;
      cv.Clear();
    }

    // Element var
    if (pe == PARSEELEMENT::STARTVAR && l) {
      if (InVar != 0) return XML_PARSE::VARERROR;
      cv.Clear();
      InVar = 1;
      cv.SetName(Decode(t1.c_str()).c_str());
      t1.clear();
    }
    if (pe == PARSEELEMENT::ENDVAR && l) {
      if (InVar != 2) return XML_PARSE::VARERROR;
      InVar = 0;
      cv.SetValue(Decode(t1.c_str()).c_str());
      t1.clear();
      l->AddVariable(cv.GetName().c_str(), cv.GetValue().c_str());
      cv.Clear();
    }

    return e;
  };

  XML_PARSE e = XML_PARSE::OK;
  for (size_t i = 0; i < len;) {
    const char* mm      = m + i;
    bool        IsSpace = false;
    if (m[i] == ' ' || m[i] == '\t' || m[i] == '\r' || m[i] == '\n') IsSpace = true;

    // Opti CData
    if (InCData) {
      const char* s1 = strstr(mm, "]]>");
      if (!s1) {
        e = XML_PARSE::COMMENTERROR;
        break;
      }
      t1.assign(mm, s1 - mm);
      i += t1.length();
      e = ParseFunc(PARSEELEMENT::ENDCD);
      if (e != XML_PARSE::OK) break;
      i += 3;
      continue;
    }
    // Opti Comment
    if (InComment) {
      const char* s1 = strstr(mm, "-->");
      if (!s1) {
        e = XML_PARSE::COMMENTERROR;
        break;
      }
      t1.assign(mm, s1 - mm);
      i += t1.length();
      e = ParseFunc(PARSEELEMENT::ENDCOMMENT);
      if (e != XML_PARSE::OK) break;
      i += 3;
      continue;
    }

    // El name?
    if (l && PendingElName && IsSpace && !t1.empty()) {
      l->SetElementName(Trim(t1.c_str()).c_str());
      PendingElName = false;
      t1.clear();
      i++;
      continue;
    }

    // Header test
    bool CanHeader = false;
    if (!InComment && !InCData && l == 0) CanHeader = true;
    if (CanHeader) {
      if (_strnicmp(mm, "<?xml ", 6) == 0) {
        e = ParseFunc(PARSEELEMENT::STARTHDR);
        if (e != XML_PARSE::OK) break;
        i += 6;
        continue;
      }
      if (_strnicmp(mm, "<?xml?>", 7) == 0) {
        e = ParseFunc(PARSEELEMENT::STARTHDR);
        if (e != XML_PARSE::OK) break;
        i += 5;
        continue;
      }
      if (_strnicmp(mm, "?>", 2) == 0) {
        e = ParseFunc(PARSEELEMENT::ENDHDR);
        if (e != XML_PARSE::OK) break;
        i += 2;
        continue;
      }
      if (_strnicmp(mm, "<!DOCTYPE ", 10) == 0) {
        e = ParseFunc(PARSEELEMENT::STARTDOCTYPE);
        if (e != XML_PARSE::OK) break;
        i += 10;
        continue;
      }
      if (_strnicmp(mm, ">", 1) == 0 && InDocType == 1) {
        e = ParseFunc(PARSEELEMENT::ENDDOCTYPE);
        if (e != XML_PARSE::OK) break;
        i += 1;
        continue;
      }
    }

    // Comment Tests
    bool CanComment = true;
    if (l && OutsideElementLine == 0) CanComment = false;
    if (InCData) CanComment = false;
    if (CanComment) {
      if (_strnicmp(mm, "<!--", 4) == 0 && !InComment) {
        e = ParseFunc(PARSEELEMENT::STARTCOMMENT);
        if (e != XML_PARSE::OK) break;
        i += 4;
        continue;
      }
      if (_strnicmp(mm, "-->", 3) == 0 && InComment) {
        e = ParseFunc(PARSEELEMENT::ENDCOMMENT);
        if (e != XML_PARSE::OK) break;
        i += 3;
        continue;
      }
    }

    // CData Tests
    bool CanCData = true;
    if (l && OutsideElementLine == 0) CanCData = false;
    if (CanCData) {
      if (_strnicmp(mm, "<![CDATA[", 9) == 0 && !InCData) {
        e = ParseFunc(PARSEELEMENT::STARTCD);
        if (e != XML_PARSE::OK) break;
        i += 9;
        continue;
      }
      if (_strnicmp(mm, "]]>", 3) == 0 && InCData) {
        e = ParseFunc(PARSEELEMENT::ENDCD);
        if (e != XML_PARSE::OK) break;
        i += 3;
        continue;
      }
    }

    // Variable Tests
    bool CanVariable = false;
    if (!InComment && !InCData && !InQ) {
      if (InHeader == 1)
        CanVariable = true;
      else if (l && OutsideElementLine == 0)
        CanVariable = true;
    }
    if (_strnicmp(mm, "=", 1) == 0 && CanVariable && InVar != 2 && !InQ) {
      e = ParseFunc(PARSEELEMENT::STARTVAR);
      if (e != XML_PARSE::OK) break;
      i += 1;
      continue;
    }
    if (_strnicmp(mm, "\"", 1) == 0 && CanVariable && InVar == 1) {
      t1.clear();
      InVar = 2;
      i += 1;
      continue;
    }
    if (_strnicmp(mm, "\"", 1) == 0 && CanVariable && InVar == 2) {
      e = ParseFunc(PARSEELEMENT::ENDVAR);
      if (e != XML_PARSE::OK) break;
      i += 1;
      continue;
    }

    bool CanElement = false;
    if (!InComment && !InCData && !InVar && InDocType != 1) CanElement = true;

    if (CanElement) {
      if (_strnicmp(mm, "</", 2) == 0 && l && OutsideElementLine == 1) {
        // Content...
        if (l && !t1.empty()) {
          l->AddContent(t1.c_str(), (size_t)-1);
          t1.clear();
        }

        e = ParseFunc(PARSEELEMENT::ENDEL);
        if (e != XML_PARSE::OK) break;
        t1.clear();
        i += 2;
        PendingElNameClosing = true;
        continue;
      }

      if (_strnicmp(mm, "<?", 2) == 0 && OutsideElementLine == 1) {
        e = ParseFunc(PARSEELEMENT::STARTEL);
        if (e != XML_PARSE::OK) break;
        InQ                = 1;
        OutsideElementLine = 0;
        i += 2;
        continue;
      }

      if (_strnicmp(mm, "<", 1) == 0 && OutsideElementLine == 1) {
        // Content...
        if (l && !t1.empty()) {
          l->AddContent(t1.c_str(), l->GetChildren().size());
          t1.clear();
        }

        e = ParseFunc(PARSEELEMENT::STARTEL);
        if (e != XML_PARSE::OK) break;
        OutsideElementLine = 0;
        i += 1;
        continue;
      }

      if (_strnicmp(mm, "/>", 2) == 0 || _strnicmp(mm, "?>", 2) == 0) {
        // El name?
        if (l && PendingElName && !t1.empty()) {
          l->SetElementName(Trim(t1.c_str()).c_str());
          PendingElName = false;
        }

        e = ParseFunc(PARSEELEMENT::ENDEL);

        if (e != XML_PARSE::OK) break;
        OutsideElementLine = 1;
        InQ                = 0;
        i += 2;
        continue;
      }

      if (_strnicmp(mm, ">", 1) == 0 && (OutsideElementLine == 0 || PendingElNameClosing) && l) {
        // El name?
        if (PendingElName) {
          l->SetElementName(Trim(t1.c_str()).c_str());
          PendingElName = false;
          t1.clear();
          i++;
          OutsideElementLine = 1;
          continue;
        }
        // El name?
        if (PendingElNameClosing) {
          //*
          PendingElNameClosing = false;
          t1.clear();
          i++;
          OutsideElementLine = 1;
          continue;
        }
        i++;
        OutsideElementLine = 1;
        continue;
      }
    }

    if (IsSpace && t1.empty() && !InComment && !InCData) {

    } else
      t1 += m[i];

    i++;
    continue;
  }

  return e;
}

// Gets
inline XMLElement& XML::GetRootElement() {
  return root;
}

inline XMLHeader& XML::GetHeader() {
  return hdr;
}

inline size_t XML::MemoryUsage() {
  return 0;
  //			return root.MemoryUsage() + GetHeader().MemoryUsage();
}

// Sets
inline void XML::SetRootElement(XMLElement& newroot) {
  root = newroot;
}

inline void XML::SetHeader(const XMLHeader& h) {
  hdr = h;
}

inline void XML::Clear() {
  root.clear();
  hdrcomments.clear();
  doctype.Clear();
  hdr.Default();
}

inline void XML::Version(XML_VERSION_INFO* x) {
  x->VersionLow  = (XML3_VERSION & 0xFF);
  x->VersionHigh = (XML3_VERSION >> 8);
#ifdef _MSC_VER
  strcpy_s(x->RDate, sizeof(x->RDate), XML3_VERSION_REVISION_DATE);
#else
  strcpy(x->RDate, XML3_VERSION_REVISION_DATE);
#endif
}

// ser
inline string XML::Serialize(XMLSerialization* srz) const {
  XMLSerialization defsrz;
  if (!srz) srz = &defsrz;

  string v;
  v.reserve(100000);

  v += hdr.Serialize();
  if (!srz->NoCRLF && !srz->Canonical) v += "\r\n";
  if (doctype.GetValue().length()) {
    v += doctype.Serialize();
    if (!srz->NoCRLF) v += "\r\n";
  }
  for (auto a: hdrcomments) {
    v += a->Serialize();
    if (!srz->NoCRLF) v += "\r\n";
  }
  XMLSerialization s2 = *srz;
  s2.ExcludeSelf      = false;
  s2.deep             = 0;
  root.Serialize(v, &s2);
  return v;
}

}; // namespace XML3
#endif
