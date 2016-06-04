long PackCheckUBISOFT_BF(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{   

#pragma pack(1)
    struct {        
        long PackedSize;
		long Flag1;
        long Flag2;
        long Type;
        long Junk;
        char FileName[64];
    }FileNameRecord;

	struct {        
        long Offset;		
        long Junk;        
    }OffsetRecord;

	struct {
        long Ver;
        long Files;
        long Junk1_1;
        long Junk001;
        long Junk002;
		long JunkFF1;
        long JunkFF2;
		long NameTableOct;
		long Junk2;
		long Junk3;
		long Files2;
		long Junk1_2;
		long OffsetTable;
		long JunkFF3;
		long Junk003;
		long NameTableOct2;
    }Header;

#pragma pack(8)
	char s[MAX_PATH];
    unsigned char           t[] = {'B','I','G',0x00};
    long rd, SOffset, FOffset, TOffset;

    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+4;

    rd =  func->ReadFromFile(FileName, FOffset, &Header, sizeof Header);
    if (rd < sizeof Header) return -1;

    FileSize = Header.NameTableOct*8+Header.OffsetTable;

    if (FileSize >= WorkFileSize) return -1;
    FOffset = SOffset+Header.OffsetTable;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*Header.Files);
    if (fi == NULL) return -1;

    *finfo = fi;
    FilesNumber = Header.Files;

	TOffset = SOffset+Header.NameTableOct*8+Header.OffsetTable;
    for (int i = 0; i<FilesNumber; i++){
		long FSize;
        rd =  func->ReadFromFile(FileName, FOffset, &OffsetRecord, sizeof OffsetRecord);
        if (rd < sizeof OffsetRecord) return -1;
        FOffset += rd;

		rd =  func->ReadFromFile(FileName, TOffset, &FileNameRecord, sizeof FileNameRecord);
        if (rd < sizeof FileNameRecord) return -1;
        TOffset += rd;

		if (FileNameRecord.PackedSize == 0)
			rd =  func->ReadFromFile(FileName, OffsetRecord.Offset, &FSize, sizeof FSize);
		else FSize = FileNameRecord.PackedSize;

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = OffsetRecord.Offset+4;
        fi[i].FileSize    = FSize-4;
		if ((OffsetRecord.Junk & 0xF0000000) == 0xF0000000) fi[i].UnpSize     = 0;
		else fi[i].UnpSize     = FSize-4;

		if (FileSize < OffsetRecord.Offset+FSize) FileSize = OffsetRecord.Offset+FSize;         

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);
            }
            my_free(fi);
            return -1;
        }

		if (FileNameRecord.FileName[0]==0) {
			if (i< 10)           sprintf(s, "big000000%d",  i);
			else if (i< 100)     sprintf(s, "big00000%d",   i);
			else if (i< 1000)    sprintf(s, "big0000%d",    i);
			else if (i< 10000)   sprintf(s, "big000%d",     i);
			else if (i< 100000)  sprintf(s, "big00%d",      i);
			else if (i< 1000000) sprintf(s, "big0%d",       i);
			else                 sprintf(s, "big%d",        i);

			fi[i].FullFileName = my_strdup(s);
		} else {
			fi[i].FullFileName = my_strdup(FileNameRecord.FileName);			
		}
		sprintf(s, "OR: %x, FNR: %d, %d, %d, %x", OffsetRecord.Junk, FileNameRecord.Flag1, FileNameRecord.Flag2, FileNameRecord.Type, FileNameRecord.Junk);
		fi[i].Description = my_strdup(s);			
    }

    FileOffset = SOffset;
    return WorkFileSize;
}

BOOL GetFileUBISOFT_BF(const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize, long UnpSize, struct RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags){
	unsigned char * bufIn;
    unsigned char * bufOut;
    char * dstpath;    
    unsigned long rd, FrameUnp, FramePack, FOffset;
	unsigned int iFrameUnp;
	BOOL CanUnpacking;

	FOffset = StartOffset;
	CanUnpacking = TRUE;	

	if (UnpSize == 0) {
		dstpath = my_strdup(PathName, strlen(FileName)+2);
		strcat(dstpath, "\\");
		strcat(dstpath, FileName);
		while (FOffset-StartOffset < FileSize){
			rd = func->ReadFromFile(PackName, FOffset, &FrameUnp, sizeof FrameUnp);
			if (rd < sizeof FrameUnp) {my_free(dstpath); return FALSE;}
			FOffset += rd;

			rd = func->ReadFromFile(PackName, FOffset, &FramePack, sizeof FramePack);
			if (rd < sizeof FramePack) {my_free(dstpath); return FALSE;}
			FOffset += rd;

			if ((FramePack == 0)||(FrameUnp == 0)) {my_free(dstpath); return TRUE;}

			if (FramePack != FrameUnp){
				bufIn = (unsigned char *)my_malloc(FramePack);
				if (!bufIn) {my_free(dstpath); return FALSE;}

				bufOut = (unsigned char *)my_malloc(FrameUnp);
				if (!bufOut) {my_free(bufIn);my_free(dstpath); return FALSE;}

				rd = func->ReadFromFile(PackName, FOffset, bufIn, FramePack);
				if (rd < FramePack) {
					my_free(dstpath);
					my_free(bufIn);
					my_free(bufOut);				
					return FALSE;
				}
				FOffset += rd;			
				iFrameUnp = FrameUnp;
				int r = lzo1x_decompress_safe(bufIn, FramePack, bufOut, &iFrameUnp, NULL);
				if(r == LZO_E_OK || r == LZO_E_INPUT_NOT_CONSUMED && iFrameUnp){
				} else {
					my_free(dstpath); 
					my_free(bufIn);
					my_free(bufOut);				
					return FALSE;
				}
				my_free(bufIn);
				
				if (func->AppendToFile(dstpath, bufOut, iFrameUnp) != iFrameUnp){
					my_free(dstpath);
					my_free(bufOut);
					return FALSE;
				}
				my_free(bufOut);
			} else {
				bufIn = (unsigned char *)my_malloc(FramePack);
				if (!bufIn) {my_free(dstpath); return FALSE;}

				rd = func->ReadFromFile(PackName, FOffset, bufIn, FramePack);
				if (rd < FramePack) {
					my_free(dstpath);
					my_free(bufIn);					
					return FALSE;
				}
				FOffset += rd;

				if (func->AppendToFile(dstpath, bufIn, FramePack) != FramePack){
					my_free(dstpath);
					my_free(bufIn);
					return FALSE;
				}
			}
		}
		my_free(dstpath);
		return TRUE;
	}
	return FALSE;
}