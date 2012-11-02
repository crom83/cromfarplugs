#include "stdafx.h"
#include "reg.hpp"

TCHAR *FmtSSS=_T("%s%s%s");

static CString FullKeyName;

HKEY CreateRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key);
HKEY OpenRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key);

void SetRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,BYTE *ValueData,DWORD ValueSize)
{
  HKEY hKey = CreateRegKey(hRoot, bEx, Key);
  RegSetValueEx(hKey,ValueName,0,REG_BINARY,ValueData,ValueSize);
  RegCloseKey(hKey);
}

void SetRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,DWORD ValueData)
{
  HKEY hKey=CreateRegKey(hRoot, bEx, Key);
  RegSetValueEx(hKey,ValueName,0,REG_DWORD,(BYTE *)&ValueData,sizeof(ValueData));
  RegCloseKey(hKey);
}


void SetRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,const CString &ValueData)
{
  HKEY hKey=CreateRegKey(hRoot, bEx, Key);
  RegSetValueEx(hKey,ValueName,0,REG_SZ,(const BYTE*)(LPCTSTR)ValueData,(ValueData.GetLength()+1)*sizeof(TCHAR));
  RegCloseKey(hKey);
}


int GetRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,int &ValueData,DWORD Default)
{
  HKEY hKey=OpenRegKey(hRoot, bEx, Key);
  DWORD Type,Size=sizeof(ValueData);
  int ExitCode=RegQueryValueEx(hKey,ValueName,0,&Type,(BYTE *)&ValueData,&Size);
  RegCloseKey(hKey);
  if (hKey==NULL || ExitCode!=ERROR_SUCCESS)
  {
      ValueData=Default;
      return(FALSE);
  }
  return(TRUE);
}


int GetRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,DWORD Default)
{
  int ValueData;
  GetRegKey(hRoot, bEx, Key,ValueName,ValueData,Default);
  return(ValueData);
}


int GetRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,CString &ValueData,TCHAR *Default)
{
  HKEY hKey=OpenRegKey(hRoot, bEx, Key);
  DWORD Type;
	DWORD DataSize = 1000;
  int ExitCode=RegQueryValueEx(hKey,ValueName,0,&Type,(LPBYTE)ValueData.GetBuffer(1000),&DataSize);
	ValueData.ReleaseBuffer();
  RegCloseKey(hKey);
  if (hKey==NULL || ExitCode!=ERROR_SUCCESS)
  {
      ValueData = Default;
      return(FALSE);
  }
  return(TRUE);
}

int GetRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,BYTE *ValueData,BYTE *Default,DWORD DataSize)
{
  HKEY hKey=OpenRegKey(hRoot, bEx, Key);
  DWORD Type,Required=DataSize;
  int ExitCode=RegQueryValueEx(hKey,ValueName,0,&Type,ValueData,&Required);
  RegCloseKey(hKey);
  if (hKey==NULL || ExitCode!=ERROR_SUCCESS)
  {
    if (Default!=NULL)
      my_memcpy(ValueData,Default,DataSize);
    else
      my_memset(ValueData,0,DataSize);
    return(FALSE);
  }
  return(DataSize);
}


HKEY CreateRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key)
{
  HKEY hKey;
  DWORD Disposition;
	CString sKey;
	if (!bEx)
		FullKeyName.Format(FmtSSS,conf.PluginRootKey,*Key ? _T("\\"):_T(""), Key);
	else
		FullKeyName = Key;

  RegCreateKeyEx(hRoot,FullKeyName,0,NULL,0,KEY_WRITE,NULL,
  &hKey,&Disposition);
  return(hKey);
}


HKEY OpenRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key)
{
  HKEY hKey;
	if (!bEx)
		FullKeyName.Format(FmtSSS, conf.PluginRootKey,*Key ? _T("\\"):_T(""), Key);
	else
		FullKeyName = Key;

  if (RegOpenKeyEx(hRoot,FullKeyName,0,KEY_QUERY_VALUE,&hKey)!=ERROR_SUCCESS)
      return(NULL);
  return(hKey);
}

void DeleteRegKey(HKEY hRoot, BOOL bEx, const TCHAR *Key)
{
	if (!bEx)
		FullKeyName.Format(FmtSSS,conf.PluginRootKey,*Key ? _T("\\"):_T(""),Key);
	else
		FullKeyName = Key;

  RegDeleteKey(hRoot,FullKeyName);
}
