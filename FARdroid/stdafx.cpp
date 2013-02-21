// stdafx.cpp : source file that includes just the standard includes
// fardroid2.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

PluginStartupInfo fInfo;
FarStandardFunctions FSF;
HMODULE hInst;
CConfig conf;

wchar_t * Permissions = L"Permissions";
struct KeyBarLabel Label[2] = {{{VK_F9, SHIFT_PRESSED | LEFT_CTRL_PRESSED}, Permissions, Permissions},
{{VK_F9, SHIFT_PRESSED | RIGHT_CTRL_PRESSED}, Permissions, Permissions}};
struct KeyBarTitles KeyBar = {2, Label};
