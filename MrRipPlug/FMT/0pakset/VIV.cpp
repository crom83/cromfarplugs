long PackCheckVIV(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
#pragma pack(1)
    struct FATItem{        
        char FileName[MAX_PATH];
        long FileOffset;
        long FileSize;
    }Item;
#pragma pack(8)

    long rd, FOffset, SOffset, nFiles, z;
    unsigned char           t[] = "BIG";
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t-1, readed);
    if (fres == -1) return -1;
    
    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t;
    
    rd  =  func->ReadFromFile(FileName, FOffset, &z, sizeof z);    
    if (rd < sizeof z) return -1;
    FOffset += rd;

    z = func->SwapLong(z);
    if (z>WorkFileSize) {
        z = func->SwapLong(z);
        if (z>WorkFileSize) return -1;
    }

    rd  =  func->ReadFromFile(FileName, FOffset, &nFiles, sizeof nFiles);    
    if (rd < sizeof nFiles) return -1;
    FOffset += rd;

    nFiles = func->SwapLong(nFiles);    
    if (nFiles < 1) return -1;
    
    FOffset = SOffset+0x10;    

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;        
    
    for (int i = 0; i<FilesNumber; i++){    
        rd  =  func->ReadFromFile(FileName, FOffset, &Item.FileOffset, sizeof Item.FileOffset);             
        if (rd < sizeof Item.FileOffset)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd;

        Item.FileOffset = func->SwapLong(Item.FileOffset);

        rd  =  func->ReadFromFile(FileName, FOffset, &Item.FileSize, sizeof Item.FileSize);
        if (rd < sizeof Item.FileSize)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd;

        Item.FileSize = func->SwapLong(Item.FileSize);
        
        rd =  func->ReadFromFile(FileName, FOffset, &Item.FileName, sizeof Item.FileName);
        if (rd < sizeof Item.FileName)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += strlen(Item.FileName)+1;

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = SOffset+Item.FileOffset;
        fi[i].FileSize    = Item.FileSize;
        
        z = SOffset+fi[i].StartOffset+fi[i].FileSize>z?SOffset+fi[i].StartOffset+fi[i].FileSize:z;

        if (SOffset+fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
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
    FileSize = z-FileOffset;
    return z;    
}