long PackCheckDUKE(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct DukeInfo{
        char FileName[12];        
        long FileSize;        
    }Duke;
#pragma pack(8)

    long rd, FOffset, nFiles, z;
    unsigned char           t[] = "KenSilverman";     
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t-1, readed);
    if (fres == -1) return -1;
    
    FOffset = FileOffset+fres+sizeof t-1;

    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, 4);
    if (rd < 4) return -1;
    
    FOffset += rd;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;
    z = nFiles*16+FOffset;
    
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &Duke, sizeof Duke);
        if (rd < sizeof Duke) {
            WasError = true;
            errorfrom = i;
            break;
        }
        
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = z;
        fi[i].FileSize    = Duke.FileSize;
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_substr(Duke.FileName,0,12,1);          
        fi[i].FullFileName[12]=0;
        fi[i].Description = my_strdup("");                  
        z += Duke.FileSize;
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
    FileSize = z-FileOffset;
    return z;    
}