long PackCheckARK(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{    
    char s[MAX_PATH];

#pragma pack(1)
    struct {
        long Type;
        long Offset;
        long PackedSize;
        long Junk;
    }FATItem;
#pragma pack(8)

    long rd, FOffset, DirCount, filestotal, Unp;
    
    RIP_FILE_INFO           * fi;

    if (FileOffset>0) return -1;

    FOffset = 0;
    
    //check file: first long value must be 0
    rd =  func->ReadFromFile(FileName, FOffset, &filestotal, sizeof filestotal);
    if (rd < sizeof filestotal) return -1;    

    if (filestotal > 0) return -1;

    FOffset += 4;

    rd =  func->ReadFromFile(FileName, FOffset, &DirCount, sizeof DirCount);
    if (rd < sizeof DirCount) return -1;    

    FOffset += 4;

    rd =  func->ReadFromFile(FileName, FOffset, &filestotal, sizeof filestotal);
    if (rd < sizeof filestotal) return -1;    

    if ((filestotal*16+16 > DirCount)||
		(filestotal == 0)) return -1;

    FOffset += 16;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*filestotal);
    if (fi == NULL) return -1;

    *finfo = fi;
    FilesNumber = filestotal;
    
    for (int i = 0; i<FilesNumber; i++){        
        rd =  func->ReadFromFile(FileName, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem) return -1;
        
        FOffset += rd;

        rd =  func->ReadFromFile(FileName, FATItem.Offset, &Unp, sizeof Unp);
        if (rd < sizeof Unp) return -1;

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = FATItem.Offset;
        fi[i].FileSize    = FATItem.PackedSize;
        fi[i].UnpSize     = Unp;
        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);                
            }
            my_free(fi);
            return -1;
        }        

        if (i< 10)           sprintf(s, "ark000000%d",  i);
        else if (i< 100)     sprintf(s, "ark00000%d",   i);
        else if (i< 1000)    sprintf(s, "ark0000%d",    i);
        else if (i< 10000)   sprintf(s, "ark000%d",     i);
        else if (i< 100000)  sprintf(s, "ark00%d",      i);
        else if (i< 1000000) sprintf(s, "ark0%d",       i);
        else                 sprintf(s, "ark%d",        i);

        fi[i].FullFileName = my_strdup(s);
        fi[i].Description = my_strdup("");        
    }                  
    
    FileOffset = 0;
    FileSize   = WorkFileSize;
    return WorkFileSize;    
}

BOOL GetFileARK(const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize, long UnpSize, struct RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{
    unsigned char * bufIn;
    unsigned char * bufOut;
    char * dstpath;    
    long rd, outpos = 0;    
    
    bufOut = (unsigned char *)my_malloc(UnpSize);
    if (!bufOut) {return FALSE;}

    bufIn = (unsigned char *)my_malloc(FileSize);
    if (!bufIn) return FALSE;       

    rd = func->ReadFromFile(PackName, StartOffset, bufIn, FileSize);
    if (rd < FileSize) return FALSE;
    
    Decode(bufIn, FileSize, bufOut, UnpSize, outpos);

    my_free(bufIn);
    
    dstpath = my_strdup(PathName, strlen(FileName)+2);
    strcat(dstpath, "\\");
    strcat(dstpath, FileName);
    if (func->SaveToFile(dstpath, 0, bufOut, UnpSize) != UnpSize){
        my_free(dstpath);
        my_free(bufOut);
        return FALSE;
    }
    my_free(dstpath);
    my_free(bufOut);
    return TRUE;
}