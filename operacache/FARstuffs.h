#pragma once

#define FDI_CONTROL( tp,x,y,x1,y1,f,s,fl,def,txt )			 { tp,            x,y,x1,y1,f,s,fl,def,txt }
#define FDI_DOUBLEBOX2( x1,y1,x2,y2,txt )				FDI_CONTROL( DI_DOUBLEBOX,x1,y1,x2,y2,0,0, 0,  0,txt )
#define FDI_DOUBLEBOX( x1,y1,txt )						FDI_CONTROL( DI_DOUBLEBOX,  0,0,x1,y1,0,0, 0,  0,txt )
#define FDI_SINGLEBOX( x,y, x1,y1,txt )				FDI_CONTROL( DI_SINGLEBOX,  x,y,x1,y1,0,0, 0,  0,txt )
#define FDI_COLORSINGLEBOX(x,y, x1,y1,clr,txt)FDI_CONTROL( DI_SINGLEBOX,  x,y,x1,y1,0,0,DIF_SETCOLOR|(clr), 0,txt )
#define FDI_AMPDOUBLEBOX( x1,y1,txt )					FDI_CONTROL( DI_DOUBLEBOX,  0,0,x1,y1,0,0,DIF_SHOWAMPERSAND,  0,txt )
#define FDI_COMBOBOX( x,y,x1,fl,txt )					FDI_CONTROL( DI_COMBOBOX,		x,y,x1, 0,0,0,fl,  0,txt )
#define FDI_LISTBOX( x,y,x1,y1,fl,txt )				FDI_CONTROL( DI_LISTBOX,		x,y,x1,y1,0,0,fl|DIF_LISTWRAPMODE,  0,txt )
#define FDI_SEPARATOR( x,y,txt )              FDI_CONTROL( DI_TEXT,       x,y, 0, 0,0,0,DIF_SEPARATOR,  0,txt )
#define FDI_LABEL( x,y,txt )                  FDI_CONTROL( DI_TEXT,       x,y, 0, 0,0,0, 0,  0,txt )
#define FDI_COLORLABEL( x,y,clr,txt )         FDI_CONTROL( DI_TEXT,       x,y, 0, 0,0,0,DIF_SETCOLOR|(clr),0,txt )
#define FDI_AMPCOLORLABEL( x,y,clr,txt )      FDI_CONTROL( DI_TEXT,       x,y, 0, 0,0,0,DIF_SHOWAMPERSAND|DIF_SETCOLOR|(clr),0,txt )
#define FDI_FIXEDIT( x,y,x1,h )								FDI_CONTROL( DI_FIXEDIT,    x,y,x1, y,0,(DWORD_PTR)h, h?DIF_HISTORY|DIF_USELASTHISTORY:0, 0,NULL )
#define FDI_EDIT( x,y,x1,h )                  FDI_CONTROL( DI_EDIT,       x,y,x1, y,0,(DWORD_PTR)h, h?DIF_HISTORY|DIF_USELASTHISTORY:0, 0,NULL )
#define FDI_PASS( x,y,x1 )										FDI_CONTROL( DI_PSWEDIT,    x,y,x1, y,0,0, DIF_MASKEDIT, 0,NULL )
#define FDI_DISABLEDEDIT( x,y,x1 )            FDI_CONTROL( DI_EDIT,       x,y,x1, y,0,0,DIF_DISABLE,0,NULL )
#define FDI_COLORCHECK( x,y,clr,txt )         FDI_CONTROL( DI_CHECKBOX,   x,y, 0, 0,0,0,DIF_SETCOLOR|(clr),  0,txt )
#define FDI_CHECK( x,y,txt )                  FDI_CONTROL( DI_CHECKBOX,   x,y, 0, 0,0,0, 0,  0,txt )
#define FDI_RADIO( x,y,txt )                  FDI_CONTROL( DI_RADIOBUTTON,x,y, 0, 0,0,0, 0,  0,txt )
#define FDI_STARTRADIO( x,y,txt )             FDI_CONTROL( DI_RADIOBUTTON,x,y, 0, 0,0,0, DIF_GROUP,0,txt )
#define FDI_BUTTON( x,y,txt )                 FDI_CONTROL( DI_BUTTON,     x,y, 0, 0,0,0, 0,  0,txt )
#define FDI_BUTTON2( x,y,txt )                FDI_CONTROL( DI_BUTTON,     x,y, 0, 0,0,0,DIF_NOBRACKETS,0,txt )
#define FDI_COLORBUTTON( x,y,clr,txt )        FDI_CONTROL( DI_BUTTON,     x,y, 0, 0,0,0,DIF_SETCOLOR|(clr),0,txt )
#define FDI_COLORBUTTON2( x,y,clr,txt )       FDI_CONTROL( DI_BUTTON,     x,y, 0, 0,0,0,DIF_NOBRACKETS|DIF_NOFOCUS|DIF_SETCOLOR|(clr),0,txt )
#define FDI_DEFBUTTON( x,y,txt )              FDI_CONTROL( DI_BUTTON,     x,y, 0, 0,0,0, 0,  1,txt )
#define FDI_CBUTTON( y,txt )									FDI_CONTROL( DI_BUTTON,     0,y, 0, 0,0,0, DIF_CENTERGROUP,  0,txt )
#define FDI_DEFCBUTTON( y,txt )								FDI_CONTROL( DI_BUTTON,     0,y, 0, 0,0,0, DIF_CENTERGROUP,  1,txt )

void Text(int X, int Y, DWORD Color, const CString& str);
void InitDialogItems(InitDialogItem *Init, FarDialogItem *Item,	int ItemsNumber);
const farStr *GetMsg(int MsgId);
int ShowMessage(const farStr* msg, int buttons, const farStr* help, bool Warning = false);
void ShowMessageWait(const farStr *const*msg, int msgsize);
bool ShowInputBox(const farStr* title, const farStr* prompt, const farStr* history, const farStr* help, const farStr* src, CString &dst);
void Editor(const CString& filename, const farStr* title);
int ShowMenu(const farStr* title, const farStr* bottom, const farStr* help, const FarMenuItem *Item, int ItemsNumber);
int ShowMenu2(const farStr* title, const farStr* bottom, const farStr* help, const int * keys, int & code, const FarMenuItem *Item, int ItemsNumber);

CString GetPanelPath( bool another = false );
void ConvertNameToReal(CString &name );
bool SendMacro(const CString& macro);
void CommitOperation();
bool CheckListBoxItem(const HANDLE &hDlg, int id, int i, bool checked);
bool ListBoxItemChecked(const HANDLE &hDlg, int id, int i);
void ExpandEnvironmentStr( const CString & str, CString &expanded);
int ShowDialog( int width, int height, const farStr *help, FarDialogItem * items, int count, HANDLE &hDlg );
UINT64 GetPackSize( bool another, bool selected, int i );
BOOL GetItemSelected(const HANDLE &hDlg, DWORD item );
CString GetItemData(const HANDLE &hDlg, DWORD item );
CString GetFileName( bool another, bool selected, int i );
DWORD GetFileAttributes( bool another, bool selected, int i );
FarDialogItem * GetFarDialogItem(const HANDLE &hDlg, DWORD item);
PluginPanelItem * GetFarPluginPanelItem(HANDLE hPanel, int FCTL, int i );