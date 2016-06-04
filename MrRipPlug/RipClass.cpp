bool LessDir(const RIPPER_FILE_INFO& a, const RIPPER_FILE_INFO& b)
{
        return (lstrcmp(a.FileName, b.FileName) < 0);
}

MrRipper::MrRipper()
{
    glbFmt = -1;
    glbPackOffset = 0;
    glbPackSize = 0;
    glbAllFiles = 0;
    glbAllSize = 0;

    lstrcpy(FarCurDir, "");    
}

MrRipper::~MrRipper()
{    
    Free_shownfiles();
    Free_copyfiles();
    Free_dirs();
    Free_dummyfiles();    
    FAT.DelItem(FAT.Root);
#ifdef _DEBUG
    DumpUnfreed();
#endif
}

void MrRipper::Free_dirs()
{
    for (int i=0; i< dirs.size(); i++){        
        if (dirs[i].DirName)    {
            SaveToLog("Free_dirs", dirs[i].DirName, lstrlen(dirs[i].DirName), TRUE, FALSE, FALSE);
            my_free(dirs[i].DirName);            
        }
    }
    dirs.clear();
    
}

void MrRipper::Free_copyfiles()
{
    for (int i=0; i< copyfiles.size(); i++){        
        SaveToLog("Free_copyfiles", copyfiles[i].FileName, lstrlen(copyfiles[i].FileName), TRUE, FALSE, FALSE);
        my_free(copyfiles[i].FileName);        
    }
    copyfiles.clear();
}

void MrRipper::Free_putfiles()
{
    for (int i=0; i< putfiles.size(); i++){
        SaveToLog("Free_putfiles", putfiles[i].FileName, lstrlen(putfiles[i].FileName), TRUE, FALSE, FALSE);
        my_free(putfiles[i].FileName);
        SaveToLog("Free_putfiles", putfiles[i].Description, lstrlen(putfiles[i].Description), TRUE, FALSE, FALSE);
        my_free(putfiles[i].Description);
        SaveToLog("Free_putfiles", putfiles[i].Plugin, lstrlen(putfiles[i].Plugin), TRUE, FALSE, FALSE);
        my_free(putfiles[i].Plugin);
        
    }
    putfiles.clear();
}

void MrRipper::Free_dummyfiles()
{
    for (int i=0; i< dummyfiles.size(); i++){
        SaveToLog("Free_dummyfiles", dummyfiles[i].FileName, lstrlen(dummyfiles[i].FileName), TRUE, FALSE, FALSE);
        my_free(dummyfiles[i].FileName);
        SaveToLog("Free_dummyfiles", dummyfiles[i].Description, lstrlen(dummyfiles[i].Description), TRUE, FALSE, FALSE);
        my_free(dummyfiles[i].Description);
        SaveToLog("Free_dummyfiles", dummyfiles[i].Plugin, lstrlen(dummyfiles[i].Plugin), TRUE, FALSE, FALSE);
        my_free(dummyfiles[i].Plugin);
        
    }
    dummyfiles.clear();
}

void MrRipper::Free_shownfiles()
{
    for (int i=0; i< shownfiles.size(); i++){
        SaveToLog("Free_shownfiles", shownfiles[i].FileName, lstrlen(shownfiles[i].FileName), TRUE, FALSE, FALSE);
        my_free(shownfiles[i].FileName);                
    }
    shownfiles.clear();
}

