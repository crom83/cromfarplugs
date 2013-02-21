// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:

//#define _ATL_MIN_CRT 
#include <windows.h>
#include <shellapi.h>
#include <atlbase.h>
#include <atlapp.h>
#define _WTL_USE_CSTRING
#include <atlmisc.h>
#include "SortedArray.h"

typedef CSimpleMap<CString, CString> strmap;
typedef CSortedArray<CString, CString&> strvec;
typedef CSortedArray<int, int&> simpvec;

//FAR includes
#define _FAR_USE_FARFINDDATA
#include "includes\FARUnicode\eplugin.hpp"
#include "includes\FARUnicode\plugin.hpp"
#include "includes\FARUnicode\farcolor.hpp"

#define _F(x)     L ## x
#define farStr wchar_t
#define lstrlenF lstrlenW
#define lstrcpyF lstrcpyW
#define lstrcatF lstrcatW
#define lstrcmpF lstrcmpW

#define _C(x)      (wchar_t*)(LPCTSTR)##x


#include "version.info"

#include "lang.hpp"

#define IS_FLAG( val,flag )       (((val)&(flag))==(flag))
#define SET_FLAG( val,flag )      (val |= (flag))
#define CLR_FLAG( val,flag )      (val &= ~(flag))

#define P_Copy1 _F("operac4 FAR plugin, (c) 2003-2008 Vladimir Kubyshev")

#define INFOLINES 1
//#define USELOGGING

#include "myrtl.h"
#include "mystr.h"
#include "stuffs.h"
#include "FARstuffs.h"
#include "Config.h"

#include "fardroid.h"

extern PluginStartupInfo fInfo;
extern FarStandardFunctions FSF;
extern HMODULE hInst;
extern CConfig conf;

extern struct KeyBarTitles KeyBar;
#include <initguid.h>
// {B4C0E3A4-E340-4931-8951-ED2EE3846FA0}
DEFINE_GUID(MainGuid, 0xb4c0e3a4, 0xe340, 0x4931, 0x89, 0x51, 0xed, 0x2e, 0xe3, 0x84, 0x6f, 0xa0);
// {430613E6-CEB0-4c2a-BFD6-E0526B3D452F}
DEFINE_GUID(MsgGuid, 0x430613e6, 0xceb0, 0x4c2a, 0xbf, 0xd6, 0xe0, 0x52, 0x6b, 0x3d, 0x45, 0x2f);
// {7C5EE147-FB83-477b-91D8-731F5C563C32}
DEFINE_GUID(MsgWaitGuid, 0x7c5ee147, 0xfb83, 0x477b, 0x91, 0xd8, 0x73, 0x1f, 0x5c, 0x56, 0x3c, 0x32);
// {28E513F6-E9EF-4a85-9B95-6AB129B04492}
DEFINE_GUID(DialogGuid, 0x28e513f6, 0xe9ef, 0x4a85, 0x9b, 0x95, 0x6a, 0xb1, 0x29, 0xb0, 0x44, 0x92);
// {D63502EB-0081-4dc3-B203-564959CC3194}
DEFINE_GUID(MenuGuid, 0xd63502eb, 0x81, 0x4dc3, 0xb2, 0x3, 0x56, 0x49, 0x59, 0xcc, 0x31, 0x94);
