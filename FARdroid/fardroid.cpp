#include "StdAfx.h"
#include "fardroid.h"

DWORD WINAPI ProcessThreadProc( LPVOID lpParam ) 
{ 
	fardroid * android = (fardroid *)lpParam;
	if (android)
	{
		while (true)
		{
			if (CheckForKey(VK_ESCAPE) && 
					android->BreakProcessDialog(GetMsg(MGetFile)))
				android->m_bForceBreak = true;

			if (!android->ShowProgressMessage())
				break;

			Sleep(10);
		}
	}

	return 0; 
}


fardroid::fardroid(void)
{
	m_currentPath = _T("");
	InfoPanelLineArray = NULL;
	lastError = S_OK;
	m_bForceBreak = false;
}
fardroid::~fardroid(void)
{
	//conf.Save();
	if (InfoPanelLineArray)
		delete [] InfoPanelLineArray;

	DeleteRecords(records);
}
bool fardroid::CopyFilesDialog(CString &dest) 
{
	struct InitDialogItem InitItemsC[]={
		/*00*/FDI_DOUBLEBOX (55, 9,(farStr *)MGetFile),
		/*01*/FDI_LABEL			( 2, 2, (farStr *)MCopyDest),
		/*02*/FDI_EDIT			( 2, 3,52, _F("fardroidDestinationDir")),
		/*03*/FDI_DEFCBUTTON( 7,(farStr *)MOk),
		/*04*/FDI_CBUTTON		( 7,(farStr *)MCancel)
	};
	FarDialogItem DialogItemsC[sizeof(InitItemsC)/sizeof(InitItemsC[0])];
	InitDialogItems(InitItemsC,DialogItemsC,sizeof(InitItemsC)/sizeof(InitItemsC[0]));

	wchar_t * editbuf = (wchar_t *)my_malloc(100);
	lstrcpyW(editbuf, _C(dest));
	DialogItemsC[2].PtrData = editbuf;
	HANDLE hdlg;

	int res = ShowDialog(55, 9, _F("CopyDialog"), DialogItemsC, sizeof(InitItemsC)/sizeof(InitItemsC[0]), hdlg);
	if (res == 3)
		dest = GetItemData(hdlg, 2);

	fInfo.DialogFree(hdlg);
	my_free(editbuf);

	return res == 3;
}
bool fardroid::CreateDirDialog(CString &dest) 
{
	struct InitDialogItem InitItemsC[]={
		/*00*/FDI_DOUBLEBOX (55, 9,(farStr *)MCreateDir),
		/*01*/FDI_LABEL			( 2, 2, (farStr *)MDirName),
		/*02*/FDI_EDIT			( 2, 3,52, _F("fardroidDirName")),
		/*03*/FDI_DEFCBUTTON( 7,(farStr *)MOk),
		/*04*/FDI_CBUTTON		( 7,(farStr *)MCancel)
	};
	FarDialogItem DialogItemsC[sizeof(InitItemsC)/sizeof(InitItemsC[0])];
	InitDialogItems(InitItemsC,DialogItemsC,sizeof(InitItemsC)/sizeof(InitItemsC[0]));

	wchar_t * editbuf = (wchar_t *)my_malloc(100);
	lstrcpyW(editbuf, _C(dest));
	DialogItemsC[2].PtrData = editbuf;
	HANDLE hdlg;

	int res = ShowDialog(55, 9, _F("CreateDirDialog"), DialogItemsC, sizeof(InitItemsC)/sizeof(InitItemsC[0]), hdlg);
	if (res == 3)
		dest = GetItemData(hdlg, 2);

	fInfo.DialogFree(hdlg);
	my_free(editbuf);

	return res == 3;
}
HANDLE fardroid::OpenFromMainMenu() 
{
	fileUnderCursor = ExtractName(GetCurrentFileName());

	if (DeviceTest())
	{
		if (conf.RemountSystem)
		{
			CString sRes;
			//ADB_mount(_T("/"), FALSE, sRes, false);
			ADB_mount(_T("/system"), TRUE, sRes, false);
		}

		if (ChangeDir(_T("/")))
			return (HANDLE)this;
		else
			return INVALID_HANDLE_VALUE;
	}
	return INVALID_HANDLE_VALUE;
}
bool fardroid::DeleteFilesDialog() 
{
	CString msg;
	msg.Format(L"%s\n%s\n%s\n%s", GetMsg(MDeleteTitle), GetMsg(MDeleteWarn), GetMsg(MYes), GetMsg(MNo));

	return ShowMessage(msg, 2, NULL, true) == 0;
}

bool fardroid::BreakProcessDialog(LPCTSTR sTitle) 
{
	if (m_procStruct.Hide())
	{
		CString msg;
		msg.Format(L"%s\n%s\n%s\n%s", sTitle, GetMsg(MBreakWarn), GetMsg(MYes), GetMsg(MNo));

		bool bOk = ShowMessage(msg, 2, NULL, false) == 0;
		m_procStruct.Restore();

		return bOk;
	}
	return false;
}

int fardroid::FileExistsDialog(LPCTSTR sName) 
{
	if (m_procStruct.Hide())
	{
		CString msg;
		CString msgexists;
		msg.Format(L"%s\n%s\n%s\n%s\n%s\n%s\n%s", GetMsg(MGetFile), GetMsg(MCopyWarnIfExists), GetMsg(MYes), GetMsg(MNo), GetMsg(MAlwaysYes), GetMsg(MAlwaysNo), GetMsg(MCancel));
		msgexists.Format(msg, sName);

		int res = ShowMessage(msgexists, 5, _F("warnifexists"), true);

		m_procStruct.Restore();
		return res;
	}
	return 4;
}

int fardroid::CopyErrorDialog(LPCTSTR sTitle, LPCTSTR sErr) 
{
	if (m_procStruct.Hide())
	{
		CString errmsg;
		errmsg.Format(_T("%s\n%s\n\n%s\n\n%s\n%s\n%s"), sTitle, GetMsg(MCopyError), sErr, GetMsg(MYes), GetMsg(MNo), GetMsg(MCancel));
		int ret = ShowMessage(errmsg, 3, _F("copyerror"), true);

		m_procStruct.Restore();
		return ret;
	}
	return 2;
}

