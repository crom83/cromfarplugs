long FormatCheckAUSND(const char * FileName, unsigned char* BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long rd, SOffset, FOffset;
    unsigned char t[] = ".snd";
    
    long fres = func->Find(t, BaseString, 4, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres+4;
    FOffset = SOffset + 4;

    rd = func->ReadFromFile(FileName, FOffset, &FileSize, 4);
    if (rd < 4)return -1;        

    FileSize = func->SwapLong(FileSize);

    FOffset += rd;
    FileSize += 24;

    if ((FileSize+FileOffset+fres<0)||
        (WorkFileSize<(FileSize+FileOffset+fres))||
        (FileSize<=0)) return -1;       
        
    UnpSize = FileSize;

    strcpy(finfo->ExtNormal,   ".au");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "Sun audio file");    
        
    return SOffset+FileSize;
}