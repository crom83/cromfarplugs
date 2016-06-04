bool LessName (const RIP_PLUGIN & a, const RIP_PLUGIN & b)
{
        return (strcmp(a.PluginPath, b.PluginPath) < 0);
}

FormatFinder::FormatFinder(char *PluginPath)
{       
    char temp[MAX_PATH];
    FSF.sprintf(temp, "%s\\FORMATS", PluginPath);    
        this->PluginPath = my_strdup(temp);
        ScanFormats();
}

FormatFinder::~FormatFinder()
{
    for (int i=0; i<PluginsNumber; i++){
        for (int j=0; j<Plugins[i].nKnownFormats; j++){            
            my_free(Plugins[i].KnownFormats[j].RegPath);
            my_free(Plugins[i].KnownFormats[j].OptionString); 
        }
        my_free(Plugins[i].PluginPath);            
        my_free(Plugins[i].KnownFormats);        
        my_free(Plugins[i].PluginCopyright);        
        FreeLibrary(Plugins[i].DLLHandle);
    }
    my_free(Plugins);
    my_free(PluginPath);
}

int FormatFinder::GetAllFormatCount()
{       
    int fmts = 0;
    for (int i=0; i<PluginsNumber;i++) fmts += Plugins[i].nKnownFormats;
        return fmts;
}

int FormatFinder::GetFormatCount()
{    
        return PluginsNumber;
}

int FormatFinder::GetnKnownFormats(int Plug)
{
    return Plugins[Plug].nKnownFormats;
}

void FormatFinder::GetPluginInfo(int Plug, short Fmt, RIP_PLUGIN_GETNFO &pinfo)
{    
    pinfo.Active          = Plugins[Plug].KnownFormats[Fmt].Active;
    pinfo.OptionString    = Plugins[Plug].KnownFormats[Fmt].OptionString;
    pinfo.RegPath         = Plugins[Plug].KnownFormats[Fmt].RegPath;
    pinfo.Type            = Plugins[Plug].KnownFormats[Fmt].Type;
    pinfo.PluginCopyright = Plugins[Plug].PluginCopyright;
    pinfo.PluginPath      = Plugins[Plug].PluginPath;
    pinfo.RipVer          = Plugins[Plug].KnownFormats[Fmt].RipVer;
    pinfo.Version         = Plugins[Plug].KnownFormats[Fmt].Version;
}

void FormatFinder::SetActiveOpt(int Plug, short Fmt, BOOL Active)
{
        Plugins[Plug].KnownFormats[Fmt].Active = Active;
}

BOOL FormatFinder::GetActiveOpt(int Plug, short Fmt)
{
        return Plugins[Plug].KnownFormats[Fmt].Active;
}

char *FormatFinder::GetOptionString(int Plug, short Fmt)
{
        return Plugins[Plug].KnownFormats[Fmt].OptionString;
}

char *FormatFinder::GetRegPath(int Plug, short Fmt)
{
        return Plugins[Plug].KnownFormats[Fmt].RegPath;
}

void FormatFinder::ScanFormats()
{
    _finddata_t c_file;
    long hFile;
    char path[MAX_PATH];
    char fmt[MAX_PATH];
    RIP_PLUGIN plg;
    vector<RIP_PLUGIN> tvec;

    PluginsNumber = 0;

    FSF.sprintf(path, "%s\\*.rip", PluginPath);    
    if( (hFile = _findfirst(path, &c_file )) != -1L )
    {           
        FSF.sprintf(fmt, "%s\\%s", PluginPath, c_file.name);        
        if (CheckPlugin(fmt, plg)) {                                            
            tvec.push_back(plg);
            PluginsNumber++;
        } 
        while( _findnext(hFile, &c_file ) == 0 )
        {
            FSF.sprintf(fmt, "%s\\%s", PluginPath, c_file.name);
            if (CheckPlugin(fmt, plg)) {
                tvec.push_back(plg);
                PluginsNumber++;                
            }
        }
    }
        
        sort(tvec.begin(), tvec.end(), LessName);

    Plugins = (RIP_PLUGIN*)my_malloc(sizeof(RIP_PLUGIN)*PluginsNumber);
    for (int i=0; i<tvec.size(); i++)
    {        
        Plugins[i].nKnownFormats   = tvec[i].nKnownFormats;
        Plugins[i].KnownFormats    = tvec[i].KnownFormats;
        Plugins[i].PluginCopyright = tvec[i].PluginCopyright;
        Plugins[i].PluginPath      = tvec[i].PluginPath;
        Plugins[i].DLLHandle       = tvec[i].DLLHandle;
        Plugins[i].formatcheck     = tvec[i].formatcheck;
        Plugins[i].packcheck       = tvec[i].packcheck;
        Plugins[i].packcheckfree   = tvec[i].packcheckfree;
        Plugins[i].getfile         = tvec[i].getfile;
        Plugins[i].getfileud       = tvec[i].getfileud;
        Plugins[i].plugininfo      = tvec[i].plugininfo;
        Plugins[i].plugininfofree  = tvec[i].plugininfofree;
        Plugins[i].getfat          = tvec[i].getfat;
        Plugins[i].freefat         = tvec[i].freefat;
        Plugins[i].putfile         = tvec[i].putfile;
        Plugins[i].delfile         = tvec[i].delfile;

    }
}