HANDLE fardroid::OpenFromCommandLine(const CString& cmd ) 
{
	if (!DeviceTest())
		return INVALID_HANDLE_VALUE;

	CString sRes;
	if (conf.RemountSystem)
		ADB_mount(_T("/system"), TRUE, sRes, false);

	strvec tokens;
	Tokenize(cmd, tokens, _T(" -"));

	if (tokens.GetSize() == 0) 
		return OpenFromMainMenu();

	TokensToParams(tokens, params);

	CString dir  = GetParam(params, _T("filename"));
	bool	havefile  = !dir.IsEmpty();

	if (ExistsParam(params, _T("remount")))
	{
		CString fs = _T("/system");
		if (havefile) fs = dir;
		CString par = GetParam(params, _T("remount"));
		if (par.IsEmpty()) par = _T("rw");
		par.MakeLower();

		ADB_mount(fs, par == _T("rw"), sRes, false);
		return INVALID_HANDLE_VALUE;
	}

	if (havefile && ExistsParam(params, _T("fb")))
	{
		GetFrameBuffer(dir);
		return INVALID_HANDLE_VALUE;
	}

	if (havefile)
	{
		fileUnderCursor = ExtractName(GetCurrentFileName());

		DelEndSlash(dir, true);
		if (ChangeDir(dir))
			return (HANDLE)this;
		else
			return OpenFromMainMenu();
	}
	return INVALID_HANDLE_VALUE;
}
bool fardroid::GetItems(PluginPanelItem *PanelItem, int ItemsNumber, const CString& srcdir, const CString& dstdir, bool &noPromt, bool &ansYes, bool bSilent)
{
	for (int i = 0; i < ItemsNumber; i++)
	{
		if (!bSilent && CheckForKey(VK_ESCAPE) && 
				BreakProcessDialog(GetMsg(MGetFile)))
			return true;

		CFileRecord * item = records[static_cast<int>(PanelItem[i].FindData.nPackSize)];
		if (item)
		{
			CString dstfile = dstdir;
			CString srcfile = srcdir;
			srcfile += item->filename;
			dstfile += item->filename;
			if (m_procStruct.Lock())
			{
				m_procStruct.spos.Format(_T("%s %d/%d"), GetMsg(MProcessed), i, ItemsNumber);
				m_procStruct.from = srcfile;
				m_procStruct.to		= dstfile;
				m_procStruct.bSilent = bSilent;
				m_procStruct.nTransmitted = 0;
				m_procStruct.nFileSize = (int)item->size;

				m_procStruct.Unlock();
			}

			if (!CopyFileFrom(srcfile, dstfile, noPromt, ansYes, bSilent))
				return false;
		}
	}
	return true;
}
bool fardroid::PutItems(PluginPanelItem *PanelItem, int ItemsNumber, const CString& srcdir, const CString& dstdir, bool &noPromt, bool &ansYes, bool bSilent)
{
	for (int i = 0; i < ItemsNumber; i++)
	{
		if (!bSilent && CheckForKey(VK_ESCAPE) && 
				BreakProcessDialog(GetMsg(MGetFile)))
			return true;

		CString dstfile = dstdir;
		CString srcfile = srcdir;
		srcfile += PanelItem[i].FindData.lpwszFileName;
		dstfile += PanelItem[i].FindData.lpwszFileName;
		if (m_procStruct.Lock())
		{
			m_procStruct.spos.Format(_T("%s %d/%d"), GetMsg(MProcessed), i, ItemsNumber);
			m_procStruct.from = srcfile;
			m_procStruct.to		= dstfile;
			m_procStruct.bSilent = bSilent;

			m_procStruct.Unlock();
		}

		if (!CopyFileTo(srcfile, dstfile, noPromt, ansYes, bSilent))
			return false;
	}

	Reread();
	return true;
}
bool fardroid::DeleteFile(const CString& name, bool bSilent ) 
{
	if (name.IsEmpty())
		return false;

	CString msg;
	CString msgfail;
	msg.Format(L"%s\n%s", GetMsg(MDeleteTitle), GetMsg(MCopyDeleteError));
	msgfail.Format(msg, name, GetMsg(MRetry), GetMsg(MCancel));
deltry:
	if (!::DeleteFile(name)) 
	{
		if (bSilent)
			return false;

		if (ShowMessage(msgfail, 2, L"delerror", true) == 0)
			goto deltry;

		return false;
	}

	return true;
}
bool fardroid::CopyFileFrom(const CString& src, const CString& dst, bool &noPromt, bool &ansYes, bool bSilent)
{
	if (FileExists(dst))
	{
		if (!noPromt)
		{
			switch (FileExistsDialog(dst))
			{
				case 0: 
					if (!DeleteFile(dst, false))  
						return false;
					break;
				case 1: 
					return true;
				case 2: 
					noPromt = true;
					ansYes	= true;
					if (!DeleteFile(dst, false))  
						return false;
					break;
				case 3:
					noPromt = true;
					ansYes	= false;
					return true;
				default: 
					return false;
			}
		} 
		else 
		{
			if (ansYes) 
			{
				if(!DeleteFile(dst, true)) 
					return false;
			} 
			else 
				return true;
		}
	}

	CString sRes;
repeatcopy:
	// "adb.exe pull" в принципе не может читать файл с устройства с правами Superuser.
	// Если у файла не установлены права доступа на чтения для простых пользователей,
	// то файл не будет прочитан. 

	// Чтобы такие файлы все же прочитать, добавим к правам доступа файла, право на чтение
	// для простых пользователей. А затем вернем оригинальные права доступа к файлу.
	// Этот работает только в Native Mode с включенным Superuser

	// Добавляем к правам доступа файла право на чтение для всех пользователей.
	CString old_permissions;
	if(conf.WorkMode == WORKMODE_NATIVE)
	{
		old_permissions = GetPermissionsFile(src);
	
		if(!old_permissions.IsEmpty())
		{
			CString new_permissions = old_permissions;
			new_permissions.SetAt(new_permissions.GetLength()-3, _T('r'));

			SetPermissionsFile(src, new_permissions);
		}
	}

	// Читаем файл
	BOOL res = ADB_pull(src, dst, sRes, bSilent);

	// Восстановим оригинальные права на файл
	if(conf.WorkMode == WORKMODE_NATIVE)
	{	
		if(!old_permissions.IsEmpty())
			SetPermissionsFile(src, old_permissions);
	}


	if (!res)
	{
		// Silent предполагает не задавать пользователю лишних вопросов. 
		//        Но сообщения об ошибках нужно все же выводить.
		//   if (bSilent)//если отключен вывод на экран, то просто возвращаем что все ОК
		//	    return true;

		if(conf.WorkMode == WORKMODE_NATIVE)
		{
			if(!sRes.IsEmpty()) sRes += _T("\n");
			sRes += (conf.UseSU)? GetMsg(MNeedFolderExePerm): GetMsg(MNeedSuperuserPerm);
		}
		else
		{
			if(!sRes.IsEmpty()) sRes += _T("\n");
			sRes +=  GetMsg(MNeedNativeSuperuserPerm);
		}

		int ret = CopyErrorDialog(GetMsg(MGetFile), sRes);
		switch(ret)
		{
		case 0:
			sRes = _T("");
			goto repeatcopy;
		case 1:
			return true;
		case 2:
			return false;
		}
	}

	return true;
}

bool fardroid::CopyFileTo(const CString& src, const CString& dst, bool &noPromt, bool &ansYes, bool bSilent)
{
	CString sRes;
repeatcopy:
	// "adb.exe push" в принципе не может перезаписывать файл в устройстве с правами Superuser.
	// Если у файла не установлены прав доступа на запись для простых пользователей,
	// то файл не будет перезаписан. (т.е. например? после редактирования файл нельзя будет сохранить)

	// Чтобы такие файлы все же перезаписать, добавим к правам доступа файла, право на запись
	// для простых пользователей. А затем вернем оригинальные права доступа к файлу.
	// Этот работает только в Native Mode с включенным Superuser

	// Добавляем к правам доступа файла право на запись для всех пользователей.	
	CString old_permissions;
	if(conf.WorkMode == WORKMODE_NATIVE)
	{
		old_permissions = GetPermissionsFile(dst);
	
		if(!old_permissions.IsEmpty())
		{
			CString new_permissions = old_permissions;
			new_permissions.SetAt(new_permissions.GetLength()-2, _T('w'));

			SetPermissionsFile(dst, new_permissions);
		}
	}

	// Запись файла
	BOOL res = ADB_push(src, dst, sRes, bSilent);

	// Восстановим оригинальные права на файл
	if(conf.WorkMode == WORKMODE_NATIVE)
	{	
		if(!old_permissions.IsEmpty())
			SetPermissionsFile(dst, old_permissions);
	}

	if (!res)
	{
		// Silent предполагает не задавать пользователю лишних вопросов. 
		//        Но сообщения об ошибках нужно все же выводить.
		//  if (bSilent) //если отключен вывод на экран, то просто возвращаем что все ОК
		//	   return true;

		if(conf.WorkMode == WORKMODE_NATIVE)
		{
			if(!sRes.IsEmpty()) sRes += _T("\n");
			sRes += (conf.UseSU)? GetMsg(MNeedFolderExePerm): GetMsg(MNeedSuperuserPerm);
		}
		else
		{
			if(!sRes.IsEmpty()) sRes += _T("\n");
			sRes +=  GetMsg(MNeedNativeSuperuserPerm);
		}

		int ret = CopyErrorDialog(GetMsg(MPutFile), sRes);
		switch(ret)
		{
		case 0:
			sRes = _T("");
			goto repeatcopy;
		case 1:
			return true;
		case 2:
			return false;
		}
	}

	return true;
}
bool fardroid::DeleteFileFrom(const CString& src, bool &noPromt, bool &ansYes, bool bSilent)
{
	CString sRes;
	BOOL res = ADB_rm(src, sRes, bSilent);
	if (!res)
	{
		if (bSilent)//если отключен вывод на экран, то просто возвращаем что все ОК
			return true;

		/*CString errmsg;
		errmsg.Format(_T("%s\n%s\n\n%s\n\n%s\n%s\n%s"), GetMsg(MCopyTitle), GetMsg(MCopyError), errstr, GetMsg(MYes), GetMsg(MNo), GetMsg(MCancel));
		int ret = ShowMessage(errmsg, 3, _F("copyerror"), true);
		switch(ret)
		{
		case 0:
			goto repeatcopy;
		case 1:
			return true;
		case 2:
			return false;
		}*/

		return true;
	}

	return true;
}
void fardroid::DeleteRecords(CFileRecords & recs)
{
	for (int i = 0; i < recs.GetSize(); i++)
	{
		delete recs[i];
	}
	recs.RemoveAll();
}

int fardroid::GetFindData( struct PluginPanelItem **pPanelItem,int *pItemsNumber,int OpMode )
{
	*pPanelItem=NULL;
	*pItemsNumber=0;

	int items = records.GetSize();

	PluginPanelItem *NewPanelItem=(PluginPanelItem *)my_malloc(sizeof(PluginPanelItem)*items);
	*pPanelItem=NewPanelItem;

	if (NewPanelItem == NULL) 
		return FALSE;

	CFileRecord * item = NULL;

	for (int Z = 0; Z < items; Z++)
	{
		my_memset(&NewPanelItem[Z],0,sizeof(PluginPanelItem));

		item = records[Z];
		NewPanelItem[Z].FindData.dwFileAttributes	= item->attr;
		NewPanelItem[Z].FindData.nPackSize = Z;
		NewPanelItem[Z].FindData.nFileSize		 = item->size;
		NewPanelItem[Z].FindData.ftLastWriteTime = UnixTimeToFileTime(item->time);

		NewPanelItem[Z].FindData.lpwszFileName = my_strdupW(item->filename);
		NewPanelItem[Z].Owner = my_strdupW(item->owner);
		NewPanelItem[Z].Description = my_strdupW(item->desc);

		//3 custom columns
		//NewPanelItem[Z].CustomColumnData=(farStr**)my_malloc(sizeof(farStr*)*2);
		NewPanelItem[Z].CustomColumnNumber = 0;
	}
	*pItemsNumber = items;

	return TRUE;
}

void fardroid::FreeFindData( struct PluginPanelItem *PanelItem,int ItemsNumber )
{
	for (int I = 0; I < ItemsNumber; I++)
	{
		if (PanelItem[I].FindData.lpwszFileName)
			my_free((void*)PanelItem[I].FindData.lpwszFileName);
		if (PanelItem[I].Owner)
			my_free((void*)PanelItem[I].Owner);
		if (PanelItem[I].Description)
			my_free((void*)PanelItem[I].Description);
	}
	if (PanelItem)
	{
		my_free(PanelItem);
		PanelItem = NULL;
	}
}

