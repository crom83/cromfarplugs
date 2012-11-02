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
#include "includes\FARUnicode\farkeys.hpp"
#define _F(x)     L ## x
#define farStr wchar_t
#define lstrlenF lstrlenW
#define lstrcpyF lstrcpyW
#define lstrcatF lstrcatW
#define lstrcmpF lstrcmpW

#define _C(x)      (wchar_t*)(LPCTSTR)##x

//#include <math.h>

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
#include "reg.hpp"
#include "Config.h"

#include "fardroid.h"

extern PluginStartupInfo fInfo;
extern FarStandardFunctions FSF;
extern BOOL IsOldFAR;
extern HMODULE hInst;
extern CConfig conf;

extern struct KeyBarTitles KeyBar;
