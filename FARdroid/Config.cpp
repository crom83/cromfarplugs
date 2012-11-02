#include "StdAfx.h"
#include "Config.h"

CConfig::CConfig(void)
{
	FromReg = TRUE;
}

CConfig::~CConfig(void)
{
	Save();
}

bool CConfig::Load(CString RootReg) 
{
	PluginRootKey.Format(_T("%s%s"), RootReg, _T("\\CrOm\\fardroid"));

	FromReg = GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("FromReg"), TRUE);

	if (FromReg)
	{
		PanelMode					= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("PanelMode"),			1);
		SortMode					= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("SortMode"),				1);
		SortOrder					= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("SortOrder"),			0);
		WorkMode					= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("WorkMode"),			WORKMODE_SAFE);
		HotKeyInDisk			= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("HotKeyInDisk"),			_T('0'));
		ShowLinksAsDir		= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("ShowLinksAsDir"),	FALSE);
		ShowAllPartitions	= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("ShowAllPartitions"),	FALSE);
		UseSU							= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("UseSU"),	FALSE);
		AddToDiskMenu			= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("AddToDiskMenu"),	FALSE);
		TimeOut						= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("TimeOut"),	1000);
		RemountSystem			= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("RemountSystem"),	FALSE);
		GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("Prefix"), Prefix, _T("fardroid"));
		GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("ADBPath"), ADBPath, _T(""));
		/*GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("Mode1CTypes"), Mode1CTypes, _T("N,C0"));
		GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("Mode1CSizes"), Mode1CSizes, _T("0,9"));
		GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("Mode2CTypes"), Mode2CTypes, _T("N,C0,Z"));
		GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("Mode2CSizes"), Mode2CSizes, _T("0,9,15"));*/
		return true;
	}
	return false;
}
void CConfig::SetPaths( CString modulePath ) 
{
	ModulePath = ExtractPath(modulePath);
}

void CConfig::Save()
{
	SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("FromReg"),	FromReg);

	if (FromReg)
	{
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("PanelMode"),					PanelMode);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("SortMode"),						SortMode);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("SortOrder"),					SortOrder);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("HotKeyInDisk"),				HotKeyInDisk);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("Prefix"),							Prefix);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("ADBPath"),						ADBPath);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("WorkMode"),						WorkMode);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("AddToDiskMenu"),			AddToDiskMenu);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("ShowLinksAsDir"),			ShowLinksAsDir);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("ShowAllPartitions"),	ShowAllPartitions);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("UseSU"),							UseSU);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("TimeOut"),						TimeOut);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("RemountSystem"),			RemountSystem);
	}
}

BOOL CConfig::LinksAsDir()
{
	if (WorkMode == WORKMODE_BUSYBOX)
		return ShowLinksAsDir;
	if (WorkMode == WORKMODE_SAFE)
		return TRUE;

	return FALSE;
}