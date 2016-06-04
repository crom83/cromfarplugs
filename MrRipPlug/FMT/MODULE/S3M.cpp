#define ADLIB_SIZE 0x4F

long FormatCheckS3M(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    long rd, FOffset, SOffset, MaxEndOffset;
    long sdSize;  //sample data size
    long lsdOffset;

    short nOrders;
    short nPatterns;
    short nInstruments;    
    unsigned short sdOffset;
    short pdSize; //pattern data size
    short temp;

    long *iOffsets;//sample offsets    
    long *pOffsets;//pattern offsets

    char c;

    unsigned char t[] = "SCRM"; //S3M start signature         

    long fres = func->Find(t, BaseString, (sizeof t)-1, readed);
    if (fres == -1) return -1;
    
    MaxEndOffset = 0;

    SOffset = (FileOffset+fres)-44;
    FOffset = (FileOffset+fres)-12; 

    rd = func->ReadFromFile(FileName, FOffset, &nOrders, 2);
    if (rd < 2) return -1;    
    FOffset += 2;

    rd = func->ReadFromFile(FileName, FOffset, &nInstruments, 2);
    if (rd < 2) return -1;    
    FOffset += 2;

    rd = func->ReadFromFile(FileName, FOffset, &nPatterns, 2);
    if (rd < 2) return -1;    
    
    FOffset = SOffset+0x60+nOrders;    

    MaxEndOffset = FOffset-SOffset;

    iOffsets = new long[nInstruments];
    for (int i=0; i<nInstruments;i++){
        rd = func->ReadFromFile(FileName, FOffset, &temp, 2);
        if (rd < 2) {                
            delete [] iOffsets;
            return -1;    
        }
        iOffsets[i] = temp*16;
        FOffset += 2;        
    }
    
    pOffsets = new long[nPatterns];
    for (i=0; i<nPatterns;i++){
        rd = func->ReadFromFile(FileName, FOffset, &temp, 2);
        if (rd < 2) {
            delete [] iOffsets;
            delete [] pOffsets;
            return -1;    
        }
        FOffset += 2;
        pOffsets[i] = temp*16;
    }    

    MaxEndOffset = FOffset-SOffset;

    for (i=0; i<nInstruments;i++){
        rd = func->ReadFromFile(FileName, SOffset+iOffsets[i], &c, 1);
        if (rd < 1) {
            delete [] iOffsets;
            delete [] pOffsets;
            return -1;    
        }        
        if (c==1){
            FOffset = SOffset+iOffsets[i]+0x0E;
            rd = func->ReadFromFile(FileName, FOffset, &sdOffset, 2);
            if (rd < 2) {
                delete [] iOffsets;
                delete [] pOffsets;
                return -1;    
            }
            lsdOffset = sdOffset*16;            
            FOffset = SOffset+iOffsets[i]+0x10;
            rd = func->ReadFromFile(FileName, FOffset, &sdSize, 4);
            if (rd < 4) {
                delete [] iOffsets;
                delete [] pOffsets;
                return -1;    
            }            
            if (sdSize>0&&lsdOffset>0)
                MaxEndOffset = (lsdOffset+sdSize>MaxEndOffset)?(lsdOffset+sdSize):MaxEndOffset;
        } else {
            MaxEndOffset = (iOffsets[i]+ADLIB_SIZE>MaxEndOffset)?(iOffsets[i]+ADLIB_SIZE):MaxEndOffset;
        }

    }
    delete [] iOffsets;

    for (i=0; i<nPatterns;i++){
        rd = func->ReadFromFile(FileName, SOffset+pOffsets[i], &pdSize, 2);
        if (rd < 2) {                                
            delete [] pOffsets;                
            return -1;    
        }
        if (pdSize>0&&pOffsets[i]>0)
                MaxEndOffset = (pOffsets[i]+pdSize+2>MaxEndOffset)?(pOffsets[i]+pdSize+2):MaxEndOffset;
    }
    delete [] pOffsets;
    
    while (MaxEndOffset%16!=0) MaxEndOffset++;

    FileSize = MaxEndOffset;
    MaxEndOffset += SOffset;

    if ((FileSize+SOffset<0)||
        (WorkFileSize<(FileSize+SOffset))||
        (FileSize<=0)) return -1;        

    
    strcpy(finfo->ExtNormal,   ".s3m");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "ScreamTracker3 module");

    HeadOffset = 0x2C;
    FileOffset = SOffset;
    UnpSize = FileSize;
         
    return MaxEndOffset;
}