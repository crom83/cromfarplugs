struct crimson_t{       
        char filename[MAX_PATH];
        long filesize;
        long filepos;
}crimson_p;

long PackCheckCRIMSON(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{    
    vector<crimson_t> cp;

    char FN[MAX_PATH];        
    long Len;            
    
    long rd, FOffset, SOffset;
    ULONG nFiles, z=0;
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;
    
    unsigned char           t[] = "paq";
    
    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t;

    while (true) {
        z = FOffset;

        rd =  func->ReadFromFile(FileName, FOffset, &FN, sizeof FN);
        if (rd == 0) break;
        FOffset += lstrlen(FN)+1;
        
        rd =  func->ReadFromFile(FileName, FOffset, &Len, sizeof Len);
        if (rd < sizeof Len) return -1;
        
        FOffset += rd;

        lstrcpy(crimson_p.filename, FN);
        crimson_p.filesize = Len;
        crimson_p.filepos  = FOffset;

        cp.push_back(crimson_p);

        FOffset += Len;
    }    

    nFiles = cp.size();

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;   
    
    for (int i=0; i<nFiles; i++){
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));        
    
        fi[i].StartOffset = cp[i].filepos;
        fi[i].FileSize    = cp[i].filesize;        
        if ((lstrlen(cp[i].filename)==0)||cp[i].filepos+cp[i].filesize>WorkFileSize||cp[i].filesize<=0){
            WasError = true;
            errorfrom = i;
            break;
        }
        fi[i].FullFileName= my_strdup(cp[i].filename);        
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
    return FileSize;    
}