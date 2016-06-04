long PackCheckSH(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct SHInfo{
        char FileName[24];
        long FileOffset;
        long FileSize;
    }SH;
#pragma pack(8)

    long rd, FOffset, nFiles, z;
    bool WasError = false;
    int errorfrom;
        
    RIP_FILE_INFO           * fi;

    FOffset = WorkFileSize-8;

    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, 4);
    if (rd < 4) return -1;
    
    if (nFiles <= 0) return -1;

    FOffset += rd;

    rd =  func->ReadFromFile(FileName, FOffset, &z, 4);
    if (rd < 4) return -1;

    if ((z>WorkFileSize)||
        (z<=0)) return -1;

    FOffset = WorkFileSize-z;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;    
    
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &SH, sizeof SH);
        if (rd < sizeof SH)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = WorkFileSize-SH.FileOffset-z;
        fi[i].FileSize    = SH.FileSize;
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_substr(SH.FileName,0,24,1);          
        fi[i].FullFileName[24]=0;
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