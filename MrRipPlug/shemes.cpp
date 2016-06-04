bool LessPreset(const PresetsSort& a, const PresetsSort& b)
{
        return (lstrcmp(a.Preset, b.Preset) < 0);
}

BOOL MrRipper::ShemeOverwrite(char* ShemeName)
{
  TSaveScreen SS;

    const char *MsgItems[]={GetMsg(MNewPreset),GetMsg(MOverwritePreset), ShemeName, ""};
    int ExitCode=Info.Message(Info.ModuleNumber,FMSG_WARNING|FMSG_MB_YESNO,"OverwritePreset",MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),2);    
    return ExitCode == 0;
}

BOOL MrRipper::SavePlugins()
{
    if (bPluginManager) {
        ADDPLUGININFO *PI = NULL;
        DWORD D;
        PanelInfo PInfo;
        Info.Control(INVALID_HANDLE_VALUE,FCTL_GETPANELINFO,&PInfo);
        
        for (int I=0;I<PInfo.ItemsNumber;I++){            
            UData *UD = (UData*)PInfo.PanelItems[I].UserData;
            if (UD)
                PI = (ADDPLUGININFO*)UD->Data;            
            if (!PI) continue;
            if (PInfo.PanelItems[I].Flags&PPIF_SELECTED){
                D = 1;
                SetRegKey(HKEY_CURRENT_USER,"",PI->RegPath, D);
            }else {
                D = 0;
                SetRegKey(HKEY_CURRENT_USER,"",PI->RegPath, D);
            }
        }
    }    
    return TRUE;
}

