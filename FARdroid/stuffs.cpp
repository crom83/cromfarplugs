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
	ver.Format(_T("fardroid %u.%u build %u"), MAJORVERSION, MINORVERSION, BUILDNUMBER);
#else
	ver.Format(_T("fardroid %u.%u build %u (x64)"), MAJORVERSION, MINORVERSION, BUILDNUMBER);
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
bool CheckAndConvertParh( bool another, CString & res, bool selected, int i )
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
bool IsLink(DWORD attr)
{
	return (attr & FILE_ATTRIBUTE_REPARSE_POINT)?true:false;
}
bool IsDevice(DWORD attr)
{
	return (attr & FILE_ATTRIBUTE_DEVICE)?true:false;
}
bool IsDirectory( bool another, bool selected, int i )
{
	DWORD attr = GetFileAttributes(another, selected, i);

	return (attr & FILE_ATTRIBUTE_DIRECTORY)?true:false;
}
bool IsDirectory( LPCTSTR sPath)
{
	DWORD attr = GetFileAttributes(sPath);

	return ((attr != -1) && (attr & FILE_ATTRIBUTE_DIRECTORY))?true:false;
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

time_t StringTimeToUnixTime( CString sData, CString sTime )
{
	SYSTEMTIME time = {0};
	strvec a;
	Tokenize(sData, a, _T("-"), false);
	if (a.GetSize() == 3)
	{
		time.wYear = _ttoi(a[0]);
		time.wMonth = _ttoi(a[1]);
		time.wDay = _ttoi(a[2]);
	}
	a.RemoveAll();
	Tokenize(sTime, a, _T(":"), false);
	if (a.GetSize() == 2)
	{
		time.wHour = _ttoi(a[0]);
		time.wMinute = _ttoi(a[1]);
	}

	time_t t = 0;
	SystemTimeToUnixTime(&time, &t);
	return t;
}

time_t StringTimeToUnixTime( CString sDay, CString sMonth, CString sYear, CString sTime )
{
	SYSTEMTIME time = {0};
	time.wYear	= _ttoi(sYear);
	time.wDay		= _ttoi(sDay);

	if (sMonth.CompareNoCase(_T("Jan")) == 0)
		time.wMonth = 1;
	else if (sMonth.CompareNoCase(_T("Feb")) == 0)
		time.wMonth = 2;
	else if (sMonth.CompareNoCase(_T("Mar")) == 0)
		time.wMonth = 3;
	else if (sMonth.CompareNoCase(_T("Apr")) == 0)
		time.wMonth = 4;
	else if (sMonth.CompareNoCase(_T("May")) == 0)
		time.wMonth = 5;
	else if (sMonth.CompareNoCase(_T("Jun")) == 0)
		time.wMonth = 6;
	else if (sMonth.CompareNoCase(_T("Jul")) == 0)
		time.wMonth = 7;
	else if (sMonth.CompareNoCase(_T("Aug")) == 0)
		time.wMonth = 8;
	else if (sMonth.CompareNoCase(_T("Sep")) == 0)
		time.wMonth = 9;
	else if (sMonth.CompareNoCase(_T("Oct")) == 0)
		time.wMonth = 10;
	else if (sMonth.CompareNoCase(_T("Nov")) == 0)
		time.wMonth = 11;
	else if (sMonth.CompareNoCase(_T("Dec")) == 0)
		time.wMonth = 12;

	strvec a;
	Tokenize(sTime, a, _T(":"), false);
	if (a.GetSize() == 3)
	{
		time.wHour = _ttoi(a[0]);
		time.wMinute = _ttoi(a[1]);
		time.wSecond = _ttoi(a[2]);
	}

	time_t t = 0;
	SystemTimeToUnixTime(&time, &t);
	return t;
}
time_t *SystemTimeToUnixTime(LPSYSTEMTIME pst, time_t* pt) 
{ 
	FILETIME ft; 
	SystemTimeToFileTime(pst, &ft); 
	FileTimeToUnixTime(&ft, pt); 
	return pt;
}
CString SystemTimeToString(LPSYSTEMTIME pst)
{
	CString str;
	str.Format(_T("%0d.%0d.%04d %0d:%0d:%0d"), pst->wDay, pst->wMonth, pst->wYear, pst->wHour, pst->wMinute, pst->wSecond);
	return str;
}
FILETIME UnixTimeToFileTime( time_t time )
{
	FILETIME ft = {0};
	UINT64 i64 = time;
	i64 *= 10000000;
	i64 += 116444736000000000;
	ft.dwLowDateTime = (DWORD)i64;
	ft.dwHighDateTime = (DWORD)(i64 >> 32);
	return ft;
}
void FileTimeToUnixTime(LPFILETIME pft, time_t* pt) 
{ 
	LONGLONG ll; // 64 bit value 
	ll = (((LONGLONG)(pft->dwHighDateTime)) << 32) + pft->dwLowDateTime; 
	*pt = (time_t)((ll - 116444736000000000ui64)/10000000ui64); 
}

DWORD StringToAttr( CString sAttr )
{
	if (sAttr.GetLength() != 10)
		return FILE_ATTRIBUTE_OFFLINE;
	return 
		((sAttr[0] == _T('d'))?FILE_ATTRIBUTE_DIRECTORY:0)| //directory flag
		((sAttr[0] == _T('l'))?FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT:0)| //link
		((sAttr[0] == _T('-'))?FILE_ATTRIBUTE_ARCHIVE:0)| //file
		((sAttr[0] == _T('c'))?FILE_ATTRIBUTE_DEVICE:0)| //symbol device
		((sAttr[0] == _T('b'))?FILE_ATTRIBUTE_DEVICE:0)| //block device
		((sAttr[0] == _T('p'))?FILE_ATTRIBUTE_READONLY:0)| //FIFO device
		((sAttr[0] == _T('s'))?FILE_ATTRIBUTE_DEVICE:0)| //socket device
		((sAttr[5] != _T('w'))?FILE_ATTRIBUTE_READONLY:0);//writable flag
}
DWORD ModeToAttr( int mode )
{
	
	return 
		(IS_FLAG(mode, S_IFDIR)?FILE_ATTRIBUTE_DIRECTORY:0)| //directory flag
		(IS_FLAG(mode, S_IFLNK)?FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT:0)| //link
		(IS_FLAG(mode, S_IFREG)?FILE_ATTRIBUTE_ARCHIVE:0)| //file
		(IS_FLAG(mode, S_IFCHR)?FILE_ATTRIBUTE_DEVICE:0)| //symbol device
		(IS_FLAG(mode, S_IFBLK)?FILE_ATTRIBUTE_DEVICE:0)| //block device
		(IS_FLAG(mode, S_IFIFO)?FILE_ATTRIBUTE_READONLY:0)| //FIFO device
		(IS_FLAG(mode, S_IFSOCK)?FILE_ATTRIBUTE_DEVICE:0)| //socket device
		(!IS_FLAG(mode, S_IWRITE)?FILE_ATTRIBUTE_READONLY:0);//writable flag
}

bool WriteBMP( LPCTSTR sPath, byte * buffer, int buffersize, int w, int h, int bpp )
{
#pragma pack(push, 1)
	struct {
		int filesz;
		int reserved;
		int bmp_offset;
	} bmpfile_header;

	struct {
		int header_sz;
		int width;
		int height;
		short nplanes;
		short bitspp;
		int compress_type;
		int bmp_bytesz;
		int hres;
		int vres;
		int ncolors;
		int nimpcolors;
	} bmp_dib_v3_header_t;
#pragma pack(pop)

	int rowsize = 4 * (bpp*w)/32;
	bmpfile_header.filesz = 54 + w*h*3;
	bmpfile_header.reserved = 0;
	bmpfile_header.bmp_offset = 54;

	bmp_dib_v3_header_t.header_sz = 40;
	bmp_dib_v3_header_t.width = w;
	bmp_dib_v3_header_t.height = h;
	bmp_dib_v3_header_t.nplanes = 1;
	bmp_dib_v3_header_t.bitspp = 24;
	bmp_dib_v3_header_t.compress_type = 0;
	bmp_dib_v3_header_t.bmp_bytesz = w*h*3;
	bmp_dib_v3_header_t.hres = 0;
	bmp_dib_v3_header_t.vres = 0;
	bmp_dib_v3_header_t.ncolors = 0;
	bmp_dib_v3_header_t.nimpcolors = 0;

	HANDLE hFile = hFile = CreateFile(sPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD writed = 0;
		WriteFile(hFile, "BM", 2, &writed, NULL);

		WriteFile(hFile, &bmpfile_header, sizeof(bmpfile_header), &writed, NULL);
		WriteFile(hFile, &bmp_dib_v3_header_t, sizeof(bmp_dib_v3_header_t), &writed, NULL);

		int index = 0;
		for (int y = h - 1 ; y >= 0; y--)
		{
			index = y*w*2;
			for (int x = 0 ; x < w; x++)
			{
				int val = buffer[index++] & 0x00FF;
				val |= (buffer[index++] << 8) & 0x0FF00;

				int r = ((val >> 11) & 0x01F) << 3;
				int g = ((val >> 5) & 0x03F) << 2;
				int b = ((val >> 0) & 0x01F) << 3;

				val = 0xFF << 24 | r << 16 | g << 8 | b;

				WriteFile(hFile, &val, 3, &writed, NULL);
			}
		}

		/*byte * buf = (byte*)buffer + (buffersize-2);
		for (int i = 0; i < w*h; i++)
		{
			short pixel = *buf;
			buf -= 2;

			SWAP_WORD(pixel);

			WORD red_mask = 0x7C00;
			WORD green_mask = 0x3E0;
			WORD blue_mask = 0x1F;

			BYTE red_value = (pixel & red_mask) >> 10;
			BYTE green_value = (pixel & green_mask) >> 5;
			BYTE blue_value = (pixel & blue_mask);

			// Expand to 8-bit values:
			BYTE red   = red_value << 3;
			BYTE green = green_value << 3;
			BYTE blue  = blue_value << 3;

			WriteFile(hFile, &red, 1, &writed, NULL);
			WriteFile(hFile, &green, 1, &writed, NULL);
			WriteFile(hFile, &blue, 1, &writed, NULL);
		}*/
		CloseHandle(hFile);
	}

	return TRUE;
}
#ifdef USELOGGING
void CPerfCounter::Log( LPCTSTR lpMsg )
{
	CString s;
	SYSTEMTIME st;
	GetSystemTime(&st);
	s.Format(_T("%s: %s"), SystemTimeToString(&st), (lpMsg == NULL)?lastStr:lpMsg);
	OutputDebugString(lpMsg);

	HANDLE hFile; 
	CString path;
	GetTempPath(1024, path.GetBuffer(1024));
	path.ReleaseBuffer();
	AddEndSlash(path);
	path += _T("fardroid.log");
	hFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE) 
	{ 
		SetFilePointer(hFile, 0, NULL, FILE_END);
		WriteLine(hFile, s, CodePage_ANSI);
		CloseHandle(hFile);
	}
}
#endif

