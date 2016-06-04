BOOL MrRipper::AddDummy()
{
    if (bPluginManager) return FALSE;

    RIPPER_FILE_INFO FI;
    if (AddCustomFile(WorkFileSize)){           
        FI.FileName = my_strdup(Opt.DummyName);         
        for (int i=0; i<strlen(FI.FileName);i++)
            if (FI.FileName[i]=='/') FI.FileName[i] ='\\';                        

        if (FI.FileName[strlen(FI.FileName)-1]=='\\') {
            my_free(FI.FileName);
            return FALSE;
        }

        char * t = ExtractName(FI.FileName);
        my_free(FI.FileName);
        FI.FileName = my_strdup("\\",strlen(t));
        strcat(FI.FileName, t);                         
        my_free(t);

        FI.StartOffset = Opt.DummyOffset;
        FI.FileSize    = Opt.DummySize;
        FI.UnpSize     = Opt.DummySize;
        FI.Description = my_strdup(Opt.DummyDesc);
        FI.Plugin      = my_strdup("dummy");
        FI.Fmt         = FMT_DUMMY;
        FI.UserData.Data = NULL;
        FI.UserData.Size = 0;
        FI.IsExtension = FALSE;
        FI.IsDirectory = FALSE;
        dummyfiles.push_back(FI);            
                
        SetRegKey(HKEY_CURRENT_USER,"",StrDummyName,  Opt.DummyName);
        SetRegKey(HKEY_CURRENT_USER,"",StrDummyDesc,  Opt.DummyDesc);
        SetRegKey(HKEY_CURRENT_USER,"",StrDummyOffset,Opt.DummyOffset);
        SetRegKey(HKEY_CURRENT_USER,"",StrDummySize,  Opt.DummySize);
        SetRegKey(HKEY_CURRENT_USER,"",StrDummyEnd,   Opt.DummyEnd);

        return TRUE;
    }
    return FALSE;
}

BOOL MrRipper::AddDummySilent(char * Name, char * Desc, ULONG Offset, ULONG Size)
{
    if (bPluginManager) return FALSE;

    RIPPER_FILE_INFO FI;

    FI.FileName = my_strdup(Name);          

    for (int i=0; i<strlen(FI.FileName);i++)
        if (FI.FileName[i]=='/') FI.FileName[i] ='\\';                        

    if (FI.FileName[strlen(FI.FileName)-1]=='\\') {
        my_free(FI.FileName);
        return FALSE;
    }

    char * t = ExtractName(FI.FileName);
    my_free(FI.FileName);
    FI.FileName = my_strdup("\\",strlen(t));
    strcat(FI.FileName, t);                         
    my_free(t);

    FI.StartOffset = Offset;
    FI.FileSize    = Size;
    FI.UnpSize     = Size;
    FI.Description = my_strdup(Desc);
    FI.Plugin      = my_strdup("dummy");
    FI.Fmt         = FMT_DUMMY;
    FI.UserData.Data = NULL;
    FI.UserData.Size = 0;
    FI.IsExtension = FALSE;
    FI.IsDirectory = FALSE;
    dummyfiles.push_back(FI);            
                
    return TRUE;    
}

BOOL MrRipper::SplitFile()
{
    if (bPluginManager) return FALSE;

    TSaveScreen SS;                 
    DWORD StartTime=GetTickCount();
    BOOL WaitMessage=FALSE; 
    double Progressf;        

    if (SplitFileDialog()){
        char  s[20];
        ULONG SplitCount = 0;
        ULONG fres2;

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

        ULONG fres = FindInFile(RipFileName, StartOffset, (UCHAR *)Opt.SplitString, Opt.SplitSize);
        if (fres == -1) return FALSE;

        while (TRUE) {          
            if (GetTickCount()-StartTime>500){
                if (CheckForKey(VK_ESCAPE)) break;
                Progressf = fres-StartOffset; Progressf *= 100; Progressf /= (EndOffset-StartOffset);
                char FileMsg[50], FoundMsg[50], ProgressMsg[50];
                char *pdest;
                pdest = strrchr(RipFileName, '\\');  
                pdest++;
                FSF.sprintf(FileMsg,GetMsg(MScanning), pdest);
                FSF.sprintf(FoundMsg,GetMsg(MFound), SplitCount);
                FSF.sprintf(ProgressMsg,"%3.0f %%   %10d/%10d", Progressf, fres, EndOffset);                        
                const char *MsgItems[]={GetMsg(MScan),FileMsg, FoundMsg, ProgressMsg};
                Info.Message(Info.ModuleNumber,WaitMessage ? FMSG_LEFTALIGN|FMSG_KEEPBACKGROUND:FMSG_LEFTALIGN,NULL,MsgItems,sizeof(MsgItems)/sizeof(MsgItems[0]),0);
                WaitMessage=TRUE;                
            }

            fres2 = fres;
            fres = FindInFile(RipFileName, fres, (UCHAR *)Opt.SplitString, Opt.SplitSize);
            if (fres == -1) fres = EndOffset;

            SplitCount++;

            if (SplitCount< 10)           FSF.sprintf(s, "s00000%d.%s", SplitCount, Opt.SplitExt);
            else if (SplitCount< 100)     FSF.sprintf(s, "s0000%d.%s",  SplitCount, Opt.SplitExt);
            else if (SplitCount< 1000)    FSF.sprintf(s, "s000%d.%s",   SplitCount, Opt.SplitExt);
            else if (SplitCount< 10000)   FSF.sprintf(s, "s00%d.%s",    SplitCount, Opt.SplitExt);
            else if (SplitCount< 100000)  FSF.sprintf(s, "s0%d.%s",     SplitCount, Opt.SplitExt);
            else if (SplitCount< 1000000) FSF.sprintf(s, "s%d.%s",      SplitCount, Opt.SplitExt);
            else                          FSF.sprintf(s, "%d.%s",       SplitCount, Opt.SplitExt);

            if (fres != EndOffset) {
                if (Opt.SplitOffset > 0)
                    AddDummySilent(s, "Split", fres2+Opt.SplitOffset-Opt.SplitSize, (fres-Opt.SplitSize)-(fres2+Opt.SplitOffset-Opt.SplitSize));
                else AddDummySilent(s, "Split", fres2+Opt.SplitOffset-Opt.SplitSize, (fres+Opt.SplitOffset-Opt.SplitSize)-(fres2+Opt.SplitOffset-Opt.SplitSize));
            } else   AddDummySilent(s, "Split", fres2+Opt.SplitOffset-Opt.SplitSize, fres-(fres2+Opt.SplitOffset-Opt.SplitSize));                   
            if (fres >= EndOffset) break;
        }
    }
    return TRUE;
}