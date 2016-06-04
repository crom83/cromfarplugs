#include "stdafx.h"
#include "memory.cpp"
#include "rtf.cpp"
#include "pdf.cpp"

enum {
    SND_RTF,
    SND_PDF,
    GRP_END
};

long WINAPI FormatCheck(short Fmt, const char * FileName, PUCHAR BaseString, long readed, long *HeadOffset, long *SomeOffset, long *FileSize, long *FileOffset, long *UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    memset(finfo, 0, sizeof(RIP_FORM_INFO));
    finfo->Flags = FLAG_FORM_EXTENSION;
    switch (Fmt){
    case SND_RTF:  return FormatCheckRTF(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case SND_PDF:  return FormatCheckPDF(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    }    
    return -1;
}

BOOL WINAPI PluginInfo (struct RIP_NFO *RipNfo, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    RipNfo->Copyright = my_strdup("(c) Kubyshev Vladimir, 2003");
    RipNfo->nKnownFormats = GRP_END;
    RipNfo->KnownFormats  = (RIP_NFO_FMT*)my_malloc(sizeof(RIP_NFO_FMT)*RipNfo->nKnownFormats);
    
    RipNfo->KnownFormats[0].Type         = RIP_INFO_TYPE_TEXT;
    RipNfo->KnownFormats[0].RegPath      = my_strdup("enableRTF");
    RipNfo->KnownFormats[0].OptionString = my_strdup("RTF file");
    RipNfo->KnownFormats[0].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[0].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[1].Type         = RIP_INFO_TYPE_TEXT;
    RipNfo->KnownFormats[1].RegPath      = my_strdup("enablePDF");
    RipNfo->KnownFormats[1].OptionString = my_strdup("PDF file");
    RipNfo->KnownFormats[1].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[1].RipperVer    = func->Char2Ver("0.20 alpha 2");

    return TRUE;
}

BOOL WINAPI PluginInfoFree (struct RIP_NFO *RipNfo, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    for (int i = 0; i<RipNfo->nKnownFormats; i++){
        my_free(RipNfo->KnownFormats[i].RegPath);
        my_free(RipNfo->KnownFormats[i].OptionString);
    }
    my_free(RipNfo->KnownFormats);
    my_free(RipNfo->Copyright);
        
    return TRUE;
}