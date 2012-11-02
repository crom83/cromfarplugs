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
	PluginRootKey.Format(_T("%s%s"), RootReg, _T("\\CrOm\\operac4"));

	FromReg = GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("FromReg"), TRUE);

	if (FromReg)
	{
		PanelMode				= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("PanelMode"),			1);
		SortMode				= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("SortMode"),				1);
		SortOrder				= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("SortOrder"),			0);
		UserWorkFolder	= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("UserWorkFolder"),	FALSE);
		ShortNames			= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("ShortNames"),			FALSE);
		UseExtensions		= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("UseExtensions"),	TRUE);
		UseCachedName		= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("UseCachedName"),	FALSE);
		UseOpenFile			= GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("UseOpenFile"),	TRUE);
		GetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("Prefix"), Prefix, _T("opera"));
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
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("PanelMode"),			PanelMode);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("SortMode"),				SortMode);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("SortOrder"),			SortOrder);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("UserWorkFolder"),	UserWorkFolder);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("Prefix"),					Prefix);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("ShortNames"),			ShortNames);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("UseExtensions"),	UseExtensions);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("UseCachedName"),	UseCachedName);
		SetRegKey(HKEY_CURRENT_USER,FALSE, _T(""),_T("UseOpenFile"),		UseOpenFile);
	}
}