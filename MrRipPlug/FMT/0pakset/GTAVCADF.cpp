#pragma pack(1)
    struct GTAVCADF_FATType{
        long StructSize;
        char * AddFileName;
        BOOL Compress;
        ULONG offset;
        ULONG size;
    };
#pragma pack(8)

int GetFileGTAVCADF(const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize, long UnpSize, struct RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{
    char buf [16384];
    long rd, sv;
    char * dstpath = my_strdup(PathName, strlen(FileName)+2);
    strcat(dstpath, "\\");
    strcat(dstpath, FileName);
    char msg[MAX_PATH] = "Decoding\n";
    char msg2[MAX_PATH];

    if (!flags->Silent){
        strcat(msg, FileName);
        strcat(msg, "\n");
    }

    for (int i = StartOffset; i<FileSize+StartOffset;){
        rd = func->ReadFromFile(PackName, i, buf, sizeof buf);
        if (rd == -1) {
            my_free(dstpath);
            return FALSE;
        }
        if (rd > 0) {
            i += rd;
            for (int j = 0; j<rd; j++) buf[j] ^= 0x22;
            sv = func->AppendToFile(dstpath, buf, rd);
            if (sv < rd) {
                my_free(dstpath);
                return FALSE;
            }
        }

        if (func->CheckKey(VK_ESCAPE)){
            my_free(dstpath);
            return 2;
        }
        if (!flags->Silent){
            sprintf(msg2, "%s%d/%d", msg, i-StartOffset, FileSize);
            func->ShowMessage(FALSE, FALSE, FALSE, TRUE, (const char * const *)msg2, 0, 0);
        }
    }
    my_free(dstpath);
    return TRUE;
}

BOOL WINAPI GetFATGTAVCADF (const char * PackName, long Offset, long Size, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    GTAVCADF_FATType * fat = (GTAVCADF_FATType*)my_malloc(sizeof(GTAVCADF_FATType));
    fat->StructSize = sizeof(GTAVCADF_FATType);
    fat->AddFileName = NULL;
    *FAT = fat;

    return TRUE;
}

BOOL WINAPI PutFileGTAVCADF(const char * PackName, const char * FileName, const char * PathName, const char * DiskPathName, long & StartOffset, long & FileSize, long & UnpSize, long Offset, long Size, void ** FAT, BOOL Compress, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    GTAVCADF_FATType * fat = (GTAVCADF_FATType*)*FAT;

    fat->AddFileName = (char*)my_malloc(strlen(DiskPathName)+strlen(FileName)+2);
    fat->Compress = Compress;

    if (strlen(PathName)>0){
        strcpy(fat->AddFileName, DiskPathName);
        strcat(fat->AddFileName, "\\");
        strcat(fat->AddFileName, FileName);
    } else {
        strcpy(fat->AddFileName, FileName);
    }

    StartOffset = 0;
    UnpSize = FileSize;
    fat->offset = StartOffset;
    fat->size = UnpSize;

    return TRUE;
}

BOOL WINAPI DelFileGTAVCADF(const char * PackName, const char * FileName, long StartOffset, long FileSize, long UnpSize, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{
    return TRUE;
}

BOOL WINAPI FreeFATGTAVCADF (const char * PackName, const char * TmpName, long Offset, long Size, long WorkFileSize, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{

    GTAVCADF_FATType * fat = (GTAVCADF_FATType*)*FAT;

    if (fat->AddFileName == NULL){
        my_free(fat);
        return FALSE;
    }

    //Copy from PackName to TmpName unused space
    if (Offset>0) func->FileToFileCopy(PackName, TmpName, 0, Offset, 0);

    char buf [16384];
    long rd, sv;
    char msg[MAX_PATH] = "Encoding\n";


    for (int i = fat->offset; i<fat->size+fat->offset;){
        rd = func->ReadFromFile(fat->AddFileName, i, buf, sizeof buf);
        if (rd == -1) {
            my_free(fat->AddFileName);
            my_free(fat);
            return FALSE;
        }
        if (rd > 0){
            i += rd;
            if (fat->Compress)
                for (int j = 0; j<rd; j++) buf[j] ^= 0x22;
            sv = func->AppendToFile(TmpName, buf, rd);
            if (sv < rd) {
                my_free(fat->AddFileName);
                my_free(fat);
                return FALSE;
            }
        }
    }

    //Save unused space
    if (WorkFileSize-(Offset+Size)>0)
        func->FileToFileCopy(PackName, TmpName, Offset+Size, WorkFileSize-(Offset+Size), -1);

    my_free(fat->AddFileName);
    my_free(fat);

#ifdef _DEBUG
    DumpUnfreed();
#endif
    return TRUE;
}

long PackCheckGTAVCADF(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    char fname[MAX_PATH];
    char fext[3];
    unsigned char tt[] = {0xDD};
    char* temp;

    RIP_FILE_INFO           * fi;

    long fres = func->Find(tt, BaseString, sizeof tt, readed);
    if (fres == -1) return -1;

    if ((FileOffset != 0)||(fres!=0)) return -1;

    temp = func->ExtractName(FileName);

    strcpy(fname, temp);
    func->FreeMem(temp);
    strcpy(fext, fname+(strlen(fname)-3));

    if (strcmp(fext, "adf") != 0)
        if (strcmp(fext, "ADF") != 0) return -1;

    fname[strlen(fname)-3] = 0;
    strcat(fname, "mp3");

    FilesNumber = 1;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*FilesNumber);
    if (fi == NULL) return -1;
    *finfo = fi;

    memset(&fi[0], 0, sizeof(RIP_FILE_INFO));
    fi[0].StartOffset = 0;
    fi[0].FileSize = WorkFileSize;
    fi[0].FullFileName = func->ExtractName(FileName);
    fi[0].ConvFullFileName = my_strdup(fname);
    fi[0].Description = my_strdup("Crypted GTA VC radio file");

    FileOffset = 0;
    FileSize = WorkFileSize;
    return FileSize;
}
