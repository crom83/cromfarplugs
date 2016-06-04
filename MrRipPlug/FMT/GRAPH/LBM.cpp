long FormatCheckLBM(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    union {
        unsigned char c[4]; //DEJAVU size
        unsigned long  i;
    } lbm;

    long rd, FOffset;       
    unsigned char t1[] = "FORM";
    char t2[] = "PBM";
    char c[4];
    long fs;     

    long fres = func->Find(t1, BaseString, sizeof t1-1, readed);
    if (fres == -1) return -1;

    FOffset = FileOffset+fres+sizeof t1-1;

    rd =  func->ReadFromFile(FileName, FOffset++, &lbm.c[3], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &lbm.c[2], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &lbm.c[1], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &lbm.c[0], 1);    
    if (rd < 4) return -1;

    fs = lbm.i+8;

    rd = func->ReadFromFile(FileName, FOffset, &c, 3);
    if (rd < 3) return -1;        
    
    c[3] = 0;

    if (strcmp(c, t2)!=0) return -1;

    if ((fs+FileOffset+fres<0)||
        (WorkFileSize<fs+FileOffset+fres)||
        (fs <= 0)) return -1;
    
    FileSize = fs;
    UnpSize  = FileSize;
    strcpy(finfo->ExtNormal, ".lbm");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "IFF Bitmap file");    

    FileOffset += fres;
    FOffset     = FileOffset+FileSize;
    
    return FOffset;    
}
