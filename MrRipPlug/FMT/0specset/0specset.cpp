#include "stdafx.h"
#include "memory.cpp"
#include "JAM.cpp"

enum {
    GRP_JAM,
    GRP_END
};

long WINAPI PackCheck(short Fmt, const char * FileName, PUCHAR BaseString, long readed, long *HeadOffset, long *SomeOffset, long *FileSize, long *FileOffset, struct RIP_FILE_INFO **finfo, long *FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func)
{
    switch (Fmt){    
    case GRP_JAM:  return PackCheckJAM(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func);
    }    
    return -1;
}

BOOL WINAPI PackCheckFree(struct RIP_FILE_INFO *finfo, long FilesNumber)
{
    for (int i=0; i<FilesNumber; i++)
    {
        my_free(finfo[i].FullFileName);
        my_free(finfo[i].Description);        
    }
    my_free(finfo);
    return TRUE;
}

BOOL WINAPI PluginInfo (struct RIP_NFO *RipNfo)
{
    RipNfo->Copyright = my_strdup("(c) Kubyshev Vladimir, 2003");
    RipNfo->nKnownFormats = GRP_END;
    RipNfo->KnownFormats  = (RIP_NFO_FMT*)my_malloc(sizeof(RIP_NFO_FMT)*RipNfo->nKnownFormats);
    
    RipNfo->KnownFormats[0].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[0].RegPath      = my_strdup("enableJAMFIDO");
    RipNfo->KnownFormats[0].OptionString = my_strdup("JAM package");    

    return TRUE;
}

BOOL WINAPI PluginInfoFree (struct RIP_NFO *RipNfo)
{
    for (int i = 0; i<RipNfo->nKnownFormats; i++){
        my_free(RipNfo->KnownFormats[i].RegPath);
        my_free(RipNfo->KnownFormats[i].OptionString);
    }
    my_free(RipNfo->KnownFormats);
    my_free(RipNfo->Copyright);
        
    return TRUE;
}