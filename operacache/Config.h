#pragma once

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
	BOOL		UserWorkFolder;
	BOOL		UseOpenFile;
	//interface
	int			PanelMode;
	int			SortMode;
	int			SortOrder;
	BOOL		ShortNames;
	BOOL		UseExtensions;
	/*CString Mode1CTypes;
	CString Mode1CSizes;
	CString Mode2CTypes;
	CString Mode2CSizes;*/
	//copy
	BOOL		UseCachedName;
	CConfig(void);
	~CConfig(void);
	bool Load(CString RootReg);
	void Save();
	void SetPaths(CString ModulePath);
};
