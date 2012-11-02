#include "stdafx.h"

#include "FARstuffs.h"

const farStr * GetMsg( int MsgId ) 
{
	return(fInfo.GetMsg(fInfo.ModuleNumber,MsgId));
}
int ShowMessage(const farStr *msg, int buttons, const farStr* help, bool Warning /*= false*/ ) 
{
	int res = fInfo.Message(fInfo.ModuleNumber,(Warning?FMSG_WARNING:0)|FMSG_ALLINONE, help, (const farStr * const *)msg, 0, buttons);
	return res;
}
void ShowMessageWait(const farStr *const*msg, int msgsize) 
{
	fInfo.Message(fInfo.ModuleNumber,0/*FMSG_LEFTALIGN*/, NULL, msg, msgsize, 0);
}
int ShowDialog( int width, int height, const farStr *help, FarDialogItem * items, int count, HANDLE &hDlg ) 
{
	hDlg = fInfo.DialogInit(fInfo.ModuleNumber, -1, -1, width, height, help, items, count, 0, 0, NULL, 0);
	int res = fInfo.DialogRun(hDlg);
	return res;
}
CString GetPanelPath( bool another /*= false*/ ) 
{
	CString ret;
	fInfo.Control(another?PANEL_PASSIVE:PANEL_ACTIVE, FCTL_GETPANELDIR, 1024, (LONG_PTR)ret.GetBuffer(1024));
	ret.ReleaseBuffer();
	return ret;
}

FarDialogItem * GetFarDialogItem(const HANDLE &hDlg, DWORD item)
{
	//try
	//{
	if (hDlg)
	{
		LONG_PTR size = fInfo.SendDlgMessage(hDlg,DM_GETDLGITEM, item, 0);
		if (size > 0)
		{
			BYTE *buf = (BYTE*)my_malloc(size);
			if (hDlg)
			{
				fInfo.SendDlgMessage(hDlg,DM_GETDLGITEM, item, (LONG_PTR)buf);
				return (FarDialogItem *)buf;
			}
			else
				my_free(buf);
		}
	}
	//}
	//catch (...){}
	return NULL;
}
BOOL GetItemSelected(const HANDLE &hDlg, DWORD item ) 
{
	FarDialogItem *DialogItem = GetFarDialogItem(hDlg, item);
	if (DialogItem)
	{
		BOOL s = DialogItem->Selected;
		my_free(DialogItem);
		return s;
	}
	return FALSE;
}
CString GetItemData(const HANDLE &hDlg, DWORD item ) 
{
	FarDialogItem *DialogItem = GetFarDialogItem(hDlg, item);
	if (DialogItem)
	{
		CString str = DialogItem->PtrData;
		my_free(DialogItem);
		return str;
	}
	return _F("");
}
bool SetItemData(const HANDLE &hDlg, DWORD item, const CString &data) 
{
	if (!hDlg)
		return false;

	try
	{
		FarDialogItem *DialogItem = (FarDialogItem *)fInfo.SendDlgMessage(hDlg,DM_GETDLGITEM, item, 0);
		if (DialogItem)
		{
			DialogItem->PtrData = _C(data);
			fInfo.SendDlgMessage(hDlg,DM_SETDLGITEM, item, (LONG_PTR)DialogItem);
			return true;
		}
	}
	catch (...)
	{
		
	}
	return false;
}
bool SendMacro(const CString &macro ) 
{
	if (macro.IsEmpty()) return false;
	ActlKeyMacro km;
	km.Command = MCMD_POSTMACROSTRING;
	km.Param.PlainText.Flags=KSFLAGS_DISABLEOUTPUT;
	km.Param.PlainText.SequenceText = (wchar_t*)(LPCTSTR)macro;
	bool res = fInfo.AdvControl(fInfo.ModuleNumber, ACTL_KEYMACRO, &km)?true:false;
	return res;
}
int ShowMenu( const farStr* title, const farStr* bottom, const farStr* help, const FarMenuItem *Item, int ItemsNumber ) 
{
	int res = fInfo.Menu(fInfo.ModuleNumber, -1, -1, 0, FMENU_WRAPMODE|FMENU_AUTOHIGHLIGHT, title,
		bottom, help, 0, NULL, Item, ItemsNumber);
	return res;
}
int ShowMenu2( const farStr* title, const farStr* bottom, const farStr* help, const int * keys, int & code, const FarMenuItem *Item, int ItemsNumber ) 
{
	int res = fInfo.Menu(fInfo.ModuleNumber, -1, -1, 0, FMENU_WRAPMODE|FMENU_AUTOHIGHLIGHT, title,
		bottom, help, keys, &code, Item, ItemsNumber);
	return res;
}
bool ShowInputBox( const wchar_t* title, const wchar_t* prompt, const wchar_t* history, const wchar_t* help, const wchar_t* src, CString &dst ) 
{
	CString save = dst;
	BOOL bok = fInfo.InputBox(title, prompt, history, src, dst.GetBuffer(1000), 1000, help, FIB_EXPANDENV|FIB_BUTTONS);
	dst.ReleaseBuffer();
	if (!bok)
		dst = save;
	return bok?true:false;
}
void CommitOperation() 
{
	fInfo.AdvControl(fInfo.ModuleNumber, ACTL_COMMIT, 0);
}
void Editor( const CString& filename, const wchar_t* title ) 
{
	fInfo.Editor(filename, title, 0, 0, -1, -1, EF_DISABLEHISTORY, 0, 0, CP_AUTODETECT);
}

