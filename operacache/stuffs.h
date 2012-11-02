#pragma once

enum
{
	CodePage_Unicode,
	CodePage_ANSI,
	CodePage_OEM
};

void		MakeDirs(CString path);
bool		FileExists(const CString &path);
DWORD		GetFileSizeS(const CString &path);
CString GetVersionString();
bool WriteLine( HANDLE stream, const CString &line, int CodePage);
bool CheckForKey(const int key);
int  rnd(int maxRnd);
bool IsDirectory(DWORD attr);
FILETIME UnixToFileTime(time_t time);

bool IsDirectory(bool another, bool selected, int i);
void PrepareInfoLine(const wchar_t * str, void *ansi, CString& line, CString format = _T("%s%s\n"));
bool CheckAndConvertPath(bool another, CString & res, bool selected, int i);
void PrepareInfoLineDate(const wchar_t * str, time_t * time, CString& line, bool b64);