BOOL FormatFinder::CheckPlugin(char * Name, RIP_PLUGIN &plg)
{
    FORMATCHECK     RipCheckFmt_Ptr      = NULL;
    PACKCHECK       RipCheckPak_Ptr      = NULL;
    GETFILE         RipGetFile_Ptr       = NULL;
    GETFILEUD       RipGetFileUD_Ptr     = NULL;
    PACKCHECKFREE   RipCheckPakFree_Ptr  = NULL;
    PLUGININFO      RipPluginNfo_Ptr     = NULL;    
    PLUGININFOFREE  RipPluginNfoFree_Ptr = NULL;
    GETFAT          RipGetFAT_Ptr        = NULL;
    FREEFAT         RipFreeFAT_Ptr       = NULL;
    PUTFILE         RipPutFile_Ptr       = NULL;
    DELFILE         RipDelFile_Ptr       = NULL;

    HINSTANCE       dllHandle = NULL;                 
    RIP_NFO         nfo;
    RIP_SERVICE_FUNC   func;
    RIP_FLAGS       flags;

    dllHandle = LoadLibrary(Name);
    if (NULL != dllHandle) 
    { 
        RipCheckFmt_Ptr     = (FORMATCHECK)GetProcAddress(dllHandle, "FormatCheck");
        RipCheckPak_Ptr     = (PACKCHECK)GetProcAddress(dllHandle, "PackCheck");
        RipPluginNfo_Ptr    = (PLUGININFO)GetProcAddress(dllHandle, "PluginInfo");                
        RipCheckPakFree_Ptr = (PACKCHECKFREE)GetProcAddress(dllHandle, "PackCheckFree");
        RipGetFile_Ptr      = (GETFILE)GetProcAddress(dllHandle, "GetFile");                
        RipGetFileUD_Ptr    = (GETFILEUD)GetProcAddress(dllHandle, "GetFileUD");
        RipPluginNfoFree_Ptr= (PLUGININFOFREE)GetProcAddress(dllHandle, "PluginInfoFree");
        RipGetFAT_Ptr       = (GETFAT)GetProcAddress(dllHandle, "GetFAT");
        RipFreeFAT_Ptr      = (FREEFAT)GetProcAddress(dllHandle, "FreeFAT");
        RipPutFile_Ptr      = (PUTFILE)GetProcAddress(dllHandle, "PutFile");
        RipDelFile_Ptr      = (DELFILE)GetProcAddress(dllHandle, "DelFile");

        if (NULL != RipPluginNfo_Ptr)
        {                    
            FillServiceFunc(func);
            FillFlags(flags, TRUE);

            memset(&nfo, 0, sizeof nfo);                    
            RipPluginNfo_Ptr(&nfo, &func, &flags);

            plg.KnownFormats    = (RIP_PLUGIN_FMT*)my_malloc(sizeof(RIP_PLUGIN_FMT)*nfo.nKnownFormats);
            plg.nKnownFormats   = nfo.nKnownFormats;
            for (int i=0; i<nfo.nKnownFormats; i++){
                plg.KnownFormats[i].OptionString        = my_strdup(nfo.KnownFormats[i].OptionString);
                plg.KnownFormats[i].RegPath             = my_strdup(nfo.KnownFormats[i].RegPath);
                plg.KnownFormats[i].Type                = nfo.KnownFormats[i].Type;
                plg.KnownFormats[i].Version             = nfo.KnownFormats[i].Version;
                plg.KnownFormats[i].RipVer              = nfo.KnownFormats[i].RipperVer;
            }
            plg.PluginCopyright = my_strdup(nfo.Copyright);
            plg.PluginPath      = my_strdup(Name);
            RipPluginNfoFree_Ptr(&nfo, &func, &flags);
            plg.DLLHandle       = dllHandle;
            plg.formatcheck     = RipCheckFmt_Ptr;
            plg.packcheck       = RipCheckPak_Ptr;
            plg.packcheckfree   = RipCheckPakFree_Ptr;
            plg.getfile         = RipGetFile_Ptr;
            plg.getfileud       = RipGetFileUD_Ptr;
            plg.plugininfo      = RipPluginNfo_Ptr;
            plg.plugininfofree  = RipPluginNfoFree_Ptr;
            plg.getfat          = RipGetFAT_Ptr;
            plg.freefat         = RipFreeFAT_Ptr;
            plg.putfile         = RipPutFile_Ptr;
            plg.delfile         = RipDelFile_Ptr;
        } else return FALSE;             
    } else return FALSE;
        return TRUE;
}

