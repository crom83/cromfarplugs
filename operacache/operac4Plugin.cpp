// operac42.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "operac4Plugin.h"


BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hInst = hModule;
		::DisableThreadLibraryCalls(hInst);
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void WINAPI _export SetStartupInfoW( const struct PluginStartupInfo *Info ) 
{
	IsOldFAR = TRUE;
	if(Info->StructSize >= sizeof(struct PluginStartupInfo))
	{
		fInfo = *Info;
		FSF = *Info->FSF;
		fInfo.FSF = &FSF;
		IsOldFAR = FALSE;

		conf.SetPaths(fInfo.ModuleName);
		conf.Load(fInfo.RootKey);

		//подготовка заголовков колонок
		ColumnTitles1[0]=GetMsg(MColumnURL);
		ColumnTitles1[1]=GetMsg(MColumnFile);

		PanelModesArray[0].ColumnTypes=_F("N,C0");
		PanelModesArray[0].ColumnWidths=_F("0,20");
		PanelModesArray[0].ColumnTitles=(farStr**)ColumnTitles1;
		PanelModesArray[0].StatusColumnTypes=_F("N,Z");
		PanelModesArray[0].StatusColumnWidths=_F("0,20");
		PanelModesArray[0].AlignExtensions = FALSE;
		PanelModesArray[0].FullScreen=FALSE;

		ColumnTitles2[0]=GetMsg(MColumnURL);
		ColumnTitles2[1]=GetMsg(MColumnFile);
		ColumnTitles2[2]=GetMsg(MColumnType);

		PanelModesArray[1].ColumnTypes=_F("N,C0,Z");
		PanelModesArray[1].ColumnWidths=_F("0,20,15");
		PanelModesArray[1].ColumnTitles=(farStr**)ColumnTitles2;
		PanelModesArray[1].StatusColumnTypes=_F("N,Z");
		PanelModesArray[1].StatusColumnWidths=_F("0,15");
		PanelModesArray[1].AlignExtensions = FALSE;
		PanelModesArray[1].FullScreen=FALSE;

		KeyBar.CtrlTitles[5-1] = (farStr*)GetMsg(MPMCF5);
		KeyBar.CtrlTitles[8-1] = (farStr*)GetMsg(MPMCF8);
		KeyBar.CtrlTitles[9-1] = (farStr*)GetMsg(MPMCF9);
		KeyBar.CtrlTitles[10-1] = (farStr*)GetMsg(MPMCF10);
	}
}
void WINAPI _export GetPluginInfoW( struct PluginInfo *Info ) 
{
	Info->StructSize = sizeof(PluginInfo);

	if(IsOldFAR)
		return;

	Info->Flags=0;

	static const wchar_t * PluginConfigMenuStrings[1], *PluginMenuStrings[1];

	PluginConfigMenuStrings[0]=GetMsg(MConfTitle);
	Info->PluginConfigStrings=PluginConfigMenuStrings;
	Info->PluginConfigStringsNumber = 1;

	PluginMenuStrings[0]=GetMsg(MTitle);
	Info->PluginMenuStrings=PluginMenuStrings;
	Info->PluginMenuStringsNumber = 1;

	Info->CommandPrefix= _C(conf.Prefix);
	Info->DiskMenuStrings=0;
	Info->DiskMenuNumbers=0;
	Info->DiskMenuStringsNumber=0;
}
HANDLE WINAPI _export OpenPluginW( int OpenFrom,INT_PTR Item ) 
{
	if(IsOldFAR)
		return INVALID_HANDLE_VALUE;

	switch(OpenFrom)
	{
		case OPEN_PLUGINSMENU: 
			{
				operac4 * opera = new operac4();
				if (opera)
				{
					HANDLE res = INVALID_HANDLE_VALUE;
					res = opera->OpenFromMainMenu();

					if (res == INVALID_HANDLE_VALUE)
						delete opera;

					return res;
				}
				break;
			}
		case OPEN_COMMANDLINE:
			{
				operac4 * opera = new operac4();
				if (opera)
				{
					HANDLE res = INVALID_HANDLE_VALUE;
					CString cmd = (const wchar_t*)Item;
					res = opera->OpenFromCommandLine(cmd);
					if (res == INVALID_HANDLE_VALUE)
						delete opera;

					return res;
				}
				break;
			}
	}
	return INVALID_HANDLE_VALUE;
}
void		WINAPI _export ClosePluginW( HANDLE hPlugin ) 
{
	if(IsOldFAR || !hPlugin)
		return;
	operac4 * opera = (operac4 *)hPlugin;
	delete opera;
}
int			WINAPI _export ConfigureW( int /*ItemNumber*/ ) 
{
	if(IsOldFAR)
		return FALSE;

	struct InitDialogItem InitItems[]={
		/*00*/FDI_DOUBLEBOX (45,10,(farStr *)MConfTitle),
		/*01*/FDI_LABEL			( 2, 2, (farStr *)MConfPrefix),
		/*02*/FDI_EDIT			(10, 2,20, _F("operac4Prefix")),
		/*03*/FDI_CHECK			( 2, 4, (farStr*)MConfExt),
		/*04*/FDI_CHECK			( 2, 5, (farStr*)MConfFull),
		/*05*/FDI_CHECK			( 2, 6, (farStr*)MConfOpenFile),
		/*06*/FDI_DEFCBUTTON( 8,(farStr *)MOk),
		/*07*/FDI_CBUTTON		( 8,(farStr *)MCancel)
	};

	FarDialogItem DialogItems[sizeof(InitItems)/sizeof(InitItems[0])];
	InitDialogItems(InitItems,DialogItems,sizeof(InitItems)/sizeof(InitItems[0]));

	wchar_t * editbuf = (wchar_t *)my_malloc(100);
	lstrcpyW(editbuf, conf.Prefix);
	DialogItems[2].PtrData = editbuf;

	DialogItems[3].Selected = conf.UseExtensions;
	DialogItems[4].Selected = !conf.ShortNames;
	DialogItems[5].Selected = conf.UseOpenFile;
	HANDLE hdlg;

	int res = ShowDialog(45,10, _F("Config"), DialogItems, sizeof(InitItems)/sizeof(InitItems[0]), hdlg);
	if (res == 6)
	{
		conf.Prefix					= GetItemData(hdlg, 2);
		conf.UseExtensions	= GetItemSelected(hdlg, 3);
		conf.ShortNames			= !GetItemSelected(hdlg, 4);
		conf.UseOpenFile		= GetItemSelected(hdlg, 5);
		conf.Save();
	}
	fInfo.DialogFree(hdlg);
	my_free(editbuf);

	return res == 6;
}
void WINAPI _export GetOpenPluginInfoW(HANDLE hPlugin, OpenPluginInfo *Info)
{
	if(IsOldFAR || !hPlugin)
		return;

	operac4 * opera = (operac4 *)hPlugin;
	Info->StructSize=sizeof(*Info);
	Info->Flags = OPIF_USEFILTER|OPIF_USEHIGHLIGHTING|OPIF_ADDDOTS|OPIF_SHOWPRESERVECASE;

	Info->StartSortMode = conf.SortMode;
	Info->StartSortOrder = conf.SortOrder;

	Info->CurDir = NULL;
	Info->Format= GetMsg(MTitle);

	opera->PreparePanel(Info);

	Info->InfoLines= InfoPanelLineArray;
	Info->InfoLinesNumber = INFOLINES;

	Info->DescrFiles=NULL;
	Info->DescrFilesNumber=0;

	Info->StartPanelMode = _F('0')+conf.PanelMode;
	Info->PanelModesArray=PanelModesArray;
	Info->PanelModesNumber=sizeof(PanelModesArray)/sizeof(PanelModesArray[0]);

	if (conf.ShortNames)
		KeyBar.Titles[7-1] = (farStr*)GetMsg(MPMF7_1);
	else
		KeyBar.Titles[7-1] = (farStr*)GetMsg(MPMF7_2);

	Info->KeyBar=&KeyBar;
}
int WINAPI _export GetFindDataW(HANDLE hPlugin, struct PluginPanelItem **pPanelItem,int *pItemsNumber,int OpMode)
{
	if(IsOldFAR || !hPlugin)
		return FALSE;

	operac4 * opera = (operac4 *)hPlugin;

	return opera->GetFindData(pPanelItem, pItemsNumber, OpMode);
}
void WINAPI _export FreeFindDataW(HANDLE hPlugin, struct PluginPanelItem *PanelItem,int ItemsNumber)
{
	if(IsOldFAR || !hPlugin)
		return;

	operac4 * opera = (operac4 *)hPlugin;
	opera->FreeFindData(PanelItem, ItemsNumber);
}
int WINAPI _export ProcessKeyW(HANDLE hPlugin, int Key, unsigned int ControlState)
{
	if(IsOldFAR || !hPlugin)
		return FALSE;

	operac4 * opera = (operac4 *)hPlugin;

	if (Key & PKF_PREPROCESS) return FALSE;

	switch (ControlState)
	{
	case (PKF_CONTROL):
		if(Key == _F('R'))
		{
			opera->Reread();
			fInfo.Control(hPlugin, FCTL_UPDATEPANEL, 1, NULL);
			fInfo.Control(hPlugin, FCTL_REDRAWPANEL, 0, NULL);
			return TRUE;
		}
		return FALSE;
	case (PKF_SHIFT):
		if (Key == VK_F5)
		{
			opera->CopyFileWithNewName();
			return TRUE;
		}
		return FALSE;
	case (PKF_ALT):
		return FALSE;
	case (PKF_ALT|PKF_SHIFT):
		return FALSE;
	default:
		if (Key==VK_F7)
		{
			conf.ShortNames = !conf.ShortNames;
			fInfo.Control(hPlugin, FCTL_UPDATEPANEL, 1, NULL);
			fInfo.Control(hPlugin, FCTL_REDRAWPANEL, 0, NULL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}
int WINAPI ProcessEventW(HANDLE hPlugin,	int Event, void *Param)
{
	if(IsOldFAR || !hPlugin)
		return FALSE;

	switch(Event)
	{
	case FE_CLOSE:

		PanelInfo PInfo;
		fInfo.Control(hPlugin, FCTL_GETPANELINFO, 0, (LONG_PTR)&PInfo);
		conf.SortMode = PInfo.SortMode;
		conf.PanelMode = PInfo.ViewMode;
		conf.SortOrder = IS_FLAG(PInfo.Flags, PFLAGS_REVERSESORTORDER);
		conf.Save();

		break;
	}

	return FALSE;
}
int WINAPI _export GetMinFarVersionW(void)
{
	return MAKEFARVERSION(2,0,756);//тут был breaking change
}
int WINAPI GetFilesW(HANDLE hPlugin, PluginPanelItem *PanelItem, int ItemsNumber, int Move,	const wchar_t **DestPath,	int OpMode)
{
	if(IsOldFAR || !hPlugin || Move)
		return FALSE;

	operac4 * opera = (operac4 *)hPlugin;
	static CString dest;
	dest = *DestPath;
	int ret = opera->GetFiles(PanelItem, ItemsNumber, dest, OpMode);
	if (!ret)
		return FALSE;
	*DestPath = _C(dest);
	return ret;
}
HANDLE WINAPI _export OpenFilePluginW(const wchar_t *Name, const unsigned char *Data, int DataSize,int OpMode)
{
	if(IsOldFAR || !Name || !conf.UseOpenFile)
		return INVALID_HANDLE_VALUE;

	operac4 * opera = new operac4();
	if (opera)
	{
		CString filename;
		filename = Name;
		HANDLE res = opera->Open(filename);
		if (res == INVALID_HANDLE_VALUE)
			delete opera;

		return res;
	}

	return INVALID_HANDLE_VALUE;
}