int fardroid::GetFiles( PluginPanelItem *PanelItem, int ItemsNumber, CString &DestPath, BOOL Move, int OpMode )
{
	CString srcdir = m_currentPath;
	AddBeginSlash(srcdir);
	AddEndSlash(srcdir, true);

	CString path = DestPath;

	bool bSilent = IS_FLAG(OpMode, OPM_SILENT)||IS_FLAG(OpMode, OPM_FIND);
	bool noPromt = bSilent;

	if (IS_FLAG(OpMode, OPM_VIEW))
		bSilent = false;

	if (!IS_FLAG(OpMode, OPM_QUICKVIEW) &&
			!IS_FLAG(OpMode, OPM_VIEW) &&
			!bSilent)
	{
		if (!CopyFilesDialog(path))
			return FALSE;
		else
			DestPath = path;
	}

	AddEndSlash(path);

	if (m_procStruct.Lock())
	{
		m_procStruct.bSilent = true;
		m_procStruct.docontinue = true;
		m_procStruct.title = GetMsg(MGetFile);

		m_procStruct.Unlock();
	}

	DWORD threadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, ProcessThreadProc, this, 0, &threadID);

	BOOL bOk = GetItems(PanelItem, ItemsNumber, srcdir, path, noPromt, noPromt, bSilent)?TRUE:FALSE;

	if (m_procStruct.Lock())
	{
		m_procStruct.docontinue = false;
		m_procStruct.Unlock();
	}

	CloseHandle(hThread);

	return bOk;
}

int fardroid::UpdateInfoLines()
{
	//version first
	lines.RemoveAll();
	CPanelLine pl;
	pl.text = GetVersionString();
	pl.separator = TRUE;

	lines.Add(pl);

#ifdef USELOGGING
	CPerfCounter counter;
	counter.Start(_T("Get Memory Info"));
#endif

	GetMemoryInfo();

#ifdef USELOGGING
	counter.Stop(NULL);
	counter.Start(_T("Get Partitions Info"));
#endif

	GetPartitionsInfo();

	return lines.GetSize();
}

void fardroid::PreparePanel( OpenPluginInfo *Info )
{
	//TODO!!! - сделать динамические массивы
	Info->HostFile = _C(fileUnderCursor);

	panelTitle.Format(_T(" %s: /%s "), GetMsg(MTitle), m_currentPath);
	Info->PanelTitle = _C(panelTitle);
	Info->CurDir = _C(m_currentPath);

	if (InfoPanelLineArray)
	{
		delete InfoPanelLineArray;
		InfoPanelLineArray = NULL;
	}

	int len = lines.GetSize();

	if (len > 0)
	{
		InfoPanelLineArray = new InfoPanelLine[len];

		for (int i = 0; i < len; i++)
		{
			InfoPanelLineArray[i].Text = lines[i].text;
			InfoPanelLineArray[i].Data = lines[i].data;
			InfoPanelLineArray[i].Separator = lines[i].separator;
		}
	}

	Info->InfoLines= InfoPanelLineArray;
	Info->InfoLinesNumber = len;
}

void fardroid::ChangePermissionsDialog()
{
	if(conf.WorkMode != WORKMODE_NATIVE)
	{
	    CString msg;
		msg.Format(L"%s\n%s\n%s", GetMsg(MWarningTitle), GetMsg(MOnlyNative), GetMsg(MOk));	
		int res = ShowMessage(msg, 1, NULL, true);
		return;
	}

	CString CurrentDir = GetPanelPath(false);
	CString CurrentFileName = GetCurrentFileName(false);
	CString CurrentFullFileName = CString(_T("/")) + CurrentDir + _T("/") + CurrentFileName;

	CString permissions = GetPermissionsFile(CurrentFullFileName);

	struct InitDialogItem InitItems[]={
		/*00*/FDI_DOUBLEBOX     (47, 21, (farStr *)MPermTitle),
		/*01*/FDI_LABEL         ( 2, 2,  (farStr *)MPermFileName),
		/*02*/FDI_LABEL         ( 2, 3,  (farStr *)MPermFileAttr),

		/*03*/FDI_LABEL         ( 5,  5, _T("          R   W   X")),
		/*04*/FDI_LABEL         ( 5,  6, _T("Owner :")),
		/*05*/FDI_LABEL         ( 5,  7, _T("Group :")),
		/*06*/FDI_LABEL         ( 5,  8, _T("Others:")),

		/*07*/FDI_CHECK			( 14,  6, _T("") ),
		/*08*/FDI_CHECK			( 14,  7, _T("") ),
		/*09*/FDI_CHECK			( 14,  8, _T("") ),

		/*10*/FDI_CHECK			( 18,  6, _T("") ),
		/*11*/FDI_CHECK			( 18,  7, _T("") ),
		/*12*/FDI_CHECK			( 18,  8, _T("") ),

		/*13*/FDI_CHECK			( 22,  6, _T("") ),
		/*14*/FDI_CHECK			( 22,  7, _T("") ),
		/*15*/FDI_CHECK			( 22,  8, _T("") ),

		/*16*/FDI_DEFCBUTTON    (10, _T("Set")),
		/*17*/FDI_CBUTTON		(10,(farStr *)MCancel)

	};

	FarDialogItem DialogItems[sizeof(InitItems)/sizeof(InitItems[0])];
	InitDialogItems(InitItems,DialogItems,sizeof(InitItems)/sizeof(InitItems[0]));
	
	CString LabelTxt1 = GetMsg(MPermFileName) + CurrentFileName;
	DialogItems[1].PtrData = LabelTxt1;
	CString LabelTxt2 = GetMsg(MPermFileAttr) + permissions;
	DialogItems[2].PtrData = LabelTxt2;

	DialogItems[7].Selected  = (permissions[1] != _T('-'));
	DialogItems[10].Selected = (permissions[2] != _T('-'));
	DialogItems[13].Selected = (permissions[3] != _T('-'));

	DialogItems[8].Selected  = (permissions[4] != _T('-'));
	DialogItems[11].Selected = (permissions[5] != _T('-'));
	DialogItems[14].Selected = (permissions[6] != _T('-'));
		
	DialogItems[9].Selected  = (permissions[7] != _T('-'));
	DialogItems[12].Selected = (permissions[8] != _T('-'));
	DialogItems[15].Selected = (permissions[9] != _T('-'));

	HANDLE hdlg;

	int res = ShowDialog( (LabelTxt1.GetLength() > 34)?LabelTxt1.GetLength()+2:36, 13, NULL, DialogItems, sizeof(InitItems)/sizeof(InitItems[0]), hdlg);
	if (res == 16)
	{
		
		permissions.SetAt(1, GetItemSelected(hdlg, 7)? _T('r'):_T('-'));
		permissions.SetAt(2, GetItemSelected(hdlg, 10)? _T('w'):_T('-'));
		permissions.SetAt(3, GetItemSelected(hdlg, 13)? _T('x'):_T('-'));

		permissions.SetAt(4, GetItemSelected(hdlg, 8)? _T('r'):_T('-'));
		permissions.SetAt(5, GetItemSelected(hdlg, 11)? _T('w'):_T('-'));
		permissions.SetAt(6, GetItemSelected(hdlg, 14)? _T('x'):_T('-'));

		permissions.SetAt(7, GetItemSelected(hdlg, 9)? _T('r'):_T('-'));
		permissions.SetAt(8, GetItemSelected(hdlg, 12)? _T('w'):_T('-'));
		permissions.SetAt(9, GetItemSelected(hdlg, 15)? _T('x'):_T('-'));

		SetPermissionsFile(CurrentFullFileName, permissions);

		CString permissions_chk = GetPermissionsFile(CurrentFullFileName);

		if(permissions_chk != permissions)
		{
			CString msg;
			msg.Format(L"%s\n%s\n%s", GetMsg(MWarningTitle), GetMsg(MSetPermFail), GetMsg(MOk));	
			int res = ShowMessage(msg, 1, NULL, true);
		}
	}
	fInfo.DialogFree(hdlg);

} 

bool fardroid::CopyFileDialog( CString &destpath, CString &destname )
{
	struct InitDialogItem InitItemsC[]={
		/*00*/FDI_DOUBLEBOX (55, 9,(farStr *)MCopyTitle),
		/*01*/FDI_LABEL			( 2, 2, (farStr *)MCopyDest),
		/*02*/FDI_EDIT			( 2, 3,52, _F("fardroidDestinationDir")),
		/*03*/FDI_LABEL			( 2, 4, (farStr *)MCopyDestName),
		/*04*/FDI_EDIT			( 2, 5,52, _F("fardroidDestinationName")),
		/*05*/FDI_DEFCBUTTON( 7,(farStr *)MOk),
		/*06*/FDI_CBUTTON		( 7,(farStr *)MCancel)
	};
	FarDialogItem DialogItemsC[sizeof(InitItemsC)/sizeof(InitItemsC[0])];
	InitDialogItems(InitItemsC,DialogItemsC,sizeof(InitItemsC)/sizeof(InitItemsC[0]));

	wchar_t * editbuf = (wchar_t *)my_malloc(100);
	wchar_t * editbuf2 = (wchar_t *)my_malloc(100);
	lstrcpyW(editbuf, _C(destpath));
	lstrcpyW(editbuf2, _C(destname));
	DialogItemsC[2].PtrData = editbuf;
	DialogItemsC[4].PtrData = editbuf2;
	HANDLE hdlg;

	int res = ShowDialog(55, 9, _F("CopyDialog2"), DialogItemsC, sizeof(InitItemsC)/sizeof(InitItemsC[0]), hdlg);

	if (res == 5)
	{
		destpath = GetItemData(hdlg, 2);
		destname = GetItemData(hdlg, 4);
	}
	fInfo.DialogFree(hdlg);
	my_free(editbuf);
	my_free(editbuf2);

	return res == 5;
}

void fardroid::Reread()
{
	CString p = m_currentPath;
	AddBeginSlash(p);
	ChangeDir(p);
}

