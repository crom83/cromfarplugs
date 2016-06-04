long PackCheckZULU(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct ZuluInfo{
        char FileName[36];
        long FileOffset;
        long FileSize;        
    }Zulu;
#pragma pack(8)

    long rd, SOffset, FOffset, DirOffset, nFiles;
    unsigned char           t[] = "CAT1";     
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t-1, readed);
    if (fres == -1) return -1;
    
    SOffset = FileOffset+fres;

    FOffset = SOffset+sizeof t-1;

    rd =  func->ReadFromFile(FileName, FOffset, &DirOffset, 4);
    if (rd < 4) return -1;

    FOffset = SOffset+DirOffset;    

    nFiles= (WorkFileSize-DirOffset)/sizeof Zulu;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;    
    
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &Zulu, sizeof Zulu);
        if (rd < sizeof Zulu) {
            WasError = true;
            errorfrom = i;
            break;
        }
        
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = Zulu.FileOffset+SOffset;
        fi[i].FileSize    = Zulu.FileSize;
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_substr(Zulu.FileName,0,36,1);          
        fi[i].FullFileName[36]=0;
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
    
    FileOffset = FileOffset+fres;
    FileSize = WorkFileSize-FileOffset;
    return WorkFileSize;    
}