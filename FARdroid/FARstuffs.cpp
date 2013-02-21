#include "stdafx.h"

#include "FARstuffs.h"

const farStr * GetMsg( int MsgId ) 
{
	return(fInfo.GetMsg(&MainGuid,MsgId));
}
int ShowMessage(const farStr *msg, int buttons, const farStr* help, bool Warning /*= false*/ ) 
{
	int res = fInfo.Message(&MainGuid,&MsgGuid,(Warning?FMSG_WARNING:0)|FMSG_ALLINONE, help, (const farStr * const *)msg, 0, buttons);
	return res;
}
void ShowMessageWait(const farStr *const*msg, int msgsize) 
{
	fInfo.Message(&MainGuid,&MsgWaitGuid,0/*FMSG_LEFTALIGN*/, NULL, msg, msgsize, 0);
}
int ShowDialog( int width, int height, const farStr *help, FarDialogItem * items, int count, HANDLE &hDlg ) 
{
	hDlg = fInfo.DialogInit(&MainGuid,&DialogGuid, -1, -1, width, height, help, items, count, 0, 0, NULL, 0);
	int res = fInfo.DialogRun(hDlg);
	return res;
}
CString GetPanelPath( bool another /*= false*/ ) 
{
	CString ret;
	
	int Size=(int)fInfo.PanelControl(another?PANEL_PASSIVE:PANEL_ACTIVE,FCTL_GETPANELDIRECTORY,0,0);
	FarPanelDirectory* dirInfo=(FarPanelDirectory*)my_malloc(Size);
	dirInfo->StructSize = sizeof(FarPanelDirectory);
	fInfo.PanelControl(another?PANEL_PASSIVE:PANEL_ACTIVE,FCTL_GETPANELDIRECTORY,Size,dirInfo);
	ret = dirInfo->Name;
	my_free(dirInfo);
	//fInfo.Control(another?PANEL_PASSIVE:PANEL_ACTIVE, FCTL_GETPANELDIR, 1024, (LONG_PTR)ret.GetBuffer(1024));
	//ret.ReleaseBuffer();
	return ret;
}

FarDialogItem * GetFarDialogItem(const HANDLE &hDlg, DWORD item)
{
	if (hDlg)
	{
		struct FarGetDialogItem DialogItem={sizeof(FarGetDialogItem)};
		DialogItem.Item = (FarDialogItem *)my_malloc(DialogItem.Size=fInfo.SendDlgMessage(hDlg,DM_GETDLGITEM,item,NULL));
		if (DialogItem.Item)
		{
			fInfo.SendDlgMessage(hDlg,DM_GETDLGITEM,item,&DialogItem);
			return DialogItem.Item;
		}
	}
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
		CString str = DialogItem->Data;
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
		FarDialogItem *DialogItem = GetFarDialogItem(hDlg, item);
		if (DialogItem)
		{
			DialogItem->Data = _C(data);
			fInfo.SendDlgMessage(hDlg,DM_SETDLGITEM, item, (void *)DialogItem);
			my_free(DialogItem);
			return true;
		}
	}
	catch (...)
	{
		
	}
	return false;
}
void InitDialogItems(struct InitDialogItem *Init,struct FarDialogItem *Item, int ItemsNumber)
{
	int I;
	struct FarDialogItem *PItem=Item;
	struct InitDialogItem *PInit=Init;
	for (I=0;I<ItemsNumber;I++,PItem++,PInit++)
	{
		memset((void *)PItem, 0, sizeof(*PItem));
		PItem->Type=(FARDIALOGITEMTYPES)PInit->Type;
		PItem->X1=PInit->X1;
		PItem->Y1=PInit->Y1;
		PItem->X2=PInit->X2;
		PItem->Y2=PInit->Y2;
//		PItem->Focus=PInit->Focus;
		PItem->Flags=PInit->Flags;
		if(PInit->DefaultButton)
			PItem->Flags |= DIF_DEFAULTBUTTON;
		PItem->MaxLength=0;
		PItem->History=(const farStr *)PInit->Selected;
		if ((unsigned int)(DWORD_PTR)PInit->Data<2000)
			PItem->Data = GetMsg((unsigned int)(DWORD_PTR)PInit->Data);
		else
			PItem->Data = PInit->Data;
	}
}
CString GetFileName( bool another, bool selected, int i ) 
{
	int size = fInfo.PanelControl(another?PANEL_PASSIVE:PANEL_ACTIVE, selected?FCTL_GETSELECTEDPANELITEM:FCTL_GETPANELITEM, i, 0);
	if(size > 0)
	{
		struct FarGetPluginPanelItem item;
		item.StructSize = sizeof(item);
		item.Size = size;
		item.Item = (PluginPanelItem *)my_malloc(size);
		fInfo.PanelControl(another?PANEL_PASSIVE:PANEL_ACTIVE, selected?FCTL_GETSELECTEDPANELITEM:FCTL_GETPANELITEM, i, &item);
		CString ret = item.Item->FileName;
		my_free(item.Item);
		return ret;
	}
	return _T("");
}

DWORD GetFileAttributes( bool another, bool selected, int i ) 
{
	intptr_t size = fInfo.PanelControl(another?PANEL_PASSIVE:PANEL_ACTIVE, selected?FCTL_GETSELECTEDPANELITEM:FCTL_GETPANELITEM, i, 0);
	if (size > 0)
	{
		struct FarGetPluginPanelItem item;
		item.StructSize = sizeof(item);
		item.Size = size;
		item.Item = (PluginPanelItem *)my_malloc(size);
		fInfo.PanelControl(another?PANEL_PASSIVE:PANEL_ACTIVE, selected?FCTL_GETSELECTEDPANELITEM:FCTL_GETPANELITEM, i, 0);
		DWORD ret = item.Item->FileAttributes;
		my_free(item.Item);
		return ret;
	}
	return 0;
}

CString GetCurrentFileName( bool another) 
{
	
	intptr_t size = fInfo.PanelControl(another?PANEL_PASSIVE:PANEL_ACTIVE, FCTL_GETCURRENTPANELITEM, 0, 0);
	if(size > 0)
	{
		struct FarGetPluginPanelItem item;
		item.StructSize = sizeof(item);
		item.Size = size;
		item.Item = (PluginPanelItem *)my_malloc(size);
		fInfo.PanelControl(another?PANEL_PASSIVE:PANEL_ACTIVE, FCTL_GETCURRENTPANELITEM, 0, &item);
		CString ret = item.Item->FileName;
		my_free(item.Item);
		return ret;
	}
	return _T("");
}
