long FormatCheckBMP(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long rd, FOffset;       
    unsigned char t[2] = {0x42, 0x4D}; //"BM" signature
    long fs; 
    char bpp;

    long fres = func->Find(t, BaseString, 2, readed);
    if (fres == -1) return -1;

    FOffset = FileOffset+fres+2;

    rd = func->ReadFromFile(FileName, FOffset, &fs, 4);
    if (rd < 4) return -1;        

    FOffset += 0x1A;

    rd = func->ReadFromFile(FileName, FOffset, &bpp, sizeof bpp);
    if (rd < sizeof bpp) return -1;        
    
    switch (bpp){
    case 1:
    case 4:
    case 8:
    case 16:
    case 24:
    case 32:
    case 64:    

        if ((fs+FileOffset+fres<0)||
            (WorkFileSize<fs+FileOffset+fres)||
            (fs <= 0)) return -1;
    
        FileSize = fs;
        UnpSize  = fs;
        strcpy(finfo->ExtNormal, ".bmp");
        finfo->ExtConvrt[0] = 0;
        strcpy(finfo->Description, "Windows Bitmap file");    

        FileOffset += fres;
        FOffset     = FileOffset+FileSize;
        break;
    default: return -1;
    }
    return FOffset;    
}
