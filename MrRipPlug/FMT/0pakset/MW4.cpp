long PackCheckMW4(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATItem{        
        char unk0[8];
        long unpsize;
        long packsize;
        long offset;
        short filenum;
        char namelen;                
    }Item;
#pragma pack(8)

    long rd, SOffset, FOffset, DirEnd, z;
    unsigned short nFiles;
    bool WasError = false;
    int errorfrom;
    
    char FN[MAX_PATH];
    RIP_FILE_INFO * fi;    
    
    unsigned char t[]={0x23,0x56,0x42,0x44};//ID

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;
    
    SOffset = FileOffset+fres;

    FOffset = SOffset+sizeof t+8;
    rd =  func->ReadFromFile(FileName, FOffset, &DirEnd, 4);
    if (rd < 4) return -1;
        
    if (DirEnd+SOffset>WorkFileSize) return -1;
    FOffset += rd;
    z = DirEnd+SOffset;

    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, sizeof nFiles);
    if (rd < sizeof nFiles) return -1;

    if  (nFiles<1) return -1;
    FOffset += rd+rd;

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
        rd =  func->ReadFromFile(FileName, FOffset, &FN, Item.namelen);
        if (rd < Item.namelen)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = Item.offset+DirEnd;
        fi[i].FileSize    = Item.packsize;
        

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        if (fi[i].StartOffset+fi[i].FileSize>z) z=fi[i].StartOffset+fi[i].FileSize;

        FN[Item.namelen]=0;
        fi[i].FullFileName = my_strdup(FN);
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
    FileSize = z-SOffset;
    return z;
}