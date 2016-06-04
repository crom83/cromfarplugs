//Reading string from language file
char *GetMsg(int MsgId)
{
  return((char*)(Info.GetMsg(Info.ModuleNumber,MsgId)));
}

void InitDialogItems(struct InitDialogItem *Init,
                     struct FarDialogItem *Item,
                     int ItemsNumber)
{
  int i;
  for (i=0; i < ItemsNumber; i++)
  {
    Item[i].Type=Init[i].Type;
    Item[i].X1=Init[i].X1;
    Item[i].Y1=Init[i].Y1;
    Item[i].X2=Init[i].X2;
    Item[i].Y2=Init[i].Y2;
    Item[i].Focus=Init[i].Focus;
    Item[i].Selected=Init[i].Selected;
    Item[i].Flags=Init[i].Flags;
    Item[i].DefaultButton=Init[i].DefaultButton;

    if ((unsigned int)Init[i].Data < 2000)
      strcpy(Item[i].Data,GetMsg((unsigned int)Init[i].Data));
    else
      strcpy(Item[i].Data,Init[i].Data);
  }
}

void Menu(FormatFinder& FF, struct FarMenuItem *&MenuItems)
{   
    int ExitCode = 0;
    int curpos = 0;
    int prevfmt;
    
    do{
        int BreakKeys[] = { VK_RETURN, VK_F3, VK_F4, 0};
        int BreakCode;

        for (int i = 0; i<FF.GetFormatCount(); i++){
            if (i==0) prevfmt = 0;
            else prevfmt += FF.GetnKnownFormats(i-1);
            for (int j = 0; j<FF.GetnKnownFormats(i); j++){                
                curpos = prevfmt + j;                

                memset(&MenuItems[curpos],0,sizeof(FarMenuItem));
                if (FF.GetActiveOpt(i, j)) MenuItems[curpos].Checked = TRUE;
                else MenuItems[curpos].Checked = FALSE;
        
                strcpy(MenuItems[curpos].Text, FF.GetOptionString(i, j));
                MenuItems[curpos].Selected = FALSE;
            }
        }

        MenuItems[ExitCode].Selected = TRUE;
        ExitCode=Info.Menu(Info.ModuleNumber,-1,-1,0,FMENU_SHOWAMPERSAND|FMENU_WRAPMODE, GetMsg(MMenuTitle),
                GetMsg(MMenuBottom),"Config", BreakKeys,&BreakCode,
                MenuItems,FF.GetAllFormatCount());
    
        if (ExitCode != -1)
        {
            int x, y;
            for (int i = 0; i<FF.GetFormatCount(); i++){
                if (i==0) prevfmt = 0;
                else prevfmt += FF.GetnKnownFormats(i-1);
                for (int j = 0; j<FF.GetnKnownFormats(i); j++){
                    curpos = prevfmt + j;                
                    if (curpos == ExitCode){
                        x = i;
                        y = j;
                    }
                }
            }
            switch (BreakCode)
            {
            case 0: FF.SetActiveOpt(x, y, !FF.GetActiveOpt(x, y));
                break;
            case 1: 
                RIP_PLUGIN_GETNFO pinfo;                
                char *types[8] = {GetMsg(MInfoTypeGraph), 
                                  GetMsg(MInfoTypeVideo), 
                                  GetMsg(MInfoTypeSound),
                                  GetMsg(MInfoTypePack),
                                  GetMsg(MInfoTypeOther),
                                  GetMsg(MInfoTypeMusic),
                                  GetMsg(MInfoTypeText),
                                  GetMsg(MInfoTypeExe)};
                FF.GetPluginInfo(x, y, pinfo);                
                char P_OptString[80], P_Copy[1024], P_Path[MAX_PATH], P_Type[80];
                FSF.sprintf(P_OptString, GetMsg(MInfoOptString), pinfo.OptionString);
                FSF.sprintf(P_OptString, GetMsg(MInfoOptString), pinfo.OptionString);
                FSF.sprintf(P_Copy, GetMsg(MInfoCopyright), pinfo.PluginCopyright);
                FSF.sprintf(P_Path, GetMsg(MInfoPath), pinfo.PluginPath);                
                FSF.sprintf(P_Type, GetMsg(MInfoType), types[pinfo.Type]);
                const char *MsgItems[]={GetMsg(MInfoTitle), P_OptString, P_Copy, P_Path, P_Type};
                Info.Message(Info.ModuleNumber,FMSG_LEFTALIGN|FMSG_MB_OK,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);                
                break;
            }
        }
    }while (ExitCode != -1);
}


