#include "StdAfx.h"
#include "Config.h"

CConfig::CConfig(void)
{
	hHandle = 0;
}

CConfig::~CConfig(void)
{
	Save();
}

bool CConfig::InitHandle()
{
	if(hHandle)	
		return true;
	struct FarSettingsCreate sc;
	sc.StructSize = sizeof(sc);
	sc.Guid = MainGuid;
	sc.Handle = INVALID_HANDLE_VALUE;
	fInfo.SettingsControl(INVALID_HANDLE_VALUE, SCTL_CREATE, PSL_ROAMING, &sc);
	hHandle = (sc.Handle == INVALID_HANDLE_VALUE ? 0 : sc.Handle);
	return (hHandle != 0);
}

void CConfig::FreeHandle()
{
	if(hHandle)
		fInfo.SettingsControl(hHandle,SCTL_FREE,0,0);
	hHandle = 0;
}

void CConfig::Set(const wchar_t* Name, int Value)
{
  FarSettingsItem item={sizeof(FarSettingsItem), 0, Name, FST_QWORD, {0}};
  item.Number=Value;
  fInfo.SettingsControl(hHandle, SCTL_SET, 0, &item);
}

void CConfig::Set(const wchar_t* Name, CString &Value)
{
  FarSettingsItem item={sizeof(FarSettingsItem), 0, Name, FST_STRING, {0}};
  item.String = Value;
  fInfo.SettingsControl(hHandle, SCTL_SET, 0, &item);
}

void CConfig::Get(const wchar_t* Name, int &Value, int Default)
{
  FarSettingsItem item={sizeof(FarSettingsItem), 0, Name, FST_QWORD, {0}};
  if(fInfo.SettingsControl(hHandle, SCTL_GET, 0, &item))
	  Value=(int)item.Number;
  else
	  Value = Default;
}

void CConfig::Get(const wchar_t* Name, CString &Value, const wchar_t *Default)
{
  FarSettingsItem item={sizeof(FarSettingsItem), 0, Name, FST_STRING, {0}};
  if(fInfo.SettingsControl(hHandle, SCTL_GET, 0, &item))
	  Value = item.String;
  else
	  Value = Default;
}

bool CConfig::Load() 
{
	if(!InitHandle()) 
		return false;
	Get(_T("PanelMode"), PanelMode, 1);
	Get(_T("SortMode"), SortMode, 1);
	Get(_T("SortOrder"), SortOrder, 0);
	Get(_T("WorkMode"), WorkMode, WORKMODE_SAFE);
	Get(_T("ShowLinksAsDir"), ShowLinksAsDir, FALSE);
	Get(_T("ShowAllPartitions"), ShowAllPartitions, FALSE);
	Get(_T("UseSU"), UseSU, FALSE);
	Get(_T("UseExtendedAccess"), UseExtendedAccess, FALSE);
	Get(_T("AddToDiskMenu"), AddToDiskMenu, FALSE);
	Get(_T("TimeOut"), TimeOut, 1000);
	Get(_T("RemountSystem"), RemountSystem, FALSE);
	Get(_T("Prefix"), Prefix, _T("fardroid"));
	Get(_T("ADBPath"), ADBPath, _T(""));
	FreeHandle();
	return true;
}

void CConfig::Save()
{
	if(!InitHandle()) 
		return;
	Set(_T("PanelMode"),			PanelMode);
	Set(_T("SortMode"),				SortMode);
	Set(_T("SortOrder"),			SortOrder);
	Set(_T("Prefix"),				Prefix);
	Set(_T("ADBPath"),				ADBPath);
	Set(_T("WorkMode"),				WorkMode);
	Set(_T("AddToDiskMenu"),		AddToDiskMenu);
	Set(_T("ShowLinksAsDir"),		ShowLinksAsDir);
	Set(_T("ShowAllPartitions"),	ShowAllPartitions);
	Set(_T("UseSU"),				UseSU);
	Set(_T("UseExtendedAccess"),	UseExtendedAccess);
	Set(_T("TimeOut"),				TimeOut);
	Set(_T("RemountSystem"),		RemountSystem);
	FreeHandle();
}

BOOL CConfig::LinksAsDir()
{
	if (WorkMode == WORKMODE_BUSYBOX)
		return ShowLinksAsDir;
	if (WorkMode == WORKMODE_SAFE)
		return TRUE;

	return FALSE;
}