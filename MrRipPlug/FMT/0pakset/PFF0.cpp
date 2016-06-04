long PackCheckPFF0(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATItem{        
        long Junk1;
        long FileOffset;
        long FileSize;
        long Junk2;
        char FileName[16];                
    }Item;
#pragma pack(8)

    long rd, FOffset, SOffset, nFiles, z;
    unsigned char           t[] = "PFF0";
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t-1, readed);
    if (fres == -1) return -1;
    
    SOffset = FileOffset+fres-4;
    if (SOffset<0) return -1;
    FOffset = SOffset+sizeof t+3;
    
    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, 4);
    if (rd < 4) return -1;
    
    if (nFiles < 1) return -1;
    FOffset += rd+4;

    rd =  func->ReadFromFile(FileName, FOffset, &z, 4);
    if (rd < 4) return -1;
    
    z += SOffset;
    if (z>WorkFileSize) return -1;
    FOffset = z;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;    
        
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &Item, sizeof Item);
        if (rd < sizeof Item)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = Item.FileOffset+SOffset;
        fi[i].FileSize    = Item.FileSize;
        

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_strdup(Item.FileName);
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
    FileSize = z+nFiles*sizeof Item-SOffset;
    return FOffset;
}