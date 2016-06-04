long PackCheckDEMS(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct DemInfo{
        long FileOffset;
        long FileSize;
        char FileName[20];        
        
    }Dem;
#pragma pack(8)

    long rd, FOffset, SOffset, nFiles, z=0;
    unsigned char           t[] = "GLB2.0";
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;
    
    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t+1;
    
    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, 4);
    if (rd < 4) return -1;
    
    if (nFiles < 1) return -1;
    FOffset += rd+4;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;    
    
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &Dem, sizeof Dem);
        if (rd < sizeof Dem)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = SOffset+Dem.FileOffset;
        fi[i].FileSize    = Dem.FileSize;

        z = fi[i].StartOffset+fi[i].FileSize>z?fi[i].StartOffset+fi[i].FileSize:z;

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_strdup(Dem.FileName);
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
    
    FileOffset = SOffset;
    FileSize = z-FileOffset;
    return z;    
}