long FormatCheckWAV(const char * FileName, unsigned char* BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long rd, FOffset;
    unsigned char t[4] = {0x52, 0x49, 0x46, 0x46}; //"RIFF" signature
    unsigned char w[7]; //WAVEfmt

    long fres = func->Find(t, BaseString, 4, readed);
    if (fres == -1) return -1;

    FOffset = FileOffset+fres+4;

    rd = func->ReadFromFile(FileName, FOffset, &FileSize, 4);
    if (rd < 4)return -1;        

    FOffset += rd;
    FileSize += 8;

    if ((FileSize+FileOffset+fres<0)||
        (WorkFileSize<(FileSize+FileOffset+fres))||
        (FileSize<=0)) return -1;       
        
    rd = func->ReadFromFile(FileName, FOffset, &w, 7);
    if (rd < 7) return -1;
    
    if ((w[0] == 0x57)&&
        (w[1] == 0x41)&&
        (w[2] == 0x56)&&
        (w[3] == 0x45)&&
        (w[4] == 0x66)&&
        (w[5] == 0x6D)&&
        (w[6] == 0x74)){

        FileOffset += fres;
        FOffset      = FileOffset+FileSize;
        UnpSize = FileSize;

        strcpy(finfo->ExtNormal,   ".wav");
        finfo->ExtConvrt[0] = 0;
        strcpy(finfo->Description, "Windows audio file");    
        
        return FOffset;
    }
    return -1;  
}