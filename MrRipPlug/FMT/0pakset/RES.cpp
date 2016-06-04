#pragma pack(1)
    struct Allods_tag{
        long Jnk;
        long FilePos;
        long FileSize;
        long Type;        
        char DirName[16];
    }AllodsItem;
#pragma pack(8)

#pragma pack(1)
    struct Allods_tag2{        
        long FilePos;
        long FileSize;
        char Type;        
        char Added;
        char *Name;
    }AllodsItem2;
#pragma pack(8)

void BuildPath(vector<Allods_tag2> &v_FAT, char *AddPath, int FromItem, int ToItem)
{
    char c[MAX_PATH];
    for (int i=FromItem; i<= ToItem;i++){

        if (v_FAT[i].Added == 1) continue;
                
        sprintf(c, "%s%s", AddPath, v_FAT[i].Name);
        if (v_FAT[i].Type==1) strcat(c, "\\");
        my_free(v_FAT[i].Name);
        v_FAT[i].Name = my_strdup(c);      
        
        v_FAT[i].Added = 1;

        if (v_FAT[i].Type==1) BuildPath(v_FAT, v_FAT[i].Name,v_FAT[i].FilePos,v_FAT[i].FilePos+v_FAT[i].FileSize-1);        
        
    }  
}

long PackCheckRES(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    vector<Allods_tag2> v_FAT;    
    long rd, FOffset, DirOffset, DirLen, z, SOffset;

    long StartDir, DirCnt;

    unsigned char           t[] = {0x26, 0x59, 0x41, 0x31}; 
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t;

    rd =  func->ReadFromFile(FileName, FOffset, &StartDir, 4);
    if (rd < 4) return -1;
    FOffset += rd;

    rd =  func->ReadFromFile(FileName, FOffset, &DirCnt, 4);
    if (rd < 4) return -1;
    FOffset += rd+4;

    rd =  func->ReadFromFile(FileName, FOffset, &DirOffset, 4);
    if (rd < 4) return -1;
    FOffset += rd;

    rd =  func->ReadFromFile(FileName, FOffset, &FilesNumber, 4);
    if (rd < 4) return -1;

    if (StartDir+DirCnt-1>FilesNumber) return -1;

    DirLen = FilesNumber*sizeof AllodsItem;

    z = DirOffset+DirLen+SOffset;
    if (z>WorkFileSize)
        return -1;    

    FOffset = DirOffset;
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &AllodsItem, sizeof AllodsItem);
        if (rd < sizeof AllodsItem) return -1;

        FOffset += rd;        

        if (AllodsItem.FilePos+SOffset+AllodsItem.FileSize>WorkFileSize) return -1;

        AllodsItem2.FilePos  = AllodsItem.FilePos;
        AllodsItem2.FileSize = AllodsItem.FileSize;
        AllodsItem2.Type     = AllodsItem.Type;
        AllodsItem2.Name     = my_strdup(AllodsItem.DirName);
        AllodsItem2.Added    = 0;

        v_FAT.push_back(AllodsItem2);
    }              

    BuildPath(v_FAT, "", StartDir, StartDir+DirCnt-1);

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