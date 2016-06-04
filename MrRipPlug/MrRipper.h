//MrRipper Header file

struct UData{
    ULONG Size;
    void *Data;
};

struct RIPPER_FILE_INFO{
	char * FileName;
	char * Description;
    char * Plugin;
    short  Fmt;
	long   StartOffset;
	long   FileSize;
    long   UnpSize;
    BOOL   IsExtension;
    BOOL   IsDirectory;
    UData  UserData;    
};

struct ADDPLUGININFO{
    //long    Size;
	char    RegPath[24];
    char    FilePath[64];
    char    Copy[64];
    short   Type;
    long    Ver;
    long    RipVer;
};

struct PresetsSort{
    char * Preset;
    friend bool LessPreset(const PresetsSort& a, const PresetsSort& b);
};

typedef vector<RIPPER_FILE_INFO> rip_vec;

typedef long (WINAPI* FORMATCHECK)(short, const char *, PUCHAR, long, long *, long *, long *, long *, long *, struct RIP_FORM_INFO*, long, struct RIP_SERVICE_FUNC *, struct RIP_FLAGS *);
typedef long (WINAPI* PACKCHECK)  (short, const char *, PUCHAR, long, long *, long *, long *, long *, struct RIP_FILE_INFO**, long*, long, struct RIP_SERVICE_FUNC *, struct RIP_FLAGS *);
typedef BOOL (WINAPI* PACKCHECKFREE)(struct RIP_FILE_INFO *, long, struct RIP_SERVICE_FUNC *, struct RIP_FLAGS *);
typedef BOOL (WINAPI* GETFILE)    (short, const char *, const char *, const char *, long, long, long, struct RIP_SERVICE_FUNC *, struct RIP_FLAGS *);
typedef BOOL (WINAPI* GETFILEUD)  (short, const char *, const char *, const char *, long, long, long, struct RIP_SERVICE_FUNC *, struct RIP_FLAGS *, void* UserData);
typedef BOOL (WINAPI* PLUGININFO) (struct RIP_NFO *, struct RIP_SERVICE_FUNC *, struct RIP_FLAGS *);
typedef BOOL (WINAPI* PLUGININFOFREE) (struct RIP_NFO *, struct RIP_SERVICE_FUNC *, struct RIP_FLAGS *);
typedef BOOL (WINAPI* GETFAT)     (short, const char *, long, long, void **, struct RIP_SERVICE_FUNC *, struct RIP_FLAGS *);
typedef BOOL (WINAPI* PUTFILE)    (short, const char *, const char *, const char *, const char *, long *, long *, long *, long, long, void **, BOOL, struct RIP_SERVICE_FUNC *, struct RIP_FLAGS *);
typedef BOOL (WINAPI* DELFILE)    (short, const char *, const char *, long, long, long, void **, struct RIP_SERVICE_FUNC *, struct RIP_FLAGS *);
typedef BOOL (WINAPI* FREEFAT)    (short, const char *, const char *, long, long, long, void **, struct RIP_SERVICE_FUNC *, struct RIP_FLAGS *);

struct RIP_PLUGIN_FMT{
    char *  RegPath;
	char *  OptionString;
	BOOL    Active;
	short   Type;
    long    Version;
    long    RipVer;
};

struct RIP_PLUGIN{
	char           * PluginPath;
	char           * PluginCopyright;	        
    RIP_PLUGIN_FMT * KnownFormats;    
    short            nKnownFormats;
    HINSTANCE        DLLHandle;
    FORMATCHECK      formatcheck;
    PACKCHECK        packcheck;
    PACKCHECKFREE    packcheckfree;
    GETFILE          getfile;
    GETFILEUD        getfileud;
    PLUGININFO       plugininfo;    
    PLUGININFOFREE   plugininfofree;
    GETFAT           getfat;
    PUTFILE          putfile;
    DELFILE          delfile;
    FREEFAT          freefat;
	friend bool LessName(const RIP_PLUGIN & a, const RIP_PLUGIN & b);
};

struct RIP_PLUGIN_GETNFO{
    char * RegPath;
	char * OptionString;
	BOOL   Active;
	short  Type;
	char * PluginPath;
	char * PluginCopyright;
    long   Version;
    long   RipVer;
};

struct VirtualFileInfo{
    char * FileName;
    char * ConvFileName;
	char * Description;
    char * Plugin;
    short  Fmt;
	long   Offset;
	long   PackedSize;
    long   UnpackedSize;
    BOOL   IsExtension;
    BOOL   IsDirectory;
    UData  UserData;    
    struct VirtualFileInfo* LevelUp;
    vector<VirtualFileInfo*> Items;
};

