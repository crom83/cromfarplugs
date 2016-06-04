long PackCheckMK3LCD(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct FATInfo{
		long Junk;
        long Size;
        long Offset;
    }FATItem;
#pragma pack(8)

    long rd, FOffset, nFiles;
    char s[MAX_PATH],s2[MAX_PATH];
    char * base;
    bool WasError = false;    
    int errorfrom;
    
    RIP_FILE_INFO           * fi;
            
    FOffset = 0;

    if (FileOffset>0) return -1;

    base = my_substr(FileName, 0, strlen(FileName)-3,3);
    strcat(base, "lcd");

    //test file existing
    rd =  func->ReadFromFile(base, FOffset, &nFiles, 4);
    if (rd < 4) {
        my_free(base);
        return -1;    
    }    

    if (nFiles<1) {
        my_free(base);
        return -1;
    }    

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) {
        my_free(base);
        return -1;
    }
    *finfo = fi;    

    FilesNumber = nFiles;    

    FOffset = 4;

    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(base, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset += rd;        
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
        fi[i].StartOffset = FATItem.Offset;
        fi[i].FileSize    = FATItem.Size;
        fi[i].UnpSize     = 0;
        if ((DWORD)fi[i].StartOffset+(DWORD)fi[i].FileSize>(DWORD)WorkFileSize||
            (DWORD)fi[i].FileSize>(DWORD)WorkFileSize||
			(DWORD)fi[i].StartOffset>(DWORD)WorkFileSize){
            WasError = true;
            errorfrom = i;
            break;
        }

        if (i< 10)           sprintf(s, "sfx000000%d", i);
        else if (i< 100)     sprintf(s, "sfx00000%d",  i);
        else if (i< 1000)    sprintf(s, "sfx0000%d",   i);
        else if (i< 10000)   sprintf(s, "sfx000%d",    i);
        else if (i< 100000)  sprintf(s, "sfx00%d",     i);
        else if (i< 1000000) sprintf(s, "sfx0%d",      i);
        else                 sprintf(s, "sfx%d",       i);

        sprintf(s2, "Junk data: %d", FATItem.Junk);

        fi[i].FullFileName = my_strdup(s,4);
        lstrcat(fi[i].FullFileName, ".raw");
        fi[i].ConvFullFileName = my_strdup(s,4);
        lstrcat(fi[i].ConvFullFileName, ".wav");
        fi[i].Description = my_strdup(s2);         
    }

    if (WasError){
        for (int j=0; j<errorfrom; j++){
            my_free(fi[j].FullFileName);                
            my_free(fi[j].ConvFullFileName);
            my_free(fi[j].Description);        
        }
        my_free(fi);
        my_free(base);
        fi = NULL;
        return -1;
    }
    
    my_free(base);
    FileOffset = 0;
    FileSize = WorkFileSize;
    return WorkFileSize;    
}

BOOL GetFileMK3LCD(const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize, long UnpSize, struct RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{
    char * dstpath = my_strdup(PathName, strlen(FileName)+2);
    strcat(dstpath, "\\");
    strcat(dstpath, FileName);
    char msg[MAX_PATH] = "Converting to WAV\n";
        
    if (!flags->Silent){
        strcat(msg, FileName);
        strcat(msg, "\n");
        func->ShowMessage(FALSE, FALSE, FALSE, TRUE, (const char * const *)msg, 0, 0);
    }

    long l;
    short s;
    //long Bitrate = LOWORD(UnpSize);    
	long Bitrate = 16000;

    func->AppendToFile(dstpath, "RIFF", 4);
    l = FileSize+38;
    func->AppendToFile(dstpath, &l, 4);
    func->AppendToFile(dstpath, "WAVEfmt ", 8);

    l = 0x12;
    func->AppendToFile(dstpath, &l, 4);

    l = 0x00010001;//PCM & Mono
    func->AppendToFile(dstpath, &l, 4); 

    func->AppendToFile(dstpath, &Bitrate, 4);
    
	l = Bitrate*2;
	//l = Bitrate;

    func->AppendToFile(dstpath, &l, 4);

    //if (!GTA1)
         l = 0x00100002;
    //l = 0x00080001;    

    func->AppendToFile(dstpath, &l, 4); 
    s = 0;
    func->AppendToFile(dstpath, &s, 2); 
    func->AppendToFile(dstpath, "data", 4);
    func->AppendToFile(dstpath, &FileSize, 4);
    
    func->FileToFileCopy(PackName, dstpath, StartOffset, FileSize, -1);
    
    my_free(dstpath);
    return TRUE;
}