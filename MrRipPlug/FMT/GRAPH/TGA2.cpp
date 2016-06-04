long FormatCheckTGA2(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long rd, FOffset;       
    unsigned char t2[] = {0x02}; //Uncompressed unmapped RGB Targa
    long fs;
    unsigned char bpp, IDlength, CMType, CMbpp; 
    unsigned char ImageSZ, CMSZ;

    unsigned short CMorigin, CMlength;
    unsigned short ImageWidth, ImageHeight;

    long fres = func->Find(t2, BaseString, sizeof t2, readed);
    if (fres == -1) return -1;

    SomeOffset = FileOffset+fres;

    FOffset = FileOffset+fres+14;

    rd = func->ReadFromFile(FileName, FOffset, &bpp, 1);
    if (rd < 1) return -1;        
    
    switch (bpp){    
    case 16: ImageSZ = 2; break;
    case 24: ImageSZ = 3; break;
    case 32: ImageSZ = 4; break;
    default: return -1;
    }

    FOffset = FileOffset+fres - 2;
    
    rd = func->ReadFromFile(FileName, FOffset++, &IDlength, 1);
    if (rd < 1) return -1;

    rd = func->ReadFromFile(FileName, FOffset++, &CMType, 1);
    if (rd < 1) return -1;

    switch (CMType){    
    case 0: case 1: break;    
    default: return -1;
    }

    fs = IDlength;

    if (CMType) {
        FOffset++;
        rd = func->ReadFromFile(FileName, FOffset, &CMorigin, 2);
        if (rd < 2) return -1;
        FOffset += 2;

        rd = func->ReadFromFile(FileName, FOffset, &CMlength, 2);
        if (rd < 2) return -1;
        FOffset += 2;
    
        rd = func->ReadFromFile(FileName, FOffset++, &CMbpp, 1);
        if (rd < 1) return -1;

        switch (CMbpp){
        case 16: CMSZ = 2; break;
        case 24: CMSZ = 3; break;
        case 32: CMSZ = 4; break;
        default: return -1;
        }
        fs += CMlength*CMSZ;
        FOffset += 4;
    } else {FOffset += 10;}    

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
    strcpy(finfo->Description, "Uncompressed unmapped Targa image");    

    FileOffset += fres-2;
    FOffset     = FileOffset+FileSize;
    HeadOffset = 3;
    return FOffset;        
}
