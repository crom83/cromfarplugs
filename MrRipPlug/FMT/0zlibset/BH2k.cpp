long PackCheckBH2K(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{   
#pragma pack(1)
    struct {
        char Name[20];        
        long Offset;        
    }FATItem;
#pragma pack(8)

    long rd, FOffset, Unp;
    short filestotal;
    char * base;
    
    RIP_FILE_INFO           * fi;

    if (FileOffset>0) return -1;

    FOffset = 0;

    rd =  func->ReadFromFile(FileName, FOffset+4, &FATItem.Name, 1);
    if (rd < 1) return -1;
    if (FATItem.Name[0]!='x') return -1;

    base = my_substr(FileName, 0, strlen(FileName)-3,3);
    strcat(base, "ind");

    //test file existing
    rd =  func->ReadFromFile(base, FOffset, &Unp, 4);
    if (rd < 4) {
        my_free(base);
        return -1;    
    }        

    rd =  func->ReadFromFile(base, FOffset, &filestotal, sizeof filestotal);
    if (rd < sizeof filestotal) {
        my_free(base);
        return -1;
    }

    if (filestotal <= 0) {
        my_free(base);
        return -1;
    }

    FOffset += sizeof filestotal;    

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*filestotal);
    if (fi == NULL) {
        my_free(base);
        return -1;
    }

    *finfo = fi;
    FilesNumber = filestotal;
    
    for (int i = 0; i<FilesNumber; i++){        
        rd =  func->ReadFromFile(base, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem) {
            my_free(base);
            return -1;
        }
        
        FOffset += rd;

        rd =  func->ReadFromFile(FileName, FATItem.Offset, &Unp, sizeof Unp);
        if (rd < sizeof Unp) {
            my_free(base);
            return -1;
        }

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        if (i!=0) {
            fi[i-1].FileSize    = FATItem.Offset-fi[i-1].StartOffset;
        }
        if (i==(FilesNumber-1))
            fi[i].FileSize    = WorkFileSize-FATItem.Offset;

        fi[i].StartOffset = FATItem.Offset;            
        fi[i].UnpSize     = Unp;

        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);                
            }
            my_free(fi);         
            my_free(base);            
            return -1;
        }                

        fi[i].FullFileName = my_strdup(FATItem.Name);
        fi[i].Description = my_strdup("");        
    }                  
    
    FileOffset = 0;
    FileSize   = WorkFileSize;
    return WorkFileSize;    
}
