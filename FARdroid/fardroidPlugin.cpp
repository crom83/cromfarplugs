// fardroid2.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "fardroidPlugin.h"


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

void   WINAPI _export SetStartupInfoW(const struct PluginStartupInfo *Info)
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
	}
}
void   WINAPI _export GetPluginInfoW(struct PluginInfo *Info)
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

	static const wchar_t *	DiskMenuString[1];
	static int		DiskMenuNumber[1];

	DiskMenuString[0] = GetMsg(MTitle);
	DiskMenuNumber[0] = conf.HotKeyInDisk;
	if (conf.AddToDiskMenu)
	{
		Info->DiskMenuStrings = DiskMenuString;
		Info->DiskMenuNumbers = DiskMenuNumber;
		Info->DiskMenuStringsNumber = 1;
	}
	else
	{
		Info->DiskMenuStrings = 0;
		Info->DiskMenuNumbers = 0;
		Info->DiskMenuStringsNumber = 0;
	}
}
HANDLE WINAPI _export OpenPluginW(int OpenFrom,INT_PTR Item)
{
	if(IsOldFAR)
		return INVALID_HANDLE_VALUE;

	switch(OpenFrom)
	{
		case OPEN_DISKMENU: 
		case OPEN_PLUGINSMENU: 
			{
				fardroid * android = new fardroid();
				if (android)
				{
					HANDLE res = INVALID_HANDLE_VALUE;
					res = android->OpenFromMainMenu();

					if (res == INVALID_HANDLE_VALUE)
						delete android;

					return res;
				}
				break;
			}
		case OPEN_COMMANDLINE:
			{
				fardroid * android = new fardroid();
				if (android)
				{
					HANDLE res = INVALID_HANDLE_VALUE;
					CString cmd = (const wchar_t*)Item;
					res = android->OpenFromCommandLine(cmd);
					if (res == INVALID_HANDLE_VALUE)
						delete android;

					return res;
				}
				break;
			}
	}
	return INVALID_HANDLE_VALUE;
}
void   WINAPI _export ClosePluginW(HANDLE hPlugin)
{
	if(IsOldFAR || !hPlugin)
		return;
	fardroid * android = (fardroid *)hPlugin;
	delete android;
}
int    WINAPI _export ConfigureW(int ItemNumber)
{
	if(IsOldFAR)
		return FALSE;

	struct InitDialogItem InitItems[]={
		/*00*/FDI_DOUBLEBOX (47,21,(farStr *)MConfTitle),
		/*01*/FDI_CHECK			( 2, 2,(farStr *)MConfAddToDisk),
		/*02*/FDI_FIXEDIT   ( 4, 3, 4, NULL),
		/*03*/FDI_LABEL     ( 6, 3,(farStr *)MConfAddToDiskLabel),
		/*04*/FDI_LABEL			( 2, 5, (farStr *)MConfPrefix),
		/*05*/FDI_EDIT			(10, 5,20, _F("fardroidPrefix")),
		/*06*/FDI_RADIO			( 2, 7, (farStr*)MConfSafeMode),
		/*07*/FDI_RADIO			( 2, 8, (farStr*)MConfNative),
		/*08*/FDI_RADIO			( 2, 9, (farStr*)MConfBusybox),
		/*09*/FDI_CHECK			( 4,10, (farStr*)MConfShowLinksAsDirs),
		/*10*/FDI_CHECK			( 2,12, (farStr*)MConfShowAllFilesystems),
		/*11*/FDI_CHECK			( 2,13, (farStr*)MConfUseSU),
		/*12*/FDI_CHECK			( 2,14, (farStr*)MConfRemountSystem),
		/*13*/FDI_LABEL			( 2,15, (farStr*)MConfTimeout),
		/*14*/FDI_EDIT			(10,15,20, _F("fardroidTimeout")),
		/*15*/FDI_LABEL			( 2,17, (farStr*)MConfADBPath),
		/*16*/FDI_EDIT			(15,17,43, _F("fardroidADBPath")),
		/*17*/FDI_DEFCBUTTON(19,(farStr *)MOk),
		/*18*/FDI_CBUTTON		(19,(farStr *)MCancel)
	};

	FarDialogItem DialogItems[sizeof(InitItems)/sizeof(InitItems[0])];
	InitDialogItems(InitItems,DialogItems,sizeof(InitItems)/sizeof(InitItems[0]));

	DialogItems[1].Selected = conf.AddToDiskMenu;

	wchar_t * editbuf1 = (wchar_t *)my_malloc(100);
	FSF.sprintf(editbuf1, L"%d",conf.HotKeyInDisk);
	DialogItems[2].PtrData = editbuf1;

	wchar_t * editbuf2 = (wchar_t *)my_malloc(100);
	lstrcpyW(editbuf2, conf.Prefix);
	DialogItems[5].PtrData = editbuf2;

	DialogItems[6].Selected = conf.WorkMode == WORKMODE_SAFE;
	DialogItems[7].Selected = conf.WorkMode == WORKMODE_NATIVE;
	DialogItems[8].Selected = conf.WorkMode == WORKMODE_BUSYBOX;
	DialogItems[9].Selected = conf.ShowLinksAsDir;
	DialogItems[10].Selected = conf.ShowAllPartitions;
	DialogItems[11].Selected = conf.UseSU;
	DialogItems[12].Selected = conf.RemountSystem;

	wchar_t * editbuf3 = (wchar_t *)my_malloc(100);
	FSF.sprintf(editbuf3, L"%d", conf.TimeOut);
	DialogItems[14].PtrData = editbuf3;

	wchar_t * editbuf4 = (wchar_t *)my_malloc(1024);
	lstrcpyW(editbuf4, conf.ADBPath);
	DialogItems[16].PtrData = editbuf4;

	HANDLE hdlg;

	int res = ShowDialog(47,21, _F("Config"), DialogItems, sizeof(InitItems)/sizeof(InitItems[0]), hdlg);
	if (res == 17)
	{
		conf.AddToDiskMenu = GetItemSelected(hdlg, 1);
		conf.HotKeyInDisk= FSF.atoi(GetItemData(hdlg, 2));

		conf.Prefix = GetItemData(hdlg, 5);
		if (GetItemSelected(hdlg, 6))
			conf.WorkMode = WORKMODE_SAFE;
		else if (GetItemSelected(hdlg, 7))
			conf.WorkMode = WORKMODE_NATIVE;
		if (GetItemSelected(hdlg, 8))
			conf.WorkMode = WORKMODE_BUSYBOX;

		conf.ShowLinksAsDir	= GetItemSelected(hdlg, 9);
		conf.ShowAllPartitions	= GetItemSelected(hdlg, 10);
		conf.UseSU = GetItemSelected(hdlg, 11);
		conf.RemountSystem = GetItemSelected(hdlg, 12);

		conf.TimeOut = FSF.atoi(GetItemData(hdlg, 14));

		conf.ADBPath = GetItemData(hdlg, 16);

		conf.Save();
	}
	fInfo.DialogFree(hdlg);
	my_free(editbuf1);
	my_free(editbuf2);
	my_free(editbuf3);
	my_free(editbuf4);

	return res == 17;
}
void   WINAPI _export GetOpenPluginInfoW(HANDLE hPlugin,struct OpenPluginInfo *Info)
{
	if(IsOldFAR || !hPlugin)
		return;

	fardroid * android = (fardroid *)hPlugin;
	Info->StructSize=sizeof(*Info);
	Info->Flags = OPIF_USEFILTER|OPIF_USEHIGHLIGHTING|OPIF_ADDDOTS|OPIF_SHOWPRESERVECASE|OPIF_USESORTGROUPS;

	Info->StartSortMode = conf.SortMode;
	Info->StartSortOrder = conf.SortOrder;

	Info->CurDir = NULL;
	Info->Format= GetMsg(MTitle);

	android->PreparePanel(Info);

	Info->DescrFiles=NULL;
	Info->DescrFilesNumber=0;

	Info->StartPanelMode = _F('0')+conf.PanelMode;
	Info->PanelModesArray=NULL;
	Info->PanelModesNumber=0;

	Info->KeyBar=&KeyBar;
}
int    WINAPI _export GetFindDataW(HANDLE hPlugin,struct PluginPanelItem **pPanelItem,int *pItemsNumber,int OpMode)
{
	if(IsOldFAR || !hPlugin)
		return FALSE;

	fardroid * android = (fardroid *)hPlugin;

	return android->GetFindData(pPanelItem, pItemsNumber, OpMode);
}
void   WINAPI _export FreeFindDataW(HANDLE hPlugin,struct PluginPanelItem *PanelItem,int ItemsNumber)
{
	if(IsOldFAR || !hPlugin)
		return;

	fardroid * android = (fardroid *)hPlugin;
	android->FreeFindData(PanelItem, ItemsNumber);
}
int    WINAPI _export ProcessKeyW(HANDLE hPlugin,int Key,unsigned int ControlState)
{
	if(IsOldFAR || !hPlugin)
		return FALSE;

	fardroid * android = (fardroid *)hPlugin;

	if (Key & PKF_PREPROCESS) return FALSE;

	switch (ControlState)
	{
	case (PKF_CONTROL):
		if(Key == _F('R'))
		{
			android->Reread();
			fInfo.Control(hPlugin, FCTL_UPDATEPANEL, 1, NULL);
			fInfo.Control(hPlugin, FCTL_REDRAWPANEL, 0, NULL);
			return TRUE;
		}
		return FALSE;

	case (PKF_CONTROL|PKF_SHIFT):
		if(Key == VK_F9)
		{
			// Вызываем диалог изменения прав доступа для файлов или директорий
			android->ChangePermissionsDialog();
			return TRUE;
		}
		return FALSE;

	case (PKF_SHIFT):
		return FALSE;
	case (PKF_ALT):
		return FALSE;
	case (PKF_ALT|PKF_SHIFT):
		return FALSE;
	default:
		return FALSE;
	}
	return FALSE;
}
int    WINAPI _export ProcessEventW(HANDLE hPlugin,int Event,void *Param)
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
int    WINAPI _export GetFilesW(HANDLE hPlugin,struct PluginPanelItem *PanelItem,int ItemsNumber,int Move,const wchar_t **DestPath,int OpMode)
{
	if(IsOldFAR || !hPlugin)
		return FALSE;

	fardroid * android = (fardroid *)hPlugin;
	static CString dest;
	dest = *DestPath;
	if (!android->GetFiles(PanelItem, ItemsNumber, dest, Move, OpMode))
		return FALSE;

	if (Move)
	{
		//после удачного копирования удалим файлы
		OpMode |= OPM_SILENT;
		if (!android->DeleteFiles(PanelItem, ItemsNumber, OpMode))
			return FALSE;
	}
	*DestPath = _C(dest);

	return TRUE;
}

