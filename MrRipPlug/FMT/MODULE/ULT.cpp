long FormatCheckULT(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    long rd;        
    ULONG UltSize = 0, FOffset, SOffset;

    BYTE nPatterns;
    BYTE nChannels;
    BYTE nSamples;
    
    char vers, jmpv;
    char title[32];

#pragma pack(push, 1)
    struct Samples {
        char SampleName[32];
        char DosName[12];
        ULONG LoopStart;
        ULONG LoopEnd;
        ULONG SizeStart;
        ULONG SizeEnd;
        BYTE Volume;
        BYTE BidiLoop;
        WORD FineTune;
    } smpl;

    struct RepPatterns {
        BYTE Rep;
        BYTE Count;        
    } reppat;
    
#pragma pack(pop)

    unsigned char t[] = "MAS_UTrack_V00"; //ULT start signature         

    long fres = func->Find(t, BaseString, (sizeof t)-1, readed);
    if (fres == -1) return -1;
    
    SOffset = FileOffset+fres;
    FOffset = SOffset+lstrlen((char*)t); // to the version

    rd = func->ReadFromFile(FileName, FOffset++, &vers, 1);
    if (rd < 1) return -1;        

    rd = func->ReadFromFile(FileName, FOffset, &title, 32);
    if (rd < 32) return -1;    
    FOffset += 32;    

    rd = func->ReadFromFile(FileName, FOffset, &jmpv, 1);
    if (rd < 1) return -1;        

    switch (vers){
    case '1': FOffset++;
        break;
    case '2': 
    case '3': 
    case '4': 
        FOffset += jmpv*32+1;
        break;
    }    

    rd = func->ReadFromFile(FileName, FOffset, &nSamples, sizeof nSamples);
    if (rd < sizeof nSamples) return -1;    
    FOffset += sizeof nSamples;

    UltSize = 0;    
    for (int i=0;i<nSamples;i++){
        rd = func->ReadFromFile(FileName, FOffset, &smpl, sizeof smpl);
        if (rd < sizeof smpl) return -1;    
        FOffset += sizeof smpl;
        if (vers=='4') FOffset += 2;       
        
        UltSize+=smpl.SizeEnd-smpl.SizeStart;        
    }   

    FOffset += 256;

    rd = func->ReadFromFile(FileName, FOffset, &nChannels, sizeof nChannels);
    if (rd < sizeof nChannels) return -1;    
    FOffset += sizeof nChannels;

    rd = func->ReadFromFile(FileName, FOffset, &nPatterns, sizeof nPatterns);
    if (rd < sizeof nPatterns) return -1;    
    FOffset += sizeof nPatterns;

    ++nChannels;
    ++nPatterns;

    if (vers=='3'||vers=='4') FOffset += nChannels;    

    long Counter = 0, Max = nPatterns*64*nChannels;
    while (TRUE){    
        rd = func->ReadFromFile(FileName, FOffset, &reppat, sizeof reppat);
        if (rd < sizeof reppat) return -1;    

        if (reppat.Rep == 0xFC) {
            Counter += reppat.Count;
            FOffset += 7;
        } else {
            Counter += 1;
            FOffset += 5;
        }
        
        if (Counter>=Max) break;        
    }   

    UltSize += FOffset-SOffset;

    FileSize = UltSize;

    if ((FileSize+SOffset<0)||
        (WorkFileSize<(FileSize+SOffset))||
        (FileSize<=0)) return -1;        

    lstrcpy(finfo->ExtNormal,   ".ult");
    finfo->ExtConvrt[0] = 0;
    lstrcpy(finfo->Description, "ULT: ");
    if (lstrlen(title)!=0)
         lstrcat(finfo->Description, title);
    else lstrcat(finfo->Description, "Unknown");

    FileOffset = SOffset;
    UnpSize = FileSize;
         
    return UltSize;
}