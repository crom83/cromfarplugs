#pragma once

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

#define SECID BYTE
#define SECSZ WORD
#define MSB_VALUE 0x80

#define SWAP_WORD(x)  x = (x>>8) | (x<<8)
#define SWAP_DWORD(x)  x = (x>>24) | ((x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24)
#define SWAP_QWORD(x) x = (x>>56) | ((x<<40) & 0x00FF000000000000) | ((x<<24) & 0x0000FF0000000000) | ((x<<8)  & 0x000000FF00000000) | ((x>>8)  & 0x00000000FF000000) | ((x>>24) & 0x0000000000FF0000) | ((x>>40) & 0x000000000000FF00) | (x<<56)

#define SWAP_SECSZ(x) SWAP_WORD(x)

#pragma pack(push, 1)
struct FileHeader 
{
	DWORD FileVer;
	DWORD SoftVer;
	WORD idSize;
	WORD lenSize;
};
#pragma pack(pop)

enum {
	RECORD_CACHE,
	RECORD_DOWNLOAD,
	RECORD_LINK
};
struct CCacheRecord
{
	CString url;
	CString filename;
	DWORD		lastVisited;
	time_t	lastLoaded;
	time_t	blackDate;
	//2 — Загружен
	//4 — Загрузка прервана (пользователем)
	//5 — Загрузка оборвана 
	DWORD		status;
	UINT64	size;
	CString mime;
	CString cp;
	BYTE		recordType;

	wchar_t* m_pColumns[2];
	const wchar_t * const * GetColumns()
	{
		m_pColumns[0] = _C(filename);
		m_pColumns[1] = _C(cp);

		return m_pColumns;
	}
};

typedef CSortedArray<CCacheRecord*, CCacheRecord*> CCacheRecords;

class operac4
{
private:
	DWORD ParseSection (HANDLE hFile, BYTE sectionID);
	bool ParseSectionBuf (BYTE * buf, int &bufPos, int bufSize, CCacheRecord * record);
	CCacheRecords records;
public:
	CString fileUnderCursor;
	CString panelTitle;
	strmap params;
	operac4(void);
	~operac4(void);
	HANDLE	Open(CString filename);
	HANDLE	OpenFromMainMenu();
	HANDLE	OpenFromCommandLine(const CString &cmd);

	bool		CopyFilesDialog(CString &dest);
	bool		CopyFileDialog(CString &destpath, CString &destname);
	bool		DeleteFile(const CString& name, bool bSilent);
	bool		CopyFile(const CString& src, const CString& dst, bool &noPromt, bool &ansYes, bool bSilent);
	bool		CopyFileWithNewName();
	bool		CopyItems(const CString& srcdir, const CString& dstdir, bool &noPromt, bool &ansYes, bool bSilent);
	void		deleteRecords();
	void		PreparePanel(OpenPluginInfo *Info);

	int GetFindData(struct PluginPanelItem **pPanelItem,int *pItemsNumber,int OpMode);
	void FreeFindData(struct PluginPanelItem *PanelItem,int ItemsNumber);
	int GetFiles(PluginPanelItem *PanelItem, int ItemsNumber, CString &DestPath,	int OpMode);

	void Reread();
	CString GetName(CCacheRecord * item, CString * srcfile, CString * dstdir);
};
