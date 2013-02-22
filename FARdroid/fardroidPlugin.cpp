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

void WINAPI GetGlobalInfoW(struct GlobalInfo *Info)
{
	Info->StructSize=sizeof(struct GlobalInfo);
	Info->MinFarVersion=MAKEFARVERSION(3,0,0,3000,VS_RELEASE); 
	Info->Version=MAKEFARVERSION(1,0,0,44,VS_RC);
	Info->Guid=MainGuid;
	Info->Title=L"FARdroid";
	Info->Description=L"fardroid FAR Plugin";
	Info->Author=L"Vladimir Kubyshev";
}

void   WINAPI  SetStartupInfoW(const struct PluginStartupInfo *Info)
{
	if(Info->StructSize >= sizeof(struct PluginStartupInfo))
	{
		fInfo = *Info;
		FSF = *Info->FSF;
		fInfo.FSF = &FSF;
		conf.Load();
	}
}
void   WINAPI  GetPluginInfoW(struct PluginInfo *Info)
{
	Info->StructSize = sizeof(PluginInfo);
	Info->Flags=0;

	static const wchar_t * PluginConfigMenuStrings[1], *PluginMenuStrings[1];

	PluginConfigMenuStrings[0]=GetMsg(MConfTitle);
	Info->PluginConfig.Strings=PluginConfigMenuStrings;
	Info->PluginConfig.Count = 1;
	Info->PluginConfig.Guids = &MenuGuid;

	PluginMenuStrings[0]=GetMsg(MTitle);
	Info->PluginMenu.Strings=PluginMenuStrings;
	Info->PluginMenu.Count = 1;
	Info->PluginMenu.Guids = &MenuGuid;

	Info->CommandPrefix= _C(conf.Prefix);

	static const wchar_t *	DiskMenuString[1];
	static int		DiskMenuNumber[1];

	DiskMenuString[0] = GetMsg(MTitle);
	DiskMenuNumber[0] = conf.HotKeyInDisk;
	if (conf.AddToDiskMenu)
	{
		Info->DiskMenu.Strings = DiskMenuString;
		Info->DiskMenu.Count = 1;
		Info->DiskMenu.Guids = &MenuGuid;
	}
	else
	{
		Info->DiskMenu.Strings = 0;
		Info->DiskMenu.Count = 0;
		Info->DiskMenu.Guids = 0;
	}
}