BOOL MrRipper::Update_shownfiles(BOOL Silent)
{
    RIPPER_FILE_INFO FI;
    BOOL WaitMessage = FALSE;
    DWORD StartTime=GetTickCount();
    long Progressf;
    BOOL dup;

    Free_shownfiles();

    SVFIP SFI = FAT.GetPath(NULL, FarCurDir);
    if (!SFI) return FALSE;

    for (int i=0; i<SFI->Items.size();i++){
        if (!Silent){            
            if (GetTickCount()-StartTime>500){
                if (CheckForKey(VK_ESCAPE)) break;
                Progressf = i; Progressf *= 100; Progressf /= (SFI->Items.size()+dummyfiles.size());
                char ProgressMsg[50];                                
                FSF.sprintf(ProgressMsg,"%u %%", Progressf);
                
                const char *MsgItems[]={GetMsg(MList), ProgressMsg};
                Info.Message(Info.ModuleNumber,WaitMessage?FMSG_KEEPBACKGROUND:0,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
                WaitMessage=TRUE;
                StartTime=GetTickCount();
            }       
        }

        dup = FALSE;
        if (DelDup){
            for (int z=0; z<shownfiles.size();z++){
                if (shownfiles[z].Fmt == -1) continue;
                if (SFI->Items[i]->Offset==shownfiles[z].StartOffset) {dup = TRUE; break;}
                if (SFI->Items[i]->IsExtension){
                    if (Info.CmpName("*.mp3", SFI->Items[i]->FileName, TRUE)&&
                        (SFI->Items[i]->Offset+SFI->Items[i]->PackedSize==shownfiles[z].StartOffset+shownfiles[z].FileSize)) {dup = TRUE; break;}
                }
            }
        }
    
        if (dup) continue;

        FI.Description = SFI->Items[i]->Description;
        if (ConvView&&SFI->Items[i]->ConvFileName) 
             FI.FileName    = my_strdup(SFI->Items[i]->ConvFileName);
        else FI.FileName    = my_strdup(SFI->Items[i]->FileName);
        FI.StartOffset = SFI->Items[i]->Offset;
        FI.FileSize    = SFI->Items[i]->PackedSize;
        FI.UnpSize     = SFI->Items[i]->UnpackedSize;
        FI.Plugin      = SFI->Items[i]->Plugin;
        FI.Fmt         = SFI->Items[i]->Fmt;
        FI.IsExtension = SFI->Items[i]->IsExtension;
        FI.UserData.Data = SFI->Items[i]->UserData.Data;
        FI.UserData.Size = SFI->Items[i]->UserData.Size;
        FI.IsDirectory = SFI->Items[i]->IsDirectory;
            
        shownfiles.push_back(FI);
    }    

    if (lstrlen(FarCurDir)==0){
        for (int Z=0; Z < dummyfiles.size(); Z++){
            FI.Description = dummyfiles[Z].Description;
            FI.FileName    = ExtractName(dummyfiles[Z].FileName);
            FI.StartOffset = dummyfiles[Z].StartOffset;
            FI.FileSize    = dummyfiles[Z].FileSize;
            FI.UnpSize     = dummyfiles[Z].UnpSize;
            FI.Plugin      = dummyfiles[Z].Plugin;
            FI.Fmt         = dummyfiles[Z].Fmt;
            FI.IsExtension = dummyfiles[Z].IsExtension;
            FI.UserData.Data = dummyfiles[Z].UserData.Data;
            FI.UserData.Size = dummyfiles[Z].UserData.Size;
            FI.IsDirectory = dummyfiles[Z].IsDirectory;
            
            shownfiles.push_back(FI);        
        }
    }

    return TRUE;
}

BOOL MrRipper::ScanDialog()
{
  TSaveScreen SS;
  struct InitDialogItem InitItems[]={
  /*  0 */{DI_DOUBLEBOX, 3, 1,72,16, 0, 0, 0,                          0,(char *)MScan},  
  /*  1 */{DI_CHECKBOX,  5, 2, 0, 0, 0, 0, 0,                          0,(char *)MConfigFullScan},
  /*  2 */{DI_CHECKBOX,  5, 3, 0, 0, 0, 0, 0,                          0,(char *)MConfigDelDup},
  /*  3 */{DI_CHECKBOX,  5, 4, 0, 0, 0, 0, 0,                          0,(char *)MUseStartScan},
  /*  4 */{DI_CHECKBOX,  5, 5, 0, 0, 0, 0, 0,                          0,(char *)MSkipPack},
  /*  5 */{DI_TEXT,      5, 6, 0, 0, 0, 0, 0,                          0,""},
  /*  6 */{DI_CHECKBOX,  5, 7, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableGraph},
  /*  7 */{DI_CHECKBOX,  5, 8, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableVideo},
  /*  8 */{DI_CHECKBOX,  5, 9, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableSound},
  /*  9 */{DI_CHECKBOX,  5,10, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableMusic},
  /* 10 */{DI_CHECKBOX,  5,11, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableText},
  /* 11 */{DI_CHECKBOX,  5,12, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableExe},
  /* 12 */{DI_CHECKBOX,  5,13, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnablePacks},
  /* 13 */{DI_CHECKBOX,  5,14, 0, 0, 0, 0, 0,                          0,(char *)MConfigEnableOther},  
  /* 14 */{DI_BUTTON,    0,15, 0, 0, 0, 0, DIF_CENTERGROUP,            1,(char *)MOK},
  /* 15 */{DI_BUTTON,    0,15, 0, 0, 0, 0, DIF_CENTERGROUP,            0,(char *)MCancel}
  };

    struct FarDialogItem DialogItems[sizeof(InitItems)/sizeof(InitItems[0])];
    InitDialogItems(InitItems,DialogItems,sizeof(InitItems)/sizeof(InitItems[0]));

    DialogItems[1].Selected = Opt.FullScan;
    DialogItems[2].Selected = Opt.DelDup;
    DialogItems[3].Selected = Opt.UseStartScan;    
    DialogItems[4].Selected = Opt.SkipPack;

    DialogItems[6].Selected = Opt.EnableGraph;
    DialogItems[7].Selected = Opt.EnableVideo;
    DialogItems[8].Selected = Opt.EnableSound;
    DialogItems[9].Selected = Opt.EnableMusic;
    DialogItems[10].Selected = Opt.EnableText;
    DialogItems[11].Selected = Opt.EnableExe;
    DialogItems[12].Selected = Opt.EnablePack;
    DialogItems[13].Selected= Opt.EnableOther;

    int ExitCode=Info.Dialog(Info.ModuleNumber,-1,-1,74,18,"Scan",DialogItems,sizeof(DialogItems)/sizeof(DialogItems[0]));
    if ((ExitCode == 15)||(ExitCode == -1)) return FALSE;

    FullScan    = DialogItems[1].Selected;
    DelDup      = DialogItems[2].Selected;
    UseStartScan= DialogItems[3].Selected;
    SkipPack    = DialogItems[4].Selected;

    EnableGraph = DialogItems[6].Selected;
    EnableVideo = DialogItems[7].Selected;
    EnableSound = DialogItems[8].Selected;
    EnableMusic = DialogItems[9].Selected;
    EnableText  = DialogItems[10].Selected;
    EnableExe   = DialogItems[11].Selected;
    EnablePack  = DialogItems[12].Selected;
    EnableOther = DialogItems[13].Selected;
   
    return TRUE;
}

BOOL MrRipper::FileRipper(int OpMode)
{
    DWORD StartTime=GetTickCount();
    BOOL WaitMessage=FALSE; 
    double Progressf;        
    long LastOffset = -1;    

    RIP_FILE_INFO  *varfinfo = NULL;
    
    ConvView = Opt.DoConv;

    long pos, fnded = 0;
    char * lf = NULL;
    if (!(FAT.Root->Items.size()>0)){    
        FormatFinder FF(PluginPath);    
        for (int i = 0; i<FF.GetFormatCount(); i++)
            for (int j = 0; j<FF.GetnKnownFormats(i); j++)
                FF.SetActiveOpt(i, j, GetRegKey(HKEY_CURRENT_USER,"",FF.GetRegPath(i, j), 1));              

        FF.EnableGraph  = EnableGraph;
        FF.EnableOther  = EnableOther;
        FF.EnablePack   = EnablePack;
        FF.EnableSound  = EnableSound;
        FF.EnableMusic  = EnableMusic;
        FF.EnableText   = EnableText;
        FF.EnableExe    = EnableExe;
        FF.EnableVideo  = EnableVideo;
        FF.FullScan     = FullScan;
        FF.UseSomeOffset= UseSomeOffset;
        FF.UseStartScan = UseStartScan;
        FF.SkipPack     = SkipPack;     

        struct _stat buf;
        _stat(RipFileName, &buf);
        WorkFileSize = buf.st_size;

        if (!Opt.UserScanRange){
            StartOffset = 0;
            EndOffset   = WorkFileSize;             
        } else {
            StartOffset = Opt.StartOffset;
            EndOffset   = Opt.EndOffset;
            if ((StartOffset <  0)||
                (StartOffset >= EndOffset)) StartOffset = 0;
            if ((EndOffset >  WorkFileSize)||
                (EndOffset <= StartOffset)) EndOffset = WorkFileSize;
        }               

        glbFmt        = -1;        
        glbPackOffset = 0;
        glbPackSize   = 0;
        glbAllFiles   = 0;
        glbAllSize    = 0;
        
        FF.glbFmt       = -1;
        FF.glbPackOffset= 0;
        FF.glbPackSize  = 0;
        FF.glbAllSize   = 0;
        BOOL b = FALSE;

        if ((!Opt.UserScanRange)&&(Opt.UseLists)) 
            b = TryToLoadList(OpMode & OPM_SILENT);
        if (!b) {
            Free_dirs();
            pos = StartOffset;

            long uf = 0;
            while (pos<EndOffset){   
                if (!OpMode & OPM_SILENT){                  
                    if (GetTickCount()-StartTime>500){
                        if (CheckForKey(VK_ESCAPE)) break;
                        Progressf = pos-StartOffset; Progressf *= 100; Progressf /= (EndOffset-StartOffset);
                        char FileMsg[50], FoundMsg[50], ProgressMsg[50], LastFndMsg[50];
                        char *pdest;
                        pdest = strrchr(RipFileName, '\\');  
                        pdest++;
                        FSF.sprintf(FileMsg,GetMsg(MScanning), pdest);
                        FSF.sprintf(FoundMsg,GetMsg(MFound), glbAllFiles);
                        FSF.sprintf(ProgressMsg,"%3.0f %%   %10d/%10d", Progressf, pos, EndOffset);
                        if (lf) FSF.sprintf(LastFndMsg,GetMsg(MLastFound), lf);
                        else FSF.sprintf(LastFndMsg,GetMsg(MLastFound), "");
                        const char *MsgItems[]={GetMsg(MScan),FileMsg, FoundMsg, ProgressMsg, LastFndMsg};
                        Info.Message(Info.ModuleNumber,WaitMessage ? FMSG_LEFTALIGN|FMSG_KEEPBACKGROUND:FMSG_LEFTALIGN,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
                        WaitMessage=TRUE;
                        StartTime = GetTickCount();
                    }       
                }
                if (EndOffset<pos) break;
                
                fnded = FF.Identify(RipFileName, pos, EndOffset, WorkFileSize, FAT, dirs, OpMode & OPM_SILENT, uf, lf);
                if (fnded > 0){                                    
                    glbAllFiles += fnded;                    
                    WaitMessage=FALSE;                    
                }
            }     
        }
        lstrcpy(glbPlugin, FF.glbPlugin);
        glbFmt          = FF.glbFmt;
        glbPackOffset   = FF.glbPackOffset;
        glbPackSize     = FF.glbPackSize;
        glbAllSize      = FF.glbAllSize;
    }   
    
    if ((FAT.Root->Items.size()==0)&&!Opt.UseLists&&
        dummyfiles.size()==0) TryToLoadList(OpMode & OPM_SILENT);

    Update_shownfiles(OpMode & OPM_SILENT);

    return TRUE;
}

BOOL MrRipper::PluginManager(int OpMode)
{   
    ADDPLUGININFO      *PI;
    RIP_PLUGIN_GETNFO   GI;
    SVFIP               SFI;
    char                ver[30];

    Free_shownfiles();

    FormatFinder FF(PluginPath);
    int i, j;

    for (i = 0; i<FF.GetFormatCount(); i++){
        for (j = 0; j<FF.GetnKnownFormats(i); j++){
            FF.SetActiveOpt(i, j, GetRegKey(HKEY_CURRENT_USER,"",FF.GetRegPath(i, j), 1));
        }
    }
    
    for (i = 0; i<FF.GetFormatCount(); i++){        
        for (j = 0; j<FF.GetnKnownFormats(i); j++){
            
            FF.GetPluginInfo(i,j, GI);
            PI = (ADDPLUGININFO*)my_malloc(sizeof ADDPLUGININFO);
            //PI->Size = sizeof ADDPLUGININFO;
            lstrcpy(PI->RegPath, GI.RegPath);
            lstrcpy(PI->FilePath,GI.PluginPath);
            lstrcpy(PI->Copy,    GI.PluginCopyright);            
            PI->Type        = GI.Type;            
            PI->RipVer      = GI.RipVer;
            PI->Ver         = GI.Version;

            SFI = (SVFIP)my_malloc(sizeof(SVFI));
            SFI->FileName   = my_strdup(GI.OptionString, 6);            
            switch (PI->Type) {
            case RIP_INFO_TYPE_GRAPH: lstrcat(SFI->FileName, ".graph");
                break;
            case RIP_INFO_TYPE_VIDEO: lstrcat(SFI->FileName, ".video");
                break;
            case RIP_INFO_TYPE_SOUND: lstrcat(SFI->FileName, ".sound");
                break;
            case RIP_INFO_TYPE_PACK:  lstrcat(SFI->FileName, ".pack");
                break;
            case RIP_INFO_TYPE_OTHER: lstrcat(SFI->FileName, ".other");
                break;
            case RIP_INFO_TYPE_MUSIC: lstrcat(SFI->FileName, ".music");
                break;
            case RIP_INFO_TYPE_TEXT:  lstrcat(SFI->FileName, ".text");
                break;
            case RIP_INFO_TYPE_EXE:   lstrcat(SFI->FileName, ".exec");
                break;
            }

            
            Ver2Char(PI->Ver, ver);            
            SFI->Description   = my_strdup(ver);
            SFI->Plugin        = my_strdup("");
            SFI->IsDirectory   = FALSE;
            SFI->Fmt           = -1;
            SFI->IsExtension   = FALSE;
            SFI->Offset        = GI.Active;
            SFI->LevelUp       = NULL;
            SFI->PackedSize    = 0;
            SFI->UnpackedSize  = 0;
            SFI->UserData.Data = PI;
            SFI->UserData.Size = sizeof(ADDPLUGININFO);//->Size;

            FAT.InsertItem((SVFIP)NULL, SFI);
        }
    }   

    Update_shownfiles(OpMode & OPM_SILENT);

    return TRUE;
}

void MrRipper::MakeCopyList(char * Dir)
{
    RIPPER_FILE_INFO FI;
    char * d;    
    for (int I = 0; I < shownfiles.size(); I++){
        if (shownfiles[I].FileName[strlen(shownfiles[I].FileName)-1]!='\\'){                    
            d = my_strdup(Dir, strlen(shownfiles[I].FileName)+2);
            if (strlen(d)>0) strcat(d, "\\");
            strcat(d, shownfiles[I].FileName);                  
            FI.FileName    = my_strdup(d);
            my_free(d);
            FI.Description = shownfiles[I].Description;
            FI.FileSize    = shownfiles[I].FileSize;
            FI.UnpSize     = shownfiles[I].UnpSize;
            FI.Plugin      = shownfiles[I].Plugin;            
            FI.Fmt         = shownfiles[I].Fmt;
            FI.StartOffset = shownfiles[I].StartOffset;            
            FI.UserData.Data = shownfiles[I].UserData.Data;
            FI.UserData.Size = shownfiles[I].UserData.Size;
            copyfiles.push_back(FI);
        } else {                        
            d = my_strdup(Dir, strlen(shownfiles[I].FileName)+2);
            if (strlen(d)>0) strcat(d, "\\");
            strcat(d, shownfiles[I].FileName);
            FI.FileName    = my_strdup(d);
            my_free(d);
            FI.FileSize    = 0;
            FI.UnpSize     = 0;
            FI.Description = shownfiles[I].Description;
            FI.Plugin      = shownfiles[I].Plugin;
            FI.StartOffset = 0;
            FI.Fmt         = -1;
            FI.UserData.Data = NULL;
            FI.UserData.Size = 0;
            copyfiles.push_back(FI);
            d = my_substr(shownfiles[I].FileName, 0, strlen(shownfiles[I].FileName)-1);            
            SetDirectory(d, OPM_SILENT);
            my_free(d);
            Update_shownfiles(TRUE);
            MakeCopyList(FI.FileName);
            SetDirectory("..", OPM_SILENT);
            Update_shownfiles(TRUE);
        }
    }    
}

void MrRipper::MakeCopyList(struct PluginPanelItem *PanelItem, int ItemsNumber)
{
    RIPPER_FILE_INFO FI;
    char *d;
    Free_copyfiles();
    for (int i = 0; i<ItemsNumber;i++){
        if (PanelItem[i].FindData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY){            
                        d = my_strdup(PanelItem[i].FindData.cFileName, 3);            
            if (strlen(d)>0) strcat(d, "\\");            
            FI.FileName    = my_strdup(d);
            my_free(d);
            FI.Description = PanelItem[i].Description;
            FI.FileSize    = 0;
            FI.UnpSize     = 0;
            FI.UserData.Data = NULL;
            FI.UserData.Size = 0;
            FI.Plugin      = shownfiles[i].Plugin;
            FI.StartOffset = 0;
            FI.Fmt         = -1;
            copyfiles.push_back(FI);
            SetDirectory(PanelItem[i].FindData.cFileName, OPM_SILENT);
            Update_shownfiles(TRUE);
            MakeCopyList(FI.FileName);
            SetDirectory("..", OPM_SILENT);
            Update_shownfiles(TRUE);
        } else {
            for (int I = 0; I < shownfiles.size(); I++){
                char *s = shownfiles[I].FileName;
                int   u = shownfiles[I].FileSize;
                int   z = shownfiles[I].StartOffset;
                if ((strcmp(s, PanelItem[i].FindData.cFileName)==0)&&
                    (shownfiles[I].FileSize==PanelItem[i].FindData.nFileSizeLow)&&
                    (shownfiles[I].StartOffset==PanelItem[i].PackSize)){

                    FI.Description = shownfiles[I].Description;
                    FI.FileName    = my_strdup(shownfiles[I].FileName);
                    FI.FileSize    = shownfiles[I].FileSize;
                    FI.UnpSize     = shownfiles[I].UnpSize;
                    FI.Plugin      = shownfiles[I].Plugin;
                    FI.Fmt         = shownfiles[I].Fmt;
                    FI.StartOffset = shownfiles[I].StartOffset;
                    FI.UserData.Data = shownfiles[I].UserData.Data;
                    FI.UserData.Size = shownfiles[I].UserData.Size;
                    copyfiles.push_back(FI);
                }
            }
        }
    }
}

BOOL MrRipper::DestDirPrompt(char* Dir)
{
  TSaveScreen SS;
  struct InitDialogItem InitItems[]={
  /*  0 */{DI_DOUBLEBOX, 3, 1,72, 8, 0, 0, 0,                          0,(char *)MExtraction},
  /*  1 */{DI_TEXT,      5, 2, 0, 0, 0, 0, 0,                          0,(char *)MPath},  
  /*  2 */{DI_EDIT,      5, 3,70, 0, 0,(DWORD)HistDestDir, DIF_HISTORY|DIF_USELASTHISTORY,
                                                                                                                                           0,Dir},  
  /*  3 */{DI_TEXT,      0, 4, 0, 0, 0, 0, 0,                          0,""},
  /*  4 */{DI_CHECKBOX,  5, 5, 0, 0, 0, 0, 0,                          0,(char *)MDoConv},  
  /*  5 */{DI_TEXT,      0, 6, 0, 0, 0, 0, 0,                          0,""},
  /*  6 */{DI_BUTTON,    0, 7, 0, 0, 0, 0, DIF_CENTERGROUP,            1,(char *)MOK},
  /*  7 */{DI_BUTTON,    0, 7, 0, 0, 0, 0, DIF_CENTERGROUP,            0,(char *)MCancel}
  };

    struct FarDialogItem DialogItems[sizeof(InitItems)/sizeof(InitItems[0])];
    InitDialogItems(InitItems,DialogItems,sizeof(InitItems)/sizeof(InitItems[0]));

    DialogItems[4].Selected = Opt.DoConv;

    int ExitCode=Info.Dialog(Info.ModuleNumber,-1,-1,76,10,"Extract",DialogItems,sizeof(DialogItems)/sizeof(DialogItems[0]));
    if ((ExitCode == 15)||(ExitCode == -1)) return FALSE;

    ConvView = DialogItems[4].Selected;
    strcpy(Dir, DialogItems[2].Data);
    return TRUE;
}

int MrRipper::OverwritePrompt(char* FileName)
{
  TSaveScreen SS;

    const char *MsgItems[]={GetMsg(MExtraction),GetMsg(MOverwrite), FileName, "", GetMsg(MYes), GetMsg(MNo), GetMsg(MYesAll), GetMsg(MNoAll), GetMsg(MCancel)};
    int ExitCode=Info.Message(Info.ModuleNumber,FMSG_WARNING,"Overwrite",MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),5);    
    ExitCode = ExitCode==4?-1:ExitCode;    
    return ExitCode;
}

void MrRipper::MakePutList2(char* CurDirectory, struct PluginPanelItem *PanelItem, int ItemsNumber)
{
    RIPPER_FILE_INFO FI;
    char *d;    
    for (int i = 0; i<ItemsNumber;i++){
        if (PanelItem[i].FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){            
            char * fnd = strchr(PanelItem[i].FindData.cFileName, '\\');
            int off;
            if (fnd){
                d = my_substr(CurDirectory, 0, strlen(CurDirectory), strlen(fnd+1)+2);
                off = fnd-PanelItem[i].FindData.cFileName+1;
            }else {
                d = my_substr(CurDirectory, 0, strlen(CurDirectory), strlen(PanelItem[i].FindData.cFileName)+2);
                off = 0;
            }
            if (strlen(d)> 3) strcat(d, "\\");
            if (fnd)
                strcat(d, fnd+1);            
            else
                strcat(d, PanelItem[i].FindData.cFileName);
            strcat(d, "\\");            
            if (strlen(CurDirectory)== 3) 
                FI.FileName    = my_strdup(d+strlen(CurDirectory)-off);
            else 
                FI.FileName    = my_strdup(d+strlen(CurDirectory)-off+1);
            my_free(d);
            FI.Description = my_strdup("");
            FI.FileSize    = 0;
            FI.UnpSize     = 0;
            FI.UserData.Data = NULL;
            FI.UserData.Size = 0;
            FI.Plugin      = my_strdup("");
            FI.StartOffset = 0;
            FI.Fmt         = -1;
            putfiles.push_back(FI);                                    
        } else {
            char * fnd = strchr(PanelItem[i].FindData.cFileName, '\\');
            int off;
            if (fnd){
                d = my_substr(CurDirectory, 0, strlen(CurDirectory), strlen(fnd+1)+1);
                off = fnd-PanelItem[i].FindData.cFileName+1;
            }else {
                d = my_substr(CurDirectory, 0, strlen(CurDirectory), strlen(PanelItem[i].FindData.cFileName)+1);
                off = 0;
            }
            if (strlen(d)> 3) strcat(d, "\\");
            if (fnd)
                strcat(d, fnd+1);
            else
                strcat(d, PanelItem[i].FindData.cFileName);
            if (strlen(CurDirectory)== 3) 
                FI.FileName    = my_strdup(d+strlen(CurDirectory)-off);
            else 
                FI.FileName    = my_strdup(d+strlen(CurDirectory)-off+1);
            my_free(d);
            FI.Description = my_strdup("");            
            FI.FileSize    = PanelItem[i].FindData.nFileSizeLow;
            FI.UnpSize     = PanelItem[i].FindData.nFileSizeLow;
            if (glbFmt == -1)
                FI.Plugin      = my_strdup("");
            else
                FI.Plugin      = my_strdup(glbPlugin);
            FI.Fmt         = glbFmt;
            FI.StartOffset = 0;
            FI.UserData.Data = NULL;
            FI.UserData.Size = 0;
            putfiles.push_back(FI);               
        }
    }
}

void MrRipper::MakePutList(char* CurDirectory, struct PluginPanelItem *PanelItem, int ItemsNumber)
{
    RIPPER_FILE_INFO FI;
    char *d;
    char p[MAX_PATH];
    struct PluginPanelItem *MyPanelItem;
    int MyItemsNumber;    
    for (int i = 0; i<ItemsNumber;i++){
        if (PanelItem[i].FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){            
            char * fnd = strchr(PanelItem[i].FindData.cFileName, '\\');
            if (fnd)
                d = my_substr(CurDirectory, 0, strlen(CurDirectory), strlen(fnd+1)+2);
            else 
                d = my_substr(CurDirectory, 0, strlen(CurDirectory), strlen(PanelItem[i].FindData.cFileName)+2);
            if (strlen(d)> 3) strcat(d, "\\");
            if (fnd)
                strcat(d, fnd+1);            
            else
                strcat(d, PanelItem[i].FindData.cFileName);
            strcat(d, "\\");
            strcpy(p, d);
            if (strlen(CurDirectory)== 3) 
                FI.FileName    = my_strdup(d+strlen(CurDirectory));
            else 
                FI.FileName    = my_strdup(d+strlen(CurDirectory)+1);
            my_free(d);
            FI.Description = my_strdup("");
            FI.FileSize    = 0;
            FI.UnpSize     = 0;
            FI.UserData.Data = NULL;
            FI.UserData.Size = 0;
            FI.Plugin      = my_strdup("");
            FI.StartOffset = 0;
            FI.Fmt         = -1;
            putfiles.push_back(FI);                        
            d = my_substr(p, 0, strlen(p)-1);            
            Info.GetDirList(d, &MyPanelItem, &MyItemsNumber);
            MakePutList2(d, MyPanelItem, MyItemsNumber);
            my_free(d);
            Info.FreeDirList(MyPanelItem);
        } else {
            char * fnd = strchr(PanelItem[i].FindData.cFileName, '\\');
            if (fnd)
                d = my_substr(CurDirectory, 0, strlen(CurDirectory), strlen(fnd+1)+1);
            else d = my_substr(CurDirectory, 0, strlen(CurDirectory), strlen(PanelItem[i].FindData.cFileName)+1);
            if (strlen(d)> 3) strcat(d, "\\");
            if (fnd)
                strcat(d, fnd+1);
            else
                strcat(d, PanelItem[i].FindData.cFileName);
            if (strlen(CurDirectory)== 3) 
                FI.FileName    = my_strdup(d+strlen(CurDirectory));
            else 
                FI.FileName    = my_strdup(d+strlen(CurDirectory)+1);
            my_free(d);
            FI.Description = my_strdup("");            
            FI.FileSize    = PanelItem[i].FindData.nFileSizeLow;
            FI.UnpSize     = PanelItem[i].FindData.nFileSizeLow;
            if (glbFmt == -1)
                FI.Plugin      = my_strdup("");
            else
                FI.Plugin      = my_strdup(glbPlugin);
            FI.Fmt         = glbFmt;
            FI.StartOffset = 0;
            FI.UserData.Data = NULL;
            FI.UserData.Size = 0;
            putfiles.push_back(FI);               
        }
    }
}

int MrRipper::FileExist(char * FileName, long Fsize)
{
    /*for (int I = 0; I < files.size(); I++){
        if ((strcmp(files[I].FileName, FileName)==0)&&
            (files[I].FileSize == Fsize)) return I;
    }*/
    return -1;
}

int MrRipper::FileExist2(char * FileName)
{
    /*for (int I = 0; I < files.size(); I++)
        if (strcmp(files[I].FileName, FileName)==0) return I;*/
    return -1;
}

int MrRipper::OverwritePromptPut(char* FileName)
{
  TSaveScreen SS;

    const char *MsgItems[]={GetMsg(MExtraction),GetMsg(MOverwritePut), FileName, "", GetMsg(MOK), GetMsg(MYesAll), GetMsg(MCancel)};
    int ExitCode=Info.Message(Info.ModuleNumber,FMSG_WARNING,"OverwritePut",MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),3);    
    ExitCode = ExitCode==2?-1:ExitCode;    
    return ExitCode;
}

