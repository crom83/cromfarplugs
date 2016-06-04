long FormatCheckAVI(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func)
{       
    long rd, FOffset;
    unsigned char t[4] = {0x52, 0x49, 0x46, 0x46}; //"RIFF" signature
    unsigned char w[8]; //AVI LIST

    long fres = func->Find(t, BaseString, 4, readed);
    if (fres == -1) return -1;

    FOffset = FileOffset+fres+4;

    rd = func->ReadFromFile(FileName, FOffset, &FileSize, 4);
    if (rd < 4) return -1;
    
    FOffset += 4;

    FileSize += 8;
    if ((FileSize+FileOffset+fres<0)||
        (WorkFileSize<(FileSize+FileOffset+fres))||
        (FileSize<=0)) return -1;
        
    rd = func->ReadFromFile(FileName, FOffset, &w, 8);
    if (rd < 8) return -1;
       
        
    if ((w[0] == 0x41)&&
        (w[1] == 0x56)&&
        (w[2] == 0x49)&&
        (w[3] == 0x20)&&
        (w[4] == 0x4C)&&
        (w[5] == 0x49)&&
        (w[6] == 0x53)&&
        (w[7] == 0x54)){

        FileOffset += fres;
        FOffset     = FileOffset+FileSize;

        strcpy(finfo->ExtNormal, ".avi");
        finfo->ExtConvrt[0] = 0;
        strcpy(finfo->Description, "Audio Video Interlaced");
        UnpSize = FileSize;

        return FOffset;
        }    
    return -1;
}