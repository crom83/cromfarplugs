long PackCheckH3SND(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct {
        char Name[40];        
        long StartOffset;        
        long Size;
    }FATItem;
#pragma pack(8)

    long rd, FOffset;
    long nFiles;    
    char Ext[5];
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;
            
    FOffset = 0;

    if (FileOffset>0) return -1;

    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, sizeof nFiles);
    if (rd < sizeof nFiles) return -1;    
    FOffset += rd;

    if (nFiles*sizeof FATItem>WorkFileSize) return -1;
    if (nFiles<1) return -1;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;    

    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd;

        rd =  func->ReadFromFile(FileName, FOffset-48+strlen(FATItem.Name)+1, Ext, sizeof Ext);
        if (rd < 3)  {
            WasError = true;
            errorfrom = i;
            break;
        }

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = FATItem.StartOffset;        
        fi[i].FileSize    = FATItem.Size;
        
        if ((DWORD)fi[i].StartOffset+(DWORD)fi[i].FileSize>(DWORD)WorkFileSize||
            (DWORD)fi[i].FileSize>(DWORD)WorkFileSize||
			(DWORD)fi[i].StartOffset>(DWORD)WorkFileSize||
            lstrlen(FATItem.Name)==0||
            FATItem.Name[0]<20) {
            WasError = true;
            errorfrom = i;
            break;
        }
        
        fi[i].FullFileName = my_strdup(FATItem.Name, 5);
        strcat(fi[i].FullFileName, ".");
        strcat(fi[i].FullFileName, Ext);
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