BOOL DefineRange(ULONG & Start, ULONG & End)
{
  TSaveScreen SS;
  char S1[11],S2[11],S3[11];

  FSF.sprintf(S1, "%d", Opt.StartOffset);
  FSF.sprintf(S2, "%d", Opt.EndOffset - Opt.StartOffset);
  FSF.sprintf(S3, "%d", Opt.EndOffset);

  struct InitDialogItem InitItems[]={
  /*  0 */{DI_DOUBLEBOX,   3, 1,50, 7, 0, 0, 0,                          0,(char *)MDefineTitle},  
  /*  1 */{DI_TEXT,        5, 3, 0, 0, 0, 0, 0,                          0,(char *)MDefineStart},
  /*  2 */{DI_TEXT,       20, 3, 0, 0, 0, 0, 0,                          0,(char *)MDefineLength},
  /*  3 */{DI_TEXT,       35, 3, 0, 0, 0, 0, 0,                          0,(char *)MDefineEnd},
  /*  4 */{DI_EDIT,        5, 4,18, 0, 0, (DWORD)HistStartOffset, DIF_HISTORY|DIF_USELASTHISTORY,
                                                                                                                                                 0,S1},
  /*  5 */{DI_EDIT,       20, 4,33, 0, 0, (DWORD)HistLength, DIF_HISTORY|DIF_USELASTHISTORY,
                                                                                                                                                 0,S2},
  /*  6 */{DI_EDIT,       35, 4,48, 0, 0, (DWORD)HistEndOffset, DIF_HISTORY|DIF_USELASTHISTORY,
                                                                                                                                                 0,S3},
  /*  7 */{DI_BUTTON,      0, 6, 0, 0, 0, 0, DIF_CENTERGROUP,            1,(char *)MOK},
  /*  8 */{DI_BUTTON,      0, 6, 0, 0, 0, 0, DIF_CENTERGROUP,            0,(char *)MCancel}
  };

  ULONG Length = 0;
  {
        struct FarDialogItem DialogItems[sizeof(InitItems)/sizeof(InitItems[0])];
        InitDialogItems(InitItems,DialogItems,sizeof(InitItems)/sizeof(InitItems[0]));
  
        int ExitCode=Info.Dialog(Info.ModuleNumber,-1,-1,54,9,"DefineRange",DialogItems,sizeof(DialogItems)/sizeof(DialogItems[0]));
        if ((ExitCode == 8)||ExitCode == -1) return(FALSE);
  
        Length = FSF.atoi(DialogItems[5].Data);
        Start  = FSF.atoi(DialogItems[4].Data);
        End    = FSF.atoi(DialogItems[6].Data);  
  }  

  if (End < Start+Length) End = Start+Length;

  if ((End   < Start)||
          (Start < 0)) DefineRange(Start, End);  

  return(TRUE);
}

BOOL ConfigColumns()
{
	return TRUE;
}

BOOL ConfigStatus()
{
	return TRUE;
}