BOOL MrRipper::CreateSheme()
{
    char Dst[100];
    TSaveScreen SS;                 
    DWORD StartTime=GetTickCount();
    BOOL WaitMessage=FALSE; 
    double Progressf;
    char FileName[MAX_PATH];

    if (Info.InputBox(GetMsg(MNewPreset), GetMsg(MNewPresetCapt), NULL, NULL, Dst, 100, "CrPresets", FIB_BUTTONS)){
        
        FSF.sprintf(FileName, "%s\\PRESETS\\%s.rpr", PluginPath, Dst);         
        
        FILE * f = fopen (FileName, "rt");
        if (f != NULL) {
            if (ShemeOverwrite(Dst)) fclose(f);
            else {
                fclose(f);
                return FALSE;
            }
        }

        f = fopen (FileName, "w+t");
        if (!f) {
            const char *MsgItems[]={GetMsg(MNewPreset),"Error Saving Preset"};
            Info.Message(Info.ModuleNumber,FMSG_KEEPBACKGROUND|FMSG_MB_OK,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
            return FALSE;
        }        

        PanelInfo PInfo;
        Info.Control(INVALID_HANDLE_VALUE,FCTL_GETPANELINFO,&PInfo);
        
        for (int I=1;I<PInfo.ItemsNumber;I++){            
            if (GetTickCount()-StartTime>500){
                if (CheckForKey(VK_ESCAPE)) break;
                Progressf = I; Progressf *= 100; Progressf /= PInfo.ItemsNumber;
                char ProgressMsg[5];
                FSF.sprintf(ProgressMsg,"%3.0f %%", Progressf);         
                const char *MsgItems[]={GetMsg(MListSaving),ProgressMsg};
                Info.Message(Info.ModuleNumber,WaitMessage ? FMSG_KEEPBACKGROUND:0,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
                WaitMessage=TRUE;
                StartTime=GetTickCount();
            }            
            if (PInfo.PanelItems[I].Flags&PPIF_SELECTED)
                fprintf(f, "+");        
            else fprintf(f, "-");            
            fprintf(f, "%s\n", PInfo.PanelItems[I].FindData.cFileName);
        }
            
        fclose(f);      
    
        return TRUE;
    }
    return TRUE;
}

BOOL MrRipper::LoadSheme()
{
    _finddata_t c_file;
    long hFile;
    char path[MAX_PATH];
    struct FarMenuItem *MenuItems;
    vector<PresetsSort> vc;
    PresetsSort ps;
    char *c;
    double Progressf;
    char s[300];
    TSaveScreen SS;                 
    DWORD StartTime=GetTickCount();
    BOOL WaitMessage=FALSE; 

    FSF.sprintf(path, "%s\\PRESETS\\*.rpr", PluginPath);

    if( (hFile = _findfirst(path, &c_file )) != -1L ) {              
        c = my_strdup(c_file.name);        
        c[lstrlen(c)-4]=0;
        ps .Preset = c;
        vc.push_back(ps);
        
        while( _findnext(hFile, &c_file ) == 0 ){
            c = my_strdup(c_file.name);        
            c[lstrlen(c)-4]=0;
            ps .Preset = c;
            vc.push_back(ps);            
        }
    }

    sort(vc.begin(), vc.end(), LessPreset);
    
    MenuItems = (FarMenuItem *)my_malloc(sizeof(FarMenuItem)*(vc.size()+10));

    for (int i = 0; i<vc.size()+10; i++){
        memset(&MenuItems[i],0,sizeof(FarMenuItem));
    }

    lstrcpy(s, "&1 ");lstrcat(s, GetMsg(MInfoTypeGraph));
    lstrcpy(MenuItems[0].Text, s);
    lstrcpy(s, "&2 ");lstrcat(s, GetMsg(MInfoTypeVideo));
    lstrcpy(MenuItems[1].Text, s);
    lstrcpy(s, "&3 ");lstrcat(s, GetMsg(MInfoTypeSound));
    lstrcpy(MenuItems[2].Text, s);
    lstrcpy(s, "&4 ");lstrcat(s, GetMsg(MInfoTypePack));
    lstrcpy(MenuItems[3].Text, s);
    lstrcpy(s, "&5 ");lstrcat(s, GetMsg(MInfoTypeOther));
    lstrcpy(MenuItems[4].Text, s);
    lstrcpy(s, "&6 ");lstrcat(s, GetMsg(MInfoTypeMusic));
    lstrcpy(MenuItems[5].Text, s);
    lstrcpy(s, "&7 ");lstrcat(s, GetMsg(MInfoTypeText));
    lstrcpy(MenuItems[6].Text, s);
    lstrcpy(s, "&8 ");lstrcat(s, GetMsg(MInfoTypeExe));
    lstrcpy(MenuItems[7].Text, s);
    lstrcpy(s, "&0 ");lstrcat(s, GetMsg(MDefaultPreset));
    lstrcpy(MenuItems[8].Text, s);
    MenuItems[9].Separator = TRUE;
    
    for (i = 10; i<vc.size()+10; i++)        
        lstrcpy(MenuItems[i].Text, vc[i-10].Preset);    
         
    int BreakCode;
    
    int BreakKeys[]={VK_RETURN,                         
                        MAKELONG(VK_RETURN, PKF_CONTROL), 
                        MAKELONG(VK_RETURN, PKF_SHIFT),
                        VK_DELETE,
                        0};


    int ExitCode=Info.Menu(Info.ModuleNumber,-1,-1,0,FMENU_WRAPMODE|FMENU_AUTOHIGHLIGHT,
                           GetMsg(MLoadPreset), NULL,"Presets", BreakKeys,&BreakCode,
                           MenuItems,vc.size()+10);
    
    if (BreakCode == -1) BreakCode = 0;
    if ((ExitCode > 9)&&(BreakCode == 3)) {
        for (i=0; i<vc.size();i++)
            my_free(vc[i].Preset);
        my_free(MenuItems);

        FSF.sprintf(path, "%s\\PRESETS\\%s.rpr", PluginPath, MenuItems[ExitCode].Text);    

        DeleteFile(path);       

        return LoadSheme();
    }

    bool OldSet;
    if ((ExitCode >= 0)&&(ExitCode < 9)) {
        ADDPLUGININFO *PI;

        PanelInfo PInfo;        
        Info.Control(INVALID_HANDLE_VALUE,FCTL_GETPANELINFO,&PInfo);        

        switch (ExitCode){
        case RIP_INFO_TYPE_GRAPH:            
            EnableGraph = !EnableGraph;            
            break;
        case RIP_INFO_TYPE_VIDEO:
            EnableVideo = !EnableVideo;
            break;
        case RIP_INFO_TYPE_SOUND:
            EnableSound = !EnableSound;
            break;
        case RIP_INFO_TYPE_PACK:
            EnablePack = !EnablePack;
            break;
        case RIP_INFO_TYPE_OTHER:
            EnableOther = !EnableOther;
            break;
        case RIP_INFO_TYPE_MUSIC:
            EnableMusic = !EnableMusic;
            break;
        case RIP_INFO_TYPE_TEXT:
            EnableText = !EnableText;
            break;
        case RIP_INFO_TYPE_EXE:
            EnableExe = !EnableExe;
            break;
        case 8:
            EnableGraph = Opt.EnableGraph;
            EnableVideo = Opt.EnableVideo;
            EnableSound = Opt.EnableSound;
            EnableMusic = Opt.EnableMusic;
            EnableText  = Opt.EnableText;
            EnableExe   = Opt.EnableExe;
            EnablePack  = Opt.EnablePack;
            EnableOther = Opt.EnableOther;            
            break;
        }
        
        for (int I=1;I<PInfo.ItemsNumber;I++){
            UData *UD = (UData*)PInfo.PanelItems[I].UserData;
            if (UD)
                PI = (ADDPLUGININFO*)UD->Data;
            if (!PI) continue;

            OldSet = PInfo.PanelItems[I].Flags&PPIF_SELECTED;
            switch (PI->Type){
            
            case RIP_INFO_TYPE_GRAPH: 
                if (ExitCode==RIP_INFO_TYPE_GRAPH||ExitCode==8){
                    if (BreakCode == 0){
                        if (EnableGraph)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }
                    if (BreakCode == 1)
                        PInfo.PanelItems[I].Flags |= PPIF_SELECTED;                        
                    if (BreakCode == 2){
                        if (OldSet^1)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }
                }
                break;
            case RIP_INFO_TYPE_VIDEO:
                if (ExitCode==RIP_INFO_TYPE_VIDEO||ExitCode==8){
                    if (BreakCode == 0){
                        if (EnableVideo)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }
                    if (BreakCode == 1)
                        PInfo.PanelItems[I].Flags |= PPIF_SELECTED;                        
                    if (BreakCode == 2){
                        if (OldSet^1)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }                    
                }
                break;
            case RIP_INFO_TYPE_SOUND:
                if (ExitCode==RIP_INFO_TYPE_SOUND||ExitCode==8){
                    if (BreakCode == 0){
                        if (EnableSound)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }
                    if (BreakCode == 1)
                        PInfo.PanelItems[I].Flags |= PPIF_SELECTED;                        
                    if (BreakCode == 2){
                        if (OldSet^1)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }                    
                }
                break;
            case RIP_INFO_TYPE_PACK:
                if (ExitCode==RIP_INFO_TYPE_PACK||ExitCode==8){
                    if (BreakCode == 0){
                        if (EnablePack)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }
                    if (BreakCode == 1)
                        PInfo.PanelItems[I].Flags |= PPIF_SELECTED;                        
                    if (BreakCode == 2){
                        if (OldSet^1)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }                    
                }
                break;
            case RIP_INFO_TYPE_OTHER:
                if (ExitCode==RIP_INFO_TYPE_OTHER||ExitCode==8){
                    if (BreakCode == 0){
                        if (EnableOther)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }
                    if (BreakCode == 1)
                        PInfo.PanelItems[I].Flags |= PPIF_SELECTED;                        
                    if (BreakCode == 2){
                        if (OldSet^1)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }                    
                }
                break;
            case RIP_INFO_TYPE_MUSIC:
                if (ExitCode==RIP_INFO_TYPE_MUSIC||ExitCode==8){
                    if (BreakCode == 0){
                        if (EnableMusic)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }
                    if (BreakCode == 1)
                        PInfo.PanelItems[I].Flags |= PPIF_SELECTED;                        
                    if (BreakCode == 2){
                        if (OldSet^1)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }                    
                }
                break;
            case RIP_INFO_TYPE_TEXT:
                if (ExitCode==RIP_INFO_TYPE_TEXT||ExitCode==8){
                    if (BreakCode == 0){
                        if (EnableText)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }
                    if (BreakCode == 1)
                        PInfo.PanelItems[I].Flags |= PPIF_SELECTED;                        
                    if (BreakCode == 2){
                        if (OldSet^1)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }                    
                }
                break;
            case RIP_INFO_TYPE_EXE:
                if (ExitCode==RIP_INFO_TYPE_EXE||ExitCode==8){
                    if (BreakCode == 0){
                        if (EnableExe)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }
                    if (BreakCode == 1)
                        PInfo.PanelItems[I].Flags |= PPIF_SELECTED;                        
                    if (BreakCode == 2){
                        if (OldSet^1)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }                    
                }
                break;
            }
        }       
        
        Info.Control(INVALID_HANDLE_VALUE,FCTL_SETSELECTION,&PInfo);        
    }

    if (ExitCode > 9) {
            
        FSF.sprintf(path, "%s\\PRESETS\\%s.rpr", PluginPath, MenuItems[ExitCode].Text);    

        FILE * f = fopen (path, "rt");
        if (!f) {
            for (int i=0; i<vc.size();i++)
                my_free(vc[i].Preset);
            my_free(MenuItems);
            return FALSE;
        }

        struct _stat buf;
        _stat(path, &buf);
        long ListFileSize = buf.st_size;

        bool Active;
        char c[300];

        PanelInfo PInfo;
        Info.Control(INVALID_HANDLE_VALUE,FCTL_GETPANELINFO,&PInfo);
    
        while (TRUE) {                  
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
    
            fgets(s, 300, f);
            Active = s[0]=='+';
            lstrcpy(c, s+1);
            c[lstrlen(c)-1] = 0;
            
            for (int I=1;I<PInfo.ItemsNumber;I++){
                if (lstrcmp(PInfo.PanelItems[I].FindData.cFileName, c)== 0){
                    OldSet = PInfo.PanelItems[I].Flags&PPIF_SELECTED;
                    if (BreakCode == 0){
                        if (OldSet&Active)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }
                    if (BreakCode == 1){
                        if (OldSet|Active)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }                        
                    if (BreakCode == 2){
                        if (OldSet^Active)
                             PInfo.PanelItems[I].Flags |= PPIF_SELECTED;
                        else PInfo.PanelItems[I].Flags = (PInfo.PanelItems[I].Flags|PPIF_SELECTED)^PPIF_SELECTED;
                    }                        
                }
            }                
        
            if (feof(f)) break;
        }
        fclose(f);
        Info.Control(INVALID_HANDLE_VALUE,FCTL_SETSELECTION,&PInfo);        
    }    

    for (i=0; i<vc.size();i++)
        my_free(vc[i].Preset);
    my_free(MenuItems);
    if (ExitCode != -1) return TRUE;
    else return FALSE;
}