void FormatFinder::FillServiceFunc(RIP_SERVICE_FUNC &func)
{
    memset(&func, 0, sizeof func);
    func.StructSize     = sizeof(RIP_SERVICE_FUNC);
    func.Find           = Find;
    func.FindInFile     = FindInFile;
    func.ReadFromFile   = ReadFromFile;
    func.SaveToFile     = SaveToFile;
    func.AppendToFile   = AppendToFile;
    func.FileToFileCopy = FileToFileCopy;
    func.SwapWord       = SwapWord;
    func.SwapLong       = SwapLong;
    func.ExtractPath    = ExtractPath;
    func.ExtractName    = ExtractName;
    func.FileInDir      = FileInDir;
    func.FreeMem        = my_free;
    func.ShowMessage    = MyShowMessage;
    func.Char2Ver       = Char2Ver;
    func.Ver2Char       = Ver2Char;
    func.CheckKey       = CheckForKey;
}

void FormatFinder::FillFlags(RIP_FLAGS &flags, BOOL Silence)
{
    memset(&flags, 0, sizeof flags);
    flags.StructSize    = sizeof(RIP_FLAGS);
    flags.Silent        = Silence;
    flags.RipperVer     = Char2Ver(RIPVER);    
}

long FormatFinder::FormatCheck(RIP_PLUGIN plg, int Type, short Fmt, char *FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, VirtualFAT &FAT, long &newItemsNumber, ripd_vec &dirs, long WorkFileSize, BOOL Silent, long &UnknownFilesCount)
{
    RIP_FORM_INFO       finfo;        
    RIP_SERVICE_FUNC    func;
    RIP_FLAGS           flags;

    RIPPER_DIR_INFO DI;
    SVFIP SFI, D = NULL;

    long r = -1;    
    long UnpSize = 0;     
    newItemsNumber = 0;
    
    if (NULL != plg.DLLHandle) 
    { 
        if (Type != RIP_INFO_TYPE_PACK){                    
            if (NULL != plg.formatcheck)
            {           
                memset(&finfo, 0, sizeof finfo);

                FillServiceFunc(func);
                FillFlags(flags, Silent);

                r = plg.formatcheck(Fmt, FileName, BaseString, readed, &HeadOffset, &SomeOffset, &FileSize, &FileOffset, &UnpSize, &finfo, WorkFileSize, &func, &flags);
                if (r != -1&&FileSize>0)
                {
                    char s[100];
                    if (UnknownFilesCount< 10)           FSF.sprintf(s, "000000%d", UnknownFilesCount);
                    else if (UnknownFilesCount< 100)     FSF.sprintf(s, "00000%d",  UnknownFilesCount);
                    else if (UnknownFilesCount< 1000)    FSF.sprintf(s, "0000%d",   UnknownFilesCount);
                    else if (UnknownFilesCount< 10000)   FSF.sprintf(s, "000%d",    UnknownFilesCount);
                    else if (UnknownFilesCount< 100000)  FSF.sprintf(s, "00%d",     UnknownFilesCount);
                    else if (UnknownFilesCount< 1000000) FSF.sprintf(s, "0%d",      UnknownFilesCount);
                    else                                 FSF.sprintf(s, "%d",       UnknownFilesCount);

                    SFI = (SVFIP)my_malloc(sizeof(SVFI));
                    SFI->FileName      = my_strdup(s);

                    if (lstrlen(finfo.ExtConvrt)!=0){
                        SFI->ConvFileName = my_strdup(s, lstrlen(finfo.ExtConvrt)); 
                        lstrcat(SFI->ConvFileName, finfo.ExtConvrt);
                    }else SFI->ConvFileName = NULL;
                    
                    SFI->FileName = my_strdup(s, lstrlen(finfo.ExtNormal));
                    lstrcat(SFI->FileName, finfo.ExtNormal);

                    SFI->Description   = my_strdup(finfo.Description);
                    SFI->Plugin        = my_strdup(plg.KnownFormats[Fmt].OptionString);
                    SFI->IsDirectory   = FALSE;
                    SFI->Fmt           = Fmt;
                    SFI->IsExtension   = TRUE;
                    SFI->Offset        = FileOffset;
                    SFI->LevelUp       = NULL;
                    SFI->PackedSize    = FileSize;
                    SFI->UnpackedSize  = UnpSize;
                    finfo.Flags        = finfo.Flags;
                    if (finfo.UserDataLen>0){
                        SFI->UserData.Data = my_malloc(finfo.UserDataLen);
                        memcpy(SFI->UserData.Data, finfo.UserData, finfo.UserDataLen);
                    } else SFI->UserData.Data = NULL;
                    SFI->UserData.Size = finfo.UserDataLen;
                    FAT.InsertItem((SVFIP)NULL, SFI);
                            
                    UnknownFilesCount++;
                    
                    newItemsNumber++;                    
                }
            }
        } else {            
            if (NULL != plg.packcheck)
            {   
                FillServiceFunc(func);
                FillFlags(flags, Silent);
                
                RIP_FILE_INFO       *varfinfo = NULL;

                r = plg.packcheck(Fmt, FileName, BaseString, readed, &HeadOffset, &SomeOffset, &FileSize, &FileOffset, &varfinfo, &newItemsNumber, WorkFileSize, &func, &flags);
                if (r != -1){
                    for (long I=0; I < newItemsNumber; I++){
                        
                        for (int i=0; i<lstrlen(varfinfo[I].FullFileName);i++)
                            if (varfinfo[I].FullFileName[i]=='/') varfinfo[I].FullFileName[i]='\\';

                        for (i=0; i<lstrlen(varfinfo[I].ConvFullFileName);i++)
                            if (varfinfo[I].ConvFullFileName[i]=='/') varfinfo[I].ConvFullFileName[i]='\\';

                        SFI = (SVFIP)my_malloc(sizeof(SVFI));
                        SFI->Fmt = Fmt;
                        if (varfinfo[I].Flags&FLAG_FORM_DIRECTORY) {
                            char *d = my_strdup(varfinfo[I].FullFileName, 2);
                            lstrcat(d, "\\-");
                            SFI->Fmt = FMT_SPEC;
                            SFI->FileName      = ExtractName(d);
                            my_free(d);

                            if (varfinfo[I].ConvFullFileName){
                                char *d = my_strdup(varfinfo[I].ConvFullFileName, 2);
                                lstrcat(d, "\\-");
                                SFI->Fmt = FMT_SPEC;
                                SFI->ConvFileName = ExtractName(d);
                                my_free(d);
                            } else SFI->ConvFileName = NULL;
                        } else {
                            SFI->FileName      = ExtractName(varfinfo[I].FullFileName);
                            if (varfinfo[I].ConvFullFileName)
                                 SFI->ConvFileName = my_strdup(varfinfo[I].ConvFullFileName);                                    
                            else SFI->ConvFileName = NULL;
                        }

                        SFI->Description   = my_strdup(varfinfo[I].Description);
                        SFI->Plugin        = my_strdup(plg.KnownFormats[Fmt].OptionString);
                        SFI->IsDirectory   = varfinfo[I].Flags&FLAG_FORM_DIRECTORY;                        
                        SFI->IsExtension   = FALSE;
                        SFI->Offset        = varfinfo[I].StartOffset;
                        SFI->LevelUp       = NULL;
                        SFI->PackedSize    = varfinfo[I].FileSize;
                        SFI->UnpackedSize  = varfinfo[I].UnpSize;
                        if (varfinfo[I].UserDataLen>0){
                            SFI->UserData.Data = my_malloc(varfinfo[I].UserDataLen);
                            memcpy(SFI->UserData.Data, varfinfo[I].UserData, varfinfo[I].UserDataLen);
                        } else SFI->UserData.Data = NULL;                            

                        BOOL dup = FALSE;

                        char *p = ExtractPath(varfinfo[I].FullFileName);
                        for (i=0; i < dirs.size();i++) {
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
                                if (SFI->ConvFileName)
                                    my_free(SFI->ConvFileName);
                                my_free(SFI);
                            } else {
                                DI.DirName  = p;
                                DI.Item     = D->LevelUp;
                                dirs.push_back(DI);
                            }
                        }

                        if (glbFmt == -1) {
                            glbFmt    = Fmt;
                            lstrcpy(glbPlugin, plg.KnownFormats[Fmt].OptionString);
                            glbPackOffset = FileOffset;
                            glbPackSize   = FileSize;
                        }
                        glbAllSize  += varfinfo[I].FileSize;                        
                    }
                    FreeList(plg.KnownFormats[Fmt].OptionString, varfinfo, newItemsNumber);
                }
            }
        }               
    }
    return r;
}