typedef struct VirtualFileInfo SVFI;
typedef struct VirtualFileInfo* SVFIP;
typedef vector<VirtualFileInfo*>* VVFIP;

struct RIPPER_DIR_INFO{
	char * DirName;	
    SVFIP  Item;
};
typedef vector<RIPPER_DIR_INFO> ripd_vec;

class VirtualFAT{   
public:
    SVFIP Root;

    SVFIP GetDir(SVFIP Item, char* DirName);
    SVFIP GetPath(SVFIP Item, char* Path);
    
    BOOL DelItem(SVFIP &Item);
    SVFIP InsertItem(const char* Dir, SVFIP Item);
    SVFIP InsertItem(SVFIP Dir, SVFIP Item);

    void ReInitFAT();
    VirtualFAT();
    ~VirtualFAT();
};

//class Mr. Ripper
class MrRipper
{
  private:    
    //rip_vec files;
    rip_vec shownfiles;
    rip_vec copyfiles;
    rip_vec putfiles;
	rip_vec dummyfiles;    
    //char ExtDir[MAX_PATH];
    //char CurDir[MAX_PATH];
    char FarCurDir[MAX_PATH];    
    //void Free_files();
    void Free_shownfiles();
    BOOL Update_shownfiles(BOOL Silent);
    void Free_copyfiles();    
    void Free_putfiles();
	void Free_dummyfiles();
    void MakeCopyList(char * Dir);
    void MakeCopyList(struct PluginPanelItem *PanelItem, int ItemsNumber);
    void MakePutList(char* CurDirectory, struct PluginPanelItem *PanelItem, int ItemsNumber);
    void MakePutList2(char* CurDirectory, struct PluginPanelItem *PanelItem, int ItemsNumber);
    BOOL DestDirPrompt(char* Dir);
    int  OverwritePrompt(char* FileName);
    int  OverwritePromptPut(char* FileName);
    int  FileExist(char * FileName, long Fsize);
    int  FileExist2(char * FileName);
	BOOL AddDummy();
	BOOL AddDummySilent(char * Name, char * Desc, ULONG Offset, ULONG Size);
	BOOL SplitFile();
	BOOL TryToLoadList(BOOL Silence);
	BOOL SaveList(BOOL Silence);    
	BOOL FileRipper(int OpMode);
	BOOL PluginManager(int OpMode);
    BOOL ShemeOverwrite(char* ShemeName);
    BOOL SavePlugins();
    BOOL CreateSheme();
    BOOL LoadSheme();    
    //BOOL PrepareDirs();
  public:	
    VirtualFAT FAT;
    void Free_dirs();
    ripd_vec dirs;
	char RipFileName[MAX_PATH];	
	char Title[NM]; 
	long WorkFileSize;
	char  glbPlugin[300];
	short glbFmt;
	long  glbPackOffset;
	long  glbPackSize;
	long  glbAllFiles;
	long  glbAllSize;
	BOOL bPluginManager;
    BOOL ConvView;
    BOOL FullScan;
    BOOL DelDup;
    BOOL UseSomeOffset;
    BOOL UseStartScan;
    BOOL SkipPack;
	BOOL EnableGraph;
    BOOL EnableVideo;
    BOOL EnableSound;
    BOOL EnableMusic;
    BOOL EnableText;
    BOOL EnableExe;
    BOOL EnablePack;
    BOOL EnableOther;	
	ULONG StartOffset;
	ULONG EndOffset;

    MrRipper();
    ~MrRipper();
    BOOL ScanDialog();
    int  GetFindData(PluginPanelItem **pPanelItem,int *pItemsNumber,int OpMode);
    void FreeFindData(PluginPanelItem *PanelItem,int ItemsNumber);
    void GetOpenPluginInfo(struct OpenPluginInfo *Info);
	int  ProcessKey(int Key, unsigned int ControlState);
	int  GetFiles(struct PluginPanelItem *PanelItem, int ItemsNumber, int Move, char *DestPath, int OpMode);
    int  SetDirectory(const char *Dir,int OpMode);
    HANDLE OpenPlugin(int OpenFrom,int Item);
    int  PutFiles(struct PluginPanelItem *PanelItem, int ItemsNumber, int Move, int OpMode);
    int  DelFiles(struct PluginPanelItem *PanelItem, int ItemsNumber, int OpMode);    
};