int Config()
{
  TSaveScreen SS;
  struct InitDialogItem InitItems[]={
  /*  0 */{DI_DOUBLEBOX,   3, 1,72,23, 0, 0, 0,                          0,(char *)MConfigTitle},
  /*  1 */{DI_CHECKBOX,    5, 2, 0, 0, 0, 0, 0,                          0,(char *)MConfigDisablePlugin},
  /*  2 */{DI_CHECKBOX,    5, 3, 0, 0, 0, 0, 0,                          0,(char *)MConfigFullScan},
  /*  3 */{DI_CHECKBOX,    5, 4, 0, 0, 0, 0, 0,                          0,(char *)MConfigDelDup},
  /*  4 */{DI_CHECKBOX,    5, 5, 0, 0, 0, 0, 0,                          0,(char *)MUseStartScan},  
  /*  5 */{DI_CHECKBOX,    5, 6, 0, 0, 0, 0, 0,                          0,(char *)MSkipPack},
  /*  6 */{DI_CHECKBOX,    5, 7, 0, 0, 0, 0, 0,                          0,(char *)MDoConv},
  /*  7 */{DI_CHECKBOX,    5, 8, 0, 0, 0, 0, 0,                          0,(char *)MShowScanDialog},
  /*  8 */{DI_EDIT,        5, 9,10, 0, 0, 0, 0,                          0,Opt.Pref},
  /*  9 */{DI_TEXT,       11, 9, 0, 0, 0, 0, 0,                          0,(char *)MConfigPrefix},
  /* 10 */{DI_TEXT,        0,10, 0, 0, 0, 0, DIF_BOXCOLOR|DIF_SEPARATOR, 0,""},
  /* 11 */{DI_CHECKBOX,    5,11, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableGraph},
  /* 12 */{DI_CHECKBOX,    5,12, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableVideo},
  /* 13 */{DI_CHECKBOX,    5,13, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableSound},
  /* 14 */{DI_CHECKBOX,    5,14, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableMusic},
  /* 15 */{DI_CHECKBOX,    5,15, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableText},
  /* 16 */{DI_CHECKBOX,    5,16, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableExe},
  /* 17 */{DI_CHECKBOX,    5,17, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnablePacks},
  /* 18 */{DI_CHECKBOX,    5,18, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableOther},  
  /* 19 */{DI_TEXT,       32,11, 0, 0, 0, 0, 0,                          0,(char *)MScanRange},
  /* 20 */{DI_RADIOBUTTON,32,12, 0, 0, 0, 1, DIF_GROUP,                  0,(char *)MWholeFile},
  /* 21 */{DI_RADIOBUTTON,32,13, 0, 0, 0, 0, 0,                          0,(char *)MUserDefined},
  /* 22 */{DI_BUTTON,     32,14, 0, 0, 0, 0, 0,                                      0,(char *)MDefineRange},
  /* 23 */{DI_CHECKBOX,   32,16, 0, 0, 0, 0, 0,                          0,(char *)MUseLists},  
  /* 24 */{DI_BUTTON,      5,20, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableFormat},
  /* 25 */{DI_BUTTON,      0,22, 0, 0, 0, 0, DIF_CENTERGROUP,            1,(char *)MOK},
  /* 26 */{DI_BUTTON,      0,22, 0, 0, 0, 0, DIF_CENTERGROUP,            0,(char *)MCancel}
  };

  
        struct FarDialogItem DialogItems[sizeof(InitItems)/sizeof(InitItems[0])];
        InitDialogItems(InitItems,DialogItems,sizeof(InitItems)/sizeof(InitItems[0]));
        DialogItems[1].Selected=!Opt.UsePlugin;
        DialogItems[2].Selected=Opt.FullScan;
        DialogItems[3].Selected=Opt.DelDup;
        DialogItems[4].Selected=Opt.UseStartScan;
        DialogItems[5].Selected=Opt.SkipPack;
        DialogItems[6].Selected=Opt.DoConv;
        DialogItems[7].Selected=Opt.ShowScanDialog;

        DialogItems[11].Selected=Opt.EnableGraph;
        DialogItems[12].Selected=Opt.EnableVideo;
        DialogItems[13].Selected=Opt.EnableSound;
        DialogItems[14].Selected=Opt.EnableMusic;
        DialogItems[15].Selected=Opt.EnableText;
        DialogItems[16].Selected=Opt.EnableExe;
        DialogItems[17].Selected=Opt.EnablePack;
        DialogItems[18].Selected=Opt.EnableOther;
        DialogItems[20].Selected=!Opt.UserScanRange;
        DialogItems[21].Selected=Opt.UserScanRange;
        DialogItems[23].Selected=Opt.UseLists;

        int ExitCode=Info.Dialog(Info.ModuleNumber,-1,-1,76,25,"Config",DialogItems,sizeof(DialogItems)/sizeof(DialogItems[0]));
        if ((ExitCode == 26)||ExitCode == -1) return(FALSE);

                if (ExitCode == 22) {                   
                        if (DefineRange(Opt.StartOffset, Opt.EndOffset))
                                Opt.UserScanRange = TRUE;                       
                        Config();
                }

        if (ExitCode == 24) {
            {
                FormatFinder FF(PluginPath);
                struct FarMenuItem *MenuItems;
                MenuItems = (FarMenuItem *)my_malloc(sizeof(FarMenuItem)*FF.GetAllFormatCount());

                for (int i = 0; i<FF.GetFormatCount(); i++){
                    for (int j = 0; j<FF.GetnKnownFormats(i); j++){
                        FF.SetActiveOpt(i, j, GetRegKey(HKEY_CURRENT_USER,"",FF.GetRegPath(i, j), 1));
                    }
                }

                Menu(FF, MenuItems);

                for (i = 0; i<FF.GetFormatCount(); i++){
                    for (int j = 0; j<FF.GetnKnownFormats(i); j++){            
                        SetRegKey(HKEY_CURRENT_USER,"",FF.GetRegPath(i, j), FF.GetActiveOpt(i, j));
                    }
                }

                my_free(MenuItems);
            }
            Config();
        };
        Opt.UsePlugin   = !DialogItems[1].Selected;
        Opt.FullScan    = DialogItems[2].Selected;
        Opt.DelDup      = DialogItems[3].Selected;
        Opt.UseStartScan= DialogItems[4].Selected;
        Opt.SkipPack    = DialogItems[5].Selected;
        Opt.DoConv      = DialogItems[6].Selected;
        Opt.ShowScanDialog = DialogItems[7].Selected;

        Opt.EnableGraph = DialogItems[11].Selected;
        Opt.EnableVideo = DialogItems[12].Selected;
        Opt.EnableSound = DialogItems[13].Selected;
        Opt.EnableMusic = DialogItems[14].Selected;
        Opt.EnableText  = DialogItems[15].Selected;
        Opt.EnableExe   = DialogItems[16].Selected;
        Opt.EnablePack  = DialogItems[17].Selected;
        Opt.EnableOther = DialogItems[18].Selected;
                
        Opt.UserScanRange = DialogItems[21].Selected;

        Opt.UseLists    = DialogItems[23].Selected;
  
        SetRegKey(HKEY_CURRENT_USER,"",StrUsePlugin,  Opt.UsePlugin);
        SetRegKey(HKEY_CURRENT_USER,"",StrFullScan,   Opt.FullScan);  
        SetRegKey(HKEY_CURRENT_USER,"",StrDelDup,     Opt.DelDup);  
        SetRegKey(HKEY_CURRENT_USER,"",StrUseSomeOffset,  Opt.UseSomeOffset);  
        SetRegKey(HKEY_CURRENT_USER,"",StrUseStartScan,   Opt.UseStartScan);  
        SetRegKey(HKEY_CURRENT_USER,"",StrSkipPack,   Opt.SkipPack);  
        SetRegKey(HKEY_CURRENT_USER,"",StrDoConv,     Opt.DoConv);  
        SetRegKey(HKEY_CURRENT_USER,"",StrShowScanDialog, Opt.ShowScanDialog);
        SetRegKey(HKEY_CURRENT_USER,"",StrEnableGraph,Opt.EnableGraph);
        SetRegKey(HKEY_CURRENT_USER,"",StrEnableVideo,Opt.EnableVideo);
        SetRegKey(HKEY_CURRENT_USER,"",StrEnableSound,Opt.EnableSound);
        SetRegKey(HKEY_CURRENT_USER,"",StrEnableMusic,Opt.EnableMusic);
        SetRegKey(HKEY_CURRENT_USER,"",StrEnableText, Opt.EnableText);
        SetRegKey(HKEY_CURRENT_USER,"",StrEnableExe,  Opt.EnableExe);
        SetRegKey(HKEY_CURRENT_USER,"",StrEnablePack, Opt.EnablePack);
        SetRegKey(HKEY_CURRENT_USER,"",StrEnableOther,Opt.EnableOther);
        SetRegKey(HKEY_CURRENT_USER,"",StrPref,       Opt.Pref);    
        SetRegKey(HKEY_CURRENT_USER,"",StrUserScanRange,  Opt.UserScanRange);
        SetRegKey(HKEY_CURRENT_USER,"",StrStartOffset,Opt.StartOffset);
        SetRegKey(HKEY_CURRENT_USER,"",StrEndOffset,  Opt.EndOffset);
        SetRegKey(HKEY_CURRENT_USER,"",StrUseLists,   Opt.UseLists);

  return(TRUE);
}

