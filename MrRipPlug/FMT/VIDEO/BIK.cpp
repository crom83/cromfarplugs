long FormatCheckBIK(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func)
{       
    long rd, FOffset;       
    unsigned char ti[] = {0x42, 0x49, 0x4B, 0x69}; //"BIKi" signature
    //unsigned char tb[] = {0x42, 0x49, 0x4B, 0x62}; //"BIKb" signature
    long fs;

    long fres = func->Find(ti, BaseString, sizeof ti, readed);
    if (fres == -1) return -1;    

    FOffset = FileOffset+fres+sizeof(ti);

    rd = func->ReadFromFile(FileName, FOffset, &fs, 4);
    if (rd < 4) return -1;        

    FOffset += 4;

    if ((fs+FileOffset+fres<0)||
        (WorkFileSize<fs+FileOffset+fres)||
        (fs <= 0)) return -1;
    
    FileSize = fs+8;
    strcpy(finfo->ExtNormal, ".bik");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "RAD Game Tools Bink Video");    

    FileOffset += fres;
    FOffset     = FileOffset+FileSize;
    UnpSize = FileSize;
        
    return FOffset;    
}