int fardroid::ChangeDir( LPCTSTR sDir, int OpMode)
{
	CString s = sDir;
	CFileRecord * item = GetFileRecord(sDir);
	if (s != _T("..") && item && IsLink(item->attr) && (!conf.LinksAsDir() && OpMode == 0))
		s = item->linkto;

	CString tempPath;
	int i = s.Find(_T("/"));
	int j = s.Find(_T("\\"));
	if (i != -1 || j != -1)//перемещение с помощью команды cd
	{
		if (s[0] == _T('/') || s[0] == _T('\\')) //абсолютный путь
			tempPath = s;
		else //относительный путь в глубь иерархии (пока отбрасываем всякие ..\path. TODO!!!)
			tempPath.Format(_T("/%s%s%s"), m_currentPath, m_currentPath.IsEmpty()?_T(""):_T("/"), s);
	}
	if (i == -1 && j == -1)//простое относительное перемещение
	{
		if (s == _T(".."))
			tempPath = ExtractPath(m_currentPath);
		else
			tempPath.Format(_T("/%s%s%s"), m_currentPath, m_currentPath.IsEmpty()?_T(""):_T("/"), s);
	}

	AddBeginSlash(tempPath);

	if (OpenPanel(tempPath))
	{
		m_currentPath = tempPath;
		if (!m_currentPath.IsEmpty())
			m_currentPath.Delete(0);
		return TRUE;
	}

	if (OpMode != 0 || lastError == ERROR_DEV_NOT_EXIST)
		return FALSE;

	tempPath = m_currentPath;
	AddBeginSlash(tempPath);

	if (OpenPanel(tempPath))
	{
		m_currentPath = tempPath;
		if (!m_currentPath.IsEmpty())
			m_currentPath.Delete(0);
		return TRUE;
	}

	return FALSE;
}

void fardroid::ADBSyncQuit( SOCKET sockADB )
{
	syncmsg msg;

	msg.req.id = ID_QUIT;
	msg.req.namelen = 0;

	SendADBPacket(sockADB, &msg.req, sizeof(msg.req));
}
bool fardroid::ADBReadMode(SOCKET sockADB, LPCTSTR path, int &mode)
{
	syncmsg msg;
	int len = lstrlen(path);

	msg.req.id = ID_STAT;
	msg.req.namelen = len;

	bool bOk = false;
	if (SendADBPacket(sockADB, &msg.req, sizeof(msg.req)))
	{
		char * buf = getAnsiString(path);
		bOk = SendADBPacket(sockADB, buf, len);
		my_free(buf);
	}

	if(	(ReadADBPacket(sockADB, &msg.stat, sizeof(msg.stat)) <= 0) ||
			(msg.stat.id != ID_STAT))
		bOk = false;

	mode = msg.stat.mode;
	return bOk;
}

