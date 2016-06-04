long PackCheckRE3(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{   
    
#pragma pack(1)
    struct FAT{        
        long Offset;
        long Size;
    }FATItem;
#pragma pack(8)
    
    long rd, FOffset, SOffset, FilesCount, MaxEnd;
    bool WasError = false;
    int errorfrom;
       
    unsigned char t[]                = {0x03, 0x00, 0x00, 0x00, 
                                        0x01, 0x00, 0x00, 0x00, 
                                        0x04, 0x00, 0x00, 0x00, 
                                        0x00, 0x01, 0x01, 0x00};
    RIP_FILE_INFO           * fi;
    char Temp[MAX_PATH];

    long fres = func->Find(t, BaseString, sizeof t-1, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+0x1000;
    

    rd =  func->ReadFromFile(FileName, FOffset, &FilesCount, sizeof FilesCount);
    if (rd < sizeof FilesCount) return -1;

    if (FilesCount <= 0) return -1;

    FOffset += rd;
    MaxEnd = FOffset;
    
    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*FilesCount);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = FilesCount;
    for (int i=0; i<FilesNumber;i++){
        rd =  func->ReadFromFile(FileName, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem)  {
            WasError = true;
            errorfrom = i;
            break;
        }

        FOffset += rd;

        int j = 0;
        while (TRUE){           
            rd =  func->ReadFromFile(FileName, FOffset++, &Temp[j], sizeof Temp[j]);
            if (rd < sizeof Temp[j])  {
                WasError = true;
                errorfrom = i;
                break;
            }
        if (Temp[j++] == 0) break;      
        }
        if (WasError) break;

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = (FATItem.Offset*8)+SOffset;
        fi[i].FileSize    = FATItem.Size+SOffset;

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        if (MaxEnd<fi[i].StartOffset+fi[i].FileSize) MaxEnd = fi[i].StartOffset+fi[i].FileSize;
        
        fi[i].FullFileName = my_strdup(Temp);
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
    FileSize = MaxEnd;
    return MaxEnd;
}