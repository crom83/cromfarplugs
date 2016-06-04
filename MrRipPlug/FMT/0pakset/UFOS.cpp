long PackCheckUFOS(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATItem{        
        long offset;
        long unpsize;
        long type;
        long packsize;                
    }Item;
#pragma pack(8)

    long rd, SOffset, FOffset, z=0;
    char s[MAX_PATH];
    long nFiles;
    bool WasError = false;
    int errorfrom;

    RIP_FILE_INFO * fi;    
    
    unsigned char t[]={0x41,0x72,0x74,0x65,0x63,0x68,0x00,0x00};//ID

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;
    
    SOffset = FileOffset+fres;

    FOffset = SOffset+sizeof t+4;
    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, sizeof nFiles);
    if (rd < sizeof nFiles) return -1;

    if  ((nFiles<1)||
         (nFiles*sizeof Item>WorkFileSize)) return -1;    
        
    FOffset += rd+4;

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
        
        fi[i].StartOffset = Item.offset+SOffset;
        fi[i].FileSize    = Item.packsize;
        

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        if (fi[i].StartOffset+fi[i].FileSize>z) z=fi[i].StartOffset+fi[i].FileSize;

        if (i< 10)           sprintf(s, "strm000%d", i);
        else if (i< 100)     sprintf(s, "strm00%d",  i);
        else if (i< 1000)    sprintf(s, "strm%d",    i);
        else if (i< 10000)   sprintf(s, "str%d",     i);
        else if (i< 100000)  sprintf(s, "st%d",      i);
        else if (i< 1000000) sprintf(s, "s%d",       i);
        else                 sprintf(s, "%d",        i);

        fi[i].FullFileName = my_strdup(s);
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