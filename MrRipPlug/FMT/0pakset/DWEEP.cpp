long PackCheckDWEEP(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATInfo{
        long StartOffset;
        char FileName[13];
        long EndOffset;        
    }FATItem;
#pragma pack(8)

    long rd, FOffset, nFiles, AllSize=0;
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;
            
    FOffset = 0;

    if (FileOffset>0) return -1;
        
    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, 4);
    if (rd < 4) return -1;    

    if ((nFiles<2)||(nFiles*sizeof FATItem>WorkFileSize)) return -1;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles-1;    
    
    FOffset += rd;

    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd-4;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = FATItem.StartOffset;
        fi[i].FileSize    = FATItem.EndOffset-FATItem.StartOffset;

        AllSize += fi[i].FileSize;
        if ((AllSize<0)||
            (WorkFileSize<AllSize)||
            (fi[i].StartOffset+fi[i].FileSize>WorkFileSize)) {
            WasError = true;
            errorfrom = i;
            break;
        }        

        fi[i].FullFileName = my_strdup(FATItem.FileName);                  
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