long FormatFinder::Identify(char *FileName, long &FileOffset, long EndFileOffset, long fs, VirtualFAT &FAT, ripd_vec &dirs, BOOL Silent, long &UnknownFilesCount, char*&LastFnd)
{
    long r, RetPos, FileSize, LefterSeek, RighterSeek, fnded, HeadOffset;
    long SomeOffset;    
    long newItemsNumber = 0;
    unsigned long readed =0;    
    BOOL bSkipPack;

    int f = _open (FileName, _O_RDONLY|_O_BINARY);
    if (f == -1) return -1;

    _lseek(f, FileOffset, SEEK_SET);
                
    if (_tell(f)<EndFileOffset)/*(!_eof(f))*/
    {
        readed = _read(f, pluginbuf, BUF_SIZE);
        if (readed == 0) return -1;

        RetPos          = FileOffset;

        if (readed>maxSignature) LefterSeek = FileOffset+readed-maxSignature;
        else LefterSeek = FileOffset+readed;
        
        RighterSeek = EndFileOffset;            
        fnded           = 0;
        bSkipPack   = FALSE;
        _close(f);       

        for (int I = 0; I < PluginsNumber; I++){
            for (int j=0; j<Plugins[I].nKnownFormats; j++){
                if((((Plugins[I].KnownFormats[j].Type == RIP_INFO_TYPE_GRAPH)&&EnableGraph)||
                    ((Plugins[I].KnownFormats[j].Type == RIP_INFO_TYPE_VIDEO)&&EnableVideo)||
                    ((Plugins[I].KnownFormats[j].Type == RIP_INFO_TYPE_SOUND)&&EnableSound)||
                    ((Plugins[I].KnownFormats[j].Type == RIP_INFO_TYPE_MUSIC)&&EnableMusic)||
                    ((Plugins[I].KnownFormats[j].Type == RIP_INFO_TYPE_TEXT) &&EnableText) ||
                    ((Plugins[I].KnownFormats[j].Type == RIP_INFO_TYPE_EXE)  &&EnableExe)  ||
                    ((Plugins[I].KnownFormats[j].Type == RIP_INFO_TYPE_PACK) &&EnablePack) ||
                    ((Plugins[I].KnownFormats[j].Type == RIP_INFO_TYPE_OTHER)&&EnableOther))&&
                    Plugins[I].KnownFormats[j].Active&&
                    Plugins[I].KnownFormats[j].RipVer<=Char2Ver(RIPVER))
                {
                    if (Plugins[I].KnownFormats[j].Type == RIP_INFO_TYPE_PACK)
                        if (UseStartScan&&(FileOffset>0)) continue;
                    HeadOffset = 0;
                    SomeOffset = -1;
                    r = FormatCheck(Plugins[I], Plugins[I].KnownFormats[j].Type, j, FileName, pluginbuf, readed, HeadOffset, SomeOffset, FileSize, FileOffset, FAT, newItemsNumber, dirs, fs, Silent, UnknownFilesCount);
                    if (r != -1){
                        LastFnd = Plugins[I].KnownFormats[j].OptionString;
                        fnded += newItemsNumber;
                        if (Plugins[I].KnownFormats[j].Type == RIP_INFO_TYPE_PACK&&SkipPack){
                            FileOffset = r;
                            bSkipPack = TRUE;
                            break;
                        }
                        if (FileOffset+HeadOffset+1<LefterSeek) LefterSeek = FileOffset+HeadOffset+1;
                        if (r<RighterSeek) RighterSeek = r;
                    } else {                            
                        if (UseSomeOffset&&
                            SomeOffset != -1&&
                            SomeOffset<LefterSeek) LefterSeek = SomeOffset+1;
                    }
                    FileOffset = RetPos;                    
                }
            }
            if (bSkipPack) break;
        }
        int f = _open (FileName, _O_RDONLY|_O_BINARY);
        if (f == -1) return -1;         

        if (bSkipPack){
            _lseek(f, FileOffset, SEEK_SET);
        } else {
            if (fnded == 0){            
                if (FullScan) _lseek(f, LefterSeek, SEEK_SET);
                else {
                    if (readed>maxSignature) _lseek(f, FileOffset+readed-maxSignature, SEEK_SET);
                    else _lseek(f, FileOffset+readed, SEEK_SET);
                }
            } else {
                if (FullScan) _lseek(f, LefterSeek, SEEK_SET);
                else _lseek(f, RighterSeek, SEEK_SET);
            }
        }
    }
        
    FileOffset = _tell(f);      
    _close(f);
    return fnded;
}

