long PackCheckDK2SDT(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATInfo{
        long Junk;        
        long Size;
        char fn[16];
        long Junk2[4];        
    }FATItem;
#pragma pack(8)   

    long rd, FOffset, f_Offset;
    long nFiles;
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;    
    
    FOffset = 0;

    if (FileOffset>0) return -1;

    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, sizeof nFiles);
    if (rd < sizeof nFiles) return -1;    
    FOffset += rd;

    if (nFiles*sizeof FATItem>WorkFileSize) return -1;
    if (nFiles<1) return -1;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;        

    FilesNumber = nFiles;        

    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &f_Offset, sizeof f_Offset);
        if (rd < sizeof f_Offset)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd;

        rd =  func->ReadFromFile(FileName, f_Offset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem) return -1;                

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = f_Offset+40;
        fi[i].FileSize    = FATItem.Size;
        fi[i].UnpSize     = FATItem.Size;                

        if ((DWORD)fi[i].StartOffset+(DWORD)fi[i].FileSize>(DWORD)WorkFileSize||strlen(FATItem.fn)==0) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_strdup(FATItem.fn);
        fi[i].Description = my_strdup("");                          
    }

    if (WasError){
        for (int j=0; j<errorfrom; j++){
            my_free(fi[j].FullFileName);                
            my_free(fi[j].Description);        
        }
        my_free(fi);
        fi = NULL;
        return -1;
    }
    
    FileOffset = 0;
    FileSize = WorkFileSize;
    return FileSize;    
}