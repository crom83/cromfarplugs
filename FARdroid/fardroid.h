#pragma once

#define MKID(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))

#define ID_STAT MKID('S','T','A','T')
#define ID_LIST MKID('L','I','S','T')
#define ID_ULNK MKID('U','L','N','K')
#define ID_SEND MKID('S','E','N','D')
#define ID_RECV MKID('R','E','C','V')
#define ID_DENT MKID('D','E','N','T')
#define ID_DONE MKID('D','O','N','E')
#define ID_DATA MKID('D','A','T','A')
#define ID_OKAY MKID('O','K','A','Y')
#define ID_FAIL MKID('F','A','I','L')
#define ID_QUIT MKID('Q','U','I','T')

#define SWAP_WORD(x)  x = (x>>8) | (x<<8)
#define SWAP_DWORD(x)  x = (x>>24) | ((x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24)

typedef union {
	unsigned id;
	struct {
		unsigned id;
		unsigned namelen;
	} req;
	struct {
		unsigned id;
		unsigned mode;
		unsigned size;
		unsigned time;
	} stat;
	struct {
		unsigned id;
		unsigned mode;
		unsigned size;
		unsigned time;
		unsigned namelen;
	} dent;
	struct {
		unsigned id;
		unsigned size;
	} data;
	struct {
		unsigned id;
		unsigned msglen;
	} status;    
} syncmsg;

#define SYNC_DATA_MAX (64*1024)
struct syncsendbuf 
{
	unsigned id;
	unsigned size;
	char data[SYNC_DATA_MAX];
};

//чтение файла с проверкой результата
#define CHECKREADOK(rd_type) \
	res = ReadFile(hFile, &rd_type, sizeof(rd_type), &rd, NULL);\
	if (!res)\
	{\
		CloseHandle(hFile); \
		return INVALID_HANDLE_VALUE;\
	}\
	else if (rd != 0 && rd != sizeof(rd_type))\
	{\
		CloseHandle(hFile); \
		return INVALID_HANDLE_VALUE;\
	}\

//чтение файла с проверкой результата во вложенной функции
#define CHECKREADOK2(rd_type) \
	res = ReadFile(hFile, &rd_type, sizeof(rd_type), &rd, NULL);\
	if (res &&  rd == 0) \
		return ERROR_HANDLE_EOF;\
	else	if (!res)\
		return ERROR_READ_FAULT;\
	else if (rd != sizeof(rd_type))\
		return ERROR_BAD_FORMAT

//подготовка и запись строки в файл
#define WRITELINESTR(str1, str2)\
	PrepareInfoLine(str1, str2, line);\
	if (!WriteLine( stream, line, CodePage_ANSI)) goto exit

//подготовка и запись числа в файл
#define WRITELINEINT(str1, i)\
	PrepareInfoLine(str1, i, line, _T("%s%u\n"));\
	if (!WriteLine( stream, line, CodePage_ANSI)) goto exit

//подготовка и запись даты в файл
#define WRITELINEDATE64(str1, d)\
	PrepareInfoLineDate(str1, d, line, true);\
	if (!WriteLine( stream, line, CodePage_ANSI)) goto exit

//подготовка и запись даты в файл
#define WRITELINEDATE32(str1, d)\
	PrepareInfoLineDate(str1, d, line, false);\
	if (!WriteLine( stream, line, CodePage_ANSI)) goto exit


//запись в файл перевода строки
#define WRITELINEBRAKE\
	line = _T("\n");\
	if (!WriteLine( stream, line, CodePage_ANSI)) goto exit


struct CFileRecord
{
	CString filename;
	CString linkto;
	CString owner;
	CString grp;
	CString desc;
	time_t	time;
	UINT64	size;
	DWORD		attr;
};

typedef CSimpleArrayEx<CFileRecord*, CFileRecord*> CFileRecords;

struct ProcessStruct
{
	CString from;
	CString to;
	CString title;
	CString spos;
	bool bSilent;
	bool bPrevState;
	HANDLE mutex;
	bool docontinue;

	int nFileSize;
	int nTransmitted;
	ProcessStruct()
	{
		bPrevState = false;
		docontinue = true;
		mutex = CreateMutex(NULL, FALSE, NULL);
	}
	~ProcessStruct()
	{
		CloseHandle(mutex);
		mutex = 0;
	}
	bool Lock()
	{
		if (mutex)
		{
			switch (WaitForSingleObject(mutex, 1000))
			{
			case WAIT_OBJECT_0:
				return true;
			}
		}
		return false;
	}
	bool Unlock()
	{
		if (mutex)
		{
			ReleaseMutex(mutex);
			return true;
		}
		return false;
	}
	bool Hide()
	{
		if (Lock())
		{
			bPrevState = bSilent;
			bSilent = true;
			return true;
		}
		return false;
	}
	void Restore()
	{
		bSilent = bPrevState;
		Unlock();
	}
};

struct CPanelLine
{
	CString data;
	CString text;
	BOOL separator;
};

typedef CSimpleArrayEx<CPanelLine, CPanelLine> InfoPanelLines;

class fardroid
{
private:
	int lastError;
	CFileRecords  records;
	ProcessStruct m_procStruct;
	InfoPanelLine * InfoPanelLineArray;
	InfoPanelLines lines;

	CFileRecord * GetFileRecord(LPCTSTR sFileName);
	DWORD ParseSection (HANDLE hFile, BYTE sectionID);
	bool ParseSectionBuf (BYTE * buf, int &bufPos, int bufSize, CFileRecords * record);
	CString m_currentPath;
	void ShowADBExecError(CString err, bool bSilent);

