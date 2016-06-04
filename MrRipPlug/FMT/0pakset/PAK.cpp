#pragma pack(1)
    struct QuakeFATType{
        char * DirName;
        long FilePos;
        long FileSize;
    };

    class QuakeFATClass{
    public:
        QuakeFATType item;
        QuakeFATClass() { //simple init
            item.DirName = NULL;
            item.FilePos = 0;
            item.FileSize= 0;
        };
        QuakeFATClass(char* Name, int FilePos, int FileSize) {//full init
            item.DirName = my_strdup(Name);
            item.FilePos = FilePos;
            item.FileSize= FileSize;
        };
        QuakeFATClass(int NameLen) { //init char*
            item.DirName = (char*)my_malloc(NameLen);
            item.FilePos = 0;
            item.FileSize= 0;
        };
        QuakeFATClass(char* Name) { //full init char*
            item.DirName = my_strdup(Name);
            item.FilePos = 0;
            item.FileSize= 0;
        };        
        QuakeFATClass(const QuakeFATClass & fat) {//copy
            item.DirName = my_strdup(fat.item.DirName);
            item.FilePos = fat.item.FilePos;
            item.FileSize= fat.item.FileSize;
        };
        ~QuakeFATClass() {//destruct
            if (item.DirName)
                my_free(item.DirName);
        };
    };

    struct QuakeGlbFAT{
        long StructSize;
        vector<QuakeFATClass>*  FATList; 
        vector<QuakeFATClass>*  AddList; 
    };

    struct QuakeFAT{
        char DirName[56];
        long FilePos;
        long FileSize;
    }QuakeFATItem;
#pragma pack(8)

