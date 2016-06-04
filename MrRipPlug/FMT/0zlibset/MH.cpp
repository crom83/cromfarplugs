long PackCheckMH(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
#pragma pack(1)
    struct DatInfo{
        char fname[32];
        long Offset;
        long PackedSize;
        long RealSize;        
    }MH;
#pragma pack(8)

    long rd, FOffset, DirLen, filestotal;        
    
    RIP_FILE_INFO           * fi;

    if (FileOffset>0) return -1;

    FOffset = WorkFileSize-4;

    rd =  func->ReadFromFile(FileName, FOffset, &filestotal, 4);
    if (rd < 4) return -1;    
    if (filestotal<1) return -1;

    DirLen = filestotal*sizeof MH;

    if (DirLen > WorkFileSize||DirLen<=0) return -1;

    FOffset = WorkFileSize-DirLen-4;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*filestotal);
    if (fi == NULL) return -1;

    *finfo = fi;
    FilesNumber = filestotal;

    for (int i = 0; i<FilesNumber; i++){                    

        rd =  func->ReadFromFile(FileName, FOffset, &MH, sizeof MH);
        if (rd < sizeof MH) return -1;
        
        FOffset += rd;
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = MH.Offset;
        fi[i].FileSize    = MH.PackedSize;                
        fi[i].UnpSize = MH.RealSize;
        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);                
            }
            my_free(fi);
            return -1;
        }        
        
        fi[i].FullFileName = my_strdup(MH.fname);
        fi[i].Description = my_strdup("");        
    }              
    
    FileOffset = 0;
    FileSize   = WorkFileSize;
    return WorkFileSize;    
}