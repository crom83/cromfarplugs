#include "StdAfx.h"
#include "operac4.h"

operac4::operac4(void)
{
}
operac4::~operac4(void)
{
	//conf.Save();
	deleteRecords();
}
bool operac4::CopyFilesDialog(CString &dest) 
{
	struct InitDialogItem InitItemsC[]={
		/*00*/FDI_DOUBLEBOX (55, 9,(farStr *)MCopyTitle),
		/*01*/FDI_LABEL			( 2, 2, (farStr *)MCopyDest),
		/*02*/FDI_EDIT			( 2, 3,52, _F("operac4DestinationDir")),
		/*03*/FDI_CHECK			( 2, 5, (farStr*)MCopyUseLocalNames),
		/*04*/FDI_DEFCBUTTON( 7,(farStr *)MOk),
		/*05*/FDI_CBUTTON		( 7,(farStr *)MCancel)
	};
	FarDialogItem DialogItemsC[sizeof(InitItemsC)/sizeof(InitItemsC[0])];
	InitDialogItems(InitItemsC,DialogItemsC,sizeof(InitItemsC)/sizeof(InitItemsC[0]));

	wchar_t * editbuf = (wchar_t *)my_malloc(100);
	lstrcpyW(editbuf, _C(dest));
	DialogItemsC[2].PtrData = editbuf;
	HANDLE hdlg;
	DialogItemsC[3].Selected = conf.UseCachedName;

	int res = ShowDialog(55, 9, _F("CopyDialog"), DialogItemsC, sizeof(InitItemsC)/sizeof(InitItemsC[0]), hdlg);
	if (res == 4)
	{
		dest = GetItemData(hdlg, 2);
		conf.UseCachedName	= GetItemSelected(hdlg, 3);
	}
	fInfo.DialogFree(hdlg);
	my_free(editbuf);

	return res == 4;
}
HANDLE operac4::OpenFromMainMenu() 
{
	PanelInfo PInfo;
	fInfo.Control(INVALID_HANDLE_VALUE, FCTL_GETPANELINFO, 0, (LONG_PTR)&PInfo);
	if (!IsDirectory(false, false, PInfo.CurrentItem))
	{
		CString path = GetPanelPath();
		AddEndSlash(path);
		path += GetFileName(false, false, PInfo.CurrentItem);;
		return Open(path);
	}

	return INVALID_HANDLE_VALUE;
}
HANDLE operac4::OpenFromCommandLine(const CString& cmd ) 
{
	strvec tokens;
	Tokenize(cmd, tokens, _T(" /"));

	if (tokens.GetSize() == 0) 
		return INVALID_HANDLE_VALUE;

	TokensToParams(tokens, params);

	CString filename  = GetParam(params, _T("filename"));
	bool	havefile  = !filename.IsEmpty();

	if (havefile)
	{
		CString fn = filename;
		ExpandEnvironmentStr(fn, filename);
		if (!FileExists(filename))
			havefile = false;
	}

	if (havefile)
		return Open(filename);

	return INVALID_HANDLE_VALUE;
}
bool operac4::CopyItems(const CString& srcdir, const CString& dstdir, bool &noPromt, bool &ansYes, bool bSilent)
{
	PanelInfo PInfo;
	fInfo.Control(this, FCTL_GETPANELINFO, 0, (LONG_PTR)&PInfo);
	for (int i = 0; i < PInfo.SelectedItemsNumber; i++)
	{
		if (!IsDirectory(false, true, i))
		{
			CCacheRecord * item = records[static_cast<int>(GetPackSize(false, true, i))];
			if (item)
			{
				CString dstfile = dstdir;
				CString srcfile = srcdir;
				GetName(item, &srcfile, &dstfile);
				if (!CopyFile(srcfile, dstfile, noPromt, ansYes, bSilent))
					return false;
			}
		}
	}
	return true;
}
bool operac4::DeleteFile(const CString& name, bool bSilent ) 
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
bool operac4::CopyFile(const CString& src, const CString& dst, bool &noPromt, bool &ansYes, bool bSilent)
{
	CString msg;
	CString msgexists;
	msg.Format(L"%s\n%s\n%s\n%s\n%s\n%s\n%s", GetMsg(MCopyTitle), GetMsg(MCopyWarnIfExists), GetMsg(MYes), GetMsg(MNo), GetMsg(MAlwaysYes), GetMsg(MAlwaysNo), GetMsg(MCancel));
	msgexists.Format(msg, dst);

	CString cdir;
	CString ddir = ExtractPath(dst);

	GetCurrentDirectory(1000, cdir.GetBuffer(1000));
	cdir.ReleaseBuffer();
	if (!SetCurrentDirectory(ddir))
		MakeDirs(ddir);
	SetCurrentDirectory(cdir);

	if (FileExists(dst))
	{
		if (!noPromt)
		{
			switch (ShowMessage(msgexists, 5, _F("warnifexists"), true))
			{
				case 0: if (!DeleteFile(dst, false))  
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

repeatcopy:
	BOOL res = ::CopyFile(src, dst, true);
	if (!res)
	{
		if (bSilent)//если отключен вывод на экран, то просто возвращаем что все ОК
			return true;


		DWORD err = GetLastError();
		CString errstr;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errstr.GetBuffer(1000), 1000, NULL);
		errstr.ReleaseBuffer();
		errstr.TrimLeft();
		errstr.TrimRight();

		CString errmsg;
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
		}
	}

	return true;
}
HANDLE operac4::Open(CString filename) 
{
	//HANDLE ss = playlist->AddInit();

	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) 
		return INVALID_HANDLE_VALUE;

	FileHeader fh = {0};
	DWORD rd = 0;
	BOOL res = FALSE;

	CHECKREADOK(fh);

	if (fh.FileVer	!= 0x00100000 ||
			fh.SoftVer	!= 0x00000200 ||
			fh.idSize		!= 0x0100 ||
			fh.lenSize	!= 0x0200)
	{
		CloseHandle(hFile);
		return INVALID_HANDLE_VALUE;
	}

	SECID sectionID = 0;

	while (true)
	{
		CHECKREADOK(sectionID);
		DWORD pstat = ParseSection(hFile, sectionID);
		switch(pstat)
		{
		case ERROR_SUCCESS:
			continue;
		case ERROR_HANDLE_EOF:
			CloseHandle(hFile);
			fileUnderCursor = filename;
			return (HANDLE)this;
		default:
			CloseHandle(hFile);
			return INVALID_HANDLE_VALUE;
		}
	}
}
DWORD operac4::ParseSection( HANDLE hFile, SECID sectionID )
{
	SECSZ sectionSZ = 0;
	DWORD rd = 0;
	BOOL res = FALSE;
	int bufPos = 0;

	if (sectionID & MSB_VALUE)
	{
		//какой-то флаг
	}
	else
	{
		CHECKREADOK2(sectionSZ);
		SWAP_SECSZ(sectionSZ);

		switch(sectionID)
		{
		case 0x01: //disk cache
		case 0x41: //download
			{
				BYTE * buf = (BYTE *)my_malloc(sectionSZ);
				if (!buf)
					return false;

				res = ReadFile(hFile, buf, sectionSZ, &rd, NULL);
				if (res && rd == sectionSZ)
				{
					CCacheRecord * rec = new CCacheRecord();
					switch(sectionID)
					{
					case 0x01: //disk cache
						rec->recordType = RECORD_CACHE;
						break;
					case 0x41: //download
						rec->recordType = RECORD_DOWNLOAD;
						break;
					}
					if (!ParseSectionBuf(buf, bufPos, sectionSZ, rec))
					{
						my_free(rec);
						my_free(buf);
						return ERROR_INVALID_DATA;
					}
					my_free(buf);

					if (!rec->filename.IsEmpty())
						records.Add(rec);
					else
						delete rec;
				}
				else
				{
					my_free(buf);
					return ERROR_READ_FAULT;
				}
			}
			break;
		default:
			SetFilePointer(hFile, sectionSZ, NULL, FILE_CURRENT);
			break;
		}
	}

	return ERROR_SUCCESS;
}

