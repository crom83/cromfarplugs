#pragma pack(1)
    struct NFKFATType{        
        char *DirName;
        char PackFlag;
        long FilePos;        
        long PackedSize;
        long FileSize;        
    };

    class NFKFATClass{
    public:
        NFKFATType item;
        NFKFATClass() { //simple init
            item.DirName = NULL;
            item.FilePos = 0;
            item.FileSize= 0;
            item.PackedSize = 0;
            item.PackFlag = 0;
        };
        NFKFATClass(char* Name, int FilePos, int FileSize, int PackedSize, int PackFlag) {//full init
            item.DirName = my_strdup(Name);
            item.FilePos = FilePos;
            item.FileSize= FileSize;
            item.PackedSize = PackedSize;
            item.PackFlag = PackFlag;
        };
        NFKFATClass(int NameLen) { //init char*
            item.DirName = (char*)my_malloc(NameLen);
            item.FilePos = 0;
            item.FileSize= 0;
            item.PackedSize = 0;
            item.PackFlag = 0;
        };
        NFKFATClass(char* Name) { //full init char*
            item.DirName = my_strdup(Name);
            item.FilePos = 0;
            item.FileSize= 0;
            item.PackedSize = 0;
            item.PackFlag = 0;
        };        
        NFKFATClass(const NFKFATClass & fat) {//copy
            item.DirName = my_strdup(fat.item.DirName);
            item.FilePos = fat.item.FilePos;
            item.FileSize= fat.item.FileSize;
            item.PackedSize = fat.item.PackedSize;
            item.PackFlag = fat.item.PackFlag;
        };
        ~NFKFATClass() {//destruct
            if (item.DirName)
                my_free(item.DirName);
        };
    };

    struct NFKGlbFAT{
        long StructSize;
        vector<NFKFATClass>*  FATList; 
        vector<NFKFATClass>*  AddList; 
    };

    struct {
        char NameLen;
        char DirName[31];
        long PackFlag;
        long FilePos;
        long Junk;        
        long PackedSize;
        long RealSize;
        long Junk2[3];        
    } NFKFATItem;
#pragma pack(8)

long PackCheckNFK(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    long rd, FOffset, DirStart, DirLen, filestotal, SOffset;    

    unsigned char           t[] = "VTDB";     
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t-1, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;

    FOffset = SOffset+sizeof t-1+8;

    rd =  func->ReadFromFile(FileName, FOffset, &filestotal, 4);
    if (rd < 4) return -1;    
    if (filestotal<1) return -1;
    FOffset += rd;

    rd =  func->ReadFromFile(FileName, FOffset, &DirStart, 4);
    if (rd < 4) return -1;    
    if (DirStart<1||DirStart>WorkFileSize) return -1;

    DirLen = filestotal*sizeof NFKFATItem;

    if (DirLen > WorkFileSize||DirLen<1) return -1;

    FOffset = SOffset+DirStart;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*filestotal);
    if (fi == NULL) return -1;

    *finfo = fi;
    FilesNumber = filestotal;

    for (int i = 0; i<FilesNumber; i++){                    

        rd =  func->ReadFromFile(FileName, FOffset, &NFKFATItem, sizeof NFKFATItem);
        if (rd < sizeof NFKFATItem) return -1;
        
        FOffset += rd;
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = SOffset+NFKFATItem.FilePos;
        fi[i].FileSize    = NFKFATItem.PackedSize;        
        fi[i].UnpSize     = NFKFATItem.RealSize;        
        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);                
            }
            my_free(fi);
            return -1;
        }        

        NFKFATItem.DirName[NFKFATItem.NameLen] = 0;

        fi[i].FullFileName = my_strdup(NFKFATItem.DirName);
        fi[i].Description = my_strdup("");        
    }              
    
    FileOffset = SOffset;
    FileSize   = DirStart+DirLen;
    return SOffset+DirStart+DirLen;
}

