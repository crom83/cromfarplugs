long PackCheckGTA3GXT(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATInfo{
        ULONG Offset;
        char Name[8];
        ULONG Offset2;
    }FATItem;
#pragma pack(8)

    unsigned char           tk[] = {'T', 'K', 'E', 'Y'};
    unsigned char           td[] = {'T', 'D', 'A', 'T'};
    unsigned char           te[] = {0x0, 0x0};
    bool WasError = false;
    int errorfrom;

    long rd, SOffset, FOffset, FOffset2;
    long DirSize, DataSize;    
    unsigned char t[4];
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(tk, BaseString, sizeof tk, readed);
    if (fres == -1) return -1;  

    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t;

    rd =  func->ReadFromFile(FileName, FOffset, &DirSize, sizeof DirSize);
    if (rd < sizeof DirSize) return -1;        

    if (DirSize>=WorkFileSize||DirSize<=8) return -1;    

    FOffset = DirSize+8;

    rd =  func->ReadFromFile(FileName, FOffset, &t, sizeof t);
    if (rd < sizeof t) return -1;        
    if (memcmp(&t, &td, 4) != 0) return -1;
    FOffset += rd;

    rd =  func->ReadFromFile(FileName, FOffset, &DataSize, sizeof DataSize);
    if (rd < sizeof DataSize) return -1;
    if (DataSize>=WorkFileSize||DataSize<=8) return -1;    

    FilesNumber = DirSize/12;

    FOffset = SOffset + 8;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*FilesNumber);
    if (fi == NULL) return -1;
    *finfo = fi;        

    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd-4;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        FOffset2 = FATItem.Offset + SOffset + DirSize+16;

        rd = func->FindInFile(FileName, FOffset2, te, 2);
        if (rd == -1)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        rd++;

        fi[i].StartOffset = FATItem.Offset + SOffset + DirSize+16;
        fi[i].FileSize    = rd-FOffset2;
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_strdup(FATItem.Name, 1);
        fi[i].FullFileName[strlen(FATItem.Name)] = 0;
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
    FileSize = SOffset+DirSize+DataSize+16;
    return FileSize;    
}