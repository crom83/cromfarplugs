long PackCheckAGG(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATInfo{
        long Junk;
        long StartOffset;        
        long Size;
    }FATItem;
#pragma pack(8)

    long rd, FOffset, FOffset2;
    short nFiles;
    char FN[15];
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
    
    FOffset2 = WorkFileSize-nFiles*15;

    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem) {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        rd =  func->ReadFromFile(FileName, FOffset2, &FN, 15);
        FOffset2 += rd;

        fi[i].StartOffset = FATItem.StartOffset;
        fi[i].FileSize    = FATItem.Size;        
        if ((strlen(FN)==0)||rd < 15||fi[i].StartOffset+fi[i].FileSize>WorkFileSize||fi[i].FileSize<=0) {
            WasError = true;
            errorfrom = i;
            break;
        }
        fi[i].FullFileName = my_strdup(FN);        
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