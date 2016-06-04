#pragma pack(1)
struct dstorm_t{
    DWORD Name;
    DWORD Offset;        
    DWORD Size;
    char  Ext[4];
}dstorm;
#pragma pack(8)

long PackCheckDSTORM(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       

    vector<dstorm_t> fat;
    long rd, FOffset;
    long nFiles;    
    bool WasError=false;
    int errorfrom;
    DWORD temp;
    short temp2;
    
    RIP_FILE_INFO           * fi;
            
    FOffset = 0;

    if (FileOffset>0) return -1;

    while (TRUE) {
    
        rd =  func->ReadFromFile(FileName, FOffset, &dstorm, sizeof dstorm_t-4);
        if (rd < sizeof dstorm_t-4) return -1;        
        FOffset += rd;

        rd =  func->ReadFromFile(FileName, dstorm.Offset, &temp, sizeof temp);
        if (rd < 0) return -1;                

        rd =  func->ReadFromFile(FileName, dstorm.Offset, &temp2, sizeof temp2);
        if (rd < 0) return -1;                

        if (temp2 == 0x4D42) lstrcpy(dstorm.Ext, "BMP");
        else {
            switch (temp) {
            case 0x20534444: lstrcpy(dstorm.Ext, "DDS");
                break;
            case 0x4A424F45: lstrcpy(dstorm.Ext, "OBJ");
                break;
            case 0x00000002: lstrcpy(dstorm.Ext, "BIN");
                break;
            case 0x474E5089: lstrcpy(dstorm.Ext, "PNG");
                break;
            default: lstrcpy(dstorm.Ext, "unk");
            }
        }

        if (!dstorm.Name) break;      

        fat.push_back(dstorm);
    }

    nFiles = fat.size();
    
    if (nFiles<1) return -1;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;    

    for (int i = 0; i<FilesNumber; i++){        
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = fat[i].Offset;
        fi[i].FileSize    = fat[i].Size;
        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize||
            fi[i].FileSize<=0){
                WasError = true;
                errorfrom = i;
                break;
            }
        
        char s[256];

        sprintf(s, "%x.%s", fat[i].Name, fat[i].Ext);
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
    
    FileOffset = 0;
    FileSize = WorkFileSize;
    return WorkFileSize;    
}