bool fardroid::ADBTransmitFile(SOCKET sockADB, LPCTSTR sFileName, time_t & mtime)
{
	HANDLE hFile = CreateFile(sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	if (m_procStruct.Lock())
	{
		m_procStruct.nTransmitted = 0;
		m_procStruct.nFileSize = GetFileSize(hFile, NULL);
		m_procStruct.Unlock();
	}

	FILETIME ft;
	SYSTEMTIME st;
	GetFileTime(hFile, NULL, NULL, &ft);
	FileTimeToSystemTime(&ft, &st);
	SystemTimeToUnixTime(&st, &mtime);

	syncsendbuf *sbuf = new syncsendbuf;
	sbuf->id = ID_DATA;

	bool bOK = false;
	for(;;)
	{
		DWORD readed = 0;
		if(!ReadFile(hFile, sbuf->data, SYNC_DATA_MAX, &readed, NULL))
		{
			bOK = false;
			break;
		}

		if (readed == 0)
		{
			bOK = true;
			break;
		}

		sbuf->size = readed;
		if(!SendADBPacket(sockADB, sbuf, sizeof(unsigned) * 2 + readed))
		{
			bOK = false;
			break;
		}

		if (m_procStruct.Lock())
		{
			m_procStruct.nTransmitted += readed;
			m_procStruct.Unlock();
		}

		if (m_bForceBreak)
		{
			bOK = true;
			break;
		}
	}

	if (m_procStruct.Lock())
	{
		m_procStruct.nFileSize = 0;
		m_procStruct.Unlock();
	}

	delete sbuf;
	CloseHandle(hFile);
	return bOK;
}

bool fardroid::ADBSendFile(SOCKET sockADB, LPCTSTR sSrc, LPCTSTR sDst, CString & sRes, int mode)
{
	syncmsg msg;
	int len;
	int size = 0;

	CString sData;
	sData.Format(_T("%s,%d"), sDst, mode);

	len = lstrlen(sDst);
	if(len > 1024) return false;

	msg.req.id = ID_SEND;
	msg.req.namelen = sData.GetLength();

	if (SendADBPacket(sockADB, &msg.req, sizeof(msg.req)))
	{
		bool bOK = false;
		char * buf = getAnsiString(sData);
		bOK = SendADBPacket(sockADB, buf, sData.GetLength());
		my_free(buf);
		if (!bOK) return false;
	}

	time_t mtime;
	if (!ADBTransmitFile(sockADB, sSrc, mtime))
		return false;

	msg.data.id = ID_DONE;
	msg.data.size = (unsigned)mtime;
	if(!SendADBPacket(sockADB, &msg.data, sizeof(msg.data)))
		return false;

	if(ReadADBPacket(sockADB, &msg.status, sizeof(msg.status)) <= 0)
		return false;

	if(msg.status.id != ID_OKAY)
	{
		if(msg.status.id == ID_FAIL)
		{
			len = msg.status.msglen;
			if(len > 256) len = 256;
			char * buf = new char[257];
			if(ReadADBPacket(sockADB, buf, len))
			{
				delete buf;
				return false;
			}
			buf[len] = 0;
			sRes = buf;

			delete buf;
		} else
			sRes = _T("unknown reason");

		return false;
	}

	return true;
}

bool fardroid::ADBPushDir(SOCKET sockADB, LPCTSTR sSrc, LPCTSTR sDst, CString &sRes)
{
	if (!ADB_mkdir(sDst, sRes, true))
		return false;

	CString ddir = sDst;
	CString sdir = sSrc;
	AddEndSlash(sdir);
	AddEndSlash(ddir, true);
	CString ssaved = sdir;
	CString dsaved = ddir;

	sdir += _T("*.*");

	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(sdir, &fd);
	if (h == INVALID_HANDLE_VALUE) return false;

	CString sname, dname;
	sname.Format(_T("%s%s"), ssaved, fd.cFileName);
	dname.Format(_T("%s%s"), dsaved, fd.cFileName);
	if (!IsDirectory(fd.dwFileAttributes))
		ADBPushFile(sockADB, sname, dname, sRes);
	else if (lstrcmp(fd.cFileName, _T(".")) != 0 &&
						lstrcmp(fd.cFileName, _T("..")) != 0)
		ADBPushDir(sockADB, sname, dname, sRes);

	while (FindNextFile(h, &fd) != 0 && !m_bForceBreak) 
	{
		sname.Format(_T("%s%s"), ssaved, fd.cFileName);
		dname.Format(_T("%s%s"), dsaved, fd.cFileName);
		if (!IsDirectory(fd.dwFileAttributes))
			ADBPushFile(sockADB, sname, dname, sRes);
		else if (lstrcmp(fd.cFileName, _T(".")) != 0 &&
							lstrcmp(fd.cFileName, _T("..")) != 0)
			ADBPushDir(sockADB, sname, dname, sRes);

	}
	FindClose(h);
	return true;
}
BOOL fardroid::ADBPushFile(SOCKET sockADB, LPCTSTR sSrc, LPCTSTR sDst, CString & sRes)
{
	CString dest = WtoUTF8(sDst);
	int mode = 0;
	if(!ADBReadMode(sockADB, dest, mode)) 
		return FALSE;

	if((mode != 0) && IS_FLAG(mode, S_IFDIR))
	{
		CString name = ExtractName(sSrc, false);
		AddEndSlash(dest, true);
		dest += name;
	}

	if (m_procStruct.Lock())
	{
		m_procStruct.from = sSrc;
		m_procStruct.to = sDst;
		m_procStruct.Unlock();
	}

	if(!ADBSendFile(sockADB, sSrc, dest, sRes, mode))
		return FALSE;

	return TRUE;
}
BOOL fardroid::ADB_push( LPCTSTR sSrc, LPCTSTR sDst, CString & sRes, bool bSilent )
{
	SOCKET sock = PrepareADBSocket();

	if (!SendADBCommand(sock, _T("sync:")))
	{
		CloseADBSocket(sock);
		return FALSE;
	}

	if(IsDirectory(sSrc))
	{
		if(!ADBPushDir(sock, sSrc, sDst, sRes))
		{
			CloseADBSocket(sock);
			return FALSE;
		}
		ADBSyncQuit(sock);
	}
	else
	{
		if(!ADBPushFile(sock, sSrc, sDst, sRes))
		{
			CloseADBSocket(sock);
			return FALSE;
		}
		ADBSyncQuit(sock);
	}

	CloseADBSocket(sock);
	return TRUE;
}
bool fardroid::ADBPullDir(SOCKET sockADB, LPCTSTR sSrc, LPCTSTR sDst, CString &sRes)
{
	MakeDirs(sDst);

	CString ddir = sDst;
	CString sdir = sSrc;
	AddEndSlash(sdir, true);
	AddEndSlash(ddir);
	CString ssaved = sdir;
	CString dsaved = ddir;

	CFileRecords recs;
	if (ADB_ls(WtoUTF8(sSrc), recs, sRes, true))
	{
		CString sname, dname;
		for (int i = 0; i < recs.GetSize(); i++)
		{
			sname.Format(_T("%s%s"), ssaved, recs[i]->filename);
			dname.Format(_T("%s%s"), dsaved, recs[i]->filename);

			if (!IsDirectory(recs[i]->attr))
			{
				if (m_procStruct.Lock())
				{
					m_procStruct.from = sname;
					m_procStruct.to = dname;
					m_procStruct.nTransmitted = 0;
					m_procStruct.nFileSize = (int)recs[i]->size;
					m_procStruct.Unlock();
				}
				ADBPullFile(sockADB, sname, dname, sRes);
			}
			else 
				ADBPullDir(sockADB, sname, dname, sRes);
		}
	}

	DeleteRecords(recs);
	return true;
}
BOOL fardroid::ADBPullFile(SOCKET sockADB, LPCTSTR sSrc, LPCTSTR sDst, CString & sRes)
{
	syncmsg msg;
	HANDLE hFile = 0;
	int len;
	unsigned id;

	len = lstrlen(sSrc);
	if(len > 1024) return FALSE;

	msg.req.id = ID_RECV;
	msg.req.namelen = len;
	if(SendADBPacket(sockADB, &msg.req, sizeof(msg.req)))
	{
		bool bOK = false;
		char * buf = getAnsiString(WtoUTF8(sSrc));
		bOK = SendADBPacket(sockADB, buf, len);
		my_free(buf);
		if (!bOK) return FALSE;
	}

	if(ReadADBPacket(sockADB, &msg.data, sizeof(msg.data)) <= 0)
		return FALSE;

	id = msg.data.id;
	if((id != ID_DATA) && (id != ID_DONE))
		goto remoteerror;

	DeleteFile(sDst, true);
	MakeDirs(ExtractPath(sDst, false));
	hFile = CreateFile(sDst, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	char * buffer = new char[SYNC_DATA_MAX];
	DWORD written = 0;
	bool bFirst = true;
	for(;;) 
	{
		if (!bFirst)
		{
			if(ReadADBPacket(sockADB, &msg.data, sizeof(msg.data)) <= 0)
			{
				delete [] buffer;
				CloseHandle(hFile);
				return FALSE;
			}
			id = msg.data.id;
		}
		

		len = msg.data.size;
		if(id == ID_DONE) break;
		if(id != ID_DATA) 
		{
			delete [] buffer;
			CloseHandle(hFile);
			goto remoteerror;
		}

		if(len > SYNC_DATA_MAX)
		{
			delete [] buffer;
			CloseHandle(hFile);
			return FALSE;
		}

		if(ReadADBPacket(sockADB, buffer, len) <= 0)
		{
			delete [] buffer;
			CloseHandle(hFile);
			return FALSE;
		}

		if(!WriteFile(hFile, buffer, len, &written, NULL))
		{
			delete [] buffer;
			CloseHandle(hFile);
			return FALSE;
		}

		if (m_procStruct.Lock())
		{
			m_procStruct.nTransmitted += written;
			m_procStruct.Unlock();
		}

		if (m_bForceBreak)
		{
			delete [] buffer;
			CloseHandle(hFile);
			return TRUE;
		}
		bFirst = false;
	}

	if (m_procStruct.Lock())
	{
		m_procStruct.nFileSize = 0;
		m_procStruct.Unlock();
	}

	delete [] buffer;
	CloseHandle(hFile);
	return TRUE;

remoteerror:
	if(id == ID_FAIL)
	{
		char * buffer = new char[SYNC_DATA_MAX];
		len = msg.data.size;
		if(len > 256) len = 256;
		if(ReadADBPacket(sockADB, buffer, len) <= 0)
			return FALSE;
		buffer[len] = 0;

		sRes = buffer;
	}
	return FALSE;
}

BOOL fardroid::ADB_pull( LPCTSTR sSrc, LPCTSTR sDst, CString & sRes, bool bSilent )
{
	int mode;
	CString dest = sDst;

	SOCKET sock = PrepareADBSocket();

	if (!SendADBCommand(sock, _T("sync:")))
	{
		CloseADBSocket(sock);
		return FALSE;
	}

	if (!ADBReadMode(sock, sSrc, mode)) 
	{
		CloseADBSocket(sock);
		return FALSE;
	}
	if(IS_FLAG(mode, S_IFREG) || IS_FLAG(mode, S_IFCHR) || IS_FLAG(mode, S_IFBLK))
	{
		if (IsDirectory(dest))
		{
			AddEndSlash(dest);
			dest += ExtractName(sSrc);
		}
		if(!ADBPullFile(sock, sSrc, dest, sRes))
		{
			CloseADBSocket(sock);
			return FALSE;
		}
		ADBSyncQuit(sock);
	}
	else if(IS_FLAG(mode, S_IFDIR))
	{
		if(!ADBPullDir(sock, sSrc, dest, sRes))
		{
			CloseADBSocket(sock);
			return FALSE;
		}
		ADBSyncQuit(sock);
	}else
	{
		CloseADBSocket(sock);
	    return FALSE;
	}

	CloseADBSocket(sock);
	return TRUE;
}

BOOL fardroid::ADB_ls(LPCTSTR sDir, CFileRecords & files, CString & sRes, bool bSilent )
{
	DeleteRecords(files);

	if (conf.WorkMode != WORKMODE_SAFE)
	{
		CString s;
		switch (conf.WorkMode)
		{
		case WORKMODE_NATIVE:
			//s.Format(_T("toolbox ls -l -a \"%s\""), sDir);
			s.Format(_T("ls -l -a \"%s\""), sDir);
			break;
		case WORKMODE_BUSYBOX:
			s.Format(_T("busybox ls -lAe%s --color=never \"%s\""), conf.LinksAsDir()?_T("L"):_T(""), sDir);
			break;
		}
		if(ADBShellExecute(s, sRes, bSilent))
			return ReadFileList(sRes, files);
	}
	else 
	{
		SOCKET sockADB = PrepareADBSocket();

		if (SendADBCommand(sockADB, _T("sync:")))
		{
			syncmsg msg;
			char buf[257];
			int len;

			len = lstrlen(sDir);
			if(len > 1024) return FALSE;

			msg.req.id = ID_LIST;
			msg.req.namelen = len;

			if(SendADBPacket(sockADB, &msg.req, sizeof(msg.req)))
			{
				char * dir = getAnsiString(sDir);
				if (SendADBPacket(sockADB, dir, len))
				{
					for(;;) 
					{
						if(ReadADBPacket(sockADB, &msg.dent, sizeof(msg.dent)) <= 0) break;
						if(msg.dent.id == ID_DONE) 
						{
							my_free(dir);
							return TRUE;
						}
						if(msg.dent.id != ID_DENT) break;

						len = msg.dent.namelen;
						if(len > 256) break;

						if(ReadADBPacket(sockADB, buf, len) <= 0) break;
						buf[len] = 0;

						if (lstrcmpA(buf, ".") != 0 &&
							lstrcmpA(buf, "..") != 0)
						{
							CFileRecord * rec = new CFileRecord;
							rec->attr = ModeToAttr(msg.dent.mode);
							rec->size = msg.dent.size;
							rec->time = msg.dent.time;

							CString s = buf;
							rec->filename = UTF8toW(s);

							files.Add(rec);
						}
					}
				}
				my_free(dir);
			}
		}
	}

	return FALSE;
}
BOOL fardroid::ADB_rm( LPCTSTR sDir, CString & sRes, bool bSilent )
{
	CString s; //s.Format(_T("toolbox rm -r \"%s\""), WtoUTF8(sDir));
	s.Format(_T("rm -r \"%s\""), WtoUTF8(sDir));
	return ADBShellExecute(s, sRes, bSilent);
}
BOOL fardroid::ADB_mkdir( LPCTSTR sDir, CString & sRes, bool bSilent )
{
	CString s; //s.Format(_T("toolbox mkdir \"%s\""), WtoUTF8(sDir));
	s.Format(_T("mkdir \"%s\""), WtoUTF8(sDir));
	return ADBShellExecute(s, sRes, bSilent);
}

BOOL fardroid::ReadFileList( CString & sFileList, CFileRecords & files )
{
	DeleteRecords(files);
	strvec lines;
#ifdef USELOGGING
	CPerfCounter counter;
	counter.Start(_T("Prepare listing"));
#endif

	Tokenize(sFileList, lines, _T("\r\n"));

#ifdef USELOGGING
	counter.Stop(NULL);
	counter.Start(_T("Parse listing"));
#endif

	for (int i = 0; i < lines.GetSize(); i++)
	{
		switch (conf.WorkMode)
		{
		case WORKMODE_BUSYBOX:
			if (!ParseFileLineBB(lines[i], files))
				return FALSE;
			break;
		case WORKMODE_NATIVE:
			if (!ParseFileLine(lines[i], files))
				return FALSE;
			break;
		}
	}
	return TRUE;
}

/*bool fardroid::ParseFileLineSafe( CString & sLine )
{
	strvec tokens;
	CString regex = _T("/(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s(.+)$/");
	RegExTokenize(sLine, regex, tokens);

	if (tokens.GetSize() == 4)
	{
		CFileRecord * rec = new CFileRecord;
		rec->attr = ModeToAttr(_httoi(tokens[0]));
		rec->size = _httoi(tokens[1]);
		rec->time = _httoi(tokens[2]);
		rec->filename = UTF8toW(tokens[3]);

		if (rec->filename == _T(".") || 
				rec->filename == _T(".."))
		{
			delete rec;
			return true;
		}

		records.Add(rec);

		return true;
	}

	return false;
}*/

bool fardroid::ParseFileLine( CString & sLine, CFileRecords &files )
{
	strvec tokens;
	CString regex;
	CFileRecord * rec = NULL;
	if (sLine.IsEmpty())
		return true;

	switch (sLine[0])
	{
	case 'd'://directory
		regex = _T("/([\\w-]+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+([\\w-]+(?=\\s))\\s+([\\w:]+(?=\\s))\\s(.+)$/");
		RegExTokenize(sLine, regex, tokens);
		if (tokens.GetSize() == 6)
		{
			rec = new CFileRecord;
			rec->attr = StringToAttr(tokens[0]);
			rec->owner = tokens[1];
			rec->grp = tokens[2];
			rec->time = StringTimeToUnixTime(tokens[3], tokens[4]);
			rec->size = 0;
			rec->filename = tokens[5];
		}
		break;
	case 'l'://symlink
		regex = _T("/([\\w-]+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+([\\w-]+(?=\\s))\\s+([\\w:]+(?=\\s))\\s(.+(?=\\s->))\\s->\\s(.+)$/");
		RegExTokenize(sLine, regex, tokens);
		if (tokens.GetSize() == 7)
		{
			rec = new CFileRecord;
			rec->attr = StringToAttr(tokens[0]);
			rec->owner = tokens[1];
			rec->grp = tokens[2];
			rec->time = StringTimeToUnixTime(tokens[3], tokens[4]);
			rec->size = 0;
			rec->filename = UTF8toW(tokens[5]);
			rec->linkto = UTF8toW(tokens[6]);
			rec->desc.Format(_T("-> %s"), UTF8toW(tokens[6]));
		}
		break;
	case 'c'://device
	case 'b':
	case 's'://socket
		regex = _T("/([\\w-]+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+([\\w,]+\\s+\\w+(?=\\s))\\s+([\\w-]+(?=\\s))\\s+([\\w:]+(?=\\s))\\s(.+)$/");
		RegExTokenize(sLine, regex, tokens);
		if (tokens.GetSize() == 7)
		{
			rec = new CFileRecord;
			rec->attr = StringToAttr(tokens[0]);
			rec->owner = tokens[1];
			rec->grp = tokens[2];
			rec->size = 0;
			rec->desc = tokens[3];
			rec->time = StringTimeToUnixTime(tokens[4], tokens[5]);
			rec->filename = UTF8toW(tokens[6]);
		}else
		{
			case '-'://file
			case 'p'://FIFO	
				regex = _T("/([\\w-]+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+([\\w-]+(?=\\s))\\s+([\\w:]+(?=\\s))\\s(.+)$/");
				RegExTokenize(sLine, regex, tokens);
				if (tokens.GetSize() == 7)
				{
					rec = new CFileRecord;
					rec->attr = StringToAttr(tokens[0]);
					rec->owner = tokens[1];
					rec->grp = tokens[2];
					rec->size = _ttoi(tokens[3]);
					rec->time = StringTimeToUnixTime(tokens[4], tokens[5]);
					rec->filename = UTF8toW(tokens[6]);
				}
				else
				{
					regex = _T("/([\\w-]+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+([\\w-]+(?=\\s))\\s+([\\w:]+(?=\\s))\\s(.+)$/");
					RegExTokenize(sLine, regex, tokens);
					if (tokens.GetSize() == 6)
					{
						rec = new CFileRecord;
						rec->attr = StringToAttr(tokens[0]);
						rec->owner = tokens[1];
						rec->grp = tokens[2];
						rec->size = 0;
						rec->time = StringTimeToUnixTime(tokens[3], tokens[4]);
						rec->filename = UTF8toW(tokens[5]);
					}
				}
		}
		break;	
	}

	if (rec)
	{
		files.Add(rec);
		return true;
	}

	return false;
}

bool fardroid::ParseFileLineBB( CString & sLine, CFileRecords & files )
{
	strvec tokens;
	CString regex;
	CFileRecord * rec = NULL;
	if (sLine.IsEmpty())
		return true;

	switch (sLine[0])
	{
	case 'l'://symlink
		regex = _T("/([\\w-]+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+([\\w:]+(?=\\s))\\s+(\\w+(?=\\s))\\s(.+(?=\\s->))\\s->\\s(.+)$/");
		RegExTokenize(sLine, regex, tokens);
		if (tokens.GetSize() == 12)
		{
			rec = new CFileRecord;
			rec->attr = StringToAttr(tokens[0]);
			rec->owner = tokens[2];
			rec->grp = tokens[3];
			rec->size = _ttoi(tokens[4]);
			rec->time = StringTimeToUnixTime( tokens[7], tokens[6], tokens[9], tokens[8]);
			rec->filename = UTF8toW(tokens[10]);
			rec->linkto = UTF8toW(tokens[11]);
			rec->desc.Format(_T("-> %s"), UTF8toW(tokens[11]));
		}
		break;
	case 'd'://directory
	case '-'://file
	case 'p'://FIFO
		regex = _T("/([\\w-]+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+([\\w:]+(?=\\s))\\s+(\\w+(?=\\s))\\s(.+)$/");
		RegExTokenize(sLine, regex, tokens);
		if (tokens.GetSize() == 11)
		{
			rec = new CFileRecord;
			rec->attr = StringToAttr(tokens[0]);
			//tokens[1] - links count
			rec->owner = tokens[2];
			rec->grp = tokens[3];
			rec->size = _ttoi(tokens[4]);
			//tokens[5] - day of week
			rec->time = StringTimeToUnixTime( tokens[7], tokens[6], tokens[9], tokens[8]);
			rec->filename = UTF8toW(tokens[10]);
		}
		break;
	case 'c'://device
	case 'b':
	case 's':
		regex = _T("/([\\w-]+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+([\\w,]+\\s+\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+([\\w:]+(?=\\s))\\s+(\\w+(?=\\s))\\s(.+)$/");
		RegExTokenize(sLine, regex, tokens);
		if (tokens.GetSize() == 11)
		{
			rec = new CFileRecord;
			rec->attr = StringToAttr(tokens[0]);
			rec->owner = tokens[2];
			rec->grp = tokens[3];
			rec->size = 0;
			rec->desc = tokens[4];
			rec->time = StringTimeToUnixTime( tokens[7], tokens[6], tokens[9], tokens[8]);
			rec->filename = UTF8toW(tokens[10]);
		}
		break;
	}

	if (rec)
	{
		files.Add(rec);
		return true;
	}

	return false;
}

BOOL fardroid::OpenPanel(LPCTSTR sPath)
{
	BOOL bOK = FALSE;

	SOCKET sockADB = PrepareADBSocket();
	if (sockADB)
	{
		CloseADBSocket(sockADB);

		UpdateInfoLines();

		CString sRes;
		return ADB_ls(WtoUTF8(sPath), records, sRes, false);
	}

	return bOK;
}

void fardroid::ShowADBExecError(CString err, bool bSilent )
{
	if (bSilent)
		return;

	if (m_procStruct.Hide())
	{
		CString msg;
		if (err.IsEmpty())
			err = GetMsg(MADBExecError);

		err.TrimLeft();
		err.TrimRight();
		msg.Format(_T("%s\n%s\n%s"), GetMsg(MTitle), err, GetMsg(MOk));
		ShowMessage(msg, 1, NULL, true);

		m_procStruct.Restore();
	}
}

bool fardroid::ShowProgressMessage() 
{
	if (m_procStruct.Lock())
	{
		if (m_procStruct.bSilent)
		{
			m_procStruct.Unlock();
			return m_procStruct.docontinue;
		}

		static DWORD time = GetTickCount();
		static CString sProgress;
		if (m_procStruct.nFileSize > 0)
			sProgress.Format(_T("%d/%d"), m_procStruct.nTransmitted, m_procStruct.nFileSize);
		else
			sProgress.Empty();
		/*static TCHAR sBusy[] = _T("-----");
		static int nBusyPos = 0;*/

		if (GetTickCount() - time > 1000)
		{
			time = GetTickCount();
			/*for (int i = 0; i < lstrlen(sBusy); i++)
				sBusy[i] = 9552;
			sBusy[nBusyPos++] = 9608;
			if (nBusyPos >= lstrlen(sBusy))
				nBusyPos = 0;*/

			const farStr *MsgItems[]={m_procStruct.title, GetMsg(MFrom), m_procStruct.from, GetMsg(MTo), m_procStruct.to, _T(""), sProgress, m_procStruct.spos};
			ShowMessageWait(MsgItems, sizeof(MsgItems)/sizeof(MsgItems[0]));
		}

		m_procStruct.Unlock();
	}

	return m_procStruct.docontinue;
}

bool fardroid::DelItems(PluginPanelItem *PanelItem, int ItemsNumber, bool &noPromt, bool &ansYes, bool bSilent)
{
	for (int i = 0; i < ItemsNumber; i++)
	{
		if (!bSilent && CheckForKey(VK_ESCAPE) && 
				BreakProcessDialog(GetMsg(MGetFile)))
			return true;

		CFileRecord * item = records[static_cast<int>(PanelItem[i].FindData.nPackSize)];
		if (item)
		{
			CString srcfile = m_currentPath;
			AddBeginSlash(srcfile);
			AddEndSlash(srcfile, true);
			srcfile += item->filename;
			if (m_procStruct.Lock())
			{
				m_procStruct.spos.Format(_T("%s %d/%d"), GetMsg(MProcessed), i, ItemsNumber);
				m_procStruct.from = srcfile;
				m_procStruct.to		= _T("");
				m_procStruct.bSilent = bSilent;

				m_procStruct.Unlock();
			}

			if (!DeleteFileFrom(srcfile, noPromt, ansYes, bSilent))
				return false;
		}
	}

	Reread();
	return true;
}

int fardroid::DeleteFiles( PluginPanelItem *PanelItem, int ItemsNumber, int OpMode )
{
	bool bSilent = IS_FLAG(OpMode, OPM_SILENT)||IS_FLAG(OpMode, OPM_FIND);
	bool noPromt = bSilent;

	if (!IS_FLAG(OpMode, OPM_QUICKVIEW) &&
			!IS_FLAG(OpMode, OPM_VIEW) &&
			!bSilent)
	{
		if (!DeleteFilesDialog())
			return FALSE;
	}

	if (m_procStruct.Lock())
	{
		m_procStruct.bSilent = true;
		m_procStruct.docontinue = true;
		m_procStruct.title = GetMsg(MDelFile);

		m_procStruct.Unlock();
	}

	DWORD threadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, ProcessThreadProc, this, 0, &threadID);

	BOOL bOk = DelItems(PanelItem, ItemsNumber, noPromt, noPromt, bSilent)?TRUE:FALSE;

	if (m_procStruct.Lock())
	{
		m_procStruct.docontinue = false;
		m_procStruct.Unlock();
	}

	CloseHandle(hThread);

	return bOk;
}

int fardroid::PutFiles( PluginPanelItem *PanelItem, int ItemsNumber, CString SrcPath, BOOL Move, int OpMode )
{
	CString srcdir = SrcPath;

	CString path = m_currentPath;

	bool bSilent = IS_FLAG(OpMode, OPM_SILENT)||IS_FLAG(OpMode, OPM_FIND);
	bool noPromt = bSilent;

	if (IS_FLAG(OpMode, OPM_VIEW))
		bSilent = false;

	if (!IS_FLAG(OpMode, OPM_QUICKVIEW) &&
		!IS_FLAG(OpMode, OPM_VIEW) &&
		!bSilent)
	{
		if (!CopyFilesDialog(path))
			return FALSE;
	}

	AddBeginSlash(path);
	AddEndSlash(path, true);
	AddEndSlash(srcdir);

	if (m_procStruct.Lock())
	{
		m_procStruct.bSilent = true;
		m_procStruct.docontinue = true;
		m_procStruct.title = GetMsg(MPutFile);
		m_procStruct.nTransmitted = 0;
		m_procStruct.nFileSize = 0;

		m_procStruct.Unlock();
	}

	DWORD threadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, ProcessThreadProc, this, 0, &threadID);

	BOOL bOk = PutItems(PanelItem, ItemsNumber, srcdir, path, noPromt, noPromt, bSilent)?TRUE:FALSE;

	if (m_procStruct.Lock())
	{
		m_procStruct.docontinue = false;
		m_procStruct.Unlock();
	}

	CloseHandle(hThread);

	return bOk;
}

