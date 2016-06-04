BYTE DecodeMark(BYTE in){
	__asm{
			mov al, in
			xor al, 0xAD
			rol al, 4
			mov in, al
		}
	return in;
}


long PackCheckFM05(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
#pragma pack(1)
	union {
		struct {
			BYTE Size;
			char Name[256];		
		}FATInfo;
		char Dump[257];
	}FATItemName;

	union {
		long Size;		
		char Dump[4];
    }FATItem;
#pragma pack(8)

	unsigned char           t[] = {0x99, 0x88, 0xB9, 0xF9, 0x0F};
    long rd, FOffset, SOffset, TOffset, nFiles;
    char s[MAX_PATH],s2[MAX_PATH];    
    bool WasError = false;
    int errorfrom;	

    RIP_FILE_INFO           * fi;
    
	long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+5;
    if (WorkFileSize > 2000000)  FOffset += 3;
	
    rd =  func->ReadFromFile(FileName, FOffset, &FATItem, 4);
    if (rd < 4) return -1;    
	FOffset += rd;
	
	for (int j=0;j<sizeof FATItem.Dump;j++)
		FATItem.Dump[j] = DecodeMark(FATItem.Dump[j]);

	if (WorkFileSize < 2000000)  
		nFiles = func->SwapLong(FATItem.Size);
	else {
		nFiles = FATItem.Size;		
		nFiles &= 0x0000FFFF;
		nFiles -= 0x500;
		if (WorkFileSize > 1000000000) nFiles -= 0x280;
		FOffset -= 2;
	}
	if (nFiles < 1) return -1;

	FilesNumber = nFiles;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*FilesNumber);
    if (fi == NULL) return -1;
    *finfo = fi;

	for (int i = 0; i<FilesNumber; i++){
		rd =  func->ReadFromFile(FileName, FOffset, &FATItemName.Dump, sizeof FATItemName.Dump);
        if (rd < sizeof FATItemName.Dump)  return -1;
		BYTE tmp;
		FATItemName.Dump[0] = DecodeMark(FATItemName.Dump[0]);
        FOffset += 1+FATItemName.FATInfo.Size+sizeof FATItem.Dump;

	}

	TOffset = FOffset;
	FOffset = SOffset+9;
	if (WorkFileSize > 2000000)  FOffset += 1;

    for (int i = 0; i<FilesNumber; i++){
        rd =  func->ReadFromFile(FileName, FOffset, &FATItemName.Dump, sizeof FATItemName.Dump);
        if (rd < sizeof FATItemName.Dump)  {
            WasError = true;
            errorfrom = i;
            break;
        }
		for (int j=0;j<sizeof FATItemName.Dump;j++)
			FATItemName.Dump[j] = DecodeMark(FATItemName.Dump[j]);

		FATItemName.FATInfo.Name[FATItemName.FATInfo.Size] = 0;
        FOffset += 1+FATItemName.FATInfo.Size;

		rd =  func->ReadFromFile(FileName, FOffset, &FATItem.Dump, sizeof FATItem.Dump);
        if (rd < sizeof FATItem.Dump)  {
            WasError = true;
            errorfrom = i;
            break;
        }
		for (j=0;j<sizeof FATItem.Dump;j++)
			FATItem.Dump[j] = DecodeMark(FATItem.Dump[j]);
		
		FOffset += rd;

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = TOffset;		
        fi[i].FileSize    = FATItem.Size;
        fi[i].UnpSize     = 0;
        if ((DWORD)fi[i].StartOffset+(DWORD)fi[i].FileSize>(DWORD)WorkFileSize||
            (DWORD)fi[i].FileSize>(DWORD)WorkFileSize||
            (DWORD)fi[i].StartOffset>(DWORD)WorkFileSize){
            WasError = true;
            errorfrom = i;
            break;
        }     

		if (FileSize<TOffset+FATItem.Size) FileSize=TOffset+FATItem.Size;

        fi[i].FullFileName = my_strdup(FATItemName.FATInfo.Name);        
        fi[i].ConvFullFileName = NULL;
        fi[i].Description = my_strdup("");
		TOffset += FATItem.Size;
    }

    if (WasError){
        for (int j=0; j<errorfrom; j++){
            my_free(fi[j].FullFileName);
            my_free(fi[j].ConvFullFileName);
            my_free(fi[j].Description);
        }
        my_free(fi);        
        fi = NULL;
        return -1;
    }    
    return FileSize;
}

BOOL GetFileFM05(const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize, long UnpSize, struct RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{	
    char * dstpath = my_strdup(PathName, strlen(FileName)+2);
    strcat(dstpath, "\\");
    strcat(dstpath, FileName);
    char msg[MAX_PATH] = "Extracting...\n";
	char buf [16384];
	long rd, sv;

    if (!flags->Silent){
        strcat(msg, FileName);
        strcat(msg, "\n");        
		func->ShowMessage(FALSE, FALSE, FALSE, TRUE, (const char * const *)msg, 0, 0);
    }
	
	for (int i = StartOffset; i<FileSize+StartOffset;){
		if (i+sizeof buf<=FileSize+StartOffset)
             rd = func->ReadFromFile(PackName, i, buf, sizeof buf);
        else rd = func->ReadFromFile(PackName, i, buf, FileSize+StartOffset-i);        
        if (rd == -1) {
            my_free(dstpath);
            return FALSE;
        }
        if (rd > 0) {
            i += rd;
			for (int j = 0; j<rd; j++)
				buf[j] = DecodeMark(buf[j]);

            sv = func->AppendToFile(dstpath, buf, rd);
            if (sv < rd) {
                my_free(dstpath);
                return FALSE;           
            }
        }        
    }    

    my_free(dstpath);
    return TRUE;
}