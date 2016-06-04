long FormatCheckTGA1(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long rd, FOffset;       
    unsigned char t1[] = {0x01}; //Uncompressed mapped RGB Targa
    long fs;
    unsigned char IDlength, CMType, CMbpp; 
    unsigned char ImageSZ, CMSZ;

    unsigned short CMorigin, CMlength;
    unsigned short ImageWidth, ImageHeight;

    long fres = func->Find(t1, BaseString, sizeof t1, readed);
    if (fres == -1) return -1;

    SomeOffset = FileOffset+fres;

    FOffset = FileOffset+fres-2;

    rd = func->ReadFromFile(FileName, FOffset++, &IDlength, 1);
    if (rd < 1) return -1;

    rd = func->ReadFromFile(FileName, FOffset++, &CMType, 1);
    if (rd < 1) return -1;
    if (CMType != 1) return -1;

    FOffset = FileOffset+fres+6;

    rd = func->ReadFromFile(FileName, FOffset, &CMbpp, 1);
    if (rd < 1) return -1;        
    
    switch (CMbpp){    
    case 16: CMSZ = 2; break;
    case 24: CMSZ = 3; break;
    case 32: CMSZ = 4; break;
    default: return -1;
    }      

    fs = IDlength;

    FOffset = FileOffset+fres + 1;

    rd = func->ReadFromFile(FileName, FOffset, &CMorigin, 2);
    if (rd < 2) return -1;
    FOffset += 2;

    rd = func->ReadFromFile(FileName, FOffset, &CMlength, 2);
    if (rd < 2) return -1;
    FOffset += 2;

    if (CMlength<=0) return -1;
    if (CMlength>256) ImageSZ = 2; else ImageSZ = 1;        

    fs += CMlength*CMSZ;

    FOffset += 5;
        
    rd = func->ReadFromFile(FileName, FOffset, &ImageWidth, 2);
    if (rd < 2) return -1;
    FOffset += 2;
    if (ImageWidth <= 0) return -1;

    rd = func->ReadFromFile(FileName, FOffset, &ImageHeight, 2);
    if (rd < 2) return -1;
    FOffset += 2;
    if (ImageHeight <= 0) return -1;

    fs += ImageHeight*ImageWidth*ImageSZ;
    fs += 18;

    if ((fs+FileOffset+fres-2<0)||
    (WorkFileSize<fs+FileOffset+fres-2)||
    (fs <= 0)) return -1;
    
    FileSize = fs;
    UnpSize  = FileSize;
    strcpy(finfo->ExtNormal, ".tga");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "Uncompressed mapped Targa image");    

    FileOffset += fres-2;
    FOffset     = FileOffset+FileSize;
    HeadOffset = 3;
    return FOffset;    
}
