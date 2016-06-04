long FormatCheckMIDI(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long rd, FOffset;
    long fs, tsize;

    short tracks;

    unsigned char t[] = {'M', 'T', 'h', 'd', 0x00, 0x00, 0x00, 0x06}; //MIDI start signature         

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    FOffset = FileOffset+fres+(sizeof t)+2;
    fs = 14;

    rd  = func->ReadFromFile(FileName, FOffset, &tracks, sizeof tracks);    
    if (rd < sizeof tracks) return -1;
    FOffset += rd+2;
    tracks = func->SwapWord(tracks);
    
    for (int i=0; i<tracks;i++){
        FOffset += 4;        
        fs += 4;
        rd  = func->ReadFromFile(FileName, FOffset, &tsize, sizeof tsize);        
        if (rd < sizeof tsize) return -1;
        FOffset += rd;
        tsize = func->SwapLong(tsize);
        fs += rd + tsize;
        FOffset += tsize;
    }        
    
    if ((fs+FileOffset+fres<0)||
        (WorkFileSize<(fs+FileOffset+fres))||
        (fs<=0)) return -1;        

    FileSize = fs;
    strcpy(finfo->ExtNormal,  ".mid");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "MIDI file");

    FileOffset += fres;
    FOffset     = FileOffset+FileSize;
    UnpSize = FileSize;
    
    return FOffset;    
}