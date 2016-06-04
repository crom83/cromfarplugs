#include "stdafx.h"
#include "memory.cpp"
#include "it.cpp"
#include "midi.cpp"
#include "mod.cpp"
#include "s3m.cpp"
#include "xm.cpp"
#include "xmi.cpp"
#include "mus.cpp"
#include "ult.cpp"

enum {
    GRP_IT,
    GRP_MIDI,
    GRP_MOD,
    GRP_S3M,
    GRP_XM,
    GRP_XMI,
	GRP_MUS,
    GRP_ULT,
    GRP_END
};

long WINAPI FormatCheck(short Fmt, const char * FileName, PUCHAR BaseString, long readed, long *HeadOffset, long *SomeOffset, long *FileSize, long *FileOffset, long *UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    memset(finfo, 0, sizeof(RIP_FORM_INFO));
    finfo->Flags = FLAG_FORM_EXTENSION;
    switch (Fmt){
    case GRP_IT:  return FormatCheckIT(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_MIDI:return FormatCheckMIDI(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_MOD: return FormatCheckMOD(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_S3M: return FormatCheckS3M(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_XM:  return FormatCheckXM(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_XMI: return FormatCheckXMI(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
	case GRP_MUS: return FormatCheckMUS(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_ULT: return FormatCheckULT(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    }    
    return -1;
}

BOOL WINAPI PluginInfo (struct RIP_NFO *RipNfo, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    RipNfo->Copyright = my_strdup("(c) Kubyshev Vladimir, 2003");
    RipNfo->nKnownFormats = GRP_END;
    RipNfo->KnownFormats  = (RIP_NFO_FMT*)my_malloc(sizeof(RIP_NFO_FMT)*RipNfo->nKnownFormats);
    
    RipNfo->KnownFormats[0].Type         = RIP_INFO_TYPE_MUSIC;
    RipNfo->KnownFormats[0].RegPath      = my_strdup("enableIT");
    RipNfo->KnownFormats[0].OptionString = my_strdup("IT module");
    RipNfo->KnownFormats[0].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[0].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[1].Type         = RIP_INFO_TYPE_MUSIC;
    RipNfo->KnownFormats[1].RegPath      = my_strdup("enableMIDI");
    RipNfo->KnownFormats[1].OptionString = my_strdup("MIDI file");
    RipNfo->KnownFormats[1].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[1].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[2].Type         = RIP_INFO_TYPE_MUSIC;
    RipNfo->KnownFormats[2].RegPath      = my_strdup("enableMOD");
    RipNfo->KnownFormats[2].OptionString = my_strdup("MOD module");
    RipNfo->KnownFormats[2].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[2].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[3].Type         = RIP_INFO_TYPE_MUSIC;
    RipNfo->KnownFormats[3].RegPath      = my_strdup("enableS3M");
    RipNfo->KnownFormats[3].OptionString = my_strdup("S3M module");
    RipNfo->KnownFormats[3].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[3].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[4].Type         = RIP_INFO_TYPE_MUSIC;
    RipNfo->KnownFormats[4].RegPath      = my_strdup("enableXM");
    RipNfo->KnownFormats[4].OptionString = my_strdup("XM module");
    RipNfo->KnownFormats[4].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[4].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[5].Type         = RIP_INFO_TYPE_MUSIC;
    RipNfo->KnownFormats[5].RegPath      = my_strdup("enableXMI");
    RipNfo->KnownFormats[5].OptionString = my_strdup("XMI midi file");
    RipNfo->KnownFormats[5].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[5].RipperVer    = func->Char2Ver("0.20 alpha 2");

	RipNfo->KnownFormats[6].Type         = RIP_INFO_TYPE_MUSIC;
    RipNfo->KnownFormats[6].RegPath      = my_strdup("enableMUS");
    RipNfo->KnownFormats[6].OptionString = my_strdup("MUS midi file");
    RipNfo->KnownFormats[6].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[6].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[7].Type         = RIP_INFO_TYPE_MUSIC;
    RipNfo->KnownFormats[7].RegPath      = my_strdup("enableULT");
    RipNfo->KnownFormats[7].OptionString = my_strdup("ULT module");
    RipNfo->KnownFormats[7].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[7].RipperVer    = func->Char2Ver("0.20 alpha 2");

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