BOOL IsPointDir(LPCTSTR sName)
{
	return (lstrcmp(sName, _T(".")) == 0) || (lstrcmp(sName, _T("..")) == 0);
}

BOOL DeleteDir(CString &sName)
{
	AddEndSlash(sName);
	CString saved = sName;

	sName += _T("*.*");

	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(sName, &fd);
	if (h == INVALID_HANDLE_VALUE) return FALSE;

	BOOL bOk = TRUE;
	CString sname;
	sname.Format(_T("%s%s"), saved, fd.cFileName);
	if (!IsDirectory(fd.dwFileAttributes))
	{	if (!DeleteFile(sname)) bOk = FALSE;}
	else if (!IsPointDir(fd.cFileName))
	{	if (!DeleteDir(sname)) bOk = FALSE; }

	while (FindNextFile(h, &fd) != 0) 
	{
		sname.Format(_T("%s%s"), saved, fd.cFileName);
		if (!IsDirectory(fd.dwFileAttributes))
		{	if (!DeleteFile(sname)) bOk = FALSE;}
		else if (!IsPointDir(fd.cFileName))
		{	if (!DeleteDir(sname)) bOk = FALSE; }

	}
	FindClose(h);

	return bOk;
}

BOOL DeletePanelItems(CString &sPath, struct PluginPanelItem *PanelItem,int ItemsNumber)
{
	BOOL bOk = TRUE;
	AddEndSlash(sPath);
	CString sName;
	for (int i = 0; i < ItemsNumber; i++)
	{
		sName.Format(_T("%s%s"), sPath, PanelItem[i].FileName);

		if (IsDirectory(PanelItem[i].FileAttributes))
		{	if (!DeleteDir(sName)) bOk = FALSE;}
		else
		{	if (!DeleteFile(sName)) bOk = FALSE;}
	}

	return bOk;
}