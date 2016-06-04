long FormatCheckSMK(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func)
{       
    long rd, FOffset, SOffset;
    unsigned char t2[] = {'S', 'M', 'K', '2'}; //"BIKi" signature    
    long frames, fs, temp, flags;

    long fres = func->Find(t2, BaseString, sizeof t2, readed);
    if (fres == -1) return -1;    

    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t2+8;

    rd = func->ReadFromFile(FileName, FOffset, &frames, 4);
    if (rd < 4) return -1;        
    FOffset +=8;

    rd = func->ReadFromFile(FileName, FOffset, &flags, 4);
    if (rd < 4) return -1;        

    if (flags&0x1) frames++;

    FOffset = SOffset+0x34;

    fs = 0;

    rd = func->ReadFromFile(FileName, FOffset, &temp, 4);
    if (rd < 4) return -1;        
    
    fs += temp+0x68+frames*4;
        
    while (fs++%512!=0);

    fs--;
    FOffset = SOffset+0x68;

    for (int i=0; i<frames;i++){
        rd = func->ReadFromFile(FileName, FOffset, &temp, 4);
        if (rd < 4) return -1;        
        fs += temp;
        FOffset += 4;
    }

    if ((fs+SOffset<0)||
        (WorkFileSize<fs+SOffset)||
        (fs <= 0)) return -1;
    
    FileSize = fs;
    strcpy(finfo->ExtNormal, ".smk");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "RAD Game Tools Smacker Video");    

    FileOffset += fres;
    FOffset     = FileOffset+FileSize;
    UnpSize = FileSize;
        
    return FOffset;    
}