HANDLE WINAPI OpenW(const struct OpenInfo *Info)
{
	switch(Info->OpenFrom)
	{
		case OPEN_LEFTDISKMENU: 
		case OPEN_RIGHTDISKMENU: 
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
					CString cmd = reinterpret_cast<OpenCommandLineInfo*>(Info->Data)->CommandLine;
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

void WINAPI ClosePanelW(const struct ClosePanelInfo *Info)
{
	if(!Info->hPanel || INVALID_HANDLE_VALUE == Info->hPanel)
		return;
	fardroid * android = (fardroid *)Info->hPanel;
	delete android;
}

intptr_t WINAPI ConfigureW(const struct ConfigureInfo *Info)
{
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
	DialogItems[2].Data = editbuf1;

	wchar_t * editbuf2 = (wchar_t *)my_malloc(100);
	lstrcpyW(editbuf2, conf.Prefix);
	DialogItems[5].Data = editbuf2;

	DialogItems[6].Selected = conf.WorkMode == WORKMODE_SAFE;
	DialogItems[7].Selected = conf.WorkMode == WORKMODE_NATIVE;
	DialogItems[8].Selected = conf.WorkMode == WORKMODE_BUSYBOX;
	DialogItems[9].Selected = conf.ShowLinksAsDir;
	DialogItems[10].Selected = conf.ShowAllPartitions;
	DialogItems[11].Selected = conf.UseSU;
	DialogItems[12].Selected = conf.RemountSystem;

	wchar_t * editbuf3 = (wchar_t *)my_malloc(100);
	FSF.sprintf(editbuf3, L"%d", conf.TimeOut);
	DialogItems[14].Data = editbuf3;

	wchar_t * editbuf4 = (wchar_t *)my_malloc(1024);
	lstrcpyW(editbuf4, conf.ADBPath);
	DialogItems[16].Data = editbuf4;

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

void WINAPI GetOpenPanelInfoW(struct OpenPanelInfo *Info)
{
	
	if(!Info->hPanel || INVALID_HANDLE_VALUE == Info->hPanel)
		return;

	fardroid * android = (fardroid *)Info->hPanel;
	Info->StructSize=sizeof(*Info);
	Info->Flags = OPIF_ADDDOTS | OPIF_SHOWPRESERVECASE;

	Info->StartSortMode = (OPENPANELINFO_SORTMODES)	conf.SortMode;
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
intptr_t WINAPI GetFindDataW(struct GetFindDataInfo *Info)
{
	if(!Info->hPanel || INVALID_HANDLE_VALUE == Info->hPanel)
		return FALSE;

	fardroid * android = (fardroid *)Info->hPanel;

	return android->GetFindData(&Info->PanelItem, &Info->ItemsNumber, (int)Info->OpMode);
}
void     WINAPI FreeFindDataW(const struct FreeFindDataInfo *Info)
{
	if(!Info->hPanel || INVALID_HANDLE_VALUE == Info->hPanel)
		return;

	fardroid * android = (fardroid *)Info->hPanel;
	
	android->FreeFindData(Info->PanelItem, Info->ItemsNumber);
}
intptr_t WINAPI ProcessPanelInputW(const struct ProcessPanelInputInfo *Info)
{
	
	if(!Info->hPanel || INVALID_HANDLE_VALUE == Info->hPanel)
		return FALSE;

	fardroid * android = (fardroid *)Info->hPanel;
	
	if(Info->Rec.EventType != KEY_EVENT) return FALSE;

	DWORD dwControl = Info->Rec.Event.KeyEvent.dwControlKeyState;
	DWORD dwCTRL = dwControl & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED);
	DWORD dwALT  = dwControl & (LEFT_ALT_PRESSED  | RIGHT_ALT_PRESSED);
	DWORD dwSHIFT  = dwControl & SHIFT_PRESSED;
	if(dwALT || (dwCTRL==0)) return FALSE;
	if(dwSHIFT == 0)
	{
		if(Info->Rec.Event.KeyEvent.wVirtualKeyCode == 0x52 /* VK_R */)
		{
			android->Reread();
			fInfo.PanelControl(Info->hPanel, FCTL_UPDATEPANEL, 1, NULL);
			fInfo.PanelControl(Info->hPanel, FCTL_REDRAWPANEL, 0, NULL);
			return TRUE;
		}
	}
	else
	{
		if(Info->Rec.Event.KeyEvent.wVirtualKeyCode == VK_F9)
		{
			// Вызываем диалог изменения прав доступа для файлов или директорий
			android->ChangePermissionsDialog();
			return TRUE;
		}
	}
	return FALSE;
}
intptr_t WINAPI ProcessPanelEventW(const struct ProcessPanelEventInfo *Info)
{
	
	if(!Info->hPanel || INVALID_HANDLE_VALUE == Info->hPanel)
		return FALSE;

	switch(Info->Event)
	{
	case FE_CLOSE:

		PanelInfo PInfo;
		fInfo.PanelControl(Info->hPanel, FCTL_GETPANELINFO, 0, (void *)&PInfo);
		conf.SortMode = PInfo.SortMode;
		conf.PanelMode = PInfo.ViewMode;
		conf.SortOrder = IS_FLAG(PInfo.Flags, PFLAGS_REVERSESORTORDER);
		conf.Save();

		break;
	}

	return FALSE;
}

intptr_t WINAPI GetFilesW(struct GetFilesInfo *Info)
{
	if(!Info->hPanel || INVALID_HANDLE_VALUE == Info->hPanel)
		return FALSE;

	fardroid * android = (fardroid *)Info->hPanel;
	static CString dest;
	dest = Info->DestPath;
	if (!android->GetFiles(Info->PanelItem, Info->ItemsNumber, dest, Info->Move,  (int)Info->OpMode))
		return FALSE;

	if (Info->Move)
	{
		//после удачного копирования удалим файлы
		Info->OpMode |= OPM_SILENT;
		if (!android->DeleteFiles(Info->PanelItem, Info->ItemsNumber, (int)Info->OpMode))
			return FALSE;
	}
	Info->DestPath = _C(dest);

	return TRUE;
}

intptr_t WINAPI SetDirectoryW(const struct SetDirectoryInfo *Info)
{
	fardroid * android = (fardroid *)Info->hPanel;
	if (android)
		return android->ChangeDir(Info->Dir, (int)Info->OpMode);
	return FALSE;
}

intptr_t WINAPI PutFilesW(const struct PutFilesInfo *Info)
{
	if(!Info->hPanel || INVALID_HANDLE_VALUE == Info->hPanel)
		return FALSE;

	fardroid * android = (fardroid *)Info->hPanel;
	if (!android->PutFiles(Info->PanelItem, Info->ItemsNumber, Info->SrcPath, Info->Move, (int)Info->OpMode))
		return FALSE;

	if (Info->Move)
	{
		CString sPath = GetPanelPath();
		return DeletePanelItems(sPath, Info->PanelItem, Info->ItemsNumber);
	}
	return TRUE;
}

intptr_t WINAPI DeleteFilesW(const struct DeleteFilesInfo *Info)
{
	if(!Info->hPanel || INVALID_HANDLE_VALUE == Info->hPanel)
		return FALSE;

	fardroid * android = (fardroid *)Info->hPanel;
	return android->DeleteFiles(Info->PanelItem, Info->ItemsNumber, (int)Info->OpMode);
}

intptr_t WINAPI MakeDirectoryW(struct MakeDirectoryInfo *Info)
{
	if(!Info->hPanel || INVALID_HANDLE_VALUE == Info->hPanel)
		return FALSE;

	fardroid * android = (fardroid *)Info->hPanel;

	static CString dest;
	dest = Info->Name;
	if (!android->CreateDir(dest, (int)Info->OpMode))
		return FALSE;
	Info->Name = _C(dest);

	return TRUE;
}