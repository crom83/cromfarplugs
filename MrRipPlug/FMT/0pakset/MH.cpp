#pragma pack(1)
struct mhpack{
        char filename[48];
        long Offset;        
        long filesize;
        char junk[8];
}mh_p;
#pragma pack(8)

long PackCheckMH(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{      
    vector<mhpack> mh;
    
    long rd, FOffset, DirEnd, SOffset, MaxEnd;
       
    unsigned char t[]                = "MH";
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t-1, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+0x30;
    

    rd =  func->ReadFromFile(FileName, FOffset, &DirEnd, sizeof DirEnd);
    if (rd < sizeof DirEnd) return -1;    

    if (DirEnd+SOffset>WorkFileSize) return -1;

    FOffset += 0x10;
    MaxEnd = DirEnd;
    do{

        rd =  func->ReadFromFile(FileName, FOffset, &mh_p, sizeof mh_p);
        if (rd < sizeof mh_p) return -1;    
        
        if (mh_p.Offset+mh_p.filesize>WorkFileSize) return -1;
        FOffset += rd;

        if (strcmp(mh_p.filename, "****")==0) break;        
        
        if (MaxEnd<mh_p.Offset+mh_p.filesize) MaxEnd = mh_p.Offset+mh_p.filesize;
        mh.push_back(mh_p);               
    }
    while (FOffset<WorkFileSize);    

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
            for (int j=0; j<i; j++)
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
    
    FileOffset = SOffset;
    FileSize = MaxEnd;
    return MaxEnd+SOffset;    
}