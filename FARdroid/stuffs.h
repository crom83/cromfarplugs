#pragma once

#define 	S_IFMT		61440
#define 	S_IFLNK   40960
#define 	S_IFREG   32768
#define 	S_IFBLK   24576
#define 	S_IFDIR   16384
#define 	S_IFCHR   8192
#define 	S_IFIFO   4096
#define 	S_IFSOCK  49152
#define 	S_IRUSR   0x00400
#define 	S_IWUSR   0x00200
#define 	S_IXUSR   0x00100
#define 	S_IEXEC   S_IXUSR
#define 	S_IWRITE   S_IWUSR
#define 	S_IREAD   S_IRUSR

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
bool IsDevice(DWORD attr);
bool IsLink(DWORD attr);
bool IsDirectory(DWORD attr);
bool IsDirectory(bool another, bool selected, int i);
bool IsDirectory( LPCTSTR sPath);
void PrepareInfoLine(const wchar_t * str, void *ansi, CString& line, CString format = _T("%s%s\n"));
bool CheckAndConvertPath(bool another, CString & res, bool selected, int i);
void PrepareInfoLineDate(const wchar_t * str, time_t * time, CString& line, bool b64);

FILETIME UnixTimeToFileTime(time_t time);
time_t StringTimeToUnixTime( CString sDay, CString sMonth, CString sYear, CString sTime );
time_t StringTimeToUnixTime( CString sData, CString sTime );
void FileTimeToUnixTime(LPFILETIME pft, time_t* pt);
time_t *SystemTimeToUnixTime(LPSYSTEMTIME pst, time_t* pt);
CString SystemTimeToString(LPSYSTEMTIME pst);

DWORD StringToAttr( CString sAttr );
DWORD ModeToAttr( int mode );

bool WriteBMP(LPCTSTR sPath, byte * buffer, int buffersize, int w, int h, int bpp);
BOOL DeletePanelItems(CString &sPath, struct PluginPanelItem *PanelItem,int ItemsNumber);

#ifdef USELOGGING

class CPerfCounter
{
	CString lastStr;
	DWORD m_start;
public:
	CPerfCounter()
	{
		m_start = 0;
	}
	CPerfCounter(LPCTSTR nMsg)
	{
		m_start = 0;
		Start(nMsg);
	}
	~CPerfCounter()
	{
		Stop(NULL);
	}
	/*void Start(int nMsg)
	{
		TCHAR t[100];
		Start(_itot(nMsg, t, 10));
	}*/
	/*void Stop(int nMsg)
	{
		TCHAR t[100];
		Stop(_itot(nMsg, t, 10));
	}*/
	/*void Reset(int nMsg)
	{
		TCHAR t[100];
		_itot(nMsg, t, 10);
		Stop(t);
		Start(t);
	}*/
	void Start(LPCTSTR lpMsg)
	{
		CString msg;
		lastStr = (lpMsg == NULL)?_T(""):lpMsg;
		msg.Format(_T("Started at: %s\n"), lastStr);
		Log(msg);
		m_start = GetTickCount();
	}
	void Stop(LPCTSTR lpMsg)
	{
		m_start = GetTickCount() - m_start;
		CString msg;
		if (lpMsg) lastStr = lpMsg;
		msg.Format(_T("Finished at: %s. Time: %d\n"), lastStr, m_start);
		Log(msg);
		m_start = 0;
	}
	void Reset(LPCTSTR lpMsg)
	{
		Stop(lastStr);
		Start(lpMsg);
	}
	void Log(LPCTSTR lpMsg);
};

#endif