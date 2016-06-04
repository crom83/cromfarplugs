long PackCheckREDF(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATItem{        
        char FileName[60];                
        long FileSize;
    }Item;
#pragma pack(8)

    long rd, FOffset, SOffset, nFiles, z, DirEnd, NOffset;
    unsigned char           t[] = {0xCE, 0x0A, 0x89, 0x51, 0x01, 0x00, 0x00, 0x00};
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;
    
    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t;
    
    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, 4);
    if (rd < 4) return -1;
    
    if (nFiles < 1) return -1;
    FOffset += rd;

    rd =  func->ReadFromFile(FileName, FOffset, &z, 4);
    if (rd < 4) return -1;
    
    z += SOffset;
    if (z>WorkFileSize) return -1;
    FOffset = SOffset+0x0800;

    DirEnd = SOffset+0x0800+(nFiles*sizeof Item);

    while (DirEnd%2048!=0) DirEnd++;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;    
    NOffset = DirEnd;
    
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &Item, sizeof Item);
        if (rd < sizeof Item)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = NOffset;
        fi[i].FileSize    = Item.FileSize;
        

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_strdup(Item.FileName);
        fi[i].Description = my_strdup("");
        NOffset += Item.FileSize;
        while (NOffset%2048!=0) NOffset++;
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