bool operac4::ParseSectionBuf( BYTE * buf, int &bufPos, int bufSize, CCacheRecord * record )
{
	SECID	sectionID = 0;
	SECSZ	sectionSZ = 0;

	if (!record)
		 return false;

	while (bufPos < bufSize)
	{
		sectionID = *(buf + bufPos);
		bufPos += sizeof(SECID);
		if (sectionID & 0x80)
			continue;

		memcpy(&sectionSZ, buf + bufPos, sizeof(SECSZ));
		SWAP_SECSZ(sectionSZ);
		bufPos += sizeof(SECSZ);

		char * t = NULL;
		switch(sectionID)
		{
		case 0x03://url
			t = (char*)my_malloc(sectionSZ+1);
			memcpy(t, buf + bufPos, sectionSZ);
			t[sectionSZ] = 0;
			record->url = t;
			my_free(t);
			break;
		case 0x04://время последнего посещения
			memcpy(&record->lastVisited, buf + bufPos, sectionSZ);
			SWAP_DWORD(record->lastVisited);
			break;
		case 0x05://время последней загрузки
			memcpy(&record->lastLoaded, buf + bufPos, sectionSZ);
			SWAP_QWORD(record->lastLoaded);
			break;
		case 0x07://статус загрузки
			memcpy(&record->status, buf + bufPos, sectionSZ);
			SWAP_DWORD(record->status);
			break;
		case 0x08://размер файла
			memcpy(&record->size, buf + bufPos, sectionSZ);
			SWAP_QWORD(record->size);
			break;
		case 0x09://MIME
			t = (char*)my_malloc(sectionSZ+1);
			memcpy(t, buf + bufPos, sectionSZ);
			t[sectionSZ] = 0;
			record->mime = t;
			my_free(t);
			break;
		case 0x0a://CodePage
			t = (char*)my_malloc(sectionSZ+1);
			memcpy(t, buf + bufPos, sectionSZ);
			t[sectionSZ] = 0;
			record->cp = t;
			my_free(t);
			break;
		case 0x0d://filename
			t = (char*)my_malloc(sectionSZ+1);
			memcpy(t, buf + bufPos, sectionSZ);
			t[sectionSZ] = 0;
			utf8ToUnicode(t, record->filename.GetBuffer(1000), -1);
			record->filename.ReleaseBuffer();
			my_free(t);
			break;
		case 0x16://время истечения срока хранения
			memcpy(&record->blackDate, buf + bufPos, sectionSZ);
			SWAP_QWORD(record->blackDate);
			break;
		}
		bufPos += sectionSZ;
	}

	return true;
}