int fardroid::CreateDir( CString &DestPath, int OpMode )
{
	CString path;
	CString srcdir = m_currentPath;
	AddBeginSlash(srcdir);
	AddEndSlash(srcdir, true);

	bool bSilent = IS_FLAG(OpMode, OPM_SILENT)||IS_FLAG(OpMode, OPM_FIND);
	bool noPromt = bSilent;

	if (IS_FLAG(OpMode, OPM_VIEW))
		bSilent = false;

	if (!IS_FLAG(OpMode, OPM_QUICKVIEW) &&
		!IS_FLAG(OpMode, OPM_VIEW) &&
		!bSilent)
	{
		if (!CreateDirDialog(path))
			return FALSE;
		else
			DestPath = path;
	}

	srcdir += path;

	CString sRes;
	BOOL bOk = ADB_mkdir(srcdir, sRes, bSilent);
	Reread();
	return bOk;
}

CFileRecord * fardroid::GetFileRecord( LPCTSTR sFileName )
{
	for (int i = 0; i < records.GetSize(); i++)
	{
		if (records[i]->filename.Compare(sFileName) == 0)
			return records[i];
	}
	return NULL;
}

void fardroid::ParseMemoryInfo(CString s)
{
	strvec tokens;

	CPanelLine pl;
	pl.separator = FALSE;

	CString regex = _T("/([\\w]+(?=:)):\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s))\\s+(\\w+(?=\\s|$))/");
	RegExTokenize(s, regex, tokens);
	if (tokens.GetSize() == 4)
	{
		pl.text = tokens[0];
		pl.data.Format(_T("%s/%s"), tokens[1], tokens[3]);
		lines.Add(pl);

		/*pl.text.Format(_T("%s %s:"), tokens[0], GetMsg(MMemUsed));
		pl.data = tokens[2];
		lines.Add(pl);

		pl.text.Format(_T("%s %s:"), tokens[0], GetMsg(MMemFree));
		pl.data = tokens[3];
		lines.Add(pl);*/
	}
}
void fardroid::GetMemoryInfo()
{
	CString sRes;
	ADBShellExecute(_T("free"), sRes, false);

	strvec str;
	Tokenize(sRes, str, _T("\n"));

	CPanelLine pl;
	pl.separator = TRUE;
	pl.text = GetMsg(MMemoryInfo);
	lines.Add(pl);

	if (str.GetSize() == 4)
	{
		for (int i = 1; i < str.GetSize(); i++)
			ParseMemoryInfo(str[i]);
	}
}
void fardroid::ParsePartitionInfo(CString s)
{
	strvec tokens;

	CPanelLine pl;
	pl.separator = FALSE;

	CString regex = _T("/(.*(?=:)):\\W+(\\w+(?=\\stotal)).+,\\s(\\w+(?=\\savailable))/");
	RegExTokenize(s, regex, tokens);
	if (tokens.GetSize() == 3)
	{
		pl.text = tokens[0];
		pl.data.Format(_T("%s/%s"), tokens[1], tokens[2]);
		lines.Add(pl);
	}
}
void fardroid::GetPartitionsInfo()
{
	CString sRes;

	if (conf.ShowAllPartitions)
		//ADBShellExecute(_T("toolbox df"), sRes, false);
		ADBShellExecute(_T("df"), sRes, false);
	else
	{
		//ADBShellExecute(_T("toolbox df /data"), sRes, false);
		ADBShellExecute(_T("df /data"), sRes, false);
		sRes += _T("\n");
		//ADBShellExecute(_T("toolbox df /sdcard"), sRes, false);
		ADBShellExecute(_T("df /sdcard"), sRes, false);
	}

	strvec str;
	Tokenize(sRes, str, _T("\n"));

	CPanelLine pl;
	pl.separator = TRUE;
	pl.text = GetMsg(MPartitionsInfo);
	lines.Add(pl);

	for (int i = 0; i < str.GetSize(); i++)
		ParsePartitionInfo(str[i]);
}