/*BOOL CheckForKey(WORD vKey)
{
  INPUT_RECORD *InputRec;
  DWORD NumberOfEvents, ReadCnt,i;
  BOOL result = FALSE;
  HANDLE Console = GetStdHandle(STD_INPUT_HANDLE);

  if(GetNumberOfConsoleInputEvents(Console,&NumberOfEvents))
  {
    if((InputRec=(INPUT_RECORD *)my_malloc(NumberOfEvents*sizeof(INPUT_RECORD))) != NULL)
    {
      if(PeekConsoleInput(Console,InputRec,NumberOfEvents,&ReadCnt))
        if(ReadCnt <= NumberOfEvents)
          for(i=0; i < NumberOfEvents; ++i)
          {
            if (InputRec[i].EventType == KEY_EVENT &&
                InputRec[i].Event.KeyEvent.bKeyDown &&
                InputRec[i].Event.KeyEvent.wVirtualKeyCode == vKey)
            {
              result=TRUE;
              //if(i > 0)
                ReadConsoleInput(Console,InputRec,i,&ReadCnt);
              break;
            }
          }
      my_free((HGLOBAL)InputRec);
    }
  }
  return result;
}*/

BOOL AddCustomFile(long WorkFileSize)
{
  TSaveScreen SS;
  char S1[11], S2[11], S3[11];

  FSF.sprintf(S1, "%d", Opt.DummyOffset);
  FSF.sprintf(S2, "%d", Opt.DummySize);
  FSF.sprintf(S3, "%d", Opt.DummyOffset+Opt.DummySize);

  struct InitDialogItem InitItems[]={
  /*  0 */{DI_DOUBLEBOX,   3, 1,50,11, 0, 0, 0,                          0,(char *)MAddDummyTitle},
  /*  1 */{DI_TEXT,        5, 3, 0, 0, 0, 0, 0,                          0,(char *)MDummyName},
  /*  2 */{DI_TEXT,        5, 5, 0, 0, 0, 0, 0,                          0,(char *)MDummyDesc},
  /*  3 */{DI_TEXT,        5, 7, 0, 0, 0, 0, 0,                          0,(char *)MDummyOffset},
  /*  4 */{DI_TEXT,       20, 7, 0, 0, 0, 0, 0,                          0,(char *)MDummySize},
  /*  5 */{DI_TEXT,       35, 7, 0, 0, 0, 0, 0,                          0,(char *)MDummyEndOffset},
  /*  6 */{DI_EDIT,       20, 3,48, 0, 0, (DWORD)HistDummyName, DIF_HISTORY|DIF_USELASTHISTORY,
                                                                                                                                                 0,Opt.DummyName},
  /*  7 */{DI_EDIT,       20, 5,48, 0, 0, (DWORD)HistDummyDesc, DIF_HISTORY|DIF_USELASTHISTORY,
                                                                                                                                                 0,Opt.DummyDesc},
  /*  8 */{DI_EDIT,        5, 8,18, 0, 0, (DWORD)HistDummyOffset, DIF_HISTORY|DIF_USELASTHISTORY,
                                                                                                                                                 0,S1},
  /*  9 */{DI_EDIT,       20, 8,33, 0, 0, (DWORD)HistDummySize, DIF_HISTORY|DIF_USELASTHISTORY,
                                                                                                                                                 0,S2},
  /* 10 */{DI_EDIT,       35, 8,48, 0, 0, (DWORD)HistDummyEnd, DIF_HISTORY|DIF_USELASTHISTORY,
                                                                                                                                                 0,S3},
  /* 11 */{DI_BUTTON,      0,10, 0, 0, 0, 0, DIF_CENTERGROUP,            1,(char *)MOK},
  /* 12 */{DI_BUTTON,      0,10, 0, 0, 0, 0, DIF_CENTERGROUP,            0,(char *)MCancel}
  };
  
  {
        struct FarDialogItem DialogItems[sizeof(InitItems)/sizeof(InitItems[0])];
        InitDialogItems(InitItems,DialogItems,sizeof(InitItems)/sizeof(InitItems[0]));
  
        int ExitCode=Info.Dialog(Info.ModuleNumber,-1,-1,54,13,"DummyFile",DialogItems,sizeof(DialogItems)/sizeof(DialogItems[0]));
        if ((ExitCode == 12)||ExitCode == -1) return(FALSE);
        
        strcpy(Opt.DummyName, DialogItems[6].Data);
        strcpy(Opt.DummyDesc, DialogItems[7].Data);
        Opt.DummyOffset = FSF.atoi(DialogItems[8].Data);
        Opt.DummySize   = FSF.atoi(DialogItems[9].Data);
        Opt.DummyEnd    = FSF.atoi(DialogItems[10].Data);  
  }  

  if (Opt.DummyOffset < 0) Opt.DummyOffset = 0;
  if (Opt.DummyEnd > WorkFileSize) {
          Opt.DummyEnd = WorkFileSize;
          Opt.DummySize = Opt.DummyEnd - Opt.DummyOffset;
  }
  if (Opt.DummyEnd < Opt.DummyOffset) {
          if (Opt.DummySize > 0) {
                  Opt.DummyEnd = Opt.DummyOffset+Opt.DummySize;
          } else {
                ULONG t = Opt.DummyEnd;
                Opt.DummyEnd = Opt.DummyOffset;
                Opt.DummyOffset = t;
          }
  }

  if (Opt.DummyEnd < Opt.DummyOffset+Opt.DummySize) Opt.DummyEnd = Opt.DummyOffset+Opt.DummySize;
  if (Opt.DummyEnd-Opt.DummyOffset > Opt.DummySize) Opt.DummySize = Opt.DummyEnd-Opt.DummyOffset;

  return(TRUE);

}

