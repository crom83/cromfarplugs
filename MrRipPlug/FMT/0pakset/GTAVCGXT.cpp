struct vicecity{
        long Offset;
        char filename[20];
        long filesize;
}gtavc;

long PackCheckGTAVCGXT(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct TABInfo{        
        char Name[8];
        ULONG Offset;
    }TABItem;

    struct FATInfo{
        ULONG Offset;
        char Name[8];        
    }FATItem;
#pragma pack(8)

    vector<vicecity> fat;
    unsigned char           tk[] = {'T', 'K', 'E', 'Y'};
    unsigned char           tt[] = {'T', 'A', 'B', 'L'};
    unsigned char           td[] = {'T', 'D', 'A', 'T'};
    unsigned char           te[] = {0x0, 0x0};

    long rd, SOffset, FOffset, FOffset2, tFOffset;
    long DirSize, DirSize2, Dirs, Files, z=0;
    unsigned char t[4];
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(tt, BaseString, sizeof tt, readed);
    if (fres == -1) return -1;        

    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t;

    rd =  func->ReadFromFile(FileName, FOffset, &DirSize, sizeof DirSize);
    if (rd < sizeof DirSize) return -1;        

    if (DirSize>=WorkFileSize||DirSize<=8) return -1;    

    FOffset = SOffset+20; //jump to first item after MAIN
    Dirs = DirSize/12;

    for (int i = 1; i<Dirs; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &TABItem, sizeof TABItem);
        if (rd < sizeof TABItem) return -1;
        FOffset  += rd;
        FOffset2 = SOffset+TABItem.Offset+8;
        rd =  func->ReadFromFile(FileName, FOffset2, &t, sizeof t);
        if (rd < sizeof t) return -1;        
        if (memcmp(&t, &tk, 4) != 0) return -1;
        FOffset2 += 4;
        rd =  func->ReadFromFile(FileName, FOffset2, &DirSize2, sizeof DirSize2);
        if (rd < sizeof DirSize2) return -1;
        if (DirSize2>=WorkFileSize||DirSize2<=8) return -1;    
        FOffset2 += 4;
        Files = DirSize2/12;

        for (int i = 0; i<Files; i++){    
            rd =  func->ReadFromFile(FileName, FOffset2, &FATItem, sizeof FATItem);
            if (rd < sizeof FATItem) return -1;        
            FOffset2 += rd;
        
            tFOffset = TABItem.Offset+FATItem.Offset + SOffset + DirSize2 + 24;
            rd = func->FindInFile(FileName, tFOffset, te, 2);
            if (rd == -1) return -1;                   
            if (rd > z) z = rd;
            rd++;
            gtavc.Offset   = tFOffset;
            gtavc.filesize = rd-tFOffset;
            if (gtavc.Offset+gtavc.filesize>WorkFileSize)               
                return -1;          

            strcpy(gtavc.filename, TABItem.Name);
            strcat(gtavc.filename, "\\");
            strcat(gtavc.filename, FATItem.Name);
            fat.push_back(gtavc);
        }
    }   

    FOffset = SOffset+16;//jump to value of MAIN offset
    rd =  func->ReadFromFile(FileName, FOffset, &FOffset2, sizeof FOffset2);
    if (rd < sizeof FOffset2) return -1;
    FOffset2 += 4;
    
    rd =  func->ReadFromFile(FileName, FOffset2, &DirSize2, sizeof DirSize2);
    if (rd < sizeof DirSize2) return -1;
    if (DirSize2>=WorkFileSize||DirSize2<=8) return -1;    
    FOffset2 += 4;
    Files = DirSize2/12;

    for (i = 0; i<Files; i++){    
        rd =  func->ReadFromFile(FileName, FOffset2, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem) return -1;        
        FOffset2 += rd;
        
        tFOffset = DirSize + FATItem.Offset + SOffset + DirSize2 + 24;
        rd = func->FindInFile(FileName, tFOffset, te, 2);
        if (rd == -1) return -1;                   
        rd++;
        gtavc.Offset   = tFOffset;
        gtavc.filesize = rd-tFOffset;
        if (gtavc.Offset+gtavc.filesize>WorkFileSize)               
            return -1;          

        strcpy(gtavc.filename, FATItem.Name);
        fat.push_back(gtavc);
    }


    FilesNumber = fat.size();

    FOffset = SOffset + 8;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*FilesNumber);
    if (fi == NULL) return -1;
    *finfo = fi;        

    for (i = 0; i<FilesNumber; i++){                   
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = fat[i].Offset;
        fi[i].FileSize = fat[i].filesize;
        fi[i].FullFileName = my_strdup(fat[i].filename, 1);
        fi[i].FullFileName[strlen(fat[i].filename)] = 0;
        fi[i].Description = my_strdup("");
    }    
    FileOffset = SOffset;
    FileSize = z;
    return FileSize;    
}