BOOL FormatFinder::GetCanGetFile(short Fmt, const char *Plugin)
{
    for (int i=0; i<PluginsNumber;i++){
        if (Fmt<Plugins[i].nKnownFormats&&
            strcmp(Plugin, Plugins[i].KnownFormats[Fmt].OptionString)==0&&
            Plugins[i].KnownFormats[Fmt].RipVer<=Char2Ver(RIPVER)){
            if (Plugins[i].getfileud) return TRUE;            
            if (Plugins[i].getfile) return TRUE;            
        }
    }       
    return FALSE;
}

BOOL FormatFinder::GetCanPutFile(short Fmt, const char *Plugin)
{
    for (int i=0; i<PluginsNumber;i++){
        if (Fmt<Plugins[i].nKnownFormats&&
            strcmp(Plugin, Plugins[i].KnownFormats[Fmt].OptionString)==0&&
            Plugins[i].KnownFormats[Fmt].RipVer<=Char2Ver(RIPVER)){
            if (Plugins[i].putfile) return TRUE;                        
        }
    }       
    return FALSE;
}

BOOL FormatFinder::GetCanDelFile(short Fmt, const char *Plugin)
{
    for (int i=0; i<PluginsNumber;i++){
        if (Fmt<Plugins[i].nKnownFormats&&
            strcmp(Plugin, Plugins[i].KnownFormats[Fmt].OptionString)==0&&
            Plugins[i].KnownFormats[Fmt].RipVer<=Char2Ver(RIPVER)){
            if (Plugins[i].delfile) return TRUE;                        
        }
    }       
    return FALSE;
}

