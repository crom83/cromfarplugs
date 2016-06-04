#include "stdafx.h"
#include "memory.cpp"
#include "TD_MIX.cpp"
#include "RA_MIX.cpp"

enum {
    GRP_TD, //Tiberiun Dawn
    GRP_RA, //Red Alert
    GRP_END
};

long WINAPI PackCheck(short Fmt, const char * FileName, PUCHAR BaseString, long readed, long *HeadOffset, long *SomeOffset, long *FileSize, long *FileOffset, struct RIP_FILE_INFO **finfo, long *FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    switch (Fmt){    
    case GRP_TD:  return PackCheckTDMIX(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_RA:  return PackCheckRAMIX(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    }    
    return -1;
}

BOOL WINAPI PackCheckFree(struct RIP_FILE_INFO *finfo, long FilesNumber, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    for (int i=0; i<FilesNumber; i++)
    {
        my_free(finfo[i].FullFileName);        
        my_free(finfo[i].Description);        
    }
    my_free(finfo);
    return TRUE;
}

BOOL WINAPI PluginInfo (struct RIP_NFO *RipNfo, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    RipNfo->Copyright = my_strdup("(c) Kubyshev Vladimir, 2003");
    RipNfo->nKnownFormats = GRP_END;
    RipNfo->KnownFormats  = (RIP_NFO_FMT*)my_malloc(sizeof(RIP_NFO_FMT)*RipNfo->nKnownFormats);
    
    RipNfo->KnownFormats[0].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[0].RegPath      = my_strdup("enableTDMIX");
    RipNfo->KnownFormats[0].OptionString = my_strdup("C&C MIX package");    
    RipNfo->KnownFormats[0].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[0].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[1].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[1].RegPath      = my_strdup("enableRAMIX");
    RipNfo->KnownFormats[1].OptionString = my_strdup("Red Alert MIX package");    
    RipNfo->KnownFormats[1].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[1].RipperVer    = func->Char2Ver("0.20 alpha 2");

    return TRUE;
}

BOOL WINAPI PluginInfoFree (struct RIP_NFO *RipNfo, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    for (int i = 0; i<RipNfo->nKnownFormats; i++){
        my_free(RipNfo->KnownFormats[i].RegPath);
        my_free(RipNfo->KnownFormats[i].OptionString);
    }
    my_free(RipNfo->KnownFormats);
    my_free(RipNfo->Copyright);
        
    return TRUE;
}