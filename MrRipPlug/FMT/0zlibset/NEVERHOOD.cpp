long PackCheckNEVERHOOD(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{

#pragma pack(1)
    struct {
		char Type;
		char PackMethod;
		unsigned short Junk1;
		unsigned long  Junk2;		
        unsigned long  Offset;
		long PackedSize;
		long UnpackedSize;        
    }FileRecord;
    	

	struct {
        short Junk1;        
        long FileSize;
        long Files;        
	}Header;

#pragma pack(8)
	char s[MAX_PATH];
    unsigned char           t[] = {0x40,0x49,0x00,0x02,0x07,0x00};
    long rd, SOffset, FOffset, TOffset, Fname;

    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t;

    rd =  func->ReadFromFile(FileName, FOffset, &Header, sizeof Header);
    if (rd < sizeof Header) return -1;

    FileSize = Header.FileSize;

    if (FileSize > WorkFileSize) return -1;
    FOffset = FOffset+sizeof Header;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*Header.Files);
    if (fi == NULL) return -1;

    *finfo = fi;
    FilesNumber = Header.Files;

	TOffset = FOffset+Header.Files*4;	
	for (int i = 0; i<FilesNumber; i++){
        rd =  func->ReadFromFile(FileName, FOffset, &Fname, sizeof Fname);
        if (rd < sizeof Fname) return -1;
        FOffset += rd;

		rd =  func->ReadFromFile(FileName, TOffset, &FileRecord, sizeof FileRecord);
        if (rd < sizeof FileRecord) return -1;
        TOffset += rd;		
        		

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        		

        fi[i].StartOffset = FileRecord.Offset;
        fi[i].FileSize    = FileRecord.PackedSize;
		if (FileRecord.PackMethod == 1) fi[i].UnpSize     = 0;
		else fi[i].UnpSize     = FileRecord.UnpackedSize;		

		if (FileRecord.Offset == 0 &&
			FileRecord.UnpackedSize == 0){
			fi[i].FileSize    = 0;
			sprintf(s, "%x.%s",  Fname, "nul");
			fi[i].FullFileName = my_strdup(s);		
			sprintf(s, "T: %x, P: %x, J1: %x, J2: %x", FileRecord.Type, FileRecord.PackMethod, FileRecord.Junk1, FileRecord.Junk2);
			fi[i].Description = my_strdup(s);
			continue;
		}

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j<i; j++){
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);
            }
            my_free(fi);
            return -1;
        }

		switch (FileRecord.Type){
			case 0x0a: sprintf(s, "%x.%s",  Fname, "smk");
			break;
			case 0x06: sprintf(s, "%x.%s",  Fname, "txt");
			break;
			case 0x03: sprintf(s, "%x.%s",  Fname, "pal");
			break;
			case 0x02: sprintf(s, "%x.%s",  Fname, "spr");
			break;
			case 0x04: sprintf(s, "%x.%s",  Fname, "img");
			break;
			case 0x07: sprintf(s, "%x.%s",  Fname, "raw");
			case 0x08: sprintf(s, "%x.%s",  Fname, "raw");
			break;
			default:   sprintf(s, "%x.%s",  Fname, "unk");
		}		
		
		fi[i].FullFileName = my_strdup(s);
		
		sprintf(s, "T: %x, P: %x, J1: %x, J2: %x", FileRecord.Type, FileRecord.PackMethod, FileRecord.Junk1, FileRecord.Junk2);
		fi[i].Description = my_strdup(s);
	}

	FileOffset = SOffset;
	return FileSize;
}

struct inf_t{
	unsigned char * bufIn;
	unsigned long len;
};
struct out_t{
	unsigned char * bufOut;
	unsigned long pos;
};

static unsigned inf(void *parent, unsigned char **buf)
{
    *buf = ((inf_t *)parent)->bufIn;	
    return ((inf_t *)parent)->len;
}
static int outf(void *parent, unsigned char *buf, unsigned len)
{
	memcpy(((out_t *)parent)->bufOut+((out_t *)parent)->pos, buf, len);
	((out_t *)parent)->pos += len;
    return 0;
}

BOOL GetFileNEVERHOOD(const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize, long UnpSize, struct RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags){
	inf_t inft;
	out_t outt;    
    char * dstpath;
    long rd;    

	if (UnpSize == 0) return FALSE;
    inft.bufIn = (unsigned char *)my_malloc(FileSize);
    if (!inft.bufIn) return FALSE;
	inft.len = FileSize;

    outt.bufOut = (unsigned char *)my_malloc(UnpSize);
	if (!outt.bufOut) {my_free(inft.bufIn);return FALSE;}
	outt.pos = 0;

    rd = func->ReadFromFile(PackName, StartOffset, inft.bufIn, FileSize);
    if (rd < FileSize) return FALSE;
    	
	if(blast(inf, &inft, outf, &outt) != 0){		
        my_free(inft.bufIn);
        my_free(outt.bufOut);
        return FALSE;		
	}

    my_free(inft.bufIn);
    dstpath = my_strdup(PathName, strlen(FileName)+2+strlen(FileName));	
    strcat(dstpath, "\\");
    strcat(dstpath, FileName);
	if (func->SaveToFile(dstpath, 0, outt.bufOut, UnpSize) != (long)UnpSize){
        my_free(dstpath);
        my_free(outt.bufOut);
        return FALSE;
    }
    my_free(dstpath);
    my_free(outt.bufOut);
    return TRUE;
}	