void operac4::deleteRecords()
{
	for (int i = 0; i < records.GetSize(); i++)
	{
		delete records[i];
	}
	records.RemoveAll();
}

int operac4::GetFindData( struct PluginPanelItem **pPanelItem,int *pItemsNumber,int OpMode )
{
	*pPanelItem=NULL;
	*pItemsNumber=0;

	int items = records.GetSize();

	PluginPanelItem *NewPanelItem=(PluginPanelItem *)my_malloc(sizeof(PluginPanelItem)*items);
	*pPanelItem=NewPanelItem;

	if (NewPanelItem == NULL) 
		return FALSE;

	CCacheRecord * item;

	for (int Z = 0; Z < items; Z++)
	{
		my_memset(&NewPanelItem[Z],0,sizeof(PluginPanelItem));

		item = records[Z];
		NewPanelItem[Z].FindData.dwFileAttributes	= FILE_ATTRIBUTE_ARCHIVE;
		NewPanelItem[Z].CustomColumnNumber = 2;
		NewPanelItem[Z].FindData.nPackSize = Z;
		NewPanelItem[Z].FindData.nFileSize		 = item->size;
		NewPanelItem[Z].Description = (LPCTSTR)item->mime;
		NewPanelItem[Z].CustomColumnData = item->GetColumns();
		NewPanelItem[Z].FindData.ftCreationTime = UnixToFileTime(item->lastVisited);
		NewPanelItem[Z].FindData.ftLastAccessTime = UnixToFileTime(item->blackDate);
		NewPanelItem[Z].FindData.ftLastWriteTime = UnixToFileTime(item->lastLoaded);
		NewPanelItem[Z].Owner = NULL;

		if (item->recordType == RECORD_CACHE)
		{
			if (!conf.ShortNames)
				NewPanelItem[Z].FindData.lpwszFileName = my_strdupW((wchar_t*)(LPCTSTR)item->url);
			else
			{
				CString path = GenShortName(item->url, conf.UseExtensions, FALSE, item->mime);
				NewPanelItem[Z].FindData.lpwszFileName = my_strdupW((wchar_t*)(LPCTSTR)path);
			}
		}
		else
		{
			if (!conf.ShortNames)
				NewPanelItem[Z].FindData.lpwszFileName = my_strdupW((wchar_t*)(LPCTSTR)item->filename);
			else
			{
				CString path = ExtractName(item->filename);
				NewPanelItem[Z].FindData.lpwszFileName = my_strdupW((wchar_t*)(LPCTSTR)path);
			}
		}
	}
	*pItemsNumber = items;

	return TRUE;
}

