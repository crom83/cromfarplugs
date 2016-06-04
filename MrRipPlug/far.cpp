int MrRipper::GetFindData(PluginPanelItem **pPanelItem,int *pItemsNumber,int OpMode)
{       
    TSaveScreen SS;             
    *pPanelItem=NULL;
    *pItemsNumber=0;  
    
    if (bPluginManager) PluginManager(OpMode);
    else FileRipper(OpMode);

    PluginPanelItem *NewPanelItem=(PluginPanelItem *)my_malloc(sizeof(PluginPanelItem)*shownfiles.size());
    *pPanelItem=NewPanelItem;
    if (NewPanelItem==NULL) return(FALSE);

    if (!bPluginManager){
        int CurItemPos=0;
        for (int Z=0; Z < shownfiles.size(); Z++){
            memset(&NewPanelItem[CurItemPos],0,sizeof(PluginPanelItem));
            NewPanelItem[CurItemPos].CustomColumnData=(char**)my_malloc(sizeof(char*));
            NewPanelItem[CurItemPos].CustomColumnNumber = 1;
            char deb[1000];

            if (shownfiles[Z].IsDirectory){
                NewPanelItem[CurItemPos].FindData.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
                
                lstrcpy(NewPanelItem[CurItemPos].FindData.cFileName, shownfiles[Z].FileName);
                
                NewPanelItem[CurItemPos].PackSize = 0;
                NewPanelItem[CurItemPos].FindData.nFileSizeLow = 0;

                NewPanelItem[CurItemPos].CustomColumnData[0]=my_strdup("<dir>");
            } else {
                NewPanelItem[CurItemPos].FindData.dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
         
                FSF.sprintf(deb, "%X+%X=%X", shownfiles[Z].StartOffset, shownfiles[Z].FileSize, shownfiles[Z].StartOffset+shownfiles[Z].FileSize);
                NewPanelItem[CurItemPos].CustomColumnData[0]=my_strdup(deb);

                NewPanelItem[CurItemPos].PackSize = shownfiles[Z].StartOffset;
                
                NewPanelItem[CurItemPos].FindData.nFileSizeLow = shownfiles[Z].FileSize;
        
                lstrcpy(NewPanelItem[CurItemPos].FindData.cFileName,shownfiles[Z].FileName);

                if (shownfiles[Z].UserData.Size > 0){
                    NewPanelItem[CurItemPos].Flags |= PPIF_USERDATA;
                    NewPanelItem[CurItemPos].UserData = (ULONG)&shownfiles[Z].UserData;
                }
            }

            NewPanelItem[CurItemPos].Description= my_strdup(shownfiles[Z].Description);
        
            NewPanelItem[CurItemPos].Owner = NULL;
            CurItemPos++;
        }
    } else {
        ADDPLUGININFO *PI;
        int CurItemPos=0;        
        int u = Char2Ver(RIPVER);
        for (int Z=0; Z < shownfiles.size(); Z++){
            memset(&NewPanelItem[CurItemPos],0,sizeof(PluginPanelItem));
            NewPanelItem[CurItemPos].CustomColumnData=(char**)my_malloc(sizeof(char*));
            NewPanelItem[CurItemPos].CustomColumnNumber = 1;
            char deb[1000];

            if (shownfiles[Z].IsDirectory){
                NewPanelItem[CurItemPos].FindData.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;

                char *c = my_substr(shownfiles[Z].FileName, 0, strlen(shownfiles[Z].FileName)-1);
                strcpy(NewPanelItem[CurItemPos].FindData.cFileName, c);
                my_free(c);

                NewPanelItem[CurItemPos].PackSize = 0;
                NewPanelItem[CurItemPos].FindData.nFileSizeLow = 0;

                NewPanelItem[CurItemPos].CustomColumnData[0]=my_strdup("<dir>");
            } else {
                PI = (ADDPLUGININFO*)shownfiles[Z].UserData.Data;
                
                if (PI->RipVer <= u)
                    NewPanelItem[CurItemPos].FindData.dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
                else 
                    NewPanelItem[CurItemPos].FindData.dwFileAttributes = FILE_ATTRIBUTE_HIDDEN;
                
                NewPanelItem[CurItemPos].Flags |= PPIF_USERDATA;
                NewPanelItem[CurItemPos].UserData = (ULONG)&shownfiles[Z].UserData;
                
                NewPanelItem[CurItemPos].PackSize = shownfiles[Z].StartOffset;
         
                FSF.sprintf(deb, "%X+%X=%X", shownfiles[Z].StartOffset, shownfiles[Z].FileSize, shownfiles[Z].StartOffset+shownfiles[Z].FileSize);
                NewPanelItem[CurItemPos].CustomColumnData[0]=my_strdup(deb);

                
                
                NewPanelItem[CurItemPos].FindData.nFileSizeLow = shownfiles[Z].FileSize;
        
                strcpy(NewPanelItem[CurItemPos].FindData.cFileName,shownfiles[Z].FileName);
            }

            NewPanelItem[CurItemPos].Description= my_strdup(shownfiles[Z].Description);
        
            NewPanelItem[CurItemPos].Owner = NULL;
            CurItemPos++;
        }
    }
    *pItemsNumber= shownfiles.size();   

    return(TRUE);
}

