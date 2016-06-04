BOOL MrRipper::TryToLoadList(BOOL Silence)
{
    if (bPluginManager) return FALSE;

    TSaveScreen SS;                 
    DWORD StartTime=GetTickCount();
    BOOL WaitMessage=FALSE; 
    double Progressf;
    char FileName[MAX_PATH];
    char s[MAX_PATH];
    
    RIPPER_FILE_INFO FI;
    RIPPER_DIR_INFO DI;

    SVFIP SFI, D;

    memset(&FI, 0, sizeof(RIPPER_FILE_INFO));
    
    char * t = ExtractName(RipFileName);                
    FSF.sprintf(FileName, "%s\\LISTS\\%s.%x", PluginPath, t, WorkFileSize); 
    my_free(t);     

    struct _stat buf;
    _stat(FileName, &buf);
    long ListFileSize = buf.st_size;

    FILE * f = fopen (FileName, "rt");
    if (!f) return FALSE;
    
    fscanf(f, "%d\n", &glbFmt);
    fgets(glbPlugin, 300, f);       
    glbPlugin[strlen(glbPlugin)-1] = 0;
    fscanf(f, "%d\n", &glbPackOffset);
    fscanf(f, "%d\n", &glbPackSize);
    fscanf(f, "%d\n", &glbAllFiles);
    fscanf(f, "%d\n", &glbAllSize);
                
    Free_dummyfiles();
    while (TRUE) {
        if (!Silence){          
            if (GetTickCount()-StartTime>500){
                if (CheckForKey(VK_ESCAPE)) break;
                Progressf = ftell(f); Progressf *= 100; Progressf /= ListFileSize;
                char ProgressMsg[5];
                FSF.sprintf(ProgressMsg,"%3.0f %%", Progressf);                         
                const char *MsgItems[]={GetMsg(MListLoading),ProgressMsg};
                Info.Message(Info.ModuleNumber,WaitMessage ? FMSG_KEEPBACKGROUND:0,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
                WaitMessage=TRUE;
                StartTime = GetTickCount();
            }       
        }

        fgets(s, MAX_PATH, f);
        s[strlen(s)-1] = 0;
        FI.FileName = my_strdup(s);
        fscanf(f, "%d\n", &FI.StartOffset);
        fscanf(f, "%d\n", &FI.FileSize);
        fscanf(f, "%d\n", &FI.UnpSize);
                                
        fgets(s, MAX_PATH, f);
        s[lstrlen(s)-1] = 0;
        if (lstrcmp(s, "-")==0)
             FI.Description   = my_strdup("");
        else FI.Description   = my_strdup(s);         
        fgets(s, MAX_PATH, f);
        s[lstrlen(s)-1] = 0;
        if (lstrcmp(s, "-")==0)
             FI.Plugin   = my_strdup("");
        else FI.Plugin   = my_strdup(s);
        fscanf(f, "%d\n", &FI.Fmt);             
        fscanf(f, "%d\n", &FI.IsExtension);
        fscanf(f, "%d\n", &FI.IsDirectory);        
                
        if (FI.Fmt == FMT_DUMMY) dummyfiles.push_back(FI);
        else {

            SFI = (SVFIP)my_malloc(sizeof(SVFI));                            

            if (FI.IsDirectory) {
                char *d = my_strdup(FI.FileName, 2);
                lstrcat(d, "\\-");
                FI.Fmt = FMT_SPEC;
                my_free(FI.FileName);
                FI.FileName = d;
            }
            
            SFI->FileName      = ExtractName(FI.FileName);
            SFI->Description   = my_strdup(FI.Description);
            SFI->Plugin        = my_strdup(FI.Plugin);
            SFI->IsDirectory   = FALSE;
            SFI->Fmt           = FI.Fmt;
            SFI->IsExtension   = FALSE;
            SFI->Offset        = FI.StartOffset;
            SFI->LevelUp       = NULL;
            SFI->PackedSize    = FI.FileSize;
            SFI->UnpackedSize  = FI.UnpSize;
            SFI->UserData.Data = FI.UserData.Data;
            SFI->UserData.Size = FI.UserData.Size;

            BOOL dup = FALSE;

            char *p = ExtractPath(FI.FileName);
            for (int i=0; i < dirs.size();i++) {
                if (lstrcmp(dirs[i].DirName, p)==0) {
                    D = FAT.InsertItem(dirs[i].Item, SFI);
                    my_free(p);
                    dup = TRUE;
                    break;
                }
            } 
            if (!dup) {
                D = FAT.InsertItem(p, SFI);
                if (!D){
                    my_free(SFI->FileName);
                    my_free(SFI->Description);
                    my_free(SFI->Plugin);
                    if (SFI->UserData.Data)
                        my_free(SFI->UserData.Data);
                    my_free(SFI);
                } else {
                    DI.DirName  = p;
                    DI.Item     = D->LevelUp;
                    dirs.push_back(DI);
                }
            }
        }

        if (feof(f)) break;
    }

    fclose(f);
    return TRUE;
}

BOOL SaveLst(FILE * &f, SVFIP &Item, char * Dir)
{
    char c[256];    
    lstrcpy(c, Dir);
    if (lstrlen(Dir)!=0) {
        lstrcat(c, "\\");
    }
    if (lstrcmp(Item->FileName, "\\")!=0)
        lstrcat(c, Item->FileName);
    if (Item->IsDirectory){
        for (int i = 0; i < Item->Items.size(); i++)
            SaveLst(f, Item->Items[i], c);
    }

    if (lstrcmp(Item->FileName, "\\")!=0){
        if (Item->IsDirectory)
             fprintf(f, "%s\\\n", c);
        else fprintf(f, "%s\n", c); 

        fprintf(f, "%d\n", Item->Offset);
        fprintf(f, "%d\n", Item->PackedSize);
        fprintf(f, "%d\n", Item->UnpackedSize);
        if (lstrlen(Item->Description)==0)
             fprintf(f, "%s\n", "-");
        else fprintf(f, "%s\n", Item->Description);        
        if (lstrlen(Item->Plugin)==0)
             fprintf(f, "%s\n", "-");
        else fprintf(f, "%s\n", Item->Plugin);
        fprintf(f, "%d\n", Item->Fmt);
        fprintf(f, "%d\n", Item->IsExtension);
        fprintf(f, "%d\n", Item->IsDirectory);
    }
        
    return TRUE;
}

BOOL MrRipper::SaveList(BOOL Silence)
{
    if (bPluginManager) return FALSE;

    TSaveScreen SS;                 
    DWORD StartTime=GetTickCount();
    BOOL WaitMessage=FALSE;     
    char FileName[MAX_PATH];
    
    char * t = ExtractName(RipFileName);                
    FSF.sprintf(FileName, "%s\\LISTS\\%s.%x", PluginPath, t, WorkFileSize); 
    my_free(t);     
        
    FILE * f = fopen (FileName, "w+t");
    if (!f) {
        const char *MsgItems[]={GetMsg(MScan),"Error Saving"};
        if (!Silence) Info.Message(Info.ModuleNumber,WaitMessage ? FMSG_KEEPBACKGROUND|FMSG_MB_OK:FMSG_MB_OK,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
        return FALSE;
    }

    fprintf(f, "%d\n", glbFmt);
    fprintf(f, "%s\n", glbPlugin);  
    fprintf(f, "%d\n", glbPackOffset);
    fprintf(f, "%d\n", glbPackSize);
    fprintf(f, "%d\n", glbAllFiles);
    fprintf(f, "%d\n", glbAllSize); 

    SaveLst(f, FAT.Root, "");    

    for (int i=0; i<dummyfiles.size();i++){             
        fprintf(f, "%s\n", dummyfiles[i].FileName);
        fprintf(f, "%d\n", dummyfiles[i].StartOffset);
        fprintf(f, "%d\n", dummyfiles[i].FileSize);
        fprintf(f, "%d\n", dummyfiles[i].UnpSize);
        fprintf(f, "%s\n", dummyfiles[i].Description);
        fprintf(f, "%s\n", dummyfiles[i].Plugin);
        fprintf(f, "%d\n", dummyfiles[i].Fmt);
        fprintf(f, "%d\n", dummyfiles[i].IsExtension);
        fprintf(f, "%d\n", dummyfiles[i].IsDirectory);
    }       
        
    fclose(f);
    return TRUE;
}