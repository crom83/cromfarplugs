#ifndef __REG
#define __REG

//работа с реестром
void SetRegKey(HKEY hRoot,BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,DWORD ValueData);
void SetRegKey(HKEY hRoot,BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,const CString& ValueData);
int  GetRegKey(HKEY hRoot,BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,int &ValueData,DWORD Default);
int  GetRegKey(HKEY hRoot,BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,DWORD Default);
int  GetRegKey(HKEY hRoot,BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,CString &ValueData,TCHAR *Default);
int  GetRegKey(HKEY hRoot,BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,BYTE *ValueData,BYTE *Default,DWORD DataSize);
void SetRegKey(HKEY hRoot,BOOL bEx, const TCHAR *Key,const TCHAR *ValueName,BYTE *ValueData,DWORD ValueSize);

#endif