BOOL SplitFileDialog()
{
  TSaveScreen SS;
  char S1[11], S2[11];

  FSF.sprintf(S1, "%d", Opt.SplitSize);
  FSF.sprintf(S2, "%d", Opt.SplitOffset);  

  struct InitDialogItem InitItems[]={
  /*  0 */{DI_DOUBLEBOX,   3, 1,35, 9, 0, 0, 0,                          0,(char *)MSplitTitle},
  /*  1 */{DI_TEXT,        5, 3, 0, 0, 0, 0, 0,                          0,(char *)MSplitString},
  /*  2 */{DI_TEXT,       20, 3, 0, 0, 0, 0, 0,                          0,(char *)MSplitSize},
  /*  3 */{DI_TEXT,        5, 5, 0, 0, 0, 0, 0,                          0,(char *)MSplitOffset},
  /*  4 */{DI_TEXT,       20, 5, 0, 0, 0, 0, 0,                          0,(char *)MSplitExt},
  
  /*  5 */{DI_EDIT,        5, 4,18, 0, 0, (DWORD)HistSplitString, DIF_HISTORY|DIF_USELASTHISTORY,
                                                                                                                                                 0,Opt.SplitString},
  /*  6 */{DI_EDIT,       20, 4,33, 0, 0, 0, 0,                                                  0,S1},
  /*  7 */{DI_EDIT,        5, 6,18, 0, 0, 0, 0,                                                  0,S2},
  /*  8 */{DI_EDIT,       20, 6,33, 0, 0, (DWORD)HistSplitExt, DIF_HISTORY|DIF_USELASTHISTORY,
                                                                                                                                                 0,Opt.SplitExt},
  
  /*  9 */{DI_BUTTON,      0, 8, 0, 0, 0, 0, DIF_CENTERGROUP,            1,(char *)MOK},
  /* 10 */{DI_BUTTON,      0, 8, 0, 0, 0, 0, DIF_CENTERGROUP,            0,(char *)MCancel}
  };
  
  {
        struct FarDialogItem DialogItems[sizeof(InitItems)/sizeof(InitItems[0])];
        InitDialogItems(InitItems,DialogItems,sizeof(InitItems)/sizeof(InitItems[0]));
  
        int ExitCode=Info.Dialog(Info.ModuleNumber,-1,-1,38,12,"SplitFile",DialogItems,sizeof(DialogItems)/sizeof(DialogItems[0]));
        if ((ExitCode == 10)||ExitCode == -1) return(FALSE);
        
        strcpy(Opt.SplitString, DialogItems[5].Data);
        strcpy(Opt.SplitExt, DialogItems[8].Data);
        Opt.SplitOffset = FSF.atoi(DialogItems[7].Data);
        Opt.SplitSize   = FSF.atoi(DialogItems[6].Data);        
  }  

  if (Opt.SplitSize <= 0) Opt.SplitSize = strlen(Opt.SplitString);

  SetRegKey(HKEY_CURRENT_USER,"",StrSplitString,Opt.SplitString);
  SetRegKey(HKEY_CURRENT_USER,"",StrSplitSize,  Opt.SplitSize);
  SetRegKey(HKEY_CURRENT_USER,"",StrSplitExt,   Opt.SplitExt);
  SetRegKey(HKEY_CURRENT_USER,"",StrSplitOffset,Opt.SplitOffset);

  return(TRUE);

}

