#include "stdafx.h"
#include "memory.cpp"
#include "mp3.cpp"
#include "wav.cpp"
#include "ogg.cpp"
#include "aiff.cpp"
#include "ausnd.cpp"

enum {
    GRP_MP3,
    GRP_WAV,
    GRP_OGG,
	GRP_AIFF,
	GRP_AUSND,
    GRP_END
};

long WINAPI FormatCheck(short Fmt, const char * FileName, PUCHAR BaseString, long readed, long *HeadOffset, long *SomeOffset, long *FileSize, long *FileOffset, long *UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    memset(finfo, 0, sizeof(RIP_FORM_INFO));
    finfo->Flags = FLAG_FORM_EXTENSION;
    switch (Fmt){
    case GRP_MP3:  return FormatCheckMP3(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_WAV:  return FormatCheckWAV(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    case GRP_OGG:  return FormatCheckOGG(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
	case GRP_AIFF: return FormatCheckAIFF(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
	case GRP_AUSND:return FormatCheckAUSND(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, *UnpSize, finfo, WorkFileSize, func, flags);
    }    
    return -1;
}

BOOL WINAPI PluginInfo (struct RIP_NFO *RipNfo, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    RipNfo->Copyright = my_strdup("(c) Kubyshev Vladimir, 2003");
    RipNfo->nKnownFormats = GRP_END;
    RipNfo->KnownFormats  = (RIP_NFO_FMT*)my_malloc(sizeof(RIP_NFO_FMT)*RipNfo->nKnownFormats);
    
    RipNfo->KnownFormats[0].Type         = RIP_INFO_TYPE_SOUND;
    RipNfo->KnownFormats[0].RegPath      = my_strdup("enableMP3");
    RipNfo->KnownFormats[0].OptionString = my_strdup("MPEG audio file");    
    RipNfo->KnownFormats[0].Version      = func->Char2Ver("0.20 beta 4");
    RipNfo->KnownFormats[0].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[1].Type         = RIP_INFO_TYPE_SOUND;
    RipNfo->KnownFormats[1].RegPath      = my_strdup("enableWAV");
    RipNfo->KnownFormats[1].OptionString = my_strdup("WAV file");
    RipNfo->KnownFormats[1].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[1].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[2].Type         = RIP_INFO_TYPE_SOUND;
    RipNfo->KnownFormats[2].RegPath      = my_strdup("enableOGG");
    RipNfo->KnownFormats[2].OptionString = my_strdup("OGG Vorbis file");
    RipNfo->KnownFormats[2].Version      = func->Char2Ver("0.20 beta 3");
    RipNfo->KnownFormats[2].RipperVer    = func->Char2Ver("0.20 alpha 2");

	RipNfo->KnownFormats[3].Type         = RIP_INFO_TYPE_SOUND;
    RipNfo->KnownFormats[3].RegPath      = my_strdup("enableAIFF");
    RipNfo->KnownFormats[3].OptionString = my_strdup("AIFF file");
    RipNfo->KnownFormats[3].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[3].RipperVer    = func->Char2Ver("0.20 alpha 2");

	RipNfo->KnownFormats[4].Type         = RIP_INFO_TYPE_SOUND;
    RipNfo->KnownFormats[4].RegPath      = my_strdup("enableAUSND");
    RipNfo->KnownFormats[4].OptionString = my_strdup("AU file");
    RipNfo->KnownFormats[4].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[4].RipperVer    = func->Char2Ver("0.20 alpha 2");

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