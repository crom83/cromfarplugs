#pragma pack(1)
    struct Commandos_tag{        
        char DirName[32];
        long Type;        
        long FileSize;
        long FilePos;        
    }ComandosItem;
#pragma pack(8)

#pragma pack(1)
    struct Commandos_tag2{        
        long FilePos;
        long FileSize;
        char Type;        
        char Added;
        char *Name;
    }ComandosItem2;
#pragma pack(8)

void BuildPath(vector<Allods_tag2> &v_FAT, char *AddPath, int FromOff, int FromPos)
{
    char c[MAX_PATH];
    int i = FromPos, Off = FromOff, rd;
    while (TRUE) {

        rd =  func->ReadFromFile(FileName, i, &ComandosItem, sizeof ComandosItem);
        if (rd < sizeof ComandosItem) return -1;
        Off += rd;

        if (ComandosItem.Type == 0xCDCDCD00) {
            if (ComandosItem.FilePos+ComandosItem.FileSize>WorkFileSize) return;

            if (v_FAT[i].Added == 1) continue;

            ComandosItem2.FilePos  = ComandosItem.FilePos;
            ComandosItem2.FileSize = ComandosItem.FileSize;
            ComandosItem2.Type     = ComandosItem.Type;
            ComandosItem2.Name     = my_strdup(ComandosItem.DirName);
            ComandosItem2.Added    = 0;
            v_FAT.push_back(ComandosItem2);
            sprintf(c, "%s%s", AddPath, v_FAT[i].Name);
            my_free(v_FAT[i].Name);
            v_FAT[i].Name = my_strdup(c);           
            v_FAT[i].Added = 1;
            i++;
        }

        if (ComandosItem.Type == 0xCDCDCD01) {
            if (v_FAT[i].Added == 1) continue;
            ComandosItem2.FilePos  = ComandosItem.FilePos;
            ComandosItem2.FileSize = ComandosItem.FileSize;
            ComandosItem2.Type     = ComandosItem.Type;
            ComandosItem2.Name     = my_strdup(ComandosItem.DirName);
            ComandosItem2.Added    = 0;
            v_FAT.push_back(ComandosItem2);
            sprintf(c, "%s%s", AddPath, v_FAT[i].Name);
            if (v_FAT[i].Type==1) strcat(c, "\\");
            my_free(v_FAT[i].Name);
            v_FAT[i].Name = my_strdup(c);
            v_FAT[i].Added = 1;
            i++;
            BuildPath(v_FAT, v_FAT[i].Name,v_FAT[i].FilePos,v_FAT[i].FilePos+v_FAT[i].FileSize-1);        
        }        
    }  
}

long PackCheckDIR(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    vector<Commandos_tag2> v_FAT;    
    long rd, FOffset, DirIn, z;

    RIP_FILE_INFO           * fi;

    if (FileOffset>0) return -1;
    FOffset = 32;

    DirIn = 0;

    rd =  func->ReadFromFile(FileName, FOffset, &z, 4);
    if (rd < 4) return -1;
    FOffset += rd;

    if ((z != 0xCDCDCD00)||(z != 0xCDCDCD01)) return -1;
    
    BuildPath(v_FAT, "", 0, 0);

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*v_FAT.size());
    if (fi == NULL) return -1;
    *finfo = fi;    

    for (i = 0; i<v_FAT.size(); i++){    

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = v_FAT[i].FilePos+SOffset;
        fi[i].FileSize    = v_FAT[i].FileSize;                            

        fi[i].FullFileName = my_strdup(v_FAT[i].Name);
        fi[i].Description = my_strdup("");
    }

    for (i = 0; i<v_FAT.size(); i++){
        my_free(v_FAT[i].Name);
    }

    FileOffset = SOffset;
    FileSize = z-FileOffset;
    return z;    
}