//Exported functions

//Extract files
int WINAPI _export GetFiles(HANDLE hPlugin,struct PluginPanelItem *PanelItem,int ItemsNumber,int Move,char *DestPath,int OpMode)
{
        MrRipper *Ripper=(MrRipper *)hPlugin;
        if(!IsOldFAR)
                return(Ripper->GetFiles(PanelItem, ItemsNumber, Move, DestPath, OpMode));
        return FALSE;
}

HANDLE WINAPI _export OpenFilePlugin(char *Name, const unsigned char *Data, int DataSize)
{
#ifdef _DEBUG
    logfile = _open ("c:\\mrrip.log", _O_CREAT|_O_WRONLY|_O_TEXT, _S_IWRITE);
#endif
        if (!Opt.UsePlugin) return INVALID_HANDLE_VALUE;
        if (!Name) return INVALID_HANDLE_VALUE;
        if(!IsOldFAR)
        {           

            HANDLE hPlugin=new MrRipper;
			if (hPlugin==NULL) return(INVALID_HANDLE_VALUE);                                           
			MrRipper *Ripper=(MrRipper *)hPlugin;
			strcpy(Ripper->RipFileName, Name);
			
            Ripper->bPluginManager = FALSE;

			Ripper->EnableGraph = FALSE;
			Ripper->EnableVideo = FALSE;
			Ripper->EnableSound = FALSE;
			Ripper->EnableMusic = FALSE;
			Ripper->EnableText  = FALSE;
			Ripper->EnableExe   = FALSE;
			Ripper->EnableOther = FALSE;
			Ripper->EnablePack  = TRUE;
			Ripper->FullScan	= TRUE;			
			Ripper->UseSomeOffset = FALSE;
			Ripper->UseStartScan= TRUE;
			Ripper->SkipPack	= TRUE;

            {
                FormatFinder FF(PluginPath);
			    for (int i = 0; i<FF.GetFormatCount(); i++)
    				for (int j = 0; j<FF.GetnKnownFormats(i); j++)
	    				FF.SetActiveOpt(i, j, GetRegKey(HKEY_CURRENT_USER,"",FF.GetRegPath(i, j), 1));
    
	    		FF.EnableGraph	= Ripper->EnableGraph;
		    	FF.EnableOther	= Ripper->EnableOther;
			    FF.EnablePack	= Ripper->EnablePack;
    			FF.EnableSound	= Ripper->EnableSound;
	    		FF.EnableMusic	= Ripper->EnableMusic;
		    	FF.EnableText	= Ripper->EnableText;
			    FF.EnableExe	= Ripper->EnableExe;
    			FF.EnableVideo	= Ripper->EnableVideo;
	            FF.FullScan		= Ripper->FullScan;
		    	FF.UseSomeOffset= Ripper->UseSomeOffset;
			    FF.UseStartScan	= Ripper->UseStartScan;
    			FF.SkipPack		= Ripper->SkipPack;

	    		struct _stat buf;
		    	_stat(Ripper->RipFileName, &buf);

			    Ripper->WorkFileSize = buf.st_size;
    			Ripper->StartOffset = 0;
                Ripper->EndOffset   = Ripper->WorkFileSize;             

		    	FF.glbFmt		= -1;
			    FF.glbPackOffset= 0;
    			FF.glbPackSize	= 0;		

	    		BOOL b = FALSE;
		    	long pos = 0;			
                long items;
                long uf;
                char *c = NULL;

                if (items = FF.Identify(Ripper->RipFileName, pos, Ripper->EndOffset, Ripper->WorkFileSize, Ripper->FAT, Ripper->dirs, TRUE, uf, c)==0){
		            delete (MrRipper*)hPlugin;
			        return(INVALID_HANDLE_VALUE);
    			}
                Ripper->Free_dirs();
                Ripper->FAT.ReInitFAT();
            }
			return(hPlugin);
        }
        return(INVALID_HANDLE_VALUE);
}

