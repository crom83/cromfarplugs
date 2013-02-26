#pragma once

enum{
	WORKMODE_SAFE,
	WORKMODE_NATIVE,
	WORKMODE_BUSYBOX,
};

class CConfig
{
public:
	//common
	HANDLE hHandle;
	//plugin
	CString Prefix;
	CString ADBPath;
	//interface
	int			PanelMode;
	int			SortMode;
	int			SortOrder;
	int			WorkMode;
	int			TimeOut;
	BOOL		AddToDiskMenu;
	BOOL		ShowLinksAsDir;
	BOOL		ShowAllPartitions;
	BOOL		UseSU;
	BOOL		UseExtendedAccess;
	BOOL		RemountSystem;
	/*CString Mode1CTypes;
	CString Mode1CSizes;
	CString Mode2CTypes;
	CString Mode2CSizes;*/
	CConfig(void);
	~CConfig(void);
	bool InitHandle();
	void FreeHandle();
	void Set(const wchar_t* Name, int Value);
	void Set(const wchar_t* Name, CString &Value);
	void Get(const wchar_t* Name, int &Value, int Default);
	void Get(const wchar_t* Name, CString &Value, const wchar_t *Default);
	bool Load();
	void Save();
	BOOL LinksAsDir();
};