int DelIsNotSupportedByPlugin()
{
    TSaveScreen SS;

    const char *MsgItems[]={GetMsg(MDelError),GetMsg(MDelNotSupportMsg), "", GetMsg(MSkip), GetMsg(MCancel)};
    int ExitCode=Info.Message(Info.ModuleNumber,FMSG_WARNING,"DelNotSupported",MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),2);        
    return ExitCode;
}

void DelIsNotSupportedByPlugin2()
{
    TSaveScreen SS;

    const char *MsgItems[]={GetMsg(MDelError),GetMsg(MDelNotSupportMsg), "", GetMsg(MOK)};
    Info.Message(Info.ModuleNumber,FMSG_WARNING,"DelNotSupported",MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),1);            
}

int DelError()
{
    TSaveScreen SS;

    const char *MsgItems[]={GetMsg(MDelError),GetMsg(MDelErrorMsg), "", GetMsg(MSkip), GetMsg(MCancel)};
    int ExitCode=Info.Message(Info.ModuleNumber,FMSG_WARNING,"DelNotSupported",MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),2);        
    return ExitCode;
}

BOOL ConfirmDelete()
{
    TSaveScreen SS;

    const char *MsgItems[]={GetMsg(MDeleting),GetMsg(MConfirmDelete), "", GetMsg(MYes), GetMsg(MNo)};
    int ExitCode=Info.Message(Info.ModuleNumber,FMSG_WARNING,"DelNotSupported",MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),2);        
    return ExitCode == 0;
}