void MrRipper::FreeFindData(PluginPanelItem *PanelItem,int ItemsNumber)
{               
    for (int I=0;I<ItemsNumber;I++)
    {                   
        my_free(PanelItem[I].CustomColumnData[0]);        
        my_free(PanelItem[I].CustomColumnData); 
        my_free(PanelItem[I].Description);      
    }
    my_free(PanelItem);
    PanelInfo PInfo;
    Info.Control(INVALID_HANDLE_VALUE,FCTL_GETPANELINFO,&PInfo);
    for (I=0;I<PInfo.ItemsNumber;I++){
        if (PInfo.PanelItems[I].PackSize == 1&&
            !(PInfo.PanelItems[I].FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
            PInfo.PanelItems[I].Flags = PPIF_SELECTED;
        else PInfo.PanelItems[I].Flags = 0;
    }
    Info.Control(INVALID_HANDLE_VALUE,FCTL_SETSELECTION,&PInfo);
}

void MrRipper::GetOpenPluginInfo(struct OpenPluginInfo *Info)
{       
    Info->StructSize=sizeof(*Info);
    Info->Flags=  OPIF_USEFILTER|OPIF_USESORTGROUPS|OPIF_USEHIGHLIGHTING|
                OPIF_ADDDOTS|OPIF_SHOWPRESERVECASE;

    Info->CurDir = FarCurDir;
    Info->Format=(char *) GetMsg(MMrRipper);
    if (bPluginManager) {
        Info->HostFile=RipFileName;
        
        FSF.sprintf(Title," %s%s ",GetMsg(MMrRipperList), FarCurDir);
        Info->PanelTitle=Title;

        //static struct InfoPanelLine InfoPanelLineArray[4];
        //strcpy(InfoPanelLineArray[0].Text, GetMsg(MMrRipper));
        //InfoPanelLineArray[0].Separator = TRUE;

        Info->InfoLines= NULL;
        Info->InfoLinesNumber=0;

        Info->DescrFiles=NULL;
        Info->DescrFilesNumber=0;

        static struct PanelMode PanelModesArray[10];
        static char *ColumnTitles[2];
        ColumnTitles[0]=GetMsg(MColumnPlugin);
        ColumnTitles[1]=GetMsg(MColumnVersion);     
    
        PanelModesArray[3].ColumnTypes="N,Z";
        PanelModesArray[3].StatusColumnTypes="N, Z";
        PanelModesArray[3].StatusColumnWidths="0,20";
        PanelModesArray[3].ColumnWidths="0,20";
        PanelModesArray[3].ColumnTitles=ColumnTitles;
        PanelModesArray[3].AlignExtensions = TRUE;
        PanelModesArray[3].FullScreen=FALSE;        

        Info->PanelModesArray=PanelModesArray;
        Info->PanelModesNumber=sizeof(PanelModesArray)/sizeof(PanelModesArray[0]);
        Info->StartPanelMode='3';
        static struct KeyBarTitles KeyBar={
            {NULL,NULL,NULL,"",  "",  "",  "",  "",  NULL,NULL,NULL,NULL},//Titles
            {NULL,NULL,NULL,NULL,"",  NULL,NULL,"",  "",  NULL,"",  NULL},//Ctrl
            {NULL,NULL,NULL,NULL,NULL,"",  NULL,NULL,NULL,NULL,NULL,NULL},//Alt
            {"",  "",  "",  "",  "",  "",  "",  "",  "",  NULL,NULL,NULL},//Shift
            {NULL,NULL,"",  "",  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},//CtrlShift
            {"",  "",  "",  "",  "",  "",  "",  "",  NULL,"",  "",  ""}   //CtlAlt
        };      
        KeyBar.Titles[3-1]=GetMsg(MPMF3);
        KeyBar.Titles[6-1]=GetMsg(MPMF6);
        KeyBar.Titles[7-1]=GetMsg(MPMF7);
        KeyBar.Titles[8-1]=GetMsg(MPMF8);
        Info->KeyBar=&KeyBar;  
    } else {
        Info->HostFile=RipFileName;  

        char *pdest;
        pdest = strrchr(RipFileName, '\\');  
        pdest++;
        FSF.sprintf(Title," %s: %s\\%s ",GetMsg(MMrRipper), pdest, FarCurDir);
        Info->PanelTitle=Title;

        static struct InfoPanelLine InfoPanelLineArray[4];
        strcpy(InfoPanelLineArray[0].Text, GetMsg(MMrRipper));
        InfoPanelLineArray[0].Separator = TRUE;

        strcpy(InfoPanelLineArray[1].Text, GetMsg(MPackType));
        if (glbFmt != -1) strcpy(InfoPanelLineArray[1].Data, glbPlugin);
        else strcpy(InfoPanelLineArray[1].Data, GetMsg(MRawType));

        strcpy(InfoPanelLineArray[2].Text, GetMsg(MFilesCount));
        FSF.sprintf(InfoPanelLineArray[2].Data, "%d", glbAllFiles);
  
        strcpy(InfoPanelLineArray[3].Text, GetMsg(MAllSize));
        FSF.sprintf(InfoPanelLineArray[3].Data, "%d", glbAllSize);

        Info->InfoLines= InfoPanelLineArray;
        Info->InfoLinesNumber=4;

        Info->DescrFiles=NULL;
        Info->DescrFilesNumber=0;

        static struct PanelMode PanelModesArray[10];
        static char *ColumnTitles[4];
        ColumnTitles[0]=GetMsg(MColumnFile);
        ColumnTitles[1]=GetMsg(MColumnOffset);
        ColumnTitles[2]=GetMsg(MColumnSize);
        ColumnTitles[3]=GetMsg(MColumnDescription);
    
        PanelModesArray[3].ColumnTypes="N,PCT,SCT,Z";
        PanelModesArray[3].StatusColumnTypes="N,C0,Z";
        PanelModesArray[3].StatusColumnWidths="13,22,0";
        PanelModesArray[3].ColumnWidths="13,11,11,0";
        PanelModesArray[3].ColumnTitles=ColumnTitles;
        PanelModesArray[3].AlignExtensions = TRUE;
        PanelModesArray[3].FullScreen=FALSE;  

        PanelModesArray[4].ColumnTypes="N,PCT,SCT,Z";
        PanelModesArray[4].StatusColumnTypes="N,C0,Z";
        PanelModesArray[4].StatusColumnWidths="13,22,0";
        PanelModesArray[4].ColumnWidths="13,11,11,0";
        PanelModesArray[4].ColumnTitles=ColumnTitles;
        PanelModesArray[4].AlignExtensions = TRUE;
        PanelModesArray[4].FullScreen=TRUE;  

        Info->PanelModesArray=PanelModesArray;
        Info->PanelModesNumber=sizeof(PanelModesArray)/sizeof(PanelModesArray[0]);
        Info->StartPanelMode='3';
        static struct KeyBarTitles KeyBar={
            {NULL,NULL,NULL,"",  "",  "",  NULL,NULL,NULL,NULL,NULL,NULL},//Titles
            {NULL,NULL,NULL,NULL,"",  NULL,NULL,NULL,NULL,NULL,"",  NULL},//Ctrl
            {NULL,NULL,NULL,NULL,NULL,"",  NULL,NULL,NULL,NULL,NULL,NULL},//Alt
            {NULL,NULL,  NULL,NULL,"",  "",  "",  "",  "",  NULL,NULL,NULL},//Shift
            {NULL,NULL,NULL,"",  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},//CtrlShift
            {"",  "",  "",  "",  "",  "",  "",  "",  NULL,"",  "",  ""}   //CtlAlt
        };
        KeyBar.Titles[3-1]=GetMsg(MF3);
        KeyBar.Titles[5-1]=GetMsg(MF5);
        if (DelDup)  KeyBar.Titles[7-1]=GetMsg(MF7On);
        else         KeyBar.Titles[7-1]=GetMsg(MF7Off);         
        KeyBar.CtrlShiftTitles[3-1]=GetMsg(MCtrlShiftF3);
        KeyBar.CtrlTitles[5-1]=GetMsg(MCtrlF5);
        KeyBar.CtrlTitles[8-1]=GetMsg(MCtrlF8);
        KeyBar.CtrlTitles[9-1]=GetMsg(MCtrlF9);
        KeyBar.ShiftTitles[4-1]=GetMsg(MShiftF4);
        KeyBar.ShiftTitles[3-1]=GetMsg(MShiftF3);
        KeyBar.ShiftTitles[2-1]=GetMsg(MShiftF2);
        if (Opt.UseLists) KeyBar.ShiftTitles[0]=GetMsg(MShiftF1Off);
        else              KeyBar.ShiftTitles[0]=GetMsg(MShiftF1On);
  
        Info->KeyBar=&KeyBar;
    }
}

int MrRipper::GetFiles(struct PluginPanelItem *PanelItem, int ItemsNumber, int Move, char *DestPath, int OpMode)
{
    TSaveScreen SS;
    DWORD StartTime=GetTickCount();
    BOOL WaitMessage=FALSE;
    double Progress1, Progress2;
    long FullSize,CurSize, ProcessedBytes;
    int Owervrite = 0;
    char MyDestPath[MAX_PATH];
    char MyDestFile[MAX_PATH];

    if (!OpMode & OPM_SILENT){        
        if (!DestDirPrompt(DestPath)) return -2;        
    }

        if ((strlen(DestPath)==3)&&
                (DestPath[1]==':')&&
                (DestPath[2]=='\\')) DestPath[2] = 0;

    GetCurrentDirectory(MAX_PATH, MyDestPath);
    if (!SetCurrentDirectory(DestPath)) return FALSE;
    SetCurrentDirectory(MyDestPath);
    
    FormatFinder FF(PluginPath);        
    for (int i = 0; i<FF.GetFormatCount(); i++)             
        for (int j = 0; j<FF.GetnKnownFormats(i); j++)
            FF.SetActiveOpt(i, j, GetRegKey(HKEY_CURRENT_USER,"",FF.GetRegPath(i, j), 1));

    MakeCopyList(PanelItem, ItemsNumber);

    int f1, f2;
    int readed, pos, endpos, getret;
    
    if (!bPluginManager){
        FullSize = 0;
    
        for (int I = 0; I < copyfiles.size(); I++)
            FullSize += copyfiles[I].FileSize;
                        
        ProcessedBytes = 0;                                                     
    
        for (I = 0; I < copyfiles.size(); I++){
            if (copyfiles[I].FileName[strlen(copyfiles[I].FileName)-1]=='\\'){
                char *c = my_substr(copyfiles[I].FileName, 0, strlen(copyfiles[I].FileName)-1);
                FSF.sprintf(MyDestPath, "%s\\%s", DestPath, c);
                my_free(c);
                CreateDirectory(MyDestPath, NULL);            
            } else {    
                char *d = ExtractPath(copyfiles[I].FileName);
                if (strlen(d)>0) FSF.sprintf(MyDestPath, "%s\\%s", DestPath, d); 
                else strcpy(MyDestPath, DestPath);
                my_free(d);
            
                d = ExtractName(copyfiles[I].FileName);
                FSF.sprintf(MyDestFile, "%s", d);
                my_free(d);

                char c[MAX_PATH];
                FSF.sprintf(c, "%s\\%s", MyDestPath, MyDestFile);

                f2 = _open(c, _O_RDONLY|_O_BINARY);
                if (f2!=-1) {
                    _close(f2);
                    if (Owervrite==3) continue;
                    if (Owervrite == 0||Owervrite == 1)
                    if (!OpMode & OPM_SILENT) Owervrite = OverwritePrompt(MyDestFile);
                    if (Owervrite==-1) break;
                    if (Owervrite==3||Owervrite==1) continue;
                }   
    
                if (copyfiles[I].Fmt != FMT_DUMMY)
                    if (FF.GetCanGetFile(copyfiles[I].Fmt, copyfiles[I].Plugin)&&ConvView){
                        getret = FF.GetFile(copyfiles[I].Plugin, copyfiles[I].Fmt, RipFileName, MyDestFile, MyDestPath, copyfiles[I].StartOffset, copyfiles[I].FileSize, copyfiles[I].UnpSize, OpMode & OPM_SILENT, copyfiles[I].UserData.Data);
                        if (getret == 1){
                            ProcessedBytes += copyfiles[I].FileSize;
                            WaitMessage=FALSE;
                            if (!OpMode & OPM_SILENT){                                
                                if (GetTickCount()-StartTime>500){
                                    if (CheckForKey(VK_ESCAPE)) break;
                                    Progress1 = 100.0;
                                    Progress2 = ProcessedBytes; Progress2 *= 100; Progress2 /= FullSize;
                                    char FileMsg[100], ProgressMsg1[100], ProgressMsg2[100];
                                    FSF.sprintf(FileMsg,GetMsg(MExtracting), MyDestFile);
                                    FSF.sprintf(ProgressMsg1,GetMsg(MExtr1), Progress1);
                                    FSF.sprintf(ProgressMsg2,GetMsg(MExtr2), Progress2);                                                                                        
                                    const char *MsgItems[]={GetMsg(MExtraction),FileMsg, ProgressMsg1, ProgressMsg2};
                                    Info.Message(Info.ModuleNumber,WaitMessage ? FMSG_LEFTALIGN|FMSG_KEEPBACKGROUND:FMSG_LEFTALIGN,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
                                    WaitMessage=TRUE;
                                }       
                            }
                            continue;
                        }
                        if (getret == 2){
                            if (CheckForKey(VK_ESCAPE)) break;
                            continue;
                        }
                    }
            
                FSF.sprintf(c, "%s\\%s", MyDestPath, MyDestFile);
    
                f1 = _open(RipFileName, _O_RDONLY|_O_BINARY);
                _lseek(f1,copyfiles[I].StartOffset,SEEK_SET);                                                     
            
                f2 = _open(c, _O_CREAT|_O_WRONLY|_O_BINARY);
                                
                endpos = copyfiles[I].StartOffset+copyfiles[I].FileSize;
                CurSize = copyfiles[I].FileSize;
                pos = _tell(f1);

                while (pos<endpos){                                       
                    if (!OpMode & OPM_SILENT){                    
                        if (GetTickCount()-StartTime>500){
                            if (CheckForKey(VK_ESCAPE)) break;
                            Progress1 = (pos-copyfiles[I].StartOffset); Progress1 *= 100; Progress1 /= copyfiles[I].FileSize;
                            Progress2 = ProcessedBytes; Progress2 *= 100; Progress2 /= FullSize;
                            char FileMsg[100], ProgressMsg1[100], ProgressMsg2[100];
                            FSF.sprintf(FileMsg,GetMsg(MExtracting), MyDestFile);
                            FSF.sprintf(ProgressMsg1,GetMsg(MExtr1), Progress1);
                            FSF.sprintf(ProgressMsg2,GetMsg(MExtr2), Progress2);                                                                                        
                            const char *MsgItems[]={GetMsg(MExtraction),FileMsg, ProgressMsg1, ProgressMsg2};
                            Info.Message(Info.ModuleNumber,WaitMessage ? FMSG_LEFTALIGN|FMSG_KEEPBACKGROUND:FMSG_LEFTALIGN,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
                            WaitMessage=TRUE;
                        }       
                    }
                    readed = _read(f1, buf, BUF_SIZE);                                       
                    pos = _tell(f1);                                                                                
                    if (pos > endpos) readed = readed - (pos - endpos);
                    _write(f2, buf, readed);
                    ProcessedBytes += readed;
                    pos = _tell(f1);    
                    WaitMessage=FALSE;
                }            
                _close(f2);
                _close(f1);            
                SetFileAttributes(c, FILE_ATTRIBUTE_ARCHIVE);            
                if (!OpMode & OPM_SILENT) if (CheckForKey(VK_ESCAPE))  break;
            }
        }
        Free_copyfiles();
        return TRUE;
    } else { //For plugin manager    
        ADDPLUGININFO *PI;
        for (int I = 0; I < copyfiles.size(); I++){
            if (copyfiles[I].FileName[strlen(copyfiles[I].FileName)-1]=='\\'){
                char *c = my_substr(copyfiles[I].FileName, 0, strlen(copyfiles[I].FileName)-1);
                FSF.sprintf(MyDestPath, "%s\\%s", DestPath, c);
                my_free(c);
                CreateDirectory(MyDestPath, NULL);            
            } else {    
                char *d = ExtractPath(copyfiles[I].FileName);
                if (strlen(d)>0) FSF.sprintf(MyDestPath, "%s\\%s", DestPath, d); 
                else strcpy(MyDestPath, DestPath);
                my_free(d);
            
                d = ExtractName(copyfiles[I].FileName);
                FSF.sprintf(MyDestFile, "%s", d);
                my_free(d);

                char c[MAX_PATH];
                FSF.sprintf(c, "%s\\%s", MyDestPath, MyDestFile);

                f2 = _open(c, _O_RDONLY|_O_BINARY);
                if (f2!=-1) {
                    _close(f2);
                    if (Owervrite==3) continue;
                    if (Owervrite == 0||Owervrite == 1)
                    if (!OpMode & OPM_SILENT) Owervrite = OverwritePrompt(MyDestFile);
                    if (Owervrite==-1) break;
                    if (Owervrite==3||Owervrite==1) continue;
                }                   
            
                FSF.sprintf(c, "%s\\%s", MyDestPath, MyDestFile);                
            
                f2 = _open(c, _O_CREAT|_O_WRONLY|_O_BINARY);                                
                
                PI = (ADDPLUGININFO*)copyfiles[I].UserData.Data;

                FSF.sprintf((char*)buf, GetMsg(MInfoOptString), copyfiles[I].FileName);
                _write(f2, buf, lstrlen((char*)buf)+1);
                _write(f2, "\xD\xA\xD\xA", 4);

                FSF.sprintf((char*)buf, GetMsg(MInfoVer), copyfiles[I].Description);
                _write(f2, buf, lstrlen((char*)buf)+1);
                _write(f2, "\xD\xA", 2);
                
                char v[30];
                Ver2Char(PI->RipVer, v);

                FSF.sprintf((char*)buf, GetMsg(MInfoRipVer), v);
                _write(f2, buf, lstrlen((char*)buf)+1);                
                _write(f2, "\xD\xA\xD\xA", 4);

                FSF.sprintf((char*)buf, GetMsg(MInfoCopyright), PI->Copy);
                _write(f2, buf, lstrlen((char*)buf)+1);
                _write(f2, "\xD\xA", 2);

                FSF.sprintf((char*)buf, GetMsg(MInfoPath), PI->FilePath);
                _write(f2, buf, lstrlen((char*)buf)+1);
                _write(f2, "\xD\xA", 2);

                char *types[8] = {GetMsg(MInfoTypeGraph), 
                                  GetMsg(MInfoTypeVideo), 
                                  GetMsg(MInfoTypeSound),
                                  GetMsg(MInfoTypePack),
                                  GetMsg(MInfoTypeOther),
                                  GetMsg(MInfoTypeMusic),
                                  GetMsg(MInfoTypeText),
                                  GetMsg(MInfoTypeExe)};

                FSF.sprintf((char*)buf, GetMsg(MInfoType), types[PI->Type]);
                _write(f2, buf, lstrlen((char*)buf)+1);                

                _close(f2);
                
                SetFileAttributes(c, FILE_ATTRIBUTE_ARCHIVE);            
                if (!OpMode & OPM_SILENT) if (CheckForKey(VK_ESCAPE))  break;
            }
        }
        Free_copyfiles();

        //This for refuse reset selection
        if (!OpMode&OPM_SILENT) return -1;
        else return TRUE;
    }
    Free_copyfiles();
    return TRUE;
}

int MrRipper::ProcessKey(int Key, unsigned int ControlState)
{
        switch (ControlState)
        {               
        case (PKF_CONTROL|PKF_SHIFT): 
            if (Key==VK_F3) ConvView = (Opt.DoConv)?FALSE:TRUE;                 
            return Key==VK_F4?TRUE:FALSE;        
            break;
        case (PKF_SHIFT): 
                        if (Key == VK_F4){
                                AddDummy();
                                Info.Control(INVALID_HANDLE_VALUE, FCTL_UPDATEPANEL, (void*)1);
                                Info.Control(INVALID_HANDLE_VALUE, FCTL_REDRAWPANEL, NULL);
                                return TRUE;
                        }
                        if (Key == VK_F3){
                                SplitFile();
                                Info.Control(INVALID_HANDLE_VALUE, FCTL_UPDATEPANEL, (void*)1);
                                Info.Control(INVALID_HANDLE_VALUE, FCTL_REDRAWPANEL, NULL);
                                return TRUE;
                        }                       
                        if (Key == VK_F1){
                                Opt.UseLists = !Opt.UseLists;                                
                                Free_shownfiles();
                                Free_copyfiles();
                                Free_dirs();
                                Free_dummyfiles();    
                                FAT.ReInitFAT();
                                Info.Control(INVALID_HANDLE_VALUE, FCTL_UPDATEPANEL, (void*)1);
                                Info.Control(INVALID_HANDLE_VALUE, FCTL_REDRAWPANEL, NULL);
                                SetRegKey(HKEY_CURRENT_USER,"",StrUseLists,   Opt.UseLists);
                                return TRUE;
                        }                       
                        if (Key == VK_F2){
                                SaveList(FALSE);                                
                                return TRUE;
                        }                       
                                                  return (Key==VK_F5)||
                                 (Key==VK_F6)||
                                 (Key==VK_F7)||
                                 (Key==VK_F8)?TRUE:FALSE;
                break;
        case (PKF_ALT):  return (Key==VK_F6)?TRUE:FALSE;
                break;
        case (PKF_CONTROL):                     
            if (Key==VK_F9) {
                if (DefineRange(Opt.StartOffset, Opt.EndOffset)) {
                    Opt.UserScanRange = TRUE;
                    FAT.ReInitFAT();
                    Free_shownfiles();
                    Free_dirs();
                    Free_copyfiles();
                    Info.Control(INVALID_HANDLE_VALUE, FCTL_UPDATEPANEL, (void*)1);
                    Info.Control(INVALID_HANDLE_VALUE, FCTL_REDRAWPANEL, NULL);
                    return TRUE;
                }
            }
            if (Key==VK_F8) {
                Opt.UserScanRange = FALSE;
                FAT.ReInitFAT();
                Free_shownfiles();
                Free_dirs();
                Free_copyfiles();
                Info.Control(INVALID_HANDLE_VALUE, FCTL_UPDATEPANEL, (void*)1);
                Info.Control(INVALID_HANDLE_VALUE, FCTL_REDRAWPANEL, NULL);
                return TRUE;
            }
            if (Key==VK_F5) {
                FAT.ReInitFAT();
                Free_shownfiles();
                Free_copyfiles();
                Free_dirs();
                Info.Control(INVALID_HANDLE_VALUE, FCTL_UPDATEPANEL, (void*)1);
                Info.Control(INVALID_HANDLE_VALUE, FCTL_REDRAWPANEL, NULL);
                return TRUE;
            }
            return (Key==VK_F11)?TRUE:FALSE;
                break;        
        default: 
            if (Key==VK_F3) ConvView = (Opt.DoConv)?TRUE:FALSE;
            if (Key==VK_F5) ConvView = Opt.DoConv;
            if (Key==VK_F6) {
                if (bPluginManager) return SavePlugins();
            }
            if (Key==VK_F7){
                if (!bPluginManager) {
                    DelDup = !DelDup;                
                    Info.Control(INVALID_HANDLE_VALUE, FCTL_UPDATEPANEL, (void*)1);
                    Info.Control(INVALID_HANDLE_VALUE, FCTL_REDRAWPANEL, NULL);
                    return TRUE;
                } else {
                    CreateSheme();
                    return TRUE;
                }
            }
            if (Key==VK_F8) 
                if (bPluginManager) {
                    LoadSheme();
                    return TRUE;
                }

            return (Key==VK_F4)?TRUE:FALSE;                   
            break;
        }
        return FALSE;
}

int MrRipper::SetDirectory(const char *Dir,int OpMode)
{
  char * temp;
  if (*Dir=='\\' && *(++Dir)==0) {
    //strcpy(CurDir, "\\");
    lstrcpy(FarCurDir, "");    
    return TRUE;
  }

  if (lstrcmp(Dir,"..")==0) {   
    temp = ExtractPath(FarCurDir);
    lstrcpy(FarCurDir, temp);
    my_free(temp);        
    return TRUE;
  } else{
    if (lstrlen(FarCurDir)!=0) lstrcat(FarCurDir, "\\");
    lstrcat(FarCurDir, Dir);
    return TRUE;
  } 
  return FALSE;
}

HANDLE MrRipper::OpenPlugin(int OpenFrom,int Item)
{
    BOOL Silent=FALSE;
    bPluginManager = FALSE;

    if (OpenFrom == OPEN_FINDLIST) Silent=TRUE;

    if (OpenFrom == OPEN_PLUGINSMENU&&Item==0){
        if (!Silent&&Opt.ShowScanDialog){
            if (!ScanDialog()) return INVALID_HANDLE_VALUE;
        } else {
            FullScan    = Opt.FullScan;
            DelDup      = Opt.DelDup;
            UseSomeOffset = Opt.UseSomeOffset;
            UseStartScan= Opt.UseStartScan;
            SkipPack    = Opt.SkipPack;
            EnableGraph = Opt.EnableGraph;
            EnableVideo = Opt.EnableVideo;
            EnableSound = Opt.EnableSound;
            EnableMusic = Opt.EnableMusic;
            EnableText  = Opt.EnableText;
            EnableExe   = Opt.EnableExe;
            EnablePack  = Opt.EnablePack;
            EnableOther = Opt.EnableOther;
        }
        PanelInfo PInfo;
        Info.Control(INVALID_HANDLE_VALUE,FCTL_GETPANELINFO,&PInfo);  

        if (PInfo.PanelItems[PInfo.CurrentItem].FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
            return INVALID_HANDLE_VALUE;
        FSF.sprintf(RipFileName, "%s\\%s", PInfo.CurDir, PInfo.PanelItems[PInfo.CurrentItem].FindData.cFileName);
    }

    if (OpenFrom == OPEN_COMMANDLINE){
        char * path = ExtractPath((char*)Item);
        if (strcmp(path, "")==0){
            PanelInfo PInfo;
            Info.Control(INVALID_HANDLE_VALUE,FCTL_GETPANELINFO,&PInfo);  
            if (PInfo.PanelItems[PInfo.CurrentItem].FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                my_free(path);
                return INVALID_HANDLE_VALUE;
            }                
            FSF.sprintf(RipFileName, "%s\\%s", PInfo.CurDir, (char*)Item);                            
        } else {
            FSF.sprintf(RipFileName, "%s", (char*)Item);                
        }
        my_free(path);      
    }

    if (OpenFrom == OPEN_PLUGINSMENU&&Item==1) {        
        EnableGraph = Opt.EnableGraph;
        EnableVideo = Opt.EnableVideo;
        EnableSound = Opt.EnableSound;
        EnableMusic = Opt.EnableMusic;
        EnableText  = Opt.EnableText;
        EnableExe   = Opt.EnableExe;
        EnablePack  = Opt.EnablePack;
        EnableOther = Opt.EnableOther;
        PanelInfo PInfo;
        Info.Control(INVALID_HANDLE_VALUE,FCTL_GETPANELINFO,&PInfo);
        FSF.sprintf(RipFileName, "%s\\%s", PInfo.CurDir, PInfo.PanelItems[PInfo.CurrentItem].FindData.cFileName);        
        bPluginManager = TRUE;
    }

    return this;
}

int MrRipper::PutFiles(struct PluginPanelItem *PanelItem, int ItemsNumber, int Move, int OpMode)
{
    /*if (bPluginManager) return FALSE;

    TSaveScreen SS;
    RIPPER_FILE_INFO FI;
    DWORD StartTime=GetTickCount();
    BOOL WaitMessage=FALSE;
    BOOL UseDelete = FALSE;
    double Progress1, Progress2;
    long FullSize, ProcessedBytes;
    int Owervrite = 0;
    int FileInfiles;

    char MySrcPath[MAX_PATH];    
    char MySrcPath2[MAX_PATH];
    void * FAT = NULL;
    char MyDestPath[MAX_PATH];
    char MyDestFile[MAX_PATH];

    char TmpFile[MAX_PATH];
    
    FormatFinder FF(PluginPath);
    FF.WorkFileSize = WorkFileSize;
    for (int i = 0; i<FF.GetFormatCount(); i++)             
        for (int j = 0; j<FF.GetnKnownFormats(i); j++)
            FF.SetActiveOpt(i, j, GetRegKey(HKEY_CURRENT_USER,"",FF.GetRegPath(i, j), 1));

    GetCurrentDirectory(MAX_PATH, MySrcPath);
    Free_putfiles();
    MakePutList(MySrcPath, PanelItem, ItemsNumber);
    if (strlen(MySrcPath)==3) MySrcPath[2]=0;    

    FullSize = 0;
    
    for (int I = 0; I < putfiles.size(); I++)
        FullSize += putfiles[I].FileSize;
                        
    ProcessedBytes = 0;                                                     

    char c[MAX_PATH];

    if ((glbFmt == -1)||Move) { //Raw data putting
            for (int I = 0; I < putfiles.size(); I++){                

                char *d = ExtractPath(putfiles[I].FileName);
                if (strlen(d)>0)
                    FSF.sprintf(MyDestPath, "%s\\%s", FarCurDir, d);                    
                else strcpy(MyDestPath, FarCurDir);
                my_free(d);
            
                d = ExtractName(putfiles[I].FileName);
                FSF.sprintf(MyDestFile, "%s", d);
                my_free(d);
                
                FSF.sprintf(c, "%s\\%s", MyDestPath, MyDestFile);                
                
                if (putfiles[I].FileName[strlen(putfiles[I].FileName)-1]=='\\')                    
                    continue;

                FileInfiles = FileExist2(c);
                if (FileInfiles>-1) {
                    if (files[FileInfiles].FileSize>=putfiles[I].FileSize) {

                        if (strlen(MyDestPath)>0) strcpy(MyDestPath, MyDestPath+1);
                        
                        d = ExtractPath(putfiles[I].FileName);
                        if (strlen(d)>0)
                            FSF.sprintf(MySrcPath2, "%s\\%s", MySrcPath, d);
                        else FSF.sprintf(MySrcPath2, "%s", MySrcPath);                
                        my_free(d);

                        if (FileToFileCopy(putfiles[I].FileName, RipFileName, 0, putfiles[I].FileSize, files[FileInfiles].StartOffset)>-1){
                            ProcessedBytes += putfiles[I].FileSize;
                            WaitMessage=FALSE;
                            if (!OpMode & OPM_SILENT){                                
                                if (GetTickCount()-StartTime>500){
                                    if (CheckForKey(VK_ESCAPE)) break;
                                    Progress1 = 100.0;
                                    Progress2 = ProcessedBytes; Progress2 *= 100; Progress2 /= FullSize;
                                    char FileMsg[100], ProgressMsg1[100], ProgressMsg2[100];
                                    FSF.sprintf(FileMsg,GetMsg(MAdd), MyDestFile);
                                    FSF.sprintf(ProgressMsg1,GetMsg(MExtr1), Progress1);
                                    FSF.sprintf(ProgressMsg2,GetMsg(MExtr2), Progress2);                                                                                        
                                    const char *MsgItems[]={GetMsg(MAdding),FileMsg, ProgressMsg1, ProgressMsg2};
                                    Info.Message(Info.ModuleNumber,WaitMessage ? FMSG_LEFTALIGN|FMSG_KEEPBACKGROUND:FMSG_LEFTALIGN,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
                                    WaitMessage=TRUE;
                                }       
                            }

                        }
                    }
                } else {
                    ProcessedBytes += putfiles[I].FileSize;
                    WaitMessage=FALSE;
                    if (!OpMode & OPM_SILENT){                        
                        if (GetTickCount()-StartTime>500){
                            if (CheckForKey(VK_ESCAPE)) break;
                            Progress1 = 100.0;
                            Progress2 = ProcessedBytes; Progress2 *= 100; Progress2 /= FullSize;
                            char FileMsg[100], ProgressMsg1[100], ProgressMsg2[100];
                            FSF.sprintf(FileMsg,GetMsg(MAdd), MyDestFile);
                            FSF.sprintf(ProgressMsg1,GetMsg(MExtr1), Progress1);
                            FSF.sprintf(ProgressMsg2,GetMsg(MExtr2), Progress2);                                                                                        
                            const char *MsgItems[]={GetMsg(MAdding),FileMsg, ProgressMsg1, ProgressMsg2};
                            Info.Message(Info.ModuleNumber,WaitMessage ? FMSG_LEFTALIGN|FMSG_KEEPBACKGROUND:FMSG_LEFTALIGN,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
                            WaitMessage=TRUE;
                        }       
                    }
                }
            }
            
    } else {
        if (FF.GetCanPutFile(glbFmt, glbPlugin)) {
            if (!FF.GetFAT(glbPlugin, glbFmt, RipFileName, &FAT)) {
                Free_putfiles();
                return FALSE;
            }

            for (int I = 0; I < putfiles.size(); I++){                

                char *d = ExtractPath(putfiles[I].FileName);
                if (strlen(d)>0)
                    FSF.sprintf(MyDestPath, "%s\\%s", FarCurDir, d);                    
                else strcpy(MyDestPath, FarCurDir);
                my_free(d);
            
                d = ExtractName(putfiles[I].FileName);
                FSF.sprintf(MyDestFile, "%s", d);
                my_free(d);
                
                FSF.sprintf(c, "%s\\%s", MyDestPath, MyDestFile);                

                if (putfiles[I].FileName[strlen(putfiles[I].FileName)-1]=='\\'){
                    FI.Description = my_strdup(putfiles[I].Description);
                    FI.FileName = my_strdup(c);
                    FI.FileSize = 0;
                    FI.Plugin   = my_strdup(putfiles[I].Plugin);
                    FI.StartOffset = 0;
                    FI.Fmt         = -1;
                    FI.UserData    = NULL;
                    FI.UserDataLen = 0;
                    FI.IsExtension = FALSE;
                    files.push_back(FI);
                    continue;
                }
                
                FileInfiles = FileExist(c, putfiles[I].FileSize);
                if (FileInfiles>-1) {
                    if (Owervrite==3) continue;
                    if (Owervrite == 0||Owervrite == 1)
                        if (!OpMode & OPM_SILENT) Owervrite = OverwritePrompt(MyDestFile);
                    if (Owervrite==-1) break;
                    if (Owervrite==3||Owervrite==1) continue;
                    if (Owervrite==0||Owervrite==2) {
                        if (FF.GetCanDelFile(putfiles[I].Fmt, putfiles[I].Plugin)){
                            if (!FF.DelFile(putfiles[I].Plugin, putfiles[I].Fmt, RipFileName, putfiles[I].FileName, putfiles[I].StartOffset, putfiles[I].FileSize, putfiles[I].UnpSize, &FAT)){
                                if (DelError()==0) continue;
                                else break;
                            } else UseDelete = TRUE;
                        } else {
                            if (DelIsNotSupportedByPlugin()==0) continue;
                            else break;
                        }
                    }
                }
                    
                if (strlen(MyDestPath)>0) strcpy(MyDestPath, MyDestPath+1);

                d = ExtractPath(putfiles[I].FileName);
                if (strlen(d)>0)
                    FSF.sprintf(MySrcPath2, "%s\\%s", MySrcPath, d);
                else FSF.sprintf(MySrcPath2, "%s", MySrcPath);                
                my_free(d);

                if (FF.PutFile(putfiles[I].Plugin, putfiles[I].Fmt, RipFileName, MyDestFile, MyDestPath, MySrcPath2, &putfiles[I].StartOffset, &putfiles[I].FileSize, &putfiles[I].UnpSize, &FAT, ConvView)){
                    ProcessedBytes += putfiles[I].FileSize;
                    WaitMessage=FALSE;
                    if (!OpMode & OPM_SILENT){                        
                        if (GetTickCount()-StartTime>500){
                            if (CheckForKey(VK_ESCAPE)) break;
                            Progress1 = 100.0;
                            Progress2 = ProcessedBytes; Progress2 *= 100; Progress2 /= FullSize;
                            char FileMsg[100], ProgressMsg1[100], ProgressMsg2[100];
                            FSF.sprintf(FileMsg,GetMsg(MAdd), MyDestFile);
                            FSF.sprintf(ProgressMsg1,GetMsg(MExtr1), Progress1);
                            FSF.sprintf(ProgressMsg2,GetMsg(MExtr2), Progress2);                                                                                        
                            const char *MsgItems[]={GetMsg(MAdding),FileMsg, ProgressMsg1, ProgressMsg2};
                            Info.Message(Info.ModuleNumber,WaitMessage ? FMSG_LEFTALIGN|FMSG_KEEPBACKGROUND:FMSG_LEFTALIGN,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
                            WaitMessage=TRUE;
                        }       
                    }
                    FI.Description = my_strdup(putfiles[I].Description);
                    FI.FileName = my_strdup(c);
                    FI.FileSize = putfiles[I].FileSize;
                    FI.Plugin   = my_strdup(putfiles[I].Plugin);
                    FI.StartOffset = putfiles[I].StartOffset;
                    FI.Fmt         = putfiles[I].Fmt;
                    FI.UserData    = NULL;
                    FI.UserDataLen = 0;
                    FI.IsExtension = FALSE;
                    files.push_back(FI);
                    continue;
                }            
            }
            FSF.sprintf(TmpFile, "%s%s", RipFileName, "~");
            if (!OpMode & OPM_SILENT){
                const char *MsgItems[]={GetMsg(MRecompile), GetMsg(MRecompileMsg)};
                Info.Message(Info.ModuleNumber,0,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
            }
            if (FF.FreeFAT(glbPlugin, glbFmt, RipFileName, TmpFile, &FAT)) {                
                CopyFile(TmpFile, RipFileName, FALSE);
                DeleteFile(TmpFile);
            }
        }
    }    
    if (!UseDelete) {
        Free_putfiles();
        Update_shownfiles(OpMode & OPM_SILENT);
    } else {
        FAT.DelItem(FAT.Root);
        Free_shownfiles();
        Free_copyfiles();
        Free_dirs();
    }*/

    return TRUE;
}

int MrRipper::DelFiles(struct PluginPanelItem *PanelItem, int ItemsNumber, int OpMode)
{    
    if (bPluginManager) return FALSE;

    TSaveScreen SS;
    DWORD StartTime=GetTickCount();
    BOOL WaitMessage=FALSE;
    double Progress1, Progress2;
    long FullSize, ProcessedBytes;
    int Owervrite = 0;
    void * FAT = NULL;    
    char TmpFile[MAX_PATH];

        //Dummy removing
        
        MakeCopyList(PanelItem, ItemsNumber);

        for (rip_vec::iterator I = copyfiles.begin(); I != copyfiles.end();I++)
                if (I->FileName[strlen(I->FileName)-1]=='\\'){            
                        continue;
                } else {                
                        if (I->Fmt == FMT_DUMMY){
                                for (rip_vec::iterator z=dummyfiles.begin(); z!=dummyfiles.end(); z++){
                                        if ((I->FileSize == z->FileSize)&&
                                                (I->StartOffset == z->StartOffset)) {
                                                        my_free(z->Description);
                                                        my_free(z->FileName);
                                                        my_free(z->Plugin);
                                                        dummyfiles.erase(z);
                                                        if (z==dummyfiles.end()) break;                                                 
                                        }
                                }
                        }        
        }    

        //Real deleting
    if (glbFmt == -1) return FALSE; //Do delete only if RIP-module support it

    FormatFinder FF(PluginPath);        
    FF.WorkFileSize = WorkFileSize;
    for (int i = 0; i<FF.GetFormatCount(); i++)             
        for (int j = 0; j<FF.GetnKnownFormats(i); j++)
            FF.SetActiveOpt(i, j, GetRegKey(HKEY_CURRENT_USER,"",FF.GetRegPath(i, j), 1));

    MakeCopyList(PanelItem, ItemsNumber);       
    
    FullSize = 0;
    
    for (i = 0; i < copyfiles.size(); i++)
        FullSize += copyfiles[i].FileSize;
                        
    ProcessedBytes = 0;                                                     
    
    if (FF.GetCanDelFile(glbFmt, glbPlugin)) {
        if (!FF.GetFAT(glbPlugin, glbFmt, RipFileName, &FAT)) {
            Free_copyfiles();
            return FALSE;
        }
        if (!ConfirmDelete()) {
            Free_copyfiles();
            return FALSE;
        }
        for (rip_vec::iterator I = copyfiles.begin(); I != copyfiles.end();I++){        
            if (I->FileName[strlen(I->FileName)-1]=='\\'){            
                continue;
            } else {                
                if (FF.DelFile(I->Plugin, I->Fmt, RipFileName, I->FileName, I->StartOffset, I->FileSize, I->UnpSize, &FAT)){
                    ProcessedBytes += I->FileSize;
                    WaitMessage=FALSE;
                    if (!OpMode & OPM_SILENT){                        
                        if (GetTickCount()-StartTime>500){
                            if (CheckForKey(VK_ESCAPE)) break;
                            Progress1 = 100.0;
                            Progress2 = ProcessedBytes; Progress2 *= 100; Progress2 /= FullSize;
                            char FileMsg[100], ProgressMsg1[100], ProgressMsg2[100];
                            FSF.sprintf(FileMsg,GetMsg(MDeleting), I->FileName);
                            FSF.sprintf(ProgressMsg1,GetMsg(MExtr1), Progress1);
                            FSF.sprintf(ProgressMsg2,GetMsg(MExtr2), Progress2);                                                                                        
                            const char *MsgItems[]={GetMsg(MExtraction),FileMsg, ProgressMsg1, ProgressMsg2};
                            Info.Message(Info.ModuleNumber,WaitMessage ? FMSG_LEFTALIGN|FMSG_KEEPBACKGROUND:FMSG_LEFTALIGN,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
                            WaitMessage=TRUE;
                        }       
                    }                   
                } else {
                    if (DelError()==0) continue;
                    else break;
                }
            }
        }
    } else {
        DelIsNotSupportedByPlugin2();
    }

    FSF.sprintf(TmpFile, "%s%s", RipFileName, "~");
    if (!OpMode & OPM_SILENT){
        const char *MsgItems[]={GetMsg(MRecompile), GetMsg(MRecompileMsg)};
        Info.Message(Info.ModuleNumber,0,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
    }
    if (FF.FreeFAT(glbPlugin, glbFmt, RipFileName, TmpFile, &FAT)) {                
        CopyFile(TmpFile, RipFileName, FALSE);
        DeleteFile(TmpFile);
    }
    
    this->FAT.DelItem(this->FAT.Root);
    Free_shownfiles();
    Free_copyfiles();
    Free_dirs();
    
    return TRUE;
}