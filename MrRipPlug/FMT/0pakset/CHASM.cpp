long PackCheckCHASM(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct ChasmInfo{
        char FileNameLen; 
        char FileName[12];        
        long FileSize;        
        long FileOffset;
    }Chasm;
#pragma pack(8)

    long rd, FOffset, SOffset, z;
    short nFiles;
    unsigned char           t[] = "CSid";
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t-1, readed);
    if (fres == -1) return -1;
    
    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t-1;

    
    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, 2);
    if (rd < 2) return -1;
    
    FOffset += rd;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;
    z = nFiles*21+FOffset;
    
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &Chasm, sizeof Chasm);
        if (rd < sizeof Chasm)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = Chasm.FileOffset+SOffset;
        fi[i].FileSize    = Chasm.FileSize;
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_substr(Chasm.FileName,0,12,1);          
        fi[i].FullFileName[Chasm.FileNameLen]=0;
        fi[i].Description = my_strdup("");          
        z += Chasm.FileSize;
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