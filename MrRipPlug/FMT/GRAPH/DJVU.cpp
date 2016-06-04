long FormatCheckDJVU(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    union {
        unsigned char c[4]; //DEJAVU size
        unsigned long  i;
    } djvu;
    long rd, FOffset;
    unsigned char t[8] = {0x41, 0x54, 0x26, 0x54, 0x46, 0x4F, 0x52, 0x4D}; //DEJAVU start signature         

    long fres = func->Find(t, BaseString, 8, readed);
    if (fres == -1) return -1;

    FOffset = FileOffset+fres+8;

    rd =  func->ReadFromFile(FileName, FOffset++, &djvu.c[3], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &djvu.c[2], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &djvu.c[1], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &djvu.c[0], 1);    
    if (rd < 4) return -1;
    FileSize = djvu.i;
    FileSize += 12;
    if ((FileSize+FileOffset+fres<0)||
        (WorkFileSize<(FileSize+FileOffset+fres))||
        (FileSize<=0)) return -1;        

    strcpy(finfo->ExtNormal,  ".djvu");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "DjVu Image File");

    FileOffset += fres;
    FOffset     = FileOffset+FileSize;
    UnpSize  = FileSize;
    
    return FOffset;    
}
