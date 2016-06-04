#pragma pack(1)
struct mh2pack{
        char *filename;
        long Offset;
        long filesize;
}mh2;

struct {
        BYTE Type;
        long Junk;
        long Len;
        //char ItemName[Len];
}MH2Item;

struct {        
        long DirCapacity;
}MH2Dir;//Type = 1

struct {
        long Flag;
        long Offset;
        long Size;
        long Junk;
}MH2File;//Type = 2
#pragma pack(8)
BOOL ReadDirMH2(const char * FileName, const char * Dir, long & Offset, long nItems, vector<mh2pack> & mh, struct RIP_SERVICE_FUNC *func)
{
    char Dir1[MAX_PATH];
    char Name[MAX_PATH];
    long rd;

    for (int i=0;i<nItems;i++){
        rd =  func->ReadFromFile(FileName, Offset, &MH2Item, sizeof MH2Item);
        if (rd < sizeof MH2Item) 
            return FALSE;

        Offset += rd;

        rd =  func->ReadFromFile(FileName, Offset, &Name, MH2Item.Len);
        if (rd < MH2Item.Len) 
            return FALSE;

        Name[MH2Item.Len] = 0;

        Offset += rd;

        if (MH2Item.Type == 1) {
            if (strlen(Dir)>0){
                strcpy (Dir1, Dir);
                strcat(Dir1, "\\");
            } else strcpy (Dir1, Dir);
            strcat(Dir1, Name);

            rd =  func->ReadFromFile(FileName, Offset, &MH2Dir, sizeof MH2Dir);
            if (rd < sizeof MH2Dir) return FALSE;

            Offset += rd;

            if (!ReadDirMH2(FileName, Dir1, Offset, MH2Dir.DirCapacity, mh, func)) return FALSE;
        } else {
            rd =  func->ReadFromFile(FileName, Offset, &MH2File, sizeof MH2File);
            if (rd < sizeof MH2File) return FALSE;
            Offset += rd;

            mh2.filename = my_strdup(Dir, 1+strlen(Name));          
            strcat(mh2.filename, "\\");
            strcat(mh2.filename, Name);
            mh2.filesize = (MH2File.Size   ^ 0x3355AAFF); 
            mh2.Offset   = (MH2File.Offset ^ 0xFFAA5533); //Very funny

            mh.push_back(mh2);
        }
    }
    return TRUE;
}

long PackCheckMH2(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    vector<mh2pack> mh;
    
    long rd, FOffset, DirStart, SOffset;
           
    unsigned char t[]                = "MUDGE4.0";
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t-1, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+0x3C;
    
    rd =  func->ReadFromFile(FileName, FOffset, &DirStart, sizeof DirStart);
    if (rd < sizeof DirStart) return -1;    

    if (DirStart+SOffset>WorkFileSize) return -1;

    FOffset = SOffset+DirStart+9;    

    rd =  func->ReadFromFile(FileName, FOffset, &MH2Dir, sizeof MH2Dir);
    if (rd < sizeof MH2Dir) return FALSE;

    FOffset += rd;
    
    if (!ReadDirMH2(FileName, "", FOffset, MH2Dir.DirCapacity, mh, func)) {
        for (int i=0; i< mh.size();i++) my_free(mh[i].filename);
    };

    if (mh.size()==0) return -1;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*mh.size());
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = mh.size();
    for (int i=0; i<mh.size();i++){
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = mh[i].Offset+SOffset;
        fi[i].FileSize    = mh[i].filesize+SOffset;

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j< mh.size();j++) 
                my_free(mh[j].filename);
            for (j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);        
            }
            my_free(fi);
            fi = NULL;
            return -1;
        }
        
        fi[i].FullFileName = my_strdup(mh[i].filename);                  
        fi[i].Description = my_strdup("");
    }              
    
    for (i=0; i< mh.size();i++) my_free(mh[i].filename);

    FileOffset = SOffset;
    FileSize = FOffset-SOffset;
    return FOffset;
}

BOOL GetFileMH2(const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize, long UnpSize, struct RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{
    char buf [16384];
    long rd, sv;
    char * dstpath = my_strdup(PathName, strlen(FileName)+2);
    strcat(dstpath, "\\");
    strcat(dstpath, FileName);
    char msg[MAX_PATH] = "Decoding\n";
    char msg2[MAX_PATH];

    if (!flags->Silent){
        strcat(msg, FileName);
        strcat(msg, "\n");              
    }

    for (int i = StartOffset; i<(FileSize+StartOffset);){
        if (i+sizeof buf<=FileSize+StartOffset)
             rd = func->ReadFromFile(PackName, i, buf, sizeof buf);
        else rd = func->ReadFromFile(PackName, i, buf, FileSize+StartOffset-i);
        if (rd == -1) {
            my_free(dstpath);
            return FALSE;
        }
        if (rd > 0) {
            i += rd;
            for (int j = 0; j<rd; j++) buf[j] ^= 0x88;
            sv = func->AppendToFile(dstpath, buf, rd);
            if (sv < rd) {
                my_free(dstpath);
                return FALSE;           
            }
        }
        if (!flags->Silent){
            sprintf(msg2, "%s%d/%d", msg, i-StartOffset, FileSize);
            func->ShowMessage(FALSE, FALSE, FALSE, TRUE, (const char * const *)msg2, 0, 0);
        }
    }    
    my_free(dstpath);
    return TRUE;
}