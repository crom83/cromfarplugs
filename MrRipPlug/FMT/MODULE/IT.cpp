#define IMPI_SIZE 554

long FormatCheckIT(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    long rd, FOffset, SOffset, MaxEndOffset;
    long iOffset;
    long sdOffset;//sample data offset
    long sdSize;  //sample data size

    short nOrders;
    short nPatterns;
    short nInstruments;
    short nSamples;
    short msgOffset;
    short msgLen;
    short pdSize; //pattern data size
    
    long *sOffsets;//sample offsets
    
    long *pOffsets;//pattern offsets

    unsigned char t[] = "IMPM"; //IT start signature         

    long fres = func->Find(t, BaseString, (sizeof t)-1, readed);
    if (fres == -1) return -1;

    MaxEndOffset = 0;
    SOffset = (FileOffset+fres);
    FOffset = SOffset+32; 

    rd = func->ReadFromFile(FileName, FOffset, &nOrders, 2);
    if (rd < 2) return -1;    
    FOffset += 2;

    rd = func->ReadFromFile(FileName, FOffset, &nInstruments, 2);
    if (rd < 2) return -1;    
    FOffset += 2;

    rd = func->ReadFromFile(FileName, FOffset, &nSamples, 2);
    if (rd < 2) return -1;    
    FOffset += 2;

    rd = func->ReadFromFile(FileName, FOffset, &nPatterns, 2);
    if (rd < 2) return -1;    
    FOffset += 16;    

    rd = func->ReadFromFile(FileName, FOffset, &msgLen, 2);
    if (rd < 2) return -1;    
    FOffset += 2;

    rd = func->ReadFromFile(FileName, FOffset, &msgOffset, 2);
    if (rd < 2) return -1;    
    FOffset += 136+nOrders;

    if (msgOffset>0&&msgLen>0)
        MaxEndOffset = (msgOffset+msgLen>MaxEndOffset)?msgOffset+msgLen:MaxEndOffset;            

    if (nInstruments>0){        
        for (int i=0; i<nInstruments;i++){
            rd = func->ReadFromFile(FileName, FOffset, &iOffset, 4);
            if (rd < 4) return -1;
            FOffset += 4;
            if (iOffset>0)
                MaxEndOffset = (iOffset+IMPI_SIZE>MaxEndOffset)?(iOffset+IMPI_SIZE):MaxEndOffset;            
        }
    }

    if (nSamples>0){
        sOffsets = new long[nSamples];
        for (int i=0; i<nSamples;i++){
            rd = func->ReadFromFile(FileName, FOffset, &sOffsets[i], 4);
            if (rd < 4) {                
                delete [] sOffsets;
                return -1;    
            }
            FOffset += 4;
        }
    }

    if (nPatterns>0){
        pOffsets = new long[nPatterns];
        for (int i=0; i<nPatterns;i++){
            rd = func->ReadFromFile(FileName, FOffset, &pOffsets[i], 4);
            if (rd < 4) {                
                delete [] sOffsets;
                delete [] pOffsets;
                return -1;    
            }
            FOffset += 4;
        }
    }

    if (nSamples>0){                
        for (int i=0; i<nSamples;i++){
            rd = func->ReadFromFile(FileName, SOffset+sOffsets[i]+48, &sdSize, 4);
            if (rd < 4) {                
                delete [] sOffsets;
                delete [] pOffsets;                
                return -1;    
            }

            rd = func->ReadFromFile(FileName, SOffset+sOffsets[i]+72, &sdOffset, 4);
            if (rd < 4) {                
                delete [] sOffsets;
                delete [] pOffsets;                
                return -1;    
            }
            if (sdSize>0&&sdOffset>0)
                MaxEndOffset = (sdOffset+sdSize>MaxEndOffset)?(sdOffset+sdSize):MaxEndOffset;
        }
        delete [] sOffsets;
    }

    if (nPatterns>0){                
        for (int i=0; i<nPatterns;i++){
            rd = func->ReadFromFile(FileName, SOffset+pOffsets[i], &pdSize, 2);
            if (rd < 2) {                                
                delete [] pOffsets;                
                return -1;    
            }
            
            if (pdSize>0&&pOffsets[i]>0)
                MaxEndOffset = (pOffsets[i]+pdSize+8>MaxEndOffset)?(pOffsets[i]+pdSize+8):MaxEndOffset;
        }
        delete [] pOffsets;
    }    

    FileSize = MaxEndOffset;
    UnpSize = FileSize;
    MaxEndOffset += SOffset;
    

    if ((FileSize+SOffset<0)||
        (WorkFileSize<(FileSize+SOffset))||
        (FileSize<=0)) return -1;        

    strcpy(finfo->ExtNormal,   ".it");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "ImpulseTracker module");

    FileOffset = SOffset;
         
    return MaxEndOffset;
}