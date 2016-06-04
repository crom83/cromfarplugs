long FormatCheckRIFF(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long rd, FOffset;
    unsigned char t[4] = {0x52, 0x49, 0x46, 0x46}; //"RIFF" signature    

    long fres = func->Find(t, BaseString, 4, readed);
    if (fres == -1) return -1;

    FOffset = FileOffset+fres+4;

    rd = func->ReadFromFile(FileName, FOffset, &FileSize, 4);
    if (rd < 4) return -1;
    
    FileSize += 8;

    if ((FileSize+FileOffset+fres<0)||
        (WorkFileSize<(FileSize+FileOffset+fres))||
        (FileSize<=0)) return -1;        
    
    FileOffset += fres;
    FOffset     = FileOffset+FileSize;

    strcpy(finfo->ExtNormal, ".riff");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "Unknown RIFF file");
    UnpSize = FileSize;

    return FOffset;
}