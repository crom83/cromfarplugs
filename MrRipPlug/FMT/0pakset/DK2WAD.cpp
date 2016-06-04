long PackCheckDK2WAD(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATInfo{
        long Junk;
        long fn_offset;
        long fn_size;        
        long StartOffset;        
        long Size;
        long Junk2[5];
    }FATItem;
#pragma pack(8)

    bool WasError = false;
    int errorfrom;
    unsigned char           t[] = {'D', 'W', 'F', 'B', 0x02};

    long rd, SOffset, FOffset;
    long nFiles;
    char FN[MAX_PATH];
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+0x48;        

    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, sizeof nFiles);
    if (rd < sizeof nFiles) return -1;    
    FOffset += rd;

    if (nFiles*sizeof FATItem>WorkFileSize) return -1;
    if (nFiles<1) return -1;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FOffset = SOffset+0x58;

    FilesNumber = nFiles;        

    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = FATItem.StartOffset;
        fi[i].FileSize    = FATItem.Size;
        fi[i].UnpSize     = FATItem.Size;        
        rd =  func->ReadFromFile(FileName, SOffset+FATItem.fn_offset, &FN, FATItem.fn_size);        

        if (rd < FATItem.fn_size||fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

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
    FileSize = SOffset+FATItem.fn_offset+FATItem.fn_size;
    return FileSize;    
}