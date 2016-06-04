long PackCheckDAT(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long FNsize;
    char DirName[MAX_PATH];    

#pragma pack(1)
    struct DatInfo{
        char Type;
        long RealSize;
        long PackedSize;
        long Offset;
    }FOUT_DAT;
#pragma pack(8)

    long rd, FOffset, DatLen, DirLen, filestotal;    
    
    RIP_FILE_INFO           * fi;

    if (FileOffset>0) return -1;

    FOffset = WorkFileSize-4;

    rd =  func->ReadFromFile(FileName, FOffset, &DatLen, sizeof DatLen);
    if (rd < sizeof DatLen) return -1;
    
    if (DatLen != WorkFileSize) return -1;

    FOffset = WorkFileSize-8;

    rd =  func->ReadFromFile(FileName, FOffset, &DirLen, 4);
    if (rd < 4) return -1;

    if (DirLen > WorkFileSize||DirLen<=0) return -1;

    FOffset = DatLen-DirLen - 8;

    rd =  func->ReadFromFile(FileName, FOffset, &filestotal, 4);
    if (rd < 4) return -1;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*filestotal);
    if (fi == NULL) return -1;

    *finfo = fi;
    FilesNumber = filestotal;

    FOffset += 4;

    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &FNsize, sizeof FNsize);
        if (rd < sizeof FNsize) return -1;

        FOffset += rd;

        rd =  func->ReadFromFile(FileName, FOffset, &DirName, FNsize);
        if (rd < FNsize) return -1;

        FOffset += rd;

        DirName[FNsize] = 0;

        rd =  func->ReadFromFile(FileName, FOffset, &FOUT_DAT, sizeof FOUT_DAT);
        if (rd < sizeof FOUT_DAT) return -1;
        
        FOffset += rd;
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = FOUT_DAT.Offset;
        fi[i].FileSize    = FOUT_DAT.PackedSize;
        fi[i].UnpSize     = FOUT_DAT.RealSize;
        
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
    
    FileOffset = 0;
    FileSize   = WorkFileSize;
    return WorkFileSize;    
}

BOOL GetFileDAT(const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize, long UnpSize, struct RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{
    unsigned char * bufIn;
    unsigned char * bufOut;
    char * dstpath;    
    long rd;    

    bufIn = (unsigned char *)my_malloc(FileSize);
    if (!bufIn) return FALSE;

    bufOut = (unsigned char *)my_malloc(UnpSize);
    if (!bufOut) {my_free(bufIn);return FALSE;}

    rd = func->ReadFromFile(PackName, StartOffset, bufIn, FileSize);
    if (rd < FileSize) return FALSE;
    
    if (Z_OK != uncompress(bufOut, (unsigned long *)&UnpSize, bufIn, FileSize)){
        my_free(bufIn);
        my_free(bufOut);
        return FALSE;
    }
    my_free(bufIn);
    dstpath = my_strdup(PathName, strlen(FileName)+2);
    strcat(dstpath, "\\");
    strcat(dstpath, FileName);
    if (func->SaveToFile(dstpath, 0, bufOut, UnpSize) != (long)UnpSize){
        my_free(dstpath);
        my_free(bufOut);
        return FALSE;
    }
    my_free(dstpath);
    my_free(bufOut);
    return TRUE;
}