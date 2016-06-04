long PackCheckDK1DAT(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{   

#pragma pack(1)
    struct FATInfo{
        char fn[18];
        long Offset;
        long Junk;
        long Size;        
        short Junk2;        
    }FATItem;
#pragma pack(8)   

    long rd, FOffset, DirEOffset, DirSOffset;
    unsigned char c = 0;
    bool WasError = false;
    int errorfrom;
        
    RIP_FILE_INFO           * fi;    

    FOffset = WorkFileSize-4;

    rd =  func->ReadFromFile(FileName, FOffset, &DirEOffset, 4);
    if (rd < 4) return -1;
    FOffset = DirEOffset;
    
    do {
        rd =  func->ReadFromFile(FileName, FOffset, &c, 1);
        if (rd < 1) return -1;
        FOffset += rd;
    } while (c==0||c==1||c==0xFF);


    rd =  func->ReadFromFile(FileName, FOffset-1, &DirSOffset, 4);
    if (rd < 4) return -1;
    DirSOffset += 32;
    FOffset = DirSOffset;
    
    if (DirEOffset>WorkFileSize||DirSOffset>WorkFileSize)
        return -1;
    
    FilesNumber = (DirEOffset-DirSOffset)/32;
    if (FilesNumber<=0) return -1;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*FilesNumber);
    if (fi == NULL) return -1;
    *finfo = fi;        
    
    for (int i = 0; i<FilesNumber; i++){                    
        rd =  func->ReadFromFile(FileName, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = FATItem.Offset;
        fi[i].FileSize    = FATItem.Size;        
        fi[i].UnpSize     = FATItem.Size;        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize||
            fi[i].FileSize<=0||
            fi[i].StartOffset<0||
            fi[i].StartOffset+fi[i].FileSize<=0) {
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
    return WorkFileSize;
}
