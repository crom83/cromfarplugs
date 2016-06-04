#include <sys/types.h> 
#include <sys/stat.h>

long PackCheckGTA3(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATInfo{
        long StartOffset;
        long Size;
        char FileName[24];
    }FATItem;
#pragma pack(8)

    long rd, FOffset, nFiles;
    bool WasError = false;
    int errorfrom;

    char * base;
    
    RIP_FILE_INFO           * fi;
            
    FOffset = 0;

    if (FileOffset>0) return -1;

    base = my_substr(FileName, 0, strlen(FileName)-3,3);
    strcat(base, "dir");

    //test file existing
    rd =  func->ReadFromFile(base, FOffset, &nFiles, 4);
    if (rd < 4) {
        my_free(base);
        return -1;    
    }

    struct _stat buf;
    _stat(base, &buf);
    nFiles = buf.st_size/32;

    if (nFiles<1) {
        my_free(base);
        return -1;
    }    

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) {
        my_free(base);
        return -1;
    }
    *finfo = fi;    

    FilesNumber = nFiles;    
    
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(base, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = FATItem.StartOffset*2048;
        fi[i].FileSize    = FATItem.Size*2048;
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_strdup(FATItem.FileName);                  
        fi[i].Description = my_strdup("");                          
    }              

    if (WasError){
        for (int j=0; j<errorfrom; j++){
            my_free(fi[j].FullFileName);                
            my_free(fi[j].Description);        
        }
        my_free(fi);
        my_free(base);
        fi = NULL;
        return -1;
    }

    my_free(base);
    FileOffset = 0;
    FileSize = WorkFileSize;
    return WorkFileSize;    
}