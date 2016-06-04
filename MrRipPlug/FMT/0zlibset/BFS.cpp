long PackCheckBFS(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    char DirName[MAX_PATH];

#pragma pack(1)
    struct {
        long Type;
        long Offset;
        long RealSize;
        long PackedSize;
        long Data;
        short NameLen;
    }DatInfo;
#pragma pack(8)

    unsigned char           t[] = {'b','f','s','1',0x05,0x05,0x04,0x20};
    long rd, SOffset, FOffset, TOffset, DirLen, filestotal;

    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+8;

    rd =  func->ReadFromFile(FileName, FOffset, &DirLen, sizeof DirLen);
    if (rd < sizeof DirLen) return -1;

    FileSize = DirLen+1;

    if (DirLen >= WorkFileSize) return -1;
    FOffset += rd;

    rd =  func->ReadFromFile(FileName, FOffset, &filestotal, 4);
    if (rd < 4) return -1;
    FOffset += rd;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*filestotal);
    if (fi == NULL) return -1;

    *finfo = fi;
    FilesNumber = filestotal;

    for (int i = 0; i<FilesNumber; i++){
        rd =  func->ReadFromFile(FileName, FOffset, &TOffset, 4);
        if (rd < 4) return -1;
        FOffset += rd;

        //Read FAT record
        rd =  func->ReadFromFile(FileName, TOffset, &DatInfo, sizeof DatInfo);
        if (rd < sizeof DatInfo) return -1;
        TOffset += rd;

        rd =  func->ReadFromFile(FileName, TOffset, &DirName, DatInfo.NameLen);
        if (rd < DatInfo.NameLen) return -1;
        DirName[DatInfo.NameLen] = 0;

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = DatInfo.Offset;
        fi[i].FileSize    = DatInfo.PackedSize;
        fi[i].UnpSize     = DatInfo.RealSize;
/*    fi[i].UserData    = (long*)my_malloc(sizeof(long));
    memcpy(fi[i].UserData, &DatInfo.Type, sizeof DatInfo.Type);
    fi[i].UserDataLen = sizeof DatInfo.Type;*/


        FileSize += DatInfo.PackedSize;

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);
            }
            my_free(fi);
            return -1;
        }

        fi[i].FullFileName = (char*)my_malloc(strlen(DirName)+1);
        strcpy(fi[i].FullFileName, DirName);
        fi[i].Description = (char*)my_malloc(strlen("")+1);
        strcpy(fi[i].Description, "");
    }

    FileOffset = SOffset;
    return WorkFileSize;
}
