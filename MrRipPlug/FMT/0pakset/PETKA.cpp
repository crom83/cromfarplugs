long PackCheckPETKA(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATItem{        
        char FileName[80];        
        long FileOffset;
        long FileSize;
        long Junk1;    
    }Item;
#pragma pack(8)

    unsigned long rd, FOffset, nFiles, AllSize=0;
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;    
    
    if (FileOffset>0) return -1;

    FOffset = 0;
    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, 4);
    if (rd < 4) return -1;
    
    if ((nFiles < 1)||(nFiles*sizeof Item>WorkFileSize)) return -1;
    FOffset += rd;

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
        
        fi[i].StartOffset = Item.FileOffset;
        fi[i].FileSize    = Item.FileSize;

        char ok = 1;

        for (int j=0; j<strlen(Item.FileName);j++){
            if (Item.FileName[j]<32||Item.FileName[j]>126) {
                ok = 0;
                break;
            }
        }
        
        AllSize += Item.FileOffset+Item.FileSize;
        if (ok==0||
            (WorkFileSize<AllSize)||
            (fi[i].StartOffset+fi[i].FileSize>WorkFileSize)) {
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

    FileOffset = 0;
    FileSize = WorkFileSize;
    return WorkFileSize;
}