BOOL WINAPI GetFATNFK (const char * PackName, long Offset, long Size, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{    
    long rd, FOffset, DirOffset, FilesNumber;

    FOffset = Offset+12;
    *FAT = NULL;
    NFKGlbFAT * NFKGlbFATItem = (NFKGlbFAT*)my_malloc(sizeof(NFKGlbFAT));
    NFKGlbFATItem->AddList = NULL;

    NFKGlbFATItem->StructSize = sizeof(NFKGlbFAT);
    NFKGlbFATItem->FATList = (vector<NFKFATClass>*)my_malloc(sizeof(vector<NFKFATClass>));

    rd =  func->ReadFromFile(PackName, FOffset, &FilesNumber, 4);
    if (rd < 4) {
        my_free(NFKGlbFATItem->FATList);
        my_free(NFKGlbFATItem);
        return FALSE;
    }    
    FOffset += rd;

    rd =  func->ReadFromFile(PackName, FOffset, &DirOffset, 4);
    if (rd < 4) {        
        my_free(NFKGlbFATItem->FATList);
        my_free(NFKGlbFATItem);
        return FALSE;
    }
    FOffset += rd;    

    FOffset = DirOffset+Offset;    
    
    for (int i = 0; i<FilesNumber; i++){        
        rd =  func->ReadFromFile(PackName, FOffset, &NFKFATItem, sizeof NFKFATItem);                
        if (rd < sizeof NFKFATItem) {
            NFKGlbFATItem->FATList->clear();
            my_free(NFKGlbFATItem->FATList);
            my_free(NFKGlbFATItem);            
            return FALSE;
        }
                
        NFKFATItem.DirName[NFKFATItem.NameLen] = 0;
        NFKGlbFATItem->FATList->push_back(NFKFATClass(NFKFATItem.DirName, NFKFATItem.FilePos, NFKFATItem.RealSize, NFKFATItem.PackedSize, NFKFATItem.PackFlag));
        
        FOffset += rd;        
    }              
    *FAT = NFKGlbFATItem;

    return TRUE;
}

BOOL WINAPI PutFileNFK(const char * PackName, const char * FileName, const char * PathName, const char * DiskPathName, long & StartOffset, long & FileSize, long & UnpSize, long Offset, long Size, void ** FAT, BOOL Compress, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{   
    char DirName[MAX_PATH];
    char DirName2[MAX_PATH];
    unsigned char * bufIn;
    unsigned char * bufOut;

    bufIn = (unsigned char *)my_malloc(UnpSize);
    if (!bufIn) return FALSE;

    bufOut = (unsigned char *)my_malloc(UnpSize*2);
    if (!bufOut) {my_free(bufIn);return FALSE;}

    NFKGlbFAT * NFKGlbFATItem = (NFKGlbFAT * )*FAT;    

    if (!NFKGlbFATItem->AddList) NFKGlbFATItem->AddList = (vector<NFKFATClass>*)my_malloc(sizeof(vector<NFKFATClass>));

    if (strlen(PathName)>0){
        if  (strlen(PathName)+strlen(FileName)+1<31){        
            strcpy(NFKFATItem.DirName, PathName);
            strcat(NFKFATItem.DirName, "\\");    
            strcat(NFKFATItem.DirName, FileName);
            for (int I = 0; I < strlen(NFKFATItem.DirName); I++) if (NFKFATItem.DirName[I]=='\\') NFKFATItem.DirName[I]='/';
        } else {
            strcpy(NFKFATItem.DirName, FileName);
        }        
    } else strcpy(NFKFATItem.DirName, FileName);   

    strcpy(DirName, DiskPathName);
    strcat(DirName, "\\");    
    strcat(DirName, FileName);    

    NFKFATItem.FilePos = NFKGlbFATItem->FATList->back().item.FilePos+NFKGlbFATItem->FATList->back().item.PackedSize;
    StartOffset = NFKFATItem.FilePos;
    NFKFATItem.RealSize = UnpSize;

    long rd = func->ReadFromFile(DirName, 0, bufIn, UnpSize);
    if (rd < UnpSize) return -1;    

    if (Z_OK != compress(bufOut, (unsigned long *)&FileSize, bufIn, UnpSize)){
        my_free(bufIn);
        my_free(bufOut);
        return FALSE;
    }

    sprintf(DirName2, "%s_packed", DirName);
    
    rd = func->SaveToFile(DirName2, 0, bufOut, FileSize);
    if (rd < FileSize){
        my_free(bufIn);
        my_free(bufOut);
        return FALSE;
    }

    FileSize += sizeof NFKFATItem;

    NFKFATItem.PackedSize = FileSize;
    NFKFATItem.PackFlag   = 1;

    my_free(bufIn);
    my_free(bufOut);

    NFKGlbFATItem->FATList->push_back(NFKFATClass(NFKFATItem.DirName, NFKFATItem.FilePos, NFKFATItem.RealSize, NFKFATItem.PackedSize, NFKFATItem.PackFlag));    
    
    
    NFKGlbFATItem->AddList->push_back(NFKFATClass(DirName2, NFKFATItem.FilePos, NFKFATItem.RealSize, NFKFATItem.PackedSize, NFKFATItem.PackFlag));
        
    return TRUE;
}

BOOL WINAPI DelFileNFK(const char * PackName, const char * FileName, long StartOffset, long FileSize, long UnpSize, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{   
    NFKGlbFAT * NFKGlbFATItem = (NFKGlbFAT * )*FAT;            

    for (vector<NFKFATClass>::iterator I = NFKGlbFATItem->FATList->begin(); I != NFKGlbFATItem->FATList->end();I++){
        if ((I->item.FilePos == StartOffset)&&
            (I->item.PackedSize== FileSize)) {
            I->item.FilePos = -1;
        };
    }        
    return TRUE;
}

BOOL WINAPI FreeFATNFK (const char * PackName, const char * TmpName, long Offset, long Size, long WorkFileSize, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{       
    char s[] = "VTDB";
    unsigned long magic = 0x3f800000;

    long FOffset = 0, r = 0, files = 0;

    //Copy from PackName to TmpName unused space
    if (Offset>0) func->FileToFileCopy(PackName, TmpName, 0, Offset, 0);

    NFKGlbFAT * NFKGlbFATItem = (NFKGlbFAT * )*FAT;

    FOffset = Offset;
    r = func->SaveToFile(TmpName, FOffset, s, sizeof s-1);
    FOffset += r;

    r = func->SaveToFile(TmpName, FOffset, &magic, sizeof magic);
    FOffset += r;

    r = func->SaveToFile(TmpName, FOffset, &magic, sizeof magic);
    FOffset += r;    

    long DirOffset = 0;

    r = func->SaveToFile(TmpName, FOffset, &files, 4);
    FOffset += r;

    r = func->SaveToFile(TmpName, FOffset, &DirOffset, 4);
    FOffset += r;
    FOffset += 44;    
    
    //Copy from PackName to TmpName allfiles        
    int i = 0;
    if (NFKGlbFATItem->AddList) {
        vector<NFKFATClass>::iterator J = NFKGlbFATItem->AddList->begin();
        for (vector<NFKFATClass>::iterator I = NFKGlbFATItem->FATList->begin(); I != NFKGlbFATItem->FATList->end();I++){
            if (I->item.FilePos != -1) {
                if (i < NFKGlbFATItem->FATList->size()-NFKGlbFATItem->AddList->size()) {
                    NFKFATItem.NameLen = strlen(I->item.DirName);
                    strcpy(NFKFATItem.DirName, I->item.DirName);                    
                    NFKFATItem.PackFlag = 1;            
                    NFKFATItem.Junk = 0;
                    NFKFATItem.FilePos = FOffset;
                    NFKFATItem.RealSize = I->item.FileSize;
                    NFKFATItem.Junk2[0] = 0; NFKFATItem.Junk2[1] = 0; NFKFATItem.Junk2[2] = 0;            
                    NFKFATItem.PackedSize = I->item.PackedSize;            
                    r = func->SaveToFile(TmpName, FOffset, &NFKFATItem, sizeof NFKFATItem);                    
                    func->FileToFileCopy(PackName, TmpName, I->item.FilePos+Offset+r, I->item.PackedSize-r, -1);
                    I->item.FilePos = FOffset;
                    FOffset += I->item.PackedSize;
                    files++;
                }else {
                    NFKFATItem.NameLen = strlen(I->item.DirName);
                    strcpy(NFKFATItem.DirName, I->item.DirName);                    
                    NFKFATItem.PackFlag = 1;            
                    NFKFATItem.Junk = 0;
                    NFKFATItem.FilePos = FOffset;
                    NFKFATItem.RealSize = J->item.FileSize;
                    NFKFATItem.Junk2[0] = 0; NFKFATItem.Junk2[1] = 0; NFKFATItem.Junk2[2] = 0;            
                    NFKFATItem.PackedSize = J->item.PackedSize;            
                    r = func->SaveToFile(TmpName, FOffset, &NFKFATItem, sizeof NFKFATItem);                    
                    func->FileToFileCopy(J->item.DirName, TmpName, 0, J->item.PackedSize-r, -1);
                    I->item.FilePos = FOffset;
                    FOffset += J->item.PackedSize;
                    DeleteFile(J->item.DirName);
                    files++;
                    J++;                    
                }
            }
            i++;
        }
    } else {
        for (vector<NFKFATClass>::iterator I = NFKGlbFATItem->FATList->begin(); I != NFKGlbFATItem->FATList->end();I++){            
            if (I->item.FilePos != -1) {
                NFKFATItem.NameLen = strlen(I->item.DirName);
                strcpy(NFKFATItem.DirName, I->item.DirName);                    
                NFKFATItem.PackFlag = 1;            
                NFKFATItem.Junk = 0;
                NFKFATItem.FilePos = FOffset;
                NFKFATItem.RealSize = I->item.FileSize;
                NFKFATItem.Junk2[0] = 0; NFKFATItem.Junk2[1] = 0; NFKFATItem.Junk2[2] = 0;            
                NFKFATItem.PackedSize = I->item.PackedSize;            
                r = func->SaveToFile(TmpName, FOffset, &NFKFATItem, sizeof NFKFATItem);                
                func->FileToFileCopy(PackName, TmpName, I->item.FilePos+Offset+r, I->item.PackedSize-r, -1);
                I->item.FilePos = FOffset;
                FOffset += I->item.FileSize;
                files++;
            }
        }
    }

    DirOffset = FOffset;
    func->SaveToFile(TmpName, Offset+16, &DirOffset, 4);        
    func->SaveToFile(TmpName, Offset+12, &files, 4);    

    //Save directory
    for (vector<NFKFATClass>::iterator I = NFKGlbFATItem->FATList->begin(); I != NFKGlbFATItem->FATList->end();I++){
        if (I->item.FilePos != -1) {
            NFKFATItem.NameLen = strlen(I->item.DirName);
            strcpy(NFKFATItem.DirName, I->item.DirName);                    
            NFKFATItem.PackFlag = 1;            
            NFKFATItem.Junk = 0;            
            NFKFATItem.FilePos = I->item.FilePos;
            NFKFATItem.RealSize = I->item.FileSize;
            NFKFATItem.Junk2[0] = 0; NFKFATItem.Junk2[1] = 0; NFKFATItem.Junk2[2] = 0;            
            NFKFATItem.PackedSize = I->item.PackedSize;            
            r = func->SaveToFile(TmpName, FOffset, &NFKFATItem, sizeof NFKFATItem);
            FOffset += r;
        }
    }

    //Save unused space
    if (WorkFileSize-(Offset+Size)>0) func->FileToFileCopy(PackName, TmpName, Offset+Size, WorkFileSize-(Offset+Size), -1);

    if (NFKGlbFATItem->AddList) {
        NFKGlbFATItem->AddList->clear();
        my_free(NFKGlbFATItem->AddList);
    }
    if (NFKGlbFATItem->FATList) {
        NFKGlbFATItem->FATList->clear();
        my_free(NFKGlbFATItem->FATList);
    }
    my_free(NFKGlbFATItem);
    NFKGlbFATItem = NULL;
#ifdef _DEBUG
//    DumpUnfreed();
#endif
    return TRUE;
}