//Getting main info about module
void WINAPI _export GetPluginInfo(struct PluginInfo *Info)
{       
    Info->StructSize=sizeof(*Info);
    Info->Flags=0;    
    Info->DiskMenuStringsNumber=NULL;    

    static char *PluginMenuStrings[2];
    PluginMenuStrings[0]=GetMsg(MMrRipper);
	PluginMenuStrings[1]=GetMsg(MMrRipperList);
    Info->PluginMenuStrings=PluginMenuStrings;
    Info->PluginMenuStringsNumber=sizeof(PluginMenuStrings)/sizeof(PluginMenuStrings[0]);
        
    static char *PluginCfgStrings[1];
    PluginCfgStrings[0]=GetMsg(MMrRipper);
    Info->PluginConfigStrings=PluginCfgStrings;
    Info->PluginConfigStringsNumber=sizeof(PluginCfgStrings)/sizeof(PluginCfgStrings[0]);
    Info->CommandPrefix="rip";      
}

//Getting main info about FAR
void WINAPI _export SetStartupInfo(struct PluginStartupInfo *Info)
{                       
    ::Info=*Info;
    IsOldFAR=TRUE;

    if(Info->StructSize >= sizeof(struct PluginStartupInfo))
    {
        IsOldFAR=FALSE;

        ::FSF=*Info->FSF;
        ::Info.FSF=&::FSF;
        lstrcpy (FarRootKey, Info->RootKey);
        char *pBkSlash = strrchr (FarRootKey, '\\');
        if (pBkSlash) *pBkSlash = '\0';
        FSF.sprintf(PluginRootKey,"%s\\CrOm\\MrRipper",Info->RootKey);

        lstrcpy (PluginPath, Info->ModuleName);
        pBkSlash = strrchr (PluginPath, '\\');
        if (pBkSlash) *pBkSlash = '\0';

        Opt.EnableGraph = GetRegKey(HKEY_CURRENT_USER,"",StrEnableGraph,1);
        Opt.EnableVideo = GetRegKey(HKEY_CURRENT_USER,"",StrEnableVideo,1);
        Opt.EnableSound = GetRegKey(HKEY_CURRENT_USER,"",StrEnableSound,1);
        Opt.EnableMusic = GetRegKey(HKEY_CURRENT_USER,"",StrEnableMusic,1);
        Opt.EnableText  = GetRegKey(HKEY_CURRENT_USER,"",StrEnableText,1);
        Opt.EnableExe   = GetRegKey(HKEY_CURRENT_USER,"",StrEnableExe,1);
        Opt.EnablePack  = GetRegKey(HKEY_CURRENT_USER,"",StrEnablePack,1);
        Opt.EnableOther = GetRegKey(HKEY_CURRENT_USER,"",StrEnableOther,1);
        Opt.DoConv      = GetRegKey(HKEY_CURRENT_USER,"",StrDoConv,1);
        Opt.DelDup      = GetRegKey(HKEY_CURRENT_USER,"",StrDelDup,1);
        Opt.UseSomeOffset = GetRegKey(HKEY_CURRENT_USER,"",StrUseSomeOffset,0);
        Opt.UseStartScan= GetRegKey(HKEY_CURRENT_USER,"",StrUseStartScan,1);
        Opt.SkipPack    = GetRegKey(HKEY_CURRENT_USER,"",StrSkipPack,1);
        Opt.ShowScanDialog = GetRegKey(HKEY_CURRENT_USER,"",StrShowScanDialog,1);
        Opt.FullScan    = GetRegKey(HKEY_CURRENT_USER,"",StrFullScan,0);
        Opt.UsePlugin   = GetRegKey(HKEY_CURRENT_USER,"",StrUsePlugin,0);
        Opt.UserScanRange = GetRegKey(HKEY_CURRENT_USER,"",StrUserScanRange,0);
        Opt.StartOffset = GetRegKey(HKEY_CURRENT_USER,"",StrStartOffset,0);
        Opt.EndOffset   = GetRegKey(HKEY_CURRENT_USER,"",StrEndOffset,0);
        Opt.DummyOffset = GetRegKey(HKEY_CURRENT_USER,"",StrDummyOffset,0);
        Opt.DummySize   = GetRegKey(HKEY_CURRENT_USER,"",StrDummySize,0);
        Opt.DummyEnd    = GetRegKey(HKEY_CURRENT_USER,"",StrDummyEnd,0);
        Opt.SplitOffset = GetRegKey(HKEY_CURRENT_USER,"",StrSplitOffset,-4);
        Opt.SplitSize   = GetRegKey(HKEY_CURRENT_USER,"",StrSplitSize,0);
        Opt.UseLists    = GetRegKey(HKEY_CURRENT_USER,"",StrUseLists,1);
        GetRegKey(HKEY_CURRENT_USER,"",StrSplitExt,   Opt.SplitExt,    "wav", 10);
        GetRegKey(HKEY_CURRENT_USER,"",StrSplitString,Opt.SplitString, "WAVEfmt", 20);
        GetRegKey(HKEY_CURRENT_USER,"",StrPref,           Opt.Pref,    "rip", 10);                    
    }
}

