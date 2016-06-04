#include "stdafx.h"
#include "memory.cpp"
#include "PAK.cpp"
#include "WAD1.cpp"
#include "WAD2.cpp"
#include "WAD3.cpp"
#include "DUKE.cpp"
#include "CHASM.cpp"
#include "ZULU.cpp"
#include "DUNE2.cpp"
#include "MH.cpp"
#include "DemnStar.cpp"
#include "RedF.cpp"
#include "VIV.cpp"
#include "PFF0.cpp"
#include "PETKA.cpp"
#include "RES.cpp"
#include "DWEEP.cpp"
#include "MW4.cpp"
#include "AGG.cpp"
#include "UFOS.cpp"
#include "GTA3.cpp"
#include "DK2WAD.cpp"
#include "DK2SDT.cpp"
#include "DK1DAT.cpp"
#include "H3VID.cpp"
#include "H3SND.cpp"
#include "GTA3GXT.cpp"
#include "GTAVCGXT.cpp"
#include "GTAVCADF.cpp"
#include "SH.cpp"
#include "GTA3SFX.cpp"
#include "MH2.cpp"
#include "RE3.cpp"
#include "LEMBOX.cpp"
#include "CRIMSON.cpp"
#include "DSTORM.cpp"
#include "MK3LCD.cpp"
#include "FM03.cpp"
#include "FM05.cpp"
//#include "EAASF.cpp"
//#include "DIR.cpp"

enum {
    GRP_PAK,
    GRP_WAD1,
    GRP_WAD2,
    GRP_WAD3,
    GRP_DUKE,
    GRP_CHASM,
    GRP_ZULU,
    GRP_DUNE2,
    GRP_MH,
    GRP_DEMS,
    GRP_REDF,
    GRP_VIV,
    GRP_PFF0,
    GRP_PETKA,
    GRP_RES,
    GRP_DWEEP,
    GRP_MW4,
    GRP_AGG,
    GRP_UFOS,
    GRP_GTA3,
    GRP_DK2WAD,
    GRP_DK2SDT,
    GRP_DK1DAT,
    GRP_H3VID,
    GRP_H3SND,
    GRP_GTA3GXT,
    GRP_GTAVCGXT,
    GRP_GTAVCADF,
    GRP_SH,
    GRP_GTA3SFX,
    GRP_MH2,
    GRP_RE3,
    GRP_LEMBOX,
    GRP_CRIMSON,
    GRP_DSTORM,
    GRP_MK3LCD,
    GRP_FM03,
	GRP_FM05,
//  GRP_EAASF,
//  GRP_DIR,
    GRP_END
};