	SOCKET	CreateADBSocket();
	SOCKET	PrepareADBSocket();
	bool		SendADBPacket(SOCKET sockADB, void * packet, int size);
	bool		SendADBCommand(SOCKET sockADB, LPCTSTR sCMD);
	int			ReadADBPacket(SOCKET sockADB, void * packet, int size);
	bool		CheckADBResponse(SOCKET sockADB);
	bool		ReadADBSocket(SOCKET sockADB, char * buf, int bufSize);
	BOOL		ADBShellExecute(LPCTSTR sCMD, CString & sRes, bool bSilent);
	void		ADBSyncQuit(SOCKET sockADB);
	bool		ADBTransmitFile(SOCKET sockADB, LPCTSTR sFileName, time_t & mtime);
	bool		ADBSendFile(SOCKET sockADB, LPCTSTR sSrc, LPCTSTR sDst, CString & sRes, int mode);
	bool		ADBReadMode(SOCKET sockADB, LPCTSTR path, int &mode);
	BOOL		ADBPushFile(SOCKET sockADB, LPCTSTR sSrc, LPCTSTR sDst, CString & sRes);
	bool		ADBPushDir(SOCKET sockADB, LPCTSTR sSrc, LPCTSTR sDst, CString &sRes);
	BOOL		ADBPullFile(SOCKET sockADB, LPCTSTR sSrc, LPCTSTR sDst, CString & sRes);
	bool		ADBPullDir(SOCKET sockADB, LPCTSTR sSrc, LPCTSTR sDst, CString & sRes);
	void		CloseADBSocket(SOCKET sockADB);

	bool DeviceTest();
	//BOOL ADB_execute(LPCTSTR sCMD, CString & sRes, bool bSilent);
	BOOL ADB_ls(LPCTSTR sDir, CFileRecords & files, CString & sRes, bool bSilent);
	BOOL ADB_rm(LPCTSTR sDir, CString & sRes, bool bSilent);
	BOOL ADB_mkdir(LPCTSTR sDir, CString & sRes, bool bSilent);
	BOOL ADB_pull(LPCTSTR sSrc, LPCTSTR sDst, CString & sRes, bool bSilent);
	BOOL ADB_push(LPCTSTR sSrc, LPCTSTR sDst, CString & sRes, bool bSilent);
	BOOL ADB_findmount( LPCTSTR sFS, strvec &fs_params, CString & sRes, bool bSilent );
	BOOL ADB_mount(LPCTSTR sFS, BOOL bAsRW, CString & sRes, bool bSilent);

	bool		ParseFileLineBB(CString & sLine, CFileRecords & files);
	bool		ParseFileLine(CString & sLine, CFileRecords & files);
	//bool		ParseFileLineSafe( CString & sLine );
	BOOL		ReadFileList(CString & sFileList, CFileRecords & files);
	BOOL		OpenPanel(LPCTSTR sPath);

	bool		CopyFileFrom(const CString& src, const CString& dst, bool &noPromt, bool &ansYes, bool bSilent);
	bool		CopyFileTo(const CString& src, const CString& dst, bool &noPromt, bool &ansYes, bool bSilent);
	bool		DeleteFileFrom(const CString& src, bool &noPromt, bool &ansYes, bool bSilent);

	bool GetItems(PluginPanelItem *PanelItem, int ItemsNumber, const CString& srcdir, const CString& dstdir, bool &noPromt, bool &ansYes, bool bSilent);
	bool PutItems(PluginPanelItem *PanelItem, int ItemsNumber, const CString& srcdir, const CString& dstdir, bool &noPromt, bool &ansYes, bool bSilent);
	bool DelItems(PluginPanelItem *PanelItem, int ItemsNumber, bool &noPromt, bool &ansYes, bool bSilent);

	void ParseMemoryInfo(CString s);
	void GetMemoryInfo();
	void ParsePartitionInfo(CString s);
	void GetPartitionsInfo();
	int UpdateInfoLines();
	CString GetPermissionsFile(const CString& FullFileName);
	CString PermissionsFileToMask(CString Permission);
	bool SetPermissionsFile(const CString& FullFileName, const CString& PermissionsFile);


	bool GetFrameBuffer(LPCTSTR sDest);
public:
	bool m_bForceBreak;

	CString fileUnderCursor;
	CString panelTitle;
	strmap params;
	fardroid(void);
	~fardroid(void);

	HANDLE	OpenFromMainMenu();
	HANDLE	OpenFromCommandLine(const CString &cmd);

	int			ChangeDir(LPCTSTR sDir, int OpMode = 0);

	bool		DeleteFilesDialog();
	bool		CreateDirDialog(CString &dest);
	bool		CopyFilesDialog(CString &dest);
	bool		CopyFileDialog(CString &destpath, CString &destname);
	bool		DeleteFile(const CString& name, bool bSilent);
	void		DeleteRecords(CFileRecords & recs);
	void		PreparePanel(struct OpenPanelInfo *Info);
	void		ChangePermissionsDialog();

	int GetFindData(struct PluginPanelItem **pPanelItem,size_t *pItemsNumber,int OpMode);
	void FreeFindData(struct PluginPanelItem *PanelItem,int ItemsNumber);
	int GetFiles(PluginPanelItem *PanelItem, int ItemsNumber, CString &DestPath,	BOOL Move, int OpMode);
	int PutFiles(PluginPanelItem *PanelItem, int ItemsNumber, CString SrcPath,	BOOL Move, int OpMode);
	int DeleteFiles(PluginPanelItem *PanelItem, int ItemsNumber, int OpMode);
	int CreateDir( CString &DestPath, int OpMode );

	void Reread();

	bool ShowProgressMessage();
	bool BreakProcessDialog(LPCTSTR sTitle);
	int CopyErrorDialog(LPCTSTR sTitle, LPCTSTR sErr);
	int FileExistsDialog(LPCTSTR sName);
};