void WINAPI _export GetOpenPluginInfo(HANDLE hPlugin,struct OpenPluginInfo *Info)
{       
    MrRipper *Ripper=(MrRipper *)hPlugin;
    if(!IsOldFAR) Ripper->GetOpenPluginInfo(Info); 
}

int WINAPI _export Configure(int ItemNumber)
{
    if(!IsOldFAR)
    switch(ItemNumber)
    {
        case 0:;
        return(Config());
    }
    return(FALSE);
}

void WINAPI _export ExitFAR()
{
  //  
}

int WINAPI _export GetMinFarVersion(void)
{
  return MAKEFARVERSION(1,70,0);
}


int WINAPI _export GetFindData(HANDLE hPlugin, struct PluginPanelItem **pPanelItem,int *pItemsNumber,int OpMode)
{
    MrRipper *Ripper=(MrRipper *)hPlugin;
    if(!IsOldFAR) return(Ripper->GetFindData(pPanelItem,pItemsNumber,OpMode));
    return FALSE;
}

void WINAPI _export FreeFindData(HANDLE hPlugin, struct PluginPanelItem *PanelItem,int ItemsNumber)
{               
    MrRipper *Ripper=(MrRipper *)hPlugin;
    if(!IsOldFAR) Ripper->FreeFindData(PanelItem,ItemsNumber);
}


HANDLE WINAPI _export OpenPlugin(int OpenFrom,int Item)
{
    void * v;
#ifdef _DEBUG
    logfile = _open ("c:\\mrrip.log", _O_CREAT|_O_WRONLY|_O_TEXT, _S_IWRITE);
#endif
    if(!IsOldFAR)
    {   
        HANDLE hPlugin=new MrRipper;
        if (hPlugin==NULL) return(INVALID_HANDLE_VALUE);                                           
        MrRipper *Ripper=(MrRipper *)hPlugin;
        v = Ripper->OpenPlugin(OpenFrom, Item);        
        if (v==INVALID_HANDLE_VALUE) return(INVALID_HANDLE_VALUE);
		if (!Ripper->bPluginManager) {
			int f2 = _open(Ripper->RipFileName, _O_RDONLY|_O_BINARY);    
			if (f2!=-1) {
				_close(f2);
				return(hPlugin);
			}
		} else return(hPlugin);
    }
    return(INVALID_HANDLE_VALUE);
}

void WINAPI _export ClosePlugin(HANDLE hPlugin)
{
    if(!IsOldFAR)
        delete (MrRipper*)hPlugin;    
#ifdef _DEBUG
    _close (logfile);
#endif
}

int WINAPI _export ProcessKey(HANDLE hPlugin, int Key, unsigned int ControlState)
{
    MrRipper *Ripper=(MrRipper *)hPlugin;
    if(!IsOldFAR) return(Ripper->ProcessKey(Key,ControlState));
    return FALSE;
}

int WINAPI _export SetDirectory(HANDLE hPlugin,const char *Dir,int OpMode)
{
    MrRipper *Ripper=(MrRipper *)hPlugin;
    if(!IsOldFAR) return Ripper->SetDirectory(Dir,OpMode);
    return FALSE;
}

int WINAPI PutFiles(HANDLE hPlugin, struct PluginPanelItem *PanelItem, int ItemsNumber, int Move, int OpMode)
{
    MrRipper *Ripper=(MrRipper *)hPlugin;
    if(!IsOldFAR) return Ripper->PutFiles(PanelItem, ItemsNumber, Move, OpMode);
    return FALSE;
}

int WINAPI DeleteFiles(HANDLE hPlugin, struct PluginPanelItem *PanelItem, int ItemsNumber, int OpMode)
{
    MrRipper *Ripper=(MrRipper *)hPlugin;
    if(!IsOldFAR) return Ripper->DelFiles(PanelItem, ItemsNumber, OpMode);
    return FALSE;
}

//TSaveScreen::TSaveScreen(struct PluginStartupInfo *Info)
TSaveScreen::TSaveScreen()
{
//  TSaveScreen::Info=Info;
  hScreen=NULL;
//  if(Info)
  {
    hScreen=Info.SaveScreen(0,0,-1,-1);        
  }
}

TSaveScreen::~TSaveScreen()
{
  if (hScreen)
  {
//    if(Info)
    {
      Info.RestoreScreen(NULL);
      Info.RestoreScreen(hScreen);
    }
  }
}