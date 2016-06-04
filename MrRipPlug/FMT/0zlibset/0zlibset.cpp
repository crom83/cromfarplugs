#include "stdafx.h"
#include "memory.cpp"
#include "lzss.c"
#include "lzo/minilzo.c"
#include "pkware/blast.c"
#include "DAT.cpp"
#include "MH.cpp"
#include "NFK.cpp"
#include "LOD.cpp"
#include "DAT1.cpp"
#include "ARK.cpp"
#include "BH2k.cpp"
#include "TITBIT.cpp"
#include "BFS.cpp"
#include "UBISOFT_BF.cpp"
#include "neverhood.cpp"

enum {
    GRP_DAT,
    GRP_MH,
    GRP_NFK,
    GRP_LOD,
    GRP_DAT1,
    GRP_ARK,
    GRP_BH2K,
    GRP_TITBIT,
    GRP_BFS,
    GRP_UBISOFT_BF,
    GRP_NEVERHOOD,
    GRP_END
};

long WINAPI PackCheck(short Fmt, const char * FileName, PUCHAR BaseString, long readed, long *HeadOffset, long *SomeOffset, long *FileSize, long *FileOffset, struct RIP_FILE_INFO **finfo, long *FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_DAT:    return PackCheckDAT   (FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_MH:     return PackCheckMH    (FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_NFK:    return PackCheckNFK   (FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_LOD:    return PackCheckLOD   (FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_DAT1:   return PackCheckDAT1  (FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_ARK:    return PackCheckARK   (FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_BH2K:   return PackCheckBH2K  (FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_TITBIT: return PackCheckTITBIT(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_BFS:    return PackCheckBFS   (FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_UBISOFT_BF:    return PackCheckUBISOFT_BF(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_NEVERHOOD:     return PackCheckNEVERHOOD (FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    }
    return -1;
}

BOOL WINAPI PackCheckFree(struct RIP_FILE_INFO *finfo, long FilesNumber, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    for (int i=0; i<FilesNumber; i++)
    {
        my_free(finfo[i].FullFileName);
        if(finfo[i].ConvFullFileName)
            my_free(finfo[i].ConvFullFileName);
        my_free(finfo[i].Description);
        if (finfo[i].UserData)
            my_free((long*)finfo[i].UserData);
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
    RipNfo->KnownFormats[0].RegPath      = my_strdup("enableFalloutDAT");
    RipNfo->KnownFormats[0].OptionString = my_strdup("Fallout 2 DAT package");
    RipNfo->KnownFormats[0].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[0].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[1].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[1].RegPath      = my_strdup("enableMoorhuhnDAT");
    RipNfo->KnownFormats[1].OptionString = my_strdup("Moorhuhn DAT package");
    RipNfo->KnownFormats[1].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[1].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[2].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[2].RegPath      = my_strdup("enableNeedForKill");
    RipNfo->KnownFormats[2].OptionString = my_strdup("Need For Kill D package");
    RipNfo->KnownFormats[2].Version      = func->Char2Ver("0.20 beta 5");
    RipNfo->KnownFormats[2].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[3].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[3].RegPath      = my_strdup("enableHofMM3LOD");
    RipNfo->KnownFormats[3].OptionString = my_strdup("Heroes of M&M 3 LOD package");
    RipNfo->KnownFormats[3].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[3].RipperVer    = func->Char2Ver("0.20 alpha 2");

  RipNfo->KnownFormats[4].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[4].RegPath      = my_strdup("enableFallout1DAT");
    RipNfo->KnownFormats[4].OptionString = my_strdup("Fallout DAT package");
    RipNfo->KnownFormats[4].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[4].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[5].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[5].RegPath      = my_strdup("enableMotoGTARK");
    RipNfo->KnownFormats[5].OptionString = my_strdup("MotoGT ARK package");
    RipNfo->KnownFormats[5].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[5].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[6].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[6].RegPath      = my_strdup("enableBH2K");
    RipNfo->KnownFormats[6].OptionString = my_strdup("Beachhead 2000 IMG package");
    RipNfo->KnownFormats[6].Version      = func->Char2Ver("0.20 alpha 1");
    RipNfo->KnownFormats[6].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[7].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[7].RegPath      = my_strdup("enableTITBIT");
    RipNfo->KnownFormats[7].OptionString = my_strdup("Nuclear Titbit EZD package");
    RipNfo->KnownFormats[7].Version      = func->Char2Ver("0.20 alpha 1");
    RipNfo->KnownFormats[7].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[8].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[8].RegPath      = my_strdup("enableBFS");
    RipNfo->KnownFormats[8].OptionString = my_strdup("Flat Out BFS package");
    RipNfo->KnownFormats[8].Version      = func->Char2Ver("0.20 alpha 1");
    RipNfo->KnownFormats[8].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[9].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[9].RegPath      = my_strdup("enableUBISOFT_BF");
    RipNfo->KnownFormats[9].OptionString = my_strdup("Ubisoft BF package");
    RipNfo->KnownFormats[9].Version      = func->Char2Ver("0.20 alpha 1");
    RipNfo->KnownFormats[9].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[10].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[10].RegPath      = my_strdup("enableNEVERHOOD");
    RipNfo->KnownFormats[10].OptionString = my_strdup("Neverhood BLB package");
    RipNfo->KnownFormats[10].Version      = func->Char2Ver("0.20 alpha 1");
    RipNfo->KnownFormats[10].RipperVer    = func->Char2Ver("0.20 alpha 2");

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

BOOL WINAPI GetFile(short Fmt, const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize,  long UnpSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_DAT:
    case GRP_MH:
    case GRP_LOD:
    case GRP_TITBIT:
        return GetFileDAT(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
        break;
    case GRP_NFK:
        return GetFileDAT(PackName, FileName, PathName, StartOffset+64, FileSize-64, UnpSize, func, flags);
        break;
    case GRP_DAT1:
        return GetFileDAT1(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
        break;
    case GRP_ARK:
        return GetFileARK(PackName, FileName, PathName, StartOffset+4, FileSize-4, UnpSize, func, flags);
        break;
    case GRP_BH2K:
        return GetFileDAT(PackName, FileName, PathName, StartOffset+4, FileSize-4, UnpSize, func, flags);
        break;
    case GRP_BFS:
        if (FileSize != UnpSize)
           return GetFileDAT(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
        else return -1;
        break;
    case GRP_UBISOFT_BF:
        return GetFileUBISOFT_BF(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
    break;
    case GRP_NEVERHOOD:
        return GetFileNEVERHOOD(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
    break;
    }
    return -1;
}

BOOL WINAPI GetFAT (short Fmt, const char * PackName, long Offset, long Size, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_NFK:   return GetFATNFK(PackName, Offset, Size, FAT, func, flags);
    }
    return -1;
}

BOOL WINAPI PutFile(short Fmt, const char * PackName, const char * FileName, const char * PathName, const char * DiskPathName, long * StartOffset, long * FileSize, long * UnpSize, long Offset, long Size, void ** FAT, BOOL Compress, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_NFK:   return PutFileNFK(PackName, FileName, PathName, DiskPathName, *StartOffset, *FileSize, *UnpSize, Offset, Size, FAT, Compress, func, flags);
    }
    return -1;
}

BOOL WINAPI DelFile(short Fmt, const char * PackName, const char * FileName, long StartOffset, long FileSize, long UnpSize, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_NFK:   return DelFileNFK(PackName, FileName, StartOffset, FileSize, UnpSize, FAT, func, flags);
    }
    return -1;
}

BOOL WINAPI FreeFAT (short Fmt, const char * PackName, const char * TmpName, long Offset, long Size, long WorkFileSize, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_NFK:   return FreeFATNFK(PackName, TmpName, Offset, Size, WorkFileSize, FAT, func, flags);
    }
    return -1;
}