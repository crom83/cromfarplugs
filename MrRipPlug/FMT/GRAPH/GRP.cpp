#include "stdafx.h"
#include "memory.cpp"
#include "bmp.cpp"
#include "jpg.cpp"
#include "gif.cpp"
#include "png.cpp"
#include "djvu.cpp"
#include "tga1.cpp"
#include "tga2.cpp"
#include "lbm.cpp"
#include "pcx.cpp"

enum {
    GRP_BMP,
    GRP_JPG,
    GRP_GIF,
    GRP_PNG,
    GRP_DJVU,
    GRP_TGA1,
    GRP_TGA2,
    GRP_LBM,
    GRP_PCX,
    GRP_END
};

long WINAPI FormatCheck(short Fmt, const char * FileName, PUCHAR BaseString, long readed, long *HeadOffset, long *SomeOffset, long *FileSize, long *FileOffset, long *UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{
    memset(finfo, 0, sizeof(RIP_FORM_INFO));
    finfo->Flags = FLAG_FORM_EXTENSION;
    switch (Fmt){
    case GRP_BMP: return FormatCheckBMP(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_JPG: return FormatCheckJPG(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_GIF: return FormatCheckGIF(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_PNG: return FormatCheckPNG(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_DJVU:return FormatCheckDJVU(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_TGA1:return FormatCheckTGA1(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_TGA2:return FormatCheckTGA2(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_LBM: return FormatCheckLBM(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_PCX: return FormatCheckPCX(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    }    
    return -1;
}

BOOL WINAPI PluginInfo (struct RIP_NFO *RipNfo, RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{
    RipNfo->Copyright = my_strdup("(c) Kubyshev Vladimir, 2003");
    RipNfo->nKnownFormats = GRP_END;
    RipNfo->KnownFormats  = (RIP_NFO_FMT*)my_malloc(sizeof(RIP_NFO_FMT)*RipNfo->nKnownFormats);
    
    RipNfo->KnownFormats[0].Type         = RIP_INFO_TYPE_GRAPH;
    RipNfo->KnownFormats[0].RegPath      = my_strdup("enableBMP");
    RipNfo->KnownFormats[0].OptionString = my_strdup("BMP image");
    RipNfo->KnownFormats[0].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[0].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[1].Type         = RIP_INFO_TYPE_GRAPH;
    RipNfo->KnownFormats[1].RegPath      = my_strdup("enableJPG");
    RipNfo->KnownFormats[1].OptionString = my_strdup("JPEG image");
    RipNfo->KnownFormats[1].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[1].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[2].Type         = RIP_INFO_TYPE_GRAPH;
    RipNfo->KnownFormats[2].RegPath      = my_strdup("enableGIF");
    RipNfo->KnownFormats[2].OptionString = my_strdup("GIF image");
    RipNfo->KnownFormats[2].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[2].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[3].Type         = RIP_INFO_TYPE_GRAPH;
    RipNfo->KnownFormats[3].RegPath      = my_strdup("enablePNG");
    RipNfo->KnownFormats[3].OptionString = my_strdup("PNG image");
    RipNfo->KnownFormats[3].Version      = func->Char2Ver("0.20.0");
    RipNfo->KnownFormats[3].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[4].Type         = RIP_INFO_TYPE_GRAPH;
    RipNfo->KnownFormats[4].RegPath      = my_strdup("enableDJVU");
    RipNfo->KnownFormats[4].OptionString = my_strdup("DjVu image");
    RipNfo->KnownFormats[4].Version      = func->Char2Ver("0.20.0");
    RipNfo->KnownFormats[4].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[5].Type         = RIP_INFO_TYPE_GRAPH;
    RipNfo->KnownFormats[5].RegPath      = my_strdup("enableTGA1");
    RipNfo->KnownFormats[5].OptionString = my_strdup("TGA type 1 image");
    RipNfo->KnownFormats[5].Version      = func->Char2Ver("0.20 alpha 1");
    RipNfo->KnownFormats[5].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[6].Type         = RIP_INFO_TYPE_GRAPH;
    RipNfo->KnownFormats[6].RegPath      = my_strdup("enableTGA2");
    RipNfo->KnownFormats[6].OptionString = my_strdup("TGA type 2 image");
    RipNfo->KnownFormats[6].Version      = func->Char2Ver("0.20 alpha 1");
    RipNfo->KnownFormats[6].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[7].Type         = RIP_INFO_TYPE_GRAPH;
    RipNfo->KnownFormats[7].RegPath      = my_strdup("enableLBM");
    RipNfo->KnownFormats[7].OptionString = my_strdup("IFF Image");
    RipNfo->KnownFormats[7].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[7].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[8].Type         = RIP_INFO_TYPE_GRAPH;
    RipNfo->KnownFormats[8].RegPath      = my_strdup("enablePCX");
    RipNfo->KnownFormats[8].OptionString = my_strdup("PCX image");
    RipNfo->KnownFormats[8].Version      = func->Char2Ver("0.20 beta 3");
    RipNfo->KnownFormats[8].RipperVer    = func->Char2Ver("0.20 alpha 2");

    return TRUE;
}

BOOL WINAPI PluginInfoFree (struct RIP_NFO *RipNfo, RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{
    for (int i = 0; i<RipNfo->nKnownFormats; i++){
        my_free(RipNfo->KnownFormats[i].RegPath);
        my_free(RipNfo->KnownFormats[i].OptionString);
    }
    my_free(RipNfo->KnownFormats);
    my_free(RipNfo->Copyright);
        
    return TRUE;
}