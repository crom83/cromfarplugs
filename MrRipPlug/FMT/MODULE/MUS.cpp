long FormatCheckMUS(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long rd, FOffset, SOffset;
    long fs;
    short size1, size2;

    unsigned char t[] = {'M', 'U', 'S', 0x1A};

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+4;
   
    rd  = func->ReadFromFile(FileName, FOffset, &size1, sizeof size1);
    if (rd < sizeof size1) return -1;
    FOffset += rd;

    rd  = func->ReadFromFile(FileName, FOffset, &size2, sizeof size2);
    if (rd < sizeof size2) return -1;   

    fs = size1+size2;            
    
    if ((fs+SOffset<0)||
        (WorkFileSize<(fs+SOffset))||
        (fs<=0)) return -1;        

    FileSize = fs;
    UnpSize = FileSize;
    strcpy(finfo->ExtNormal,  ".mus");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "MUS MIDI file");

    FileOffset  = SOffset;
    FOffset     = FileOffset+FileSize;
    
    return FOffset;    
}