void operac4::FreeFindData( struct PluginPanelItem *PanelItem,int ItemsNumber )
{

	for (int I = 0; I < ItemsNumber; I++)
	{
		/*if (PanelItem[I].CustomColumnData[0])
		{
			my_free((wchar_t*)PanelItem[I].CustomColumnData[0]);
		}
		if (PanelItem[I].CustomColumnData[1])
		{
			my_free((wchar_t*)PanelItem[I].CustomColumnData[1]);
		}*/
		/*if (PanelItem[I].CustomColumnData)
		{
			my_free((wchar_t*)PanelItem[I].CustomColumnData);
		}*/
		/*if (PanelItem[I].Description)
		{
			my_free((wchar_t*)PanelItem[I].Description);
		}*/
		if (PanelItem[I].FindData.lpwszFileName)
		{
			my_free((wchar_t*)PanelItem[I].FindData.lpwszFileName);
		}
	}
	if (PanelItem)
	{
		my_free(PanelItem);
		PanelItem = NULL;
	}
}

int operac4::GetFiles( PluginPanelItem *PanelItem, int ItemsNumber, CString &DestPath, int OpMode )
{
	CCacheRecord * item = records[static_cast<int>(PanelItem->FindData.nPackSize)];
	if (!item)
		return FALSE;

	if (IS_FLAG(OpMode, OPM_QUICKVIEW))
	{
		CString path = DestPath;
		AddEndSlash(path);
		if (item->recordType == RECORD_CACHE)
			path += item->filename;
		else if (item->recordType == RECORD_DOWNLOAD)
		{
			path += ExtractName(item->filename);
		}

		HANDLE stream = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (stream == INVALID_HANDLE_VALUE) 
			return FALSE;

		CString line;
		WRITELINESTR(GetMsg(MInfoFile), (void*)(LPCTSTR)fileUnderCursor);
		WRITELINEBRAKE;
		WRITELINESTR(GetMsg(MInfoURL), (void*)(LPCTSTR)item->url);
		WRITELINESTR(GetMsg(MInfoFileName),(void*)(LPCTSTR)item->filename);
		WRITELINEINT(GetMsg(MInfoFileSize), (void*)item->size);
		WRITELINESTR(GetMsg(MInfoFileMIME), (void*)(LPCTSTR)item->mime);
		WRITELINESTR(GetMsg(MInfoFileCP), (void*)(LPCTSTR)item->cp);
		switch(item->status)
		{
		case 2:
			WRITELINESTR(GetMsg(MInfoFileStat), (void*)(LPCTSTR)_T("Loaded"));
			break;
		case 4:
			WRITELINESTR(GetMsg(MInfoFileStat), (void*)(LPCTSTR)_T("Aborted"));
		  break;
		case 5:
			WRITELINESTR(GetMsg(MInfoFileStat), (void*)(LPCTSTR)_T("Breaked"));
		}
		switch(item->recordType)
		{
		case RECORD_CACHE:
			WRITELINESTR(GetMsg(MInfoFileType), (void*)(LPCTSTR)_T("Cache"));
			break;
		case RECORD_DOWNLOAD:
			WRITELINESTR(GetMsg(MInfoFileType), (void*)(LPCTSTR)_T("Download"));
			break;
		case RECORD_LINK:
			WRITELINESTR(GetMsg(MInfoFileType), (void*)(LPCTSTR)_T("Link"));
		}
		WRITELINEDATE32(GetMsg(MInfoLastVisited), (time_t*)&item->lastVisited);
		WRITELINEDATE64(GetMsg(MInfoLastLoaded), &item->lastLoaded);
		WRITELINEDATE64(GetMsg(MInfoBlackDate), &item->blackDate);
exit:
		CloseHandle(stream);
		return TRUE;
	}
	else
	{
		CString srcdir = ExtractPath(fileUnderCursor);
		if (!srcdir.IsEmpty())
			AddEndSlash(srcdir);

		CString path = DestPath;

		if (!IS_FLAG(OpMode, OPM_VIEW) &&
				!IS_FLAG(OpMode, OPM_SILENT) &&
				!IS_FLAG(OpMode, OPM_FIND))
		{
			if (!CopyFilesDialog(path))
				return FALSE;
			else
				DestPath = path;
		}

		AddEndSlash(path);

		bool bSilent = IS_FLAG(OpMode, OPM_SILENT)||IS_FLAG(OpMode, OPM_FIND);
		bool noPromt = bSilent;
		if (IS_FLAG(OpMode, OPM_VIEW))
		{
			GetName(item, &srcdir, &path);
			return CopyFile(srcdir, path, noPromt, noPromt, bSilent)?TRUE:FALSE;
		}
		else
			return CopyItems(srcdir, path, noPromt, noPromt, bSilent)?TRUE:FALSE;
	}

	return FALSE;
}

