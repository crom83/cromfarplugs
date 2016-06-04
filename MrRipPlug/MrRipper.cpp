#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <direct.h>
#include <crtdbg.h>

#define _FAR_USE_FARFINDDATA
#include <plugin.hpp>
#include <eplugin.hpp>
#include <rip_api.h>
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#include <list>
int logfile;
#define  flushdelay 100000
#endif

//#include <string>
using namespace std;

#define RIPVER "0.20 alpha 2"
#define FMT_DUMMY -1000
#define FMT_SPEC  -5000

#include "MrRipper.h"
#include "riphlp.h"
#include "log.cpp"
#include "Memory.cpp"

BOOL IsOldFAR;

//Main information about module
struct PluginStartupInfo Info;
struct FarStandardFunctions FSF;

char PluginPath[MAX_PATH];
char PluginRootKey[80];
char FarRootKey [NM];

const char *StrUsePlugin  ="StrUsePlugin";
const char *StrFullScan   ="StrFullScan";
const char *StrDoConv     ="StrDoConv";
const char *StrDelDup     ="StrDelDup";
const char *StrUseSomeOffset ="StrUseSomeOffset";
const char *StrUseStartScan  ="StrUseStartScan";
const char *StrSkipPack   ="StrSkipPack";
const char *StrShowScanDialog="StrShowScanDialog";
const char *StrEnableGraph="StrEnableGraph";
const char *StrEnableVideo="StrEnableVideo";
const char *StrEnableSound="StrEnableSound";
const char *StrEnableMusic="StrEnableMusic";
const char *StrEnableText ="StrEnableText";
const char *StrEnableExe  ="StrEnableExe";
const char *StrEnablePack ="StrEnablePack";
const char *StrEnableOther="StrEnableOther";
const char *StrPref       ="StrPref";
const char *StrUserScanRange ="StrUserScanRange";
const char *StrStartOffset="StrStartOffset";
const char *StrEndOffset  ="StrEndOffset";
const char *StrDummyName    ="StrDummyName";
const char *StrDummyDesc    ="StrDummyDesc";
const char *StrDummyOffset  ="StrDummyOffset";
const char *StrDummySize    ="StrDummySize";
const char *StrDummyEnd     ="StrDummyEnd";
const char *StrSplitString  ="StrSplitString";
const char *StrSplitSize    ="StrSplitSize";
const char *StrSplitExt     ="StrSplitExt";
const char *StrSplitOffset  ="StrSplitOffset";
const char *StrUseLists     ="StrUseLists";

const char *HistStartOffset  ="MrRipStartOffset";
const char *HistEndOffset    ="MrRipEndOffset";
const char *HistLength       ="MrRipLength";
const char *HistDestDir      ="MrRipDestDir";
const char *HistDummyName    ="MrRipDummyName";
const char *HistDummyDesc    ="MrRipDummyDesc";
const char *HistDummyOffset  ="MrRipDummyOffset";
const char *HistDummySize    ="MrRipDummySize";
const char *HistDummyEnd     ="MrRipDummyEnd";
const char *HistSplitString  ="MrRipSplitString";
const char *HistSplitExt     ="MrRipSplitExt";


Options Opt;

#include "exported.cpp"
#include "ripreg.cpp"
#include "ripmisc.cpp"
#include "formats.cpp"
#include "vfat.cpp"
#include "ripclass.cpp"
#include "far.cpp"
#include "lists.cpp"
#include "shemes.cpp"
#include "dummies.cpp"


