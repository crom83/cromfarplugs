// stdafx.cpp : source file that includes just the standard includes
// fardroid2.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

PluginStartupInfo fInfo;
FarStandardFunctions FSF;
BOOL IsOldFAR;
HMODULE hInst;
CConfig conf;

PanelMode PanelModesArray[2];
const farStr *ColumnTitles1[2];
const farStr *ColumnTitles2[3];

wchar_t * Permissions = L"Permissions";

struct KeyBarTitles KeyBar={
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},//Titles
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},//Ctrl
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},//Alt
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},//Shift
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,Permissions,NULL,NULL,NULL},//CtrlShift
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},//AltShift
	{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},//CtrlAlt
};