void operac4::PreparePanel( OpenPluginInfo *Info )
{
	//TODO!!! - сделать динамические массивы
	Info->HostFile = _C(fileUnderCursor);

	panelTitle.Format(_T(" %s: %s "), GetMsg(MTitle), ExtractName(fileUnderCursor));
	Info->PanelTitle = _C(panelTitle);

	static CString ver;
	ver = GetVersionString();
	InfoPanelLineArray[0].Text = ver;
	InfoPanelLineArray[0].Separator = TRUE;
}

CString operac4::GetName(CCacheRecord * item, CString * srcfile, CString * dstdir)
{
	CString file;
	if (item->recordType == RECORD_CACHE)
	{
		if (conf.UseCachedName)
			file = GenShortName(item->filename, TRUE, TRUE, item->mime);
		else
			file = GenShortName(item->url, TRUE, FALSE, item->mime);
		if (srcfile)
			(*srcfile) += item->filename;
		if (dstdir)
			(*dstdir) += file;
	}
	else if (item->recordType == RECORD_DOWNLOAD)
	{
		if (dstdir)
			(*dstdir) += ExtractName(item->filename);

		file = ExtractName(item->filename);
		if (srcfile)
		{
			if (ExtractPath(item->filename).IsEmpty())
				(*srcfile) += item->filename;
			else
				(*srcfile) = item->filename;
		}
	}
	return file;
}

bool operac4::CopyFileWithNewName()
{
	PanelInfo PInfo;
	fInfo.Control(this, FCTL_GETPANELINFO, 0, (LONG_PTR)&PInfo);
	if (!IsDirectory(false, false, PInfo.CurrentItem))
	{
		CCacheRecord * item = records[static_cast<int>(GetPackSize(false, false, PInfo.CurrentItem))];
		if (item)
		{
			CString srcdir = ExtractPath(fileUnderCursor);
			if (!srcdir.IsEmpty())
				AddEndSlash(srcdir);

			CString dstfile = GetPanelPath(true);
			CString srcfile = srcdir;
			CString file = GetName(item, &srcfile, NULL);
			if (CopyFileDialog(dstfile, file))
			{
				AddEndSlash(dstfile);
				PrepareShortName(file);
				dstfile += file;
				bool noPrompt = false;
				bool ansYes = false;
				return CopyFile(srcfile, dstfile, noPrompt, ansYes, false);
			}
			return true;
		}
	}
	return false;
}

bool operac4::CopyFileDialog( CString &destpath, CString &destname )
{
	struct InitDialogItem InitItemsC[]={
		/*00*/FDI_DOUBLEBOX (55, 9,(farStr *)MCopyTitle),
		/*01*/FDI_LABEL			( 2, 2, (farStr *)MCopyDest),
		/*02*/FDI_EDIT			( 2, 3,52, _F("operac4DestinationDir")),
		/*03*/FDI_LABEL			( 2, 4, (farStr *)MCopyDestName),
		/*04*/FDI_EDIT			( 2, 5,52, _F("operac4DestinationName")),
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

void operac4::Reread()
{
	deleteRecords();

	Open(fileUnderCursor);
}