long WINAPI PackCheck(short Fmt, const char * FileName, PUCHAR BaseString, long readed, long *HeadOffset, long *SomeOffset, long *FileSize, long *FileOffset, struct RIP_FILE_INFO **finfo, long *FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_PAK:     return PackCheckPAK(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_WAD1:    return PackCheckWAD1(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_WAD2:    return PackCheckWAD2(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_WAD3:    return PackCheckWAD3(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_DUKE:    return PackCheckDUKE(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_CHASM:   return PackCheckCHASM(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_ZULU:    return PackCheckZULU(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_DUNE2:   return PackCheckDUNE2(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_MH:      return PackCheckMH(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_DEMS:    return PackCheckDEMS(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_REDF:    return PackCheckREDF(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_VIV:     return PackCheckVIV(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_PFF0:    return PackCheckPFF0(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_PETKA:   return PackCheckPETKA(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_RES:     return PackCheckRES(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_DWEEP:   return PackCheckDWEEP(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_MW4:     return PackCheckMW4(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_AGG:     return PackCheckAGG(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_UFOS:    return PackCheckUFOS(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_GTA3:    return PackCheckGTA3(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_DK2WAD:  return PackCheckDK2WAD(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_DK2SDT:  return PackCheckDK2SDT(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_DK1DAT:  return PackCheckDK1DAT(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_H3VID:   return PackCheckH3VID(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_H3SND:   return PackCheckH3SND(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_GTA3GXT: return PackCheckGTA3GXT(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_GTAVCGXT:return PackCheckGTAVCGXT(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_GTAVCADF:return PackCheckGTAVCADF(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_SH:      return PackCheckSH(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_GTA3SFX: return PackCheckGTA3SFX(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_MH2:     return PackCheckMH2(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_RE3:     return PackCheckRE3(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_LEMBOX:  return PackCheckLEMBOX(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_CRIMSON: return PackCheckCRIMSON(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_DSTORM:  return PackCheckDSTORM(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_MK3LCD:  return PackCheckMK3LCD(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    case GRP_FM03:    return PackCheckFM03(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
	case GRP_FM05:    return PackCheckFM05(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
//  case GRP_EAASF:   return PackCheckEAASF(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
//  case GRP_DIR:     return PackCheckDIR(FileName, BaseString, readed, *HeadOffset, *SomeOffset, *FileSize, *FileOffset, finfo, *FilesNumber, WorkFileSize, func, flags);
    }
    return -1;
}

BOOL WINAPI GetFAT (short Fmt, const char * PackName, long Offset, long Size, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_PAK:     return GetFATPAK(PackName, Offset, Size, FAT, func, flags);
    case GRP_GTAVCADF:return GetFATGTAVCADF(PackName, Offset, Size, FAT, func, flags);
//    case GRP_EAASF:   return GetFATEAASF(PackName, Offset, Size, FAT, func, flags);
    }
    return -1;
}

BOOL WINAPI PutFile(short Fmt, const char * PackName, const char * FileName, const char * PathName, const char * DiskPathName, long * StartOffset, long * FileSize, long * UnpSize, long Offset, long Size, void ** FAT, BOOL Compress, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_PAK:     return PutFilePAK(PackName, FileName, PathName, DiskPathName, *StartOffset, *FileSize, *UnpSize, Offset, Size, FAT, Compress, func, flags);
    case GRP_GTAVCADF:return PutFileGTAVCADF(PackName, FileName, PathName, DiskPathName, *StartOffset, *FileSize, *UnpSize, Offset, Size, FAT, Compress, func, flags);
//    case GRP_EAASF:   return PutFileEAASF(PackName, FileName, PathName, DiskPathName, *StartOffset, *FileSize, *UnpSize, Offset, Size, FAT, Compress, func, flags);
    }
    return -1;
}

BOOL WINAPI DelFile(short Fmt, const char * PackName, const char * FileName, long StartOffset, long FileSize, long UnpSize, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_PAK:     return DelFilePAK(PackName, FileName, StartOffset, FileSize, UnpSize, FAT, func, flags);
    case GRP_GTAVCADF:return DelFileGTAVCADF(PackName, FileName, StartOffset, FileSize, UnpSize, FAT, func, flags);
//    case GRP_EAASF:   return DelFileEAASF(PackName, FileName, StartOffset, FileSize, UnpSize, FAT, func, flags);
    }
    return -1;
}

BOOL WINAPI FreeFAT (short Fmt, const char * PackName, const char * TmpName, long Offset, long Size, long WorkFileSize, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_PAK:     return FreeFATPAK(PackName, TmpName, Offset, Size, WorkFileSize, FAT, func, flags);
    case GRP_GTAVCADF:return FreeFATGTAVCADF(PackName, TmpName, Offset, Size, WorkFileSize, FAT, func, flags);
//    case GRP_EAASF:   return FreeFATEAASF(PackName, TmpName, Offset, Size, WorkFileSize, FAT, func, flags);
    }
    return -1;
}

BOOL WINAPI PackCheckFree(struct RIP_FILE_INFO *finfo, long FilesNumber, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    for (int i=0; i<FilesNumber; i++)
    {
        my_free(finfo[i].FullFileName);
        if (finfo[i].ConvFullFileName)
            my_free(finfo[i].ConvFullFileName);
        my_free(finfo[i].Description);
    }
    my_free(finfo);
    return TRUE;
}

BOOL WINAPI PluginInfo (struct RIP_NFO *RipNfo, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    RipNfo->Copyright = my_strdup("(c) Kubyshev Vladimir, 2003");
    RipNfo->nKnownFormats = GRP_END;
    RipNfo->KnownFormats  = (RIP_NFO_FMT*)my_malloc(sizeof(RIP_NFO_FMT)*RipNfo->nKnownFormats);

    RipNfo->KnownFormats[0].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[0].RegPath      = my_strdup("enableQPAK");
    RipNfo->KnownFormats[0].OptionString = my_strdup("Quake PAK package");
    RipNfo->KnownFormats[0].Version      = func->Char2Ver("0.21 beta 3");
    RipNfo->KnownFormats[0].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[1].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[1].RegPath      = my_strdup("enableWAD1");
    RipNfo->KnownFormats[1].OptionString = my_strdup("Doom WAD package");
    RipNfo->KnownFormats[1].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[1].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[2].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[2].RegPath      = my_strdup("enableWAD2");
    RipNfo->KnownFormats[2].OptionString = my_strdup("Quake WAD package");
    RipNfo->KnownFormats[2].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[2].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[3].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[3].RegPath      = my_strdup("enableWAD3");
    RipNfo->KnownFormats[3].OptionString = my_strdup("Half-Life WAD package");
    RipNfo->KnownFormats[3].Version      = func->Char2Ver("0.20 beta 7");
    RipNfo->KnownFormats[3].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[4].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[4].RegPath      = my_strdup("enableDUKE");
    RipNfo->KnownFormats[4].OptionString = my_strdup("Duke3D GRP package");
    RipNfo->KnownFormats[4].Version      = func->Char2Ver("0.20 beta 3");
    RipNfo->KnownFormats[4].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[5].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[5].RegPath      = my_strdup("enableCHASM");
    RipNfo->KnownFormats[5].OptionString = my_strdup("Chasm BIN package");
    RipNfo->KnownFormats[5].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[5].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[6].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[6].RegPath      = my_strdup("enableZULU");
    RipNfo->KnownFormats[6].OptionString = my_strdup("Zulu Assault CAT package");
    RipNfo->KnownFormats[6].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[6].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[7].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[7].RegPath      = my_strdup("enableDUNE2");
    RipNfo->KnownFormats[7].OptionString = my_strdup("Dune 2 PAK package");
    RipNfo->KnownFormats[7].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[7].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[8].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[8].RegPath      = my_strdup("enableMOORHUHN3");
    RipNfo->KnownFormats[8].OptionString = my_strdup("Moorhuhn3+ DAT package");
    RipNfo->KnownFormats[8].Version      = func->Char2Ver("0.20 beta 3");
    RipNfo->KnownFormats[8].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[9].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[9].RegPath      = my_strdup("enableDEMONSTAR2");
    RipNfo->KnownFormats[9].OptionString = my_strdup("DemonStar GLB2 package");
    RipNfo->KnownFormats[9].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[9].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[10].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[10].RegPath      = my_strdup("enableREDFACT");
    RipNfo->KnownFormats[10].OptionString = my_strdup("Red Faction VPP package");
    RipNfo->KnownFormats[10].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[10].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[11].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[11].RegPath      = my_strdup("enableNFSVIV");
    RipNfo->KnownFormats[11].OptionString = my_strdup("NFS VIV package");
    RipNfo->KnownFormats[11].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[11].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[12].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[12].RegPath      = my_strdup("enableNOVAPFF");
    RipNfo->KnownFormats[12].OptionString = my_strdup("NovaLogic PFF package");
    RipNfo->KnownFormats[12].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[12].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[13].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[13].RegPath      = my_strdup("enablePETKA");
    RipNfo->KnownFormats[13].OptionString = my_strdup("VICH&&PETKA RSS package");
    RipNfo->KnownFormats[13].Version      = func->Char2Ver("0.20 beta 3");
    RipNfo->KnownFormats[13].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[14].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[14].RegPath      = my_strdup("enableALLODRES");
    RipNfo->KnownFormats[14].OptionString = my_strdup("Allods RES package");
    RipNfo->KnownFormats[14].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[14].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[15].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[15].RegPath      = my_strdup("enableDWEEPDAT");
    RipNfo->KnownFormats[15].OptionString = my_strdup("Dweep DAT package");
    RipNfo->KnownFormats[15].Version      = func->Char2Ver("0.20 alpha 3");
    RipNfo->KnownFormats[15].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[16].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[16].RegPath      = my_strdup("enableMW4");
    RipNfo->KnownFormats[16].OptionString = my_strdup("Mech Warriors 4 MW4 package");
    RipNfo->KnownFormats[16].Version      = func->Char2Ver("0.20 alpha 2");
    RipNfo->KnownFormats[16].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[17].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[17].RegPath      = my_strdup("enableAGG");
    RipNfo->KnownFormats[17].OptionString = my_strdup("Heroes of M&M 2 AGG package");
    RipNfo->KnownFormats[17].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[17].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[18].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[18].RegPath      = my_strdup("enableUFOS");
    RipNfo->KnownFormats[18].OptionString = my_strdup("UFOs DAT package");
    RipNfo->KnownFormats[18].Version      = func->Char2Ver("0.20 alpha 2");
    RipNfo->KnownFormats[18].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[19].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[19].RegPath      = my_strdup("enableGTA3");
    RipNfo->KnownFormats[19].OptionString = my_strdup("GTA3 IMG package");
    RipNfo->KnownFormats[19].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[19].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[20].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[20].RegPath      = my_strdup("enableDK2WAD");
    RipNfo->KnownFormats[20].OptionString = my_strdup("Dungeon Keeper 2 WAD package");
    RipNfo->KnownFormats[20].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[20].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[21].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[21].RegPath      = my_strdup("enableDK2SDT");
    RipNfo->KnownFormats[21].OptionString = my_strdup("Dungeon Keeper 2 SDT package");
    RipNfo->KnownFormats[21].Version      = func->Char2Ver("0.20 beta 3");
    RipNfo->KnownFormats[21].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[22].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[22].RegPath      = my_strdup("enableDK1DAT");
    RipNfo->KnownFormats[22].OptionString = my_strdup("Dungeon Keeper 1 DAT package");
    RipNfo->KnownFormats[22].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[22].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[23].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[23].RegPath      = my_strdup("enableH3VID");
    RipNfo->KnownFormats[23].OptionString = my_strdup("Heroes of M&M 3 VID package");
    RipNfo->KnownFormats[23].Version      = func->Char2Ver("0.20 beta 3");
    RipNfo->KnownFormats[23].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[24].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[24].RegPath      = my_strdup("enableH3SND");
    RipNfo->KnownFormats[24].OptionString = my_strdup("Heroes of M&M 3 SND package");
    RipNfo->KnownFormats[24].Version      = func->Char2Ver("0.20 beta 3");
    RipNfo->KnownFormats[24].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[25].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[25].RegPath      = my_strdup("enableGTA3GXT");
    RipNfo->KnownFormats[25].OptionString = my_strdup("GTA3 GXT package");
    RipNfo->KnownFormats[25].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[25].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[26].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[26].RegPath      = my_strdup("enableGTAVCGXT");
    RipNfo->KnownFormats[26].OptionString = my_strdup("GTA VICE CITY GXT package");
    RipNfo->KnownFormats[26].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[26].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[27].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[27].RegPath      = my_strdup("enableGTAVCADF");
    RipNfo->KnownFormats[27].OptionString = my_strdup("GTA VICE CITY ADF radio");
    RipNfo->KnownFormats[27].Version      = func->Char2Ver("0.20 beta 4");
    RipNfo->KnownFormats[27].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[28].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[28].RegPath      = my_strdup("enableSH");
    RipNfo->KnownFormats[28].OptionString = my_strdup("Speed Haste JCL package");
    RipNfo->KnownFormats[28].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[28].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[29].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[29].RegPath      = my_strdup("enableGTA3SFX");
    RipNfo->KnownFormats[29].OptionString = my_strdup("GTA3 RAW package");
    RipNfo->KnownFormats[29].Version      = func->Char2Ver("0.20 beta 4");
    RipNfo->KnownFormats[29].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[30].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[30].RegPath      = my_strdup("enableMH2");
    RipNfo->KnownFormats[30].OptionString = my_strdup("Moorhuhn2+ package");
    RipNfo->KnownFormats[30].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[30].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[31].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[31].RegPath      = my_strdup("enableRE3");
    RipNfo->KnownFormats[31].OptionString = my_strdup("Resident Evil 3 DAT package");
    RipNfo->KnownFormats[31].Version      = func->Char2Ver("0.20 alpha 2");
    RipNfo->KnownFormats[31].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[32].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[32].RegPath      = my_strdup("enableLEMBOX");
    RipNfo->KnownFormats[32].OptionString = my_strdup("Lemmings Revolution BOX package");
    RipNfo->KnownFormats[32].Version      = func->Char2Ver("0.20 alpha 1");
    RipNfo->KnownFormats[32].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[33].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[33].RegPath      = my_strdup("enableCRIMSON");
    RipNfo->KnownFormats[33].OptionString = my_strdup("Crimsonland PAQ package");
    RipNfo->KnownFormats[33].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[33].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[34].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[34].RegPath      = my_strdup("enableDSTORM");
    RipNfo->KnownFormats[34].OptionString = my_strdup("Desert storm DAT package");
    RipNfo->KnownFormats[34].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[34].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[35].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[35].RegPath      = my_strdup("enableMK3LCD");
    RipNfo->KnownFormats[35].OptionString = my_strdup("Mortal Kombat 3 LCD sound package");
    RipNfo->KnownFormats[35].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[35].RipperVer    = func->Char2Ver("0.20 alpha 2");

    RipNfo->KnownFormats[36].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[36].RegPath      = my_strdup("enableFuturemark03");
    RipNfo->KnownFormats[36].OptionString = my_strdup("Futuremark '03 DAT package");
    RipNfo->KnownFormats[36].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[36].RipperVer    = func->Char2Ver("0.20 alpha 2");

	RipNfo->KnownFormats[37].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[37].RegPath      = my_strdup("enableFuturemark05");
    RipNfo->KnownFormats[37].OptionString = my_strdup("Futuremark '05 DAT package");
    RipNfo->KnownFormats[37].Version      = func->Char2Ver("0.20 beta 2");
    RipNfo->KnownFormats[37].RipperVer    = func->Char2Ver("0.20 alpha 2");

/*    RipNfo->KnownFormats[35].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[35].RegPath      = my_strdup("enableEAASF");
    RipNfo->KnownFormats[35].OptionString = my_strdup("EA ASF format");
    RipNfo->KnownFormats[35].Version      = func->Char2Ver("0.20 beta 1");
    RipNfo->KnownFormats[35].RipperVer    = func->Char2Ver("0.20 alpha 2");
*/
/*  RipNfo->KnownFormats[30].Type         = RIP_INFO_TYPE_PACK;
    RipNfo->KnownFormats[30].RegPath      = my_strdup("enableComDIR");
    RipNfo->KnownFormats[30].OptionString = my_strdup("Commandos DIR package");
    RipNfo->KnownFormats[11].Version      = 0x00140200;
    RipNfo->KnownFormats[11].RipperVer    = 0x00140200;
    */

    return TRUE;
}

BOOL WINAPI PluginInfoFree (struct RIP_NFO *RipNfo, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    for (int i = 0; i<RipNfo->nKnownFormats; i++){
        my_free(RipNfo->KnownFormats[i].RegPath);
        my_free(RipNfo->KnownFormats[i].OptionString);
    }
    my_free(RipNfo->KnownFormats);
    my_free(RipNfo->Copyright);

    return TRUE;
}

int WINAPI GetFile(short Fmt, const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize,  long UnpSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    switch (Fmt){
    case GRP_GTAVCADF:
        return GetFileGTAVCADF(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
        break;
    /*case GRP_EAASF:
        return GetFileEAASF(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
        break;*/
    case GRP_GTA3SFX:
        return GetFileGTA3SFX(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
        break;
    case GRP_WAD3:
        return GetFileWAD3(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
        break;
    case GRP_MH2:
        return GetFileMH2(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
        break;
    case GRP_MK3LCD:
        return GetFileMK3LCD(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
        break;
    case GRP_FM03:
        return GetFileFM03(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
        break;
	case GRP_FM05:
        return GetFileFM05(PackName, FileName, PathName, StartOffset, FileSize, UnpSize, func, flags);
        break;
    }
    return FALSE;
}