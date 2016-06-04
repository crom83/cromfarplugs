#pragma pack(1)
    struct DatInfoFull{
        char*DirName;   
        char*DirDsc;    
        long Type;
        long Offset;
        long RealSize;
        long PackedSize;        
    };
#pragma pack(8)

long PackCheckDAT1(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    char FNsize;
    char DirName[MAX_PATH];
    char*Text;

    DatInfoFull Item;

    vector<char*>  DirLst; 
    vector<DatInfoFull> FileLst; 

#pragma pack(1)
    struct DatInfo{
        long Type;
        long Offset;
        long RealSize;
        long PackedSize;        
    }FOUT_DAT;
#pragma pack(8)

    ULONG rd, FOffset, DirCount, filestotal;    
    
    RIP_FILE_INFO           * fi;

    if (FileOffset>0) return -1;

    FOffset = 0;

    rd =  func->ReadFromFile(FileName, FOffset, &DirCount, sizeof DirCount);
    if (rd < sizeof DirCount) return -1;    

    DirCount = func->SwapLong(DirCount);
    
    if ((DirCount==0)||
        (DirCount>200000)) return -1;

    FOffset += 8;

    //checking
    rd =  func->ReadFromFile(FileName, FOffset, &filestotal, sizeof filestotal);
    if (rd < sizeof filestotal) return -1;    

    if (filestotal != 0) return -1;

    //Reading dir names
    FOffset += 8;

    for (int i=0; i<DirCount;i++){
        rd =  func->ReadFromFile(FileName, FOffset, &FNsize, sizeof FNsize);
        if (rd < sizeof FNsize) return -1;    
        FOffset += sizeof FNsize;

        rd =  func->ReadFromFile(FileName, FOffset, &DirName, FNsize);
        if (rd < FNsize) return -1;    
        FOffset += FNsize;

        DirName[FNsize] = 0;

        Text = my_strdup(DirName);
        DirLst.push_back(Text);     
    }

    //Read files
    for (i=0; i<DirCount;i++){
        rd =  func->ReadFromFile(FileName, FOffset, &filestotal, sizeof filestotal);
        if (rd < sizeof filestotal) return -1; 

        filestotal = func->SwapLong(filestotal);

        FOffset += 16;

        for (int j=0; j<filestotal;j++){
            rd =  func->ReadFromFile(FileName, FOffset, &FNsize, sizeof FNsize);
            if (rd < sizeof FNsize) return -1;    
            FOffset += sizeof FNsize;

            rd =  func->ReadFromFile(FileName, FOffset, &DirName, FNsize);
            if (rd < FNsize) return -1;    
            FOffset += FNsize;
            
            DirName[FNsize] = 0;

            rd =  func->ReadFromFile(FileName, FOffset, &FOUT_DAT, sizeof FOUT_DAT);
            if (rd < sizeof FOUT_DAT) return -1;    
            FOffset += sizeof FOUT_DAT;

            if (strcmp(".", DirLst[i])==0)
                Item.DirName = my_strdup(DirName);
            else {
                Item.DirName = my_strdup(DirLst[i],strlen(DirName)+1);
                strcat(Item.DirName, "\\");
                strcat(Item.DirName, DirName);
            }

            Item.Offset     = func->SwapLong(FOUT_DAT.Offset);
            Item.PackedSize = func->SwapLong(FOUT_DAT.PackedSize);          
            Item.RealSize   = func->SwapLong(FOUT_DAT.RealSize);
            Item.Type       = func->SwapLong(FOUT_DAT.Type);

            if (Item.Type == 0x40) Item.DirDsc = my_strdup("LZSS");
            else                   Item.DirDsc = my_strdup("Plain");
            
            if (Item.Type != 0x40) Item.PackedSize = Item.RealSize;

            FileLst.push_back(Item);
        }
    }      

    for (i=0; i<DirCount;i++) my_free(DirLst[i]);


    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*FileLst.size());
    if (fi == NULL) return -1;

    *finfo = fi;
    FilesNumber = FileLst.size();
    
    for (i = 0; i<FilesNumber; i++){
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        fi[i].StartOffset = FileLst[i].Offset;
        fi[i].FileSize    = FileLst[i].PackedSize;
        fi[i].UnpSize     = FileLst[i].RealSize;
        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            
            for (int j=0; j<FileLst.size(); j++){
                my_free(FileLst[j].DirName);
                my_free(FileLst[j].DirDsc);
            }
            

            for (j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);                
            }
            my_free(fi);
            return -1;
        }        

        fi[i].FullFileName = my_strdup(FileLst[i].DirName);        
        fi[i].Description = my_strdup(FileLst[i].DirDsc);        
    }              
    
    for (i=0; i<FileLst.size(); i++){
        my_free(FileLst[i].DirName);
        my_free(FileLst[i].DirDsc);
    }

    FileOffset = 0;
    FileSize   = WorkFileSize;
    return WorkFileSize;    
}

BOOL GetFileDAT1(const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize, long UnpSize, struct RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{
    unsigned char * bufIn;
    unsigned char * bufOut;
    char * dstpath;    
    long rd, FOffset, outpos = 0;    
    short reals;

    if (FileSize == UnpSize) return FALSE;

    bufOut = (unsigned char *)my_malloc(UnpSize);
    if (!bufOut) {return FALSE;}

    FOffset = StartOffset;

    while (outpos<UnpSize){
        rd = func->ReadFromFile(PackName, FOffset, &reals, sizeof reals);
        if (rd < sizeof reals) return FALSE;
    
        reals = func->SwapWord(reals);
        FOffset += sizeof reals;

        bufIn = (unsigned char *)my_malloc(reals);
        if (!bufIn) return FALSE;       

        rd = func->ReadFromFile(PackName, FOffset, bufIn, reals);
        if (rd < reals) return FALSE;
    
        FOffset += reals;

        Decode(bufIn, reals, bufOut, UnpSize, outpos);

        my_free(bufIn);
    }

    dstpath = my_strdup(PathName, strlen(FileName)+2);
    strcat(dstpath, "\\");
    strcat(dstpath, FileName);
    if (func->SaveToFile(dstpath, 0, bufOut, UnpSize) != UnpSize){
        my_free(dstpath);
        my_free(bufOut);
        return FALSE;
    }
    my_free(dstpath);
    my_free(bufOut);
    return TRUE;
}