int    WINAPI _export SetDirectoryW(HANDLE hPlugin,const wchar_t *Dir,int OpMode)
{
	fardroid * android = (fardroid *)hPlugin;
	if (android)
		return android->ChangeDir(Dir, OpMode);
	return FALSE;
}

int    WINAPI _export PutFilesW( HANDLE hPlugin,struct PluginPanelItem *PanelItem,int ItemsNumber,int Move,const wchar_t *SrcPath,int OpMode )
{
	if(IsOldFAR || !hPlugin)
		return FALSE;

	fardroid * android = (fardroid *)hPlugin;
	if (!android->PutFiles(PanelItem, ItemsNumber, SrcPath, Move, OpMode))
		return FALSE;

	if (Move)
	{
		CString sPath = GetPanelPath();
		return DeletePanelItems(sPath, PanelItem, ItemsNumber);
	}
	return TRUE;
}

int    WINAPI _export DeleteFilesW(HANDLE hPlugin,struct PluginPanelItem *PanelItem,int ItemsNumber,int OpMode)
{
	if(IsOldFAR || !hPlugin)
		return FALSE;

	fardroid * android = (fardroid *)hPlugin;
	return android->DeleteFiles(PanelItem, ItemsNumber, OpMode);
}
int    WINAPI _export MakeDirectoryW(HANDLE hPlugin,const wchar_t **Name,int OpMode)
{
	if(IsOldFAR || !hPlugin)
		return FALSE;

	fardroid * android = (fardroid *)hPlugin;

	static CString dest;
	dest = *Name;
	if (!android->CreateDir(dest, OpMode))
		return FALSE;
	*Name = _C(dest);

	return TRUE;
}