class FormatFinder{
private:	
	char * PluginPath;
	RIP_PLUGIN *Plugins;
    long PluginsNumber;
	void ScanFormats();
	long FormatCheck(RIP_PLUGIN plg, int Type, short Fmt, char *FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, VirtualFAT &FAT, long &newFileItems, ripd_vec &dirs, long WorkFileSize, BOOL Silent, long &UnknownFilesCount);
	BOOL CheckPlugin(char *Name, RIP_PLUGIN &plg);
    void FillServiceFunc(RIP_SERVICE_FUNC &func);
    void FillFlags(RIP_FLAGS &flags, BOOL Silent);
public:
	char  glbPlugin[300];
	short glbFmt;
	long  glbPackOffset;
	long  glbPackSize;	
    long  glbAllSize;
	long WorkFileSize;
	BOOL EnableGraph;
	BOOL EnableVideo;
	BOOL EnableSound;
    BOOL EnableMusic;
    BOOL EnableText;
    BOOL EnableExe;
	BOOL EnablePack;
	BOOL EnableOther;
	BOOL FullScan;  
    BOOL UseSomeOffset;
    BOOL UseStartScan;
    BOOL SkipPack;	
	
	FormatFinder(char *PluginPath);	
	~FormatFinder();	

	long   Identify(char *FileName, long &FileOffset, long EndFileOffset, long fs, VirtualFAT &FAT, ripd_vec &dirs, BOOL Silent, long &UnknownFilesCount, char*&LastFnd);
    BOOL   FreeList(const char *Plugin, RIP_FILE_INFO  *varfinfo, long newItemsNumber);
    int    GetAllFormatCount();
	int    GetFormatCount();
    int    GetnKnownFormats(int Plug);
    void   GetPluginInfo(int Plug, short Fmt,  RIP_PLUGIN_GETNFO &pinfo);
	void   SetActiveOpt(int Plug, short Fmt, BOOL Active);
	BOOL   GetActiveOpt(int Plug, short Fmt);
	char * GetOptionString(int Plug, short Fmt);
	char * GetRegPath(int Plug, short Fmt);
    BOOL   GetCanGetFile(short Fmt, const char *Plugin);
    BOOL   GetCanPutFile(short Fmt, const char *Plugin);
    BOOL   GetCanDelFile(short Fmt, const char *Plugin);
    BOOL   GetFAT (const char *Plugin, short Fmt, const char *PackName, void ** FAT);
    BOOL   FreeFAT(const char *Plugin, short Fmt, const char *PackName, const char *TmpName,  void ** FAT);
    BOOL   GetFile(const char *Plugin, short Fmt, const char *PackName, const char *FileName, const char *PathName, long StartOffset, long FileSize, long UnpSize, BOOL Silent, void* UserData);
    BOOL   PutFile(const char *Plugin, short Fmt, const char *PackName, const char *FileName, const char *PathName, const char *DiskPathName, long *StartOffset, long *FileSize, long *UnpSize, void ** FAT, BOOL Compress);
    BOOL   DelFile(const char *Plugin, short Fmt, const char *PackName, const char *FileName, long StartOffset, long FileSize, long UnpSize, void ** FAT);
};

class TSaveScreen{
  private:
    HANDLE hScreen;
  public:
    TSaveScreen();
   ~TSaveScreen();
};

struct Options
{
	char Pref[10];
	BOOL UsePlugin;
	BOOL FullScan;  
    BOOL DoConv;
    BOOL DelDup;
    BOOL UseSomeOffset;
    BOOL SkipPack;
    BOOL UseStartScan;
    BOOL ShowScanDialog;
	BOOL EnableGraph;
	BOOL EnableVideo;
	BOOL EnableSound;
    BOOL EnableMusic;
    BOOL EnableText;
    BOOL EnableExe;
	BOOL EnablePack;
	BOOL EnableOther;
	BOOL UserScanRange;
	ULONG StartOffset;
	ULONG EndOffset;
	char DummyName[MAX_PATH];
	char DummyDesc[300];
	ULONG DummyOffset;
	ULONG DummySize;
	ULONG DummyEnd;
	char SplitString[20];
	char SplitExt[10];
	short SplitSize;
	long SplitOffset;
	BOOL UseLists;
};

#define BUF_SIZE 16384
#define maxSignature 100 

unsigned char buf[BUF_SIZE];
unsigned char pluginbuf[BUF_SIZE];