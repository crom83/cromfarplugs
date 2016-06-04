long FormatCheckAIFF(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    union {
        unsigned char c[4];
        unsigned long  i;
    } aiff;

    long rd, FOffset;       
    unsigned char t1[] = "FORM";
    char t2[] = "AIFFCOMM";
    char c[9];
    long fs;     

    long fres = func->Find(t1, BaseString, sizeof t1-1, readed);
    if (fres == -1) return -1;

    FOffset = FileOffset+fres+sizeof t1-1;

    rd =  func->ReadFromFile(FileName, FOffset++, &aiff.c[3], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &aiff.c[2], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &aiff.c[1], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &aiff.c[0], 1);    
    if (rd < 4) return -1;

    fs = aiff.i;

    rd = func->ReadFromFile(FileName, FOffset, &c, sizeof t2-1);
    if (rd < sizeof t2-1) return -1;        
    
    c[8] = 0;

    if (strcmp(c, t2)!=0) return -1;

    if ((fs+FileOffset+fres<0)||
        (WorkFileSize<fs+FileOffset+fres)||
        (fs <= 0)) return -1;
    
    FileSize = fs;
    UnpSize  = FileSize;
    strcpy(finfo->ExtNormal, ".aiff");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "IFF Audio file");    

    FileOffset += fres;
    FOffset     = FileOffset+FileSize;
    
    return FOffset;    
}