long PackCheckPAK(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{   

    long rd, SOffset, FOffset, DirOffset, DirLen, z;
    unsigned char           t[] = {0x50, 0x41, 0x43, 0x4B}; //QuakePAK start signature    
    
    RIP_FILE_INFO           * fi;
    bool WasError = false;
    int errorfrom;

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t;

    rd =  func->ReadFromFile(FileName, FOffset, &DirOffset, 4);
    if (rd < 4) return -1;
    FOffset += rd;

    rd =  func->ReadFromFile(FileName, FOffset, &DirLen, 4);
    if (rd < 4) return -1;

    z = DirOffset+DirLen+SOffset;
    if (z>WorkFileSize)
        return -1;
    
    FilesNumber = DirLen/64;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*FilesNumber);
    if (fi == NULL) return -1;
    *finfo = fi;    
    
    FOffset = DirOffset+SOffset;
    for (int i = 0; i<FilesNumber; i++){                    
        rd =  func->ReadFromFile(FileName, FOffset, &QuakeFATItem, sizeof QuakeFATItem);
        if (rd < sizeof QuakeFATItem)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = QuakeFATItem.FilePos+SOffset;
        fi[i].FileSize    = QuakeFATItem.FileSize;
        fi[i].UnpSize     = QuakeFATItem.FileSize;        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_strdup(QuakeFATItem.DirName);
        fi[i].Description = (char*)my_malloc(strlen("")+1);        
        strcpy(fi[i].Description, "");
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
    FileSize = z-FileOffset;
    return z;    
}

BOOL WINAPI GetFATPAK (const char * PackName, long Offset, long Size, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{    
    long rd, FOffset, DirOffset, DirLen, FilesNumber;

    FOffset = Offset+4;
    *FAT = NULL;
    QuakeGlbFAT * QuakeGlbFATItem = (QuakeGlbFAT*)my_malloc(sizeof(QuakeGlbFAT));
    QuakeGlbFATItem->AddList = NULL;

    QuakeGlbFATItem->StructSize = sizeof(QuakeGlbFAT);
    QuakeGlbFATItem->FATList = (vector<QuakeFATClass>*)my_malloc(sizeof(vector<QuakeFATClass>));

    rd =  func->ReadFromFile(PackName, FOffset, &DirOffset, 4);
    if (rd < 4) {        
        my_free(QuakeGlbFATItem->FATList);
        my_free(QuakeGlbFATItem);
        return FALSE;
    }
    FOffset += rd;

    rd =  func->ReadFromFile(PackName, FOffset, &DirLen, 4);
    if (rd < 4) {        
        my_free(QuakeGlbFATItem->FATList);
        my_free(QuakeGlbFATItem);
        return FALSE;
    }    

    FOffset = DirOffset+Offset;
    FilesNumber = DirLen/64;
    
    for (int i = 0; i<FilesNumber; i++){        
        rd =  func->ReadFromFile(PackName, FOffset, &QuakeFATItem, sizeof QuakeFATItem);                
        if (rd < sizeof QuakeFATItem) {
            QuakeGlbFATItem->FATList->clear();
            my_free(QuakeGlbFATItem->FATList);
            my_free(QuakeGlbFATItem);            
            return FALSE;
        }
                
        QuakeGlbFATItem->FATList->push_back(QuakeFATClass(QuakeFATItem.DirName, QuakeFATItem.FilePos, QuakeFATItem.FileSize));        
        
        FOffset += rd;        
    }              
    *FAT = QuakeGlbFATItem;

    return TRUE;
}

BOOL WINAPI PutFilePAK(const char * PackName, const char * FileName, const char * PathName, const char * DiskPathName, long & StartOffset, long & FileSize, long & UnpSize, long Offset, long Size, void ** FAT, BOOL Compress, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{   
    char DirName[MAX_PATH];
    QuakeGlbFAT * QuakeGlbFATItem = (QuakeGlbFAT * )*FAT;    

    if (!QuakeGlbFATItem->AddList) QuakeGlbFATItem->AddList = (vector<QuakeFATClass>*)my_malloc(sizeof(vector<QuakeFATClass>));

    if (strlen(PathName)>0){
        if  (strlen(PathName)+strlen(FileName)+1<56){        
            strcpy(QuakeFATItem.DirName, PathName);
            strcat(QuakeFATItem.DirName, "\\");    
            strcat(QuakeFATItem.DirName, FileName);
            for (int I = 0; I < strlen(QuakeFATItem.DirName); I++) if (QuakeFATItem.DirName[I]=='\\') QuakeFATItem.DirName[I]='/';
        } else {
            strcpy(QuakeFATItem.DirName, FileName);
        }        
    } else strcpy(QuakeFATItem.DirName, FileName);   

    QuakeFATItem.FilePos = QuakeGlbFATItem->FATList->back().item.FilePos+QuakeGlbFATItem->FATList->back().item.FileSize;
    StartOffset = QuakeFATItem.FilePos;
    QuakeFATItem.FileSize = FileSize;
    
    QuakeGlbFATItem->FATList->push_back(QuakeFATClass(QuakeFATItem.DirName, QuakeFATItem.FilePos, QuakeFATItem.FileSize));
        
    strcpy(DirName, DiskPathName);
    strcat(DirName, "\\");    
    strcat(DirName, FileName);    
    QuakeFATItem.FileSize = FileSize;
    UnpSize = FileSize;
    
    QuakeGlbFATItem->AddList->push_back(QuakeFATClass(DirName, QuakeFATItem.FilePos, QuakeFATItem.FileSize));
        
    return TRUE;
}

BOOL WINAPI DelFilePAK(const char * PackName, const char * FileName, long StartOffset, long FileSize, long UnpSize, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{   
    QuakeGlbFAT * QuakeGlbFATItem = (QuakeGlbFAT * )*FAT;            

    for (vector<QuakeFATClass>::iterator I = QuakeGlbFATItem->FATList->begin(); I != QuakeGlbFATItem->FATList->end();I++){
        if ((I->item.FilePos == StartOffset)&&
            (I->item.FileSize== FileSize)) {
            I->item.FilePos = -1;
        };
    }        
    return TRUE;
}

BOOL WINAPI FreeFATPAK (const char * PackName, const char * TmpName, long Offset, long Size, long WorkFileSize, void ** FAT, struct RIP_SERVICE_FUNC * func, struct RIP_FLAGS *flags)
{       
    char s[] = "PACK";    

    long FOffset = 0, r = 0, files = 0;

    //Copy from PackName to TmpName unused space
    if (Offset>0) func->FileToFileCopy(PackName, TmpName, 0, Offset, 0);

    QuakeGlbFAT * QuakeGlbFATItem = (QuakeGlbFAT * )*FAT;

    FOffset = Offset;
    r = func->SaveToFile(TmpName, FOffset, s, sizeof s-1);
    FOffset += r;

    long DirSize = QuakeGlbFATItem->FATList->size()*sizeof QuakeFATItem;
    long DirOffset = 0;

    r = func->SaveToFile(TmpName, FOffset, &DirOffset, 4);
    FOffset += r;
    r = func->SaveToFile(TmpName, FOffset, &DirSize, 4);
    FOffset += r;
    
    //Copy from PackName to TmpName allfiles        
    int i = 0;
    if (QuakeGlbFATItem->AddList) {
        vector<QuakeFATClass>::iterator J = QuakeGlbFATItem->AddList->begin();
        for (vector<QuakeFATClass>::iterator I = QuakeGlbFATItem->FATList->begin(); I != QuakeGlbFATItem->FATList->end();I++){
            if (I->item.FilePos != -1) {
                if (i < QuakeGlbFATItem->FATList->size()-QuakeGlbFATItem->AddList->size()) {
                    func->FileToFileCopy(PackName, TmpName, I->item.FilePos+Offset, I->item.FileSize, -1);
                    I->item.FilePos = FOffset;
                    FOffset += I->item.FileSize;
                    files++;
                }else {
                    func->FileToFileCopy(J->item.DirName, TmpName, 0, J->item.FileSize, -1);
                    I->item.FilePos = FOffset;
                    FOffset += J->item.FileSize;
                    files++;
                    J++;
                }
            }
            i++;
        }
    } else {
        for (vector<QuakeFATClass>::iterator I = QuakeGlbFATItem->FATList->begin(); I != QuakeGlbFATItem->FATList->end();I++){            
            if (I->item.FilePos != -1) {
                func->FileToFileCopy(PackName, TmpName, I->item.FilePos+Offset, I->item.FileSize, -1);
                I->item.FilePos = FOffset;
                FOffset += I->item.FileSize;
                files++;
            }
        }
    }

    DirOffset = FOffset;
    func->SaveToFile(TmpName, Offset+4, &DirOffset, 4);    
    DirSize = files*sizeof QuakeFATItem;
    func->SaveToFile(TmpName, Offset+8, &DirSize, 4);    

    //Save directory
    for (vector<QuakeFATClass>::iterator I = QuakeGlbFATItem->FATList->begin(); I != QuakeGlbFATItem->FATList->end();I++){
        if (I->item.FilePos != -1) {
            strcpy(QuakeFATItem.DirName, I->item.DirName);
            QuakeFATItem.FilePos = I->item.FilePos;
            QuakeFATItem.FileSize = I->item.FileSize;
            r = func->SaveToFile(TmpName, FOffset, &QuakeFATItem, sizeof QuakeFATItem);
            FOffset += r;
        }
    }

    //Save unused space
    if (WorkFileSize-(Offset+Size)>0) func->FileToFileCopy(PackName, TmpName, Offset+Size, WorkFileSize-(Offset+Size), -1);

    if (QuakeGlbFATItem->AddList) {
        QuakeGlbFATItem->AddList->clear();
        my_free(QuakeGlbFATItem->AddList);
    }
    if (QuakeGlbFATItem->FATList) {
        QuakeGlbFATItem->FATList->clear();
        my_free(QuakeGlbFATItem->FATList);
    }
    my_free(QuakeGlbFATItem);
    QuakeGlbFATItem = NULL;
#ifdef _DEBUG
    DumpUnfreed();
#endif
    return TRUE;
}