CString fardroid::GetPermissionsFile(const CString& FullFileName)
{
	    CString permissions;
		CString s;
		CString sRes;
		s.Format(_T("ls -l -a -d \"%s\""), FullFileName);
		if(ADBShellExecute(s, sRes, false))
		{
			strvec lines;
			Tokenize(sRes, lines, _T(" "));

			if(!sRes.IsEmpty() && 
				sRes.Find(_T("No such file or directory"))== -1 && 
				lines[0].GetLength() == 10)
			{
				permissions = lines[0];
			}
		}		
		return permissions;
}

CString fardroid::PermissionsFileToMask(CString Permission)
{
	CString permission_mask, tmp_str;

	Permission.Replace(_T('-'),_T('0'));
	Permission.Replace(_T('r'),_T('1'));
	Permission.Replace(_T('w'),_T('1'));
	Permission.Replace(_T('x'),_T('1'));
	
	permission_mask.Format(_T("%d"),_tcstoul(Permission.Mid(1,3).GetBuffer(11),0,2));
	tmp_str.Format(_T("%d"),_tcstoul(Permission.Mid(4,3).GetBuffer(11),0,2));
	permission_mask += tmp_str;
	tmp_str.Format(_T("%d"),_tcstoul(Permission.Mid(7,3).GetBuffer(11),0,2));
	permission_mask += tmp_str;

	return permission_mask;
}

bool fardroid::SetPermissionsFile(const CString& FullFileName, const CString& PermissionsFile)
{
	CString s;
	CString sRes;
	s.Format(_T("chmod %s \"%s\""), fardroid::PermissionsFileToMask(PermissionsFile), FullFileName);
	return (ADBShellExecute(s, sRes, false) != FALSE);
}

bool fardroid::DeviceTest()
{
	SOCKET sock = PrepareADBSocket();
	if (sock)
	{
		CloseADBSocket(sock);

		return true;
	}

	return false;

	/*lastError = S_OK;
	CString s = _T("cmd /c adb devices");
	CString sRes;
	if (ADBShellExecute(s, sRes, false))
	{
		strvec tokens;
		Tokenize(sRes, tokens, _T("\n"));
		if (tokens.GetSize() <= 1)
		{
			lastError = ERROR_DEV_NOT_EXIST;
			ShowADBExecError(GetMsg(MDeviceNotFound), false);
			return false;
		}
		return true;
	}
	return false;*/
}

SOCKET fardroid::CreateADBSocket()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	SOCKET sock = socket ( AF_INET, SOCK_STREAM, 0 );
	if  (sock)
	{
		sockaddr_in  dest_addr;
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port= htons (5037);
		if( inet_addr("127.0.0.1") != INADDR_NONE )
		{
			dest_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
			if(connect( sock, (sockaddr *)&dest_addr, sizeof(dest_addr)) == 0)
				return sock;
		}
		closesocket(sock);
	}
	WSACleanup();

	return 0;
}

bool fardroid::SendADBCommand( SOCKET sockADB, LPCTSTR sCMD )
{
	if (sockADB == 0)
		return false;

	CString sCommand;
	sCommand.Format(_T("%04X%s"), lstrlen(sCMD), sCMD);
	char * buf = getAnsiString(sCommand);

	SendADBPacket(sockADB, buf, sCommand.GetLength());
	my_free(buf);

	return CheckADBResponse(sockADB);
}

void fardroid::CloseADBSocket( SOCKET sockADB )
{
	if (sockADB)
		closesocket(sockADB);
	WSACleanup();
}

bool fardroid::CheckADBResponse( SOCKET sockADB )
{
	long msg;
	ReadADBPacket(sockADB, &msg, sizeof(msg));
	return msg == ID_OKAY;
}

