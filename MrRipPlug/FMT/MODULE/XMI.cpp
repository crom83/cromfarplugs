long FormatCheckXMI(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    union {
        unsigned char c[4];
        unsigned long  i;
    } midi;

    long rd, FOffset, SOffset;
    long fs;

    unsigned char t[] = {0x46, 0x4F, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x0E}; //XMI start signature         

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+0x1A;    
   
    rd  = func->ReadFromFile(FileName, FOffset++, &midi.c[3], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &midi.c[2], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &midi.c[1], 1);
    rd += func->ReadFromFile(FileName, FOffset++, &midi.c[0], 1);    
    if (rd < 4) return -1;
    fs = midi.i+30;            
    
    if ((fs+SOffset<0)||
        (WorkFileSize<(fs+SOffset))||
        (fs<=0)) return -1;        

    FileSize = fs;
    UnpSize = FileSize;
    strcpy(finfo->ExtNormal,  ".xmi");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "XMI MIDI file");

    FileOffset  = SOffset;
    FOffset     = FileOffset+FileSize;
    
    return FOffset;    
}