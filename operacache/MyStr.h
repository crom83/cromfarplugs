#pragma once

bool		TestExt(CString ext, CString extList);
CString ExtractUrl(const CString &Path);
CString ExtractName(CString Path, bool bRev = false);
CString ExtractPath(CString Path);
CString ExtractExt(CString Path);
void Tokenize(CString str,	strvec& tokens,	const CString& sep = " ");
void TokensToParams(strvec& tokens,	strmap& params);
CString &Unquote(CString &str);
CString GetParam(strmap& params, CString opt);
bool ExistsParam(strmap& params, CString opt);
int			utf8ToUnicode ( const char* lpMultiByteStr, WCHAR* lpWideCharStr, int cmbChars );
void AddEndSlash(CString & str);
void ExtractURLParts(const CString &url, CString *proto, CString *serv, CString *port, CString *path);
char * getAnsiString(const CString& val);
char * getOemString(const CString& val);
int __cdecl my_wcstombsz(char* mbstr, const wchar_t* wcstr, size_t count, UINT CodePage = CP_ACP);
int __cdecl my_mbstowcsz(wchar_t* wcstr, const char* mbstr, size_t count, UINT CodePage = CP_ACP);
void strcpyC(char * dst, const CString& src, bool Ansi);
//bool UnicodeToutf8(const wchar_t* wcs, size_t length, byte *outbuf, size_t outlength);
CString URLEncode(char * str);
CString GetExtensionByMIME(const CString &MIME);
CString GenShortName(const CString &url, BOOL AddExt, BOOL NameFromCache, const CString &MIME);
void PrepareShortName(CString &name);