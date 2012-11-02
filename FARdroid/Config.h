#pragma once

enum{
	WORKMODE_SAFE,
	WORKMODE_NATIVE,
	WORKMODE_BUSYBOX,
};

class CConfig
{
private:
	BOOL FromReg;
public:
	//common
	CString PluginRootKey;
	CString ModulePath;
	//plugin
	CString Prefix;
	CString ADBPath;
	//interface
	int			PanelMode;
	int			SortMode;
	int			SortOrder;
	int			WorkMode;
	int			HotKeyInDisk;
	int			TimeOut;
	BOOL		AddToDiskMenu;
	BOOL		ShowLinksAsDir;
	BOOL		ShowAllPartitions;
	BOOL		UseSU;
	BOOL		RemountSystem;
	/*CString Mode1CTypes;
	CString Mode1CSizes;
	CString Mode2CTypes;
	CString Mode2CSizes;*/
	CConfig(void);
	~CConfig(void);
	bool Load(CString RootReg);
	void Save();
	void SetPaths(CString ModulePath);
	BOOL LinksAsDir();
};
