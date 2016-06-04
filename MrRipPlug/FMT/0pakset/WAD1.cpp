long PackCheckWAD1(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct DoomInfo{
        long FilePos;
        long FileSize;
        char FileName[8];        
    }Doom;
#pragma pack(8)

    long rd, FOffset, DirOffset, nFiles, z;
    unsigned char           t1[] = "IWAD"; 
    unsigned char           t2[] = "PWAD"; 
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t1, BaseString, sizeof t1-1, readed);
    if (fres == -1) {
        fres = func->Find(t2, BaseString, sizeof t2-1, readed);
        if (fres == -1) return -1;
    }

    FOffset = FileOffset+fres+sizeof t1-1;

    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, 4);
    if (rd < 4) return -1;
    FOffset += rd;

    rd =  func->ReadFromFile(FileName, FOffset, &DirOffset, 4);
    if (rd < 4) return -1;
    FOffset += rd;
 
    z = DirOffset+nFiles*sizeof Doom+FileOffset+fres;
    if (z>WorkFileSize)
        return -1;
        
    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;

    FOffset = DirOffset;
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &Doom, sizeof Doom);
        if (rd < sizeof Doom)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = Doom.FilePos+FileOffset+fres;
        fi[i].FileSize    = Doom.FileSize;        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_substr(Doom.FileName,0,8,1);          
        fi[i].FullFileName[8]=0;
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
    FileSize = z-FileOffset;
    return z;    
}