void Text( int X, int Y, DWORD Color, const CString& str ) 
{
	fInfo.Text(X, Y, Color, str);
}
void InitDialogItems(struct InitDialogItem *Init,struct FarDialogItem *Item,
										 int ItemsNumber)
{
	int I;
	struct FarDialogItem *PItem=Item;
	struct InitDialogItem *PInit=Init;
	for (I=0;I<ItemsNumber;I++,PItem++,PInit++)
	{
		PItem->Type=PInit->Type;
		PItem->X1=PInit->X1;
		PItem->Y1=PInit->Y1;
		PItem->X2=PInit->X2;
		PItem->Y2=PInit->Y2;
		PItem->Focus=PInit->Focus;
		PItem->Flags=PInit->Flags;
		PItem->DefaultButton=PInit->DefaultButton;
		PItem->MaxLen=0;
		PItem->History=(const farStr *)PInit->Selected;
		if ((unsigned int)(DWORD_PTR)PInit->Data<2000)
			PItem->PtrData = GetMsg((unsigned int)(DWORD_PTR)PInit->Data);
		else
			PItem->PtrData = PInit->Data;
	}
}

UINT64 GetPackSize( bool another, bool selected, int i )
{
	int size = fInfo.Control(another?PANEL_PASSIVE:PANEL_ACTIVE, selected?FCTL_GETSELECTEDPANELITEM:FCTL_GETPANELITEM, i, 0);
	if (size > 0)
	{
		BYTE *buf = (BYTE*)my_malloc(size);
		fInfo.Control(another?PANEL_PASSIVE:PANEL_ACTIVE, selected?FCTL_GETSELECTEDPANELITEM:FCTL_GETPANELITEM, i, (LONG_PTR)buf);
		PluginPanelItem * ffd = (PluginPanelItem *)buf;
		UINT64 res = ffd->FindData.nPackSize;
		my_free(buf);
		return res;
	}

	return -1;
}
bool CheckListBoxItem(const HANDLE &hDlg, int id, int i, bool checked )
{
	if (!hDlg)
		return false;

	FarListGetItem item = {0};
	item.ItemIndex = i;
	fInfo.SendDlgMessage(hDlg,DM_LISTGETITEM, id,(LONG_PTR)&item);

	if (checked)
		SET_FLAG(item.Item.Flags, LIF_CHECKED);
	else
		CLR_FLAG(item.Item.Flags, LIF_CHECKED);

	FarListUpdate upd = {0};
	upd.Index = item.ItemIndex;
	upd.Item = item.Item;

	fInfo.SendDlgMessage(hDlg,DM_LISTUPDATE, id,(LONG_PTR)&upd);

	return true;
}

bool ListBoxItemChecked(const HANDLE &hDlg, int id, int i)
{
	if (!hDlg)
		return false;

	FarListGetItem item = {0};
	item.ItemIndex = i;
	fInfo.SendDlgMessage(hDlg,DM_LISTGETITEM, id,(LONG_PTR)&item);

	return IS_FLAG(item.Item.Flags, LIF_CHECKED);
}
void ExpandEnvironmentStr( const CString & str, CString &expanded) 
{
	ExpandEnvironmentStrings(str, expanded.GetBuffer(1000), 1000);
	expanded.ReleaseBuffer();
}
CString GetFileName( bool another, bool selected, int i ) 
{
	int size = fInfo.Control(another?PANEL_PASSIVE:PANEL_ACTIVE, selected?FCTL_GETSELECTEDPANELITEM:FCTL_GETPANELITEM, i, 0);
	if (size > 0)
	{
		BYTE *buf = (BYTE*)my_malloc(size);
		fInfo.Control(another?PANEL_PASSIVE:PANEL_ACTIVE, selected?FCTL_GETSELECTEDPANELITEM:FCTL_GETPANELITEM, i, (LONG_PTR)buf);
		PluginPanelItem * ffd = (PluginPanelItem *)buf;
		CString res = ffd->FindData.lpwszFileName;
		my_free(buf);
		return res;
	}
	return _T("");
}
DWORD GetFileAttributes( bool another, bool selected, int i ) 
{
	int size = fInfo.Control(another?PANEL_PASSIVE:PANEL_ACTIVE, selected?FCTL_GETSELECTEDPANELITEM:FCTL_GETPANELITEM, i, 0);
	if (size > 0)
	{
		BYTE *buf = (BYTE*)my_malloc(size);
		fInfo.Control(another?PANEL_PASSIVE:PANEL_ACTIVE, selected?FCTL_GETSELECTEDPANELITEM:FCTL_GETPANELITEM, i, (LONG_PTR)buf);
		PluginPanelItem * ffd = (PluginPanelItem *)buf;
		DWORD ret = ffd->FindData.dwFileAttributes;
		my_free(buf);
		return ret;
	}
	return 0;
}
PluginPanelItem * GetFarPluginPanelItem(HANDLE hPanel, int FCTL, int i )
{
	LONG_PTR size = fInfo.Control(hPanel, FCTL, i, 0);
	if (size > 0)
	{
		BYTE *buf = (BYTE*)my_malloc(size);
		fInfo.Control(hPanel, FCTL, i, (LONG_PTR)buf);
		return (PluginPanelItem *)buf;
	}
	return NULL;
}
CString GetCurrentFileName( bool another) 
{
	PluginPanelItem *ffd = GetFarPluginPanelItem(another?PANEL_PASSIVE:PANEL_ACTIVE, FCTL_GETCURRENTPANELITEM, 0);
	if (ffd)
	{
		CString ret = ffd->FindData.lpwszFileName;
		my_free(ffd);
		return ret;
	}
	return _T("");
}