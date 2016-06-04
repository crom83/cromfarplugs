#include "misc/cc_file.cpp"

long PackCheckTDMIX(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(push, 1)
    struct FATItem{        
        long ID;
        long FileOffset;
        long FileSize;        
    }Item;
#pragma pack(pop)

    long rd, FOffset, SOffset;
    short nFiles;
    long FSize;
    char s[MAX_PATH];
    char ext[10];
    unsigned char data[1024];
    t_file_type gm = ft_unknown;
    RIP_FILE_INFO           * fi;

    if (FileOffset>0) return -1;    
    
    FOffset = 0;

    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, sizeof nFiles);
    if (rd < sizeof nFiles) return -1;
    
    SOffset = nFiles*sizeof Item;

    if (nFiles < 1||SOffset>WorkFileSize) return -1;
    FOffset += rd;

    SOffset += 6;

    rd =  func->ReadFromFile(FileName, FOffset, &FSize, sizeof FSize);
    if (rd < sizeof FSize) return -1;
    if (FSize != WorkFileSize-SOffset) return -1;    
    FOffset += rd;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;    
        
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &Item, sizeof Item);
        if (rd < sizeof Item) return -1;
        
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = Item.FileOffset+SOffset;
        fi[i].FileSize    = Item.FileSize;
        

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);        
            }
            my_free(fi);
            return -1;
        }

        rd =  func->ReadFromFile(FileName, fi[i].StartOffset, &data, 1024);
        if ((gm = get_file_type(data, rd, (char*)ext, fi[i].FileSize))==ft_unknown) strcpy(ext, "unk");
        
        sprintf(s, "%x", Item.ID);
        strcat(s, ".");strcat(s, ext);


        fi[i].FullFileName = my_strdup(s);
        fi[i].Description = my_strdup("");        
        
    }
    FileOffset = 0;
    FileSize = SOffset+FSize;
    return FileSize;
}