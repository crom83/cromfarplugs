long PackCheckTITBIT(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
#pragma pack(1)
    struct {        
		long Packed;
        long Offset;        
        long RealSize;
		long PackedSize;
    }TITBIT;
	struct {        
		char e;
        char z;        
        char d;
		char n;
		long files;
    }TITBIT_BEGIN;
#pragma pack(8)

	char fn[256];
    long rd, FOffset, DirLen;
    
    RIP_FILE_INFO           * fi;

    //if (FileOffset>0) return -1;

    FOffset = FileOffset;

    rd =  func->ReadFromFile(FileName, FOffset, &TITBIT_BEGIN, sizeof TITBIT_BEGIN);
    if (rd < sizeof TITBIT_BEGIN) return -1;    
    if (TITBIT_BEGIN.files <1) return -1;
	if ((TITBIT_BEGIN.e != 'e')||
		(TITBIT_BEGIN.z != 'z')||
		(TITBIT_BEGIN.d != 'd')||
		(TITBIT_BEGIN.n != -1)) return -1;

    DirLen = TITBIT_BEGIN.files*sizeof TITBIT;

    if (DirLen > WorkFileSize||DirLen<=0) return -1;

    FOffset = FileOffset+16;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*TITBIT_BEGIN.files);
    if (fi == NULL) return -1;

    *finfo = fi;
    FilesNumber = TITBIT_BEGIN.files;

    for (int i = 0; i<FilesNumber; i++){                    

        rd =  func->ReadFromFile(FileName, FOffset, &fn, sizeof fn);
        if (rd < 1) return -1;
        
        FOffset += strlen(fn)+2;

		rd =  func->ReadFromFile(FileName, FOffset, &TITBIT, sizeof TITBIT);
        if (rd < sizeof TITBIT) return -1;
        
        FOffset += sizeof TITBIT;

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = TITBIT.Offset;
        fi[i].FileSize    = TITBIT.PackedSize;                
        fi[i].UnpSize	  = TITBIT.RealSize;
        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);                
            }
            my_free(fi);
            return -1;
        }        
        
        fi[i].FullFileName = my_strdup(fn);
        fi[i].Description = my_strdup("");        
    }              
    
    FileOffset = 0;
    FileSize   = WorkFileSize;
    return WorkFileSize;    
}