bool fardroid::ReadADBSocket( SOCKET sockADB, char * buf, int bufSize )
{
	int nsize;
	int received = 0;
	while(received < bufSize)
	{
		nsize = recv( sockADB, buf, bufSize, 0 );
		if (nsize == SOCKET_ERROR)//ошибко
			return false;

		received += nsize;

		if (nsize > 0)
			buf[nsize] =0;

		if ( nsize == 0 || nsize == WSAECONNRESET )
			break;
	}

	return true;
}

SOCKET fardroid::PrepareADBSocket()
{
	lastError = S_OK;
	int tryCnt = 0;
tryagain:
	SOCKET sock = CreateADBSocket();
	if (sock)
	{
		if (!SendADBCommand(sock, _T("host:transport-usb")))
		{
			lastError = ERROR_DEV_NOT_EXIST;
			ShowADBExecError(GetMsg(MDeviceNotFound), false);

			CloseADBSocket(sock);
			sock = 0;
		}
	}
	else
	{
		if (tryCnt == 0)
		{
			tryCnt++;
			CString adb = conf.ADBPath;
			AddEndSlash(adb);
			adb += _T("adb.exe");
			HINSTANCE hInstance = ShellExecute(NULL, NULL, adb, _T("start-server"), NULL, SW_HIDE);
			if (hInstance > (HINSTANCE)32)
				goto tryagain;
		}
		else
		{
			lastError = ERROR_DEV_NOT_EXIST;
			ShowADBExecError(GetMsg(MDeviceNotFound), false);
		}
	}
	return sock;
}

bool fardroid::SendADBPacket( SOCKET sockADB, void * packet, int size )
{
	char *p = (char*) packet;
	int r = 0;

	while(size > 0)
	{
		r = send(sockADB, p, size, 0);

		if(r > 0)
		{
			size -= r;
			p += r;
		}
		else if (r < 0) return false;
		else if (r == 0) return true;
	}
	return true;
}

int fardroid::ReadADBPacket( SOCKET sockADB, void * packet, int size )
{
	char *p = (char*)packet;
	int r = 0;
	int received = 0;

	while(size > 0) 
	{
		r = recv(sockADB, p, size, 0);
		if(r > 0)
		{
			received += r;
			size -= r;
			p += r;
		}
		else if (r == 0) break;
		else return r;
	}

	return received;
}

BOOL fardroid::ADBShellExecute(LPCTSTR sCMD, CString & sRes, bool bSilent )
{
	SOCKET sockADB = PrepareADBSocket();

	BOOL bOK = FALSE;
	CString cmd;
	if (conf.UseSU)
		cmd.Format(_T("shell:su -c \'%s\'"), sCMD);
	else
		cmd.Format(_T("shell:%s"), sCMD);
	if (SendADBCommand(sockADB, cmd))
	{
		char * buf = new char[4097];
		while(true) 
		{
			int len = ReadADBPacket(sockADB, buf, 4096);
			if (len <= 0)
				break;

			buf[len] = 0;
			sRes += buf;
		}
		delete [] buf;
		bOK = TRUE;
	}

	CloseADBSocket(sockADB);

	return bOK;
}

bool fardroid::GetFrameBuffer( LPCTSTR sDest )
{
	SOCKET sockADB = PrepareADBSocket();

	if (SendADBCommand(sockADB, _T("framebuffer:")))
	{
		int fbinfo[13];
		if (ReadADBPacket(sockADB, fbinfo, sizeof(fbinfo)) <= 0)
		{
			CloseADBSocket(sockADB);
			return false;
		}

		int version = fbinfo[0];
		int bpp = fbinfo[1];
		int size = fbinfo[2];
		int width = fbinfo[3];
		int height = fbinfo[4];

		// Send nudge.
		char nudge[1] = {0};
		SendADBPacket(sockADB, nudge, 1);
		byte * buffer = new byte[size];
		if (ReadADBPacket(sockADB, buffer, size) <= 0)
		{
			delete [] buffer;
			CloseADBSocket(sockADB);
			return false;
		}

		WriteBMP(sDest, buffer, size, width, height, bpp);
		delete [] buffer;
	}

	CloseADBSocket(sockADB);
	return false;
}

BOOL fardroid::ADB_findmount( LPCTSTR sFS, strvec &fs_params, CString & sRes, bool bSilent )
{
	sRes.Empty();
	//if (ADBShellExecute(_T("toolbox mount"), sRes, bSilent))
	if (ADBShellExecute(_T("mount"), sRes, bSilent))
	{
		strvec tokens;
		Tokenize(sRes, tokens, _T("\n"));
		for (int i = 0; i < tokens.GetSize(); i++)
		{
			fs_params.RemoveAll();
			Tokenize(tokens[i], fs_params, _T(" "));
			if (fs_params.GetSize() == 6 && fs_params[1] == sFS)
				return TRUE;
		}
	}
	return FALSE;
}

BOOL fardroid::ADB_mount( LPCTSTR sFS, BOOL bAsRW, CString & sRes, bool bSilent )
{
	strvec fs_params;
	if (ADB_findmount(sFS, fs_params, sRes, bSilent))
	{
		CString cmd;
		//cmd.Format(_T("toolbox mount -o remount,%s -t %s %s %s"), bAsRW?_T("rw"):_T("ro"), fs_params[2], fs_params[0], sFS);
		cmd.Format(_T("mount -o remount,%s -t %s %s %s"), bAsRW?_T("rw"):_T("ro"), fs_params[2], fs_params[0], sFS);
		sRes.Empty();
		return ADBShellExecute(cmd, sRes, bSilent);
	}
	return FALSE;
}
/*BOOL fardroid::ADB_execute( LPCTSTR sCMD, CString & sRes, bool bSilent)
{
	m_bForceBreak = false;
	lastError = S_OK;
	CHAR buf[1024];

	STARTUPINFO si;
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	PROCESS_INFORMATION pi;

#ifdef USELOGGING
	CPerfCounter counter;
	counter.Start(_T("Exec prepare"));
#endif

	HANDLE newstdin,newstdout,read_stdout,write_stdin;

	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, true, NULL, false);
	sa.lpSecurityDescriptor = &sd;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = true;

#ifdef USELOGGING
	counter.Reset(_T("Exec create in-pipe"));
#endif

	if (!CreatePipe(&newstdin,&write_stdin,&sa,0))   //создаем пайп
	{
		lastError = ERROR_CREATE_FAILED;
		ShowADBExecError(_T(""), bSilent);
		return FALSE;
	}

#ifdef USELOGGING
	counter.Reset(_T("Exec create out-pipe"));
#endif

	if (!CreatePipe(&read_stdout,&newstdout,&sa,0)) //создаем пайп
	{
		lastError = ERROR_CREATE_FAILED;
		CloseHandle(newstdin);
		CloseHandle(write_stdin);
		ShowADBExecError(_T(""), bSilent);
		return FALSE;
	}

#ifdef USELOGGING
	counter.Reset(_T("Exec get startup info"));
#endif

	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	si.wShowWindow	= SW_HIDE;
	si.hStdOutput		= newstdout;
	si.hStdError		= newstdout;   //подменяем дескрипторы для
	si.hStdInput		= newstdin;    // дочернего процесса

#ifdef USELOGGING
	counter.Reset(_T("Exec create process"));
#endif
	//создаем дочерний процесс
	if (!CreateProcess(NULL, (LPWSTR)sCMD, NULL, NULL, TRUE, CREATE_NEW_CONSOLE|CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi))
	{
		lastError = ERROR_CREATE_FAILED;
		CloseHandle(newstdin);
		CloseHandle(newstdout);
		CloseHandle(read_stdout);
		CloseHandle(write_stdin);
		ShowADBExecError(_T(""), bSilent);
		return FALSE;
	}

	unsigned long exit=0;  //код завершения процесса
	unsigned long bread = 0;   //кол-во прочитанных байт
	unsigned long avail = 0;   //кол-во доступных байт

#ifdef USELOGGING
	counter.Reset(_T("Exec set buffers"));
#endif

	my_memset(buf, 0, 1024);

#ifdef USELOGGING
	counter.Reset(_T("Exec Wait"));
#endif

	WaitForSingleObject(pi.hProcess, conf.TimeOut);

	for(;;)      //основной цикл программы
	{
#ifdef USELOGGING
		counter.Reset(_T("Exec Peek"));
#endif

		PeekNamedPipe(read_stdout,buf,1023,&bread,&avail,NULL);

		if (bread != 0)
		{
#ifdef USELOGGING
			counter.Reset(_T("Exec Read"));
#endif

			if (avail > 1023)
			{
				while (bread >= 1023)
				{
					ReadFile(read_stdout,buf,1023,&bread,NULL);  //читаем из пайпа stdout
					sRes += buf;
					my_memset(buf, 0, 1024);
				}
			}
			else
			{
				ReadFile(read_stdout,buf,1023,&bread,NULL);
				sRes += buf;
			}

#ifdef USELOGGING
			counter.Reset(_T("Exec Status"));
#endif
		}

		GetExitCodeProcess(pi.hProcess,&exit); //пока дочерний процесс не закрыт
		if (exit != STILL_ACTIVE || m_bForceBreak)
			break;
	}

#ifdef USELOGGING
	counter.Reset(_T("Exec Cleanup"));
#endif

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(newstdin);            //небольшая уборка за собой
	CloseHandle(newstdout);
	CloseHandle(read_stdout);
	CloseHandle(write_stdin);

	if (exit != 0 && !m_bForceBreak)
	{
		lastError = ERROR_BAD_COMMAND;
		ShowADBExecError(sRes.Left(128), bSilent);
		return FALSE;
	}
	return TRUE;
}*/