BOOL FormatFinder::GetFile(const char *Plugin, short Fmt, const char *PackName, const char *FileName, const char *PathName, long StartOffset, long FileSize, long UnpSize, BOOL Silent, void * UserData)
{
    RIP_SERVICE_FUNC    func;
    RIP_FLAGS           flags;
    BOOL                res;
    for (int i=0; i<PluginsNumber;i++){
        if (Fmt<Plugins[i].nKnownFormats&&
            strcmp(Plugin, Plugins[i].KnownFormats[Fmt].OptionString)==0&&
            Plugins[i].KnownFormats[Fmt].RipVer<=Char2Ver(RIPVER)){

            FillServiceFunc(func);
            FillFlags(flags, Silent);
            
            if (!UserData)
                if (Plugins[i].getfile)
                    res = Plugins[i].getfile(Fmt, PackName, FileName, PathName, StartOffset, FileSize, UnpSize, &func, &flags);
                else res = FALSE;
            else
                if (Plugins[i].getfileud)
                    res = Plugins[i].getfileud(Fmt, PackName, FileName, PathName, StartOffset, FileSize, UnpSize, &func, &flags, UserData);
                else res = FALSE;
            return res;
        }            
    }       
    return FALSE;    
}

BOOL FormatFinder::GetFAT(const char *Plugin, short Fmt, const char *PackName, void **FAT)
{
    RIP_SERVICE_FUNC    func;
    BOOL                res;
    RIP_FLAGS           flags;
    for (int i=0; i<PluginsNumber;i++){
        if (Fmt<Plugins[i].nKnownFormats&&
            strcmp(Plugin, Plugins[i].KnownFormats[Fmt].OptionString)==0&&
            Plugins[i].KnownFormats[Fmt].RipVer<=Char2Ver(RIPVER)){

            FillServiceFunc(func);
            FillFlags(flags, FALSE);

            if (Plugins[i].getfat)
                res = Plugins[i].getfat(Fmt, PackName, glbPackOffset, glbPackSize, FAT, &func, &flags);
            else res = FALSE;            
            return res;
        }            
    }       
    return FALSE;    
}

