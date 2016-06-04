long FormatCheckXM(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    long rd, FOffset;
    long hSize;
    long pSize;
    long iSize;
    long sSize;

    short nPatterns;
    short nInstruments;
    short nSamples;
    short pdSize;//pattern data size
    long *sdSize;//sample data size

    unsigned char t[] = "Extended Module: "; //XM start signature         

    long fres = func->Find(t, BaseString, (sizeof t)-1, readed);
    if (fres == -1) return -1;

    FOffset = (FileOffset+fres)+60; // to the header size

    rd = func->ReadFromFile(FileName, FOffset, &hSize, 4);
    if (rd < 4) return -1;    
    FOffset += 10;

    rd = func->ReadFromFile(FileName, FOffset, &nPatterns, 2);
    if (rd < 2) return -1;    
    FOffset += 2;

    rd = func->ReadFromFile(FileName, FOffset, &nInstruments, 2);
    if (rd < 2) return -1;    
    FOffset += 264;

    for (int i=0; i!=nPatterns; i++){
        rd = func->ReadFromFile(FileName, FOffset, &pSize, 4);
        if (rd < 4) return -1;    
        FOffset += 7;

        rd = func->ReadFromFile(FileName, FOffset, &pdSize, 2);
        if (rd < 2) return -1;    
        FOffset = (FOffset - 7) + pSize + pdSize;
    }

    for (i=0; i!=nInstruments; i++){
        rd = func->ReadFromFile(FileName, FOffset, &iSize, 4);
        if (rd < 4) return -1;    
        FOffset += 27;

        rd = func->ReadFromFile(FileName, FOffset, &nSamples, 2);
        if (rd < 2) return -1;
        FOffset += 2;

        if (nSamples>0) {
            rd = func->ReadFromFile(FileName, FOffset, &sSize, 4);
            if (rd < 4) return -1;
            FOffset = (FOffset - 29) + iSize;

            sdSize = new long[nSamples];

            for (int j=0; j<nSamples; j++){
                rd = func->ReadFromFile(FileName, FOffset, &sdSize[j], 4);
                if (rd < 4) {
                    delete [] sdSize;
                    return -1;
                }
                FOffset += sSize;
            }

            for (j=0; j<nSamples; j++){                
                FOffset += sdSize[j];
            }

            delete [] sdSize;
        } else {FOffset = (FOffset - 29) + iSize;}
    }    

    FileSize = FOffset-(FileOffset+fres);

    if ((FileSize+FileOffset+fres<0)||
        (WorkFileSize<(FileSize+FileOffset+fres))||
        (FileSize<=0)) return -1;        

    strcpy(finfo->ExtNormal,   ".xm");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "FastTracker 2.0 module");

    FileOffset = FileOffset + fres;
    UnpSize = FileSize;
         
    return FOffset;
}