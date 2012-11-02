#include "StdAfx.h"
#include "stuffs.h"

void MakeDirs( CString path ) 
{
	LPTSTR DestPath = path.GetBuffer(0);

	for (LPTSTR ChPtr=DestPath; *ChPtr!=0; ChPtr++)
	{
		if (*ChPtr== _T('\\'))
		{
			*ChPtr=0;
			CreateDirectory(DestPath,NULL);
			*ChPtr= _T('\\');
		}
	}
	CreateDirectory(DestPath,NULL);
	path.ReleaseBuffer();
}
bool FileExists( const CString &path ) 
{
	return (GetFileAttributes(path) != 0xFFFFFFFF)?true:false;
}
DWORD GetFileSizeS(const CString &path)
{
	HANDLE h = CreateFile(path, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h != INVALID_HANDLE_VALUE)
	{
		DWORD ret = GetFileSize(h, NULL);
		CloseHandle(h);
		return ret;
	}
	return 0;
}
CString GetVersionString() 
{
	CString ver; 
#ifndef _WIN64
	ver.Format(_T("operac4 %u.%u build %u"), MAJORVERSION, MINORVERSION, BUILDNUMBER);
#else
	ver.Format(_T("operac4 %u.%u build %u (x64)"), MAJORVERSION, MINORVERSION, BUILDNUMBER);
#endif
	return ver;
}
bool WriteLine( HANDLE stream, const CString &line, int CodePage ) 
{
#ifdef _UNICODE
	if (stream != INVALID_HANDLE_VALUE)
	{
		DWORD wr = 0;
		switch(CodePage)
		{
		case CodePage_Unicode:
			return (WriteFile(stream, (LPCTSTR)line, line.GetAllocLength(), &wr, NULL) && wr == line.GetAllocLength());
		case CodePage_OEM:
			{
				char *oem = getOemString(line);
				int len = lstrlenA(oem);
				HRESULT res = WriteFile(stream, oem, len, &wr, NULL);
				my_free(oem);
				return res && wr == len;
			}
		case CodePage_ANSI:
			{
				char *ansi = getAnsiString(line);
				int len = lstrlenA(ansi);
				HRESULT res = WriteFile(stream, ansi, len, &wr, NULL);
				my_free(ansi);
				return res && wr == len;
			}
		}
		
	}
#else
	#error TODO!!!
#endif
	return false;
}
bool CheckForKey(const int key)
{
	bool ExitCode=false;
	while (1)
	{
		INPUT_RECORD rec;
		HANDLE hConInp=GetStdHandle(STD_INPUT_HANDLE);
		DWORD ReadCount;
		PeekConsoleInput(hConInp,&rec,1,&ReadCount);
		if (ReadCount==0)
			break;
		ReadConsoleInput(hConInp,&rec,1,&ReadCount);
		if (rec.EventType==KEY_EVENT)
			if (rec.Event.KeyEvent.wVirtualKeyCode==key &&
				rec.Event.KeyEvent.bKeyDown)
				ExitCode=true;
	}
	return ExitCode;
}

int rnd( int maxRnd )
{
	//сделаем более случайный выбор
	int rnds[10];
	for (int i = 0; i < 10; i++)
		rnds[i] = rand() % maxRnd;

	return (rnds[rand() % 10]);
}
bool CheckAndConvertPath( bool another, CString & res, bool selected, int i )
{
	res = GetPanelPath(another);
	AddEndSlash(res);
	res += GetFileName(another, selected, i);

	return true;
}
bool IsDirectory(DWORD attr)
{
	return (attr & FILE_ATTRIBUTE_DIRECTORY)?true:false;
}
bool IsDirectory( bool another, bool selected, int i )
{
	DWORD attr = GetFileAttributes(another, selected, i);

	return (attr & FILE_ATTRIBUTE_DIRECTORY)?true:false;
}

FILETIME UnixToFileTime( time_t time )
{
	FILETIME ft = {0};
	UINT64 i64 = time;
	i64 *= 10000000;
	i64 += 116444736000000000;
	ft.dwLowDateTime = (DWORD)i64;
	ft.dwHighDateTime = (DWORD)(i64 >> 32);
	return ft;
}
void PrepareInfoLine(const wchar_t * str, void *ansi, CString& line, CString format)
{
	line.Format(format, str, ansi);
}
void PrepareInfoLineDate(const wchar_t * str, time_t * time, CString& line, bool b64)
{
	CString date;
	if (b64)
		date = _wctime(time);
	else
		date = _wctime32((__time32_t*)time);

	line.Format(_T("%s%s"), str, (date && *date)?date:_T("\n"));
}