BOOL FormatFinder::PutFile(const char *Plugin, short Fmt, const char *PackName, const char *FileName, const char *PathName, const char *DiskPathName, long *StartOffset, long *FileSize, long *UnpSize, void **FAT, BOOL Compress)
{
    RIP_SERVICE_FUNC     func;
    RIP_FLAGS           flags;
    BOOL                 res;
    for (int i=0; i<PluginsNumber;i++){
        if (Fmt<Plugins[i].nKnownFormats&&
            strcmp(Plugin, Plugins[i].KnownFormats[Fmt].OptionString)==0&&
            Plugins[i].KnownFormats[Fmt].RipVer<=Char2Ver(RIPVER)){

            FillServiceFunc(func);
            FillFlags(flags, FALSE);
            
            if (Plugins[i].putfile)
                res = Plugins[i].putfile(Fmt, PackName, FileName, PathName, DiskPathName,  StartOffset, FileSize, UnpSize, glbPackOffset, glbPackSize, FAT, Compress, &func, &flags);
            else res = FALSE;            
            return res;
        }            
    }       
    return FALSE;    
}

BOOL FormatFinder::DelFile(const char *Plugin, short Fmt, const char *PackName, const char *FileName, long StartOffset, long FileSize, long UnpSize, void ** FAT)
{
    RIP_SERVICE_FUNC    func;
    RIP_FLAGS           flags;
    BOOL                res;

    for (int i=0; i<PluginsNumber;i++){
        if (Fmt<Plugins[i].nKnownFormats&&
            strcmp(Plugin, Plugins[i].KnownFormats[Fmt].OptionString)==0&&
            Plugins[i].KnownFormats[Fmt].RipVer<=Char2Ver(RIPVER)){

            FillServiceFunc(func);
            FillFlags(flags, FALSE);

            if (Plugins[i].delfile)
                res = Plugins[i].delfile(Fmt, PackName, FileName, StartOffset, FileSize, UnpSize, FAT, &func, &flags);
            else res = FALSE;            
            return res;
        }            
    }           
    return TRUE;
}

BOOL FormatFinder::FreeFAT(const char *Plugin, short Fmt, const char *PackName, const char *TmpName, void **FAT)
{
    RIP_SERVICE_FUNC    func;
    RIP_FLAGS           flags;
    BOOL                res;

    for (int i=0; i<PluginsNumber;i++){
        if (Fmt<Plugins[i].nKnownFormats&&
            strcmp(Plugin, Plugins[i].KnownFormats[Fmt].OptionString)==0&&
            Plugins[i].KnownFormats[Fmt].RipVer<=Char2Ver(RIPVER)){
            
            FillServiceFunc(func);
            FillFlags(flags, FALSE);

            if (Plugins[i].freefat)
                res = Plugins[i].freefat(Fmt, PackName, TmpName, glbPackOffset, glbPackSize, WorkFileSize, FAT, &func, &flags);
            else res = FALSE;            
            return res;
        }            
    }       
    return FALSE;    
}

BOOL FormatFinder::FreeList(const char *Plugin, RIP_FILE_INFO  *varfinfo, long newItemsNumber)
{
    RIP_SERVICE_FUNC    func;
    RIP_FLAGS           flags;
    BOOL                res;

    for (int i=0; i<PluginsNumber;i++){
        if (lstrcmp(Plugin, Plugins[i].KnownFormats[0].OptionString)==0&&
            Plugins[i].KnownFormats[0].RipVer<=Char2Ver(RIPVER)){
            
            FillServiceFunc(func);
            FillFlags(flags, FALSE);

            if (Plugins[i].packcheckfree)
                res = Plugins[i].packcheckfree(varfinfo, newItemsNumber, &func, &flags);                
            else res = FALSE;            
            return res;
        }            
    }    
    
    return FALSE;
}