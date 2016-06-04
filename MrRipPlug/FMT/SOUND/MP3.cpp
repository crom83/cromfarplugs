int getFrameSync(unsigned long bithdr)     { return ((bithdr>>21) & 2047); };
int getVersionIndex(unsigned long bithdr)  { return ((bithdr>>19) & 3);  };
int getLayerIndex(unsigned long bithdr)    { return ((bithdr>>17) & 3);  };
//int getProtectionBit(unsigned long bithdr) { return ((bithdr>>16) & 1);  };
int getBitrateIndex(unsigned long bithdr)  { return ((bithdr>>12) & 15); };
int getFrequencyIndex(unsigned long bithdr){ return ((bithdr>>10) & 3);  };
int getPaddingBit(unsigned long bithdr)    { return ((bithdr>> 9) & 1);  };
//int getPrivateBit(unsigned long bithdr)    { return ((bithdr>> 8) & 1);  };
//int getModeIndex(unsigned long bithdr)     { return ((bithdr>> 6) & 3);  };
//int getModeExtIndex(unsigned long bithdr)  { return ((bithdr>> 4) & 3);  };
//int getCoprightBit(unsigned long bithdr)   { return ((bithdr>> 3) & 1);  };
//int getOrginalBit(unsigned long bithdr)    { return ((bithdr>> 2) & 1);  };
int getEmphasisIndex(unsigned long bithdr) { return ((bithdr    ) & 3);  };

unsigned long loadHeader(char c[4]) {

    return (unsigned long)(
                              ( (c[0] & 255) << 24) |
                              ( (c[1] & 255) << 16) |
                              ( (c[2] & 255) <<  8) |
                              ( (c[3] & 255)      )
                            ); 

}

BOOL isValidHeader(unsigned long bithdr) 
{

    return ( ((getFrameSync(bithdr)      & 2047)==2047) &&
             ((getVersionIndex(bithdr)   &    3)!=   1) &&
             ((getLayerIndex(bithdr)     &    3)!=   0) && 
             ((getBitrateIndex(bithdr)   &   15)!=   0) &&  
             ((getBitrateIndex(bithdr)   &   15)!=  15) &&
             ((getFrequencyIndex(bithdr) &    3)!=   3) &&
             ((getEmphasisIndex(bithdr)  &    3)!=   2)    );

}

int getVersion(unsigned long bithdr) 
{
    
    int table[4] = {3, 0, 2, 1};

    return table[getVersionIndex(bithdr)];
}

int getLayer(unsigned long bithdr) 
{
     return ( 4 - getLayerIndex(bithdr));
}

int getBitrate(unsigned long bithdr) 
{
    const int table[2][3][16] = {
        {         //MPEG 2 & 2.5
            {0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0}, //Layer III
            {0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0}, //Layer II
            {0, 32, 48, 56, 64, 80, 96,112,128,144,160,176,192,224,256,0}  //Layer I
        },{       //MPEG 1
            {0, 32, 40, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,0}, //Layer III
            {0, 32, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,384,0}, //Layer II
            {0, 32, 64, 96,128,160,192,224,256,288,320,352,384,416,448,0}  //Layer I
        }
    };
    return table[(getVersionIndex(bithdr) & 1)][(getLayerIndex(bithdr) -1)][getBitrateIndex(bithdr)];
}


int getFrequency(unsigned long bithdr) 
{
    int table[4][3] = {
        {32000, 16000,  8000}, //MPEG 2.5
        {    0,     0,     0}, //reserved
        {22050, 24000, 16000}, //MPEG 2
        {44100, 48000, 32000}  //MPEG 1
    };
    return table[getVersionIndex(bithdr)][getFrequencyIndex(bithdr)];
}

long FormatCheckMP3(const char * FileName, unsigned char* BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long rd, FOffset, framesize, framecount = 0, fres = 0;  
    unsigned char hdr[4];
    unsigned long mpeg_header;
    int bps, sample, padding;
    BOOL fnd = FALSE;
    char sss[1000];
    unsigned char tag[3] = {0x54, 0x41, 0x47}; //TAG start signature

    if (readed-4<4) return -1;
    for (long i=0; i<=readed-4; i++)
    {
        memcpy(hdr, BaseString+i, 4);             
        mpeg_header = loadHeader((char*)hdr);
        if (isValidHeader(mpeg_header)) 
        {
            fnd     = TRUE;
            fres    = i;
            break;
        }
        }       
    if (fnd == FALSE) return -1;

    FOffset = FileOffset+fres; //seeking to begin of mp3 frame      

    switch(getVersion(mpeg_header))
    {
    case 1: {
        switch (getLayer(mpeg_header))
        {
        case 1: strcpy(sss, "MPEG v1 Layer I");
            break;
        case 2: strcpy(sss, "MPEG v1 Layer II");
            break;
        case 3: strcpy(sss, "MPEG v1 Layer III");
            break;
        }
        break;
            }
    case 2: {
        switch (getLayer(mpeg_header))
        {
        case 1: strcpy(sss, "MPEG v2 Layer I");
            break;
        case 2: strcpy(sss, "MPEG v2 Layer II");
            break;
        case 3: strcpy(sss, "MPEG v2 Layer III");
            break;
        }
        break;
            }
    case 3: {
        switch (getLayer(mpeg_header))
        {
        case 1: strcpy(sss, "MPEG v2.5 Layer I");
            break;
        case 2: strcpy(sss, "MPEG v2.5 Layer II");
            break;
        case 3: strcpy(sss, "MPEG v2.5 Layer III");
            break;
        }
        break;
            }
    }    

    FileSize = 0;
    while (FOffset<WorkFileSize)
    {       
                //calculating frame size
        bps = getBitrate(mpeg_header);
        sample = getFrequency(mpeg_header);
        padding = getPaddingBit(mpeg_header);

        if ((bps == 0)||(sample == 0)) break;

        if (getVersion(mpeg_header)==1)
        {
                        if (getLayer(mpeg_header)==1)
                                framesize = (48000*bps)/sample+padding; //only for Mpeg v1 Layer I
            else
                                framesize = (144000*bps)/sample+padding; //only for Mpeg v1 Layer II&III                        
        } else {
                        if (getLayer(mpeg_header)==1)
                                framesize = (24000*bps)/sample+padding; //only for Mpeg v2 Layer I
            else
                                framesize = (72000*bps)/sample+padding; //only for Mpeg v2 Layer II&III                 
        }
                
        FileSize += framesize;

        if (WorkFileSize<(FileSize+FileOffset+fres)) 
        {
            FileSize -= framesize;
            break;
        }

        framecount++;
        FOffset = FileOffset+fres+FileSize;
        
        rd = func->ReadFromFile(FileName, FOffset, &tag, 3);
        if (rd < 3) break;        
        if ((tag[0] == 0x54)&&
            (tag[1] == 0x41)&&
            (tag[2] == 0x47))
        {
            FileSize += 128;            
            if (WorkFileSize<(FileSize+FileOffset+fres)) FileSize -= 128;
            break; //end of MP3 file
        }
        
        rd = func->ReadFromFile(FileName, FOffset, &hdr, 4);  
        if (rd < 4) break;
        mpeg_header = loadHeader((char*)hdr);
        if (!isValidHeader(mpeg_header)) break; //end of MP3 file        
        }        
    if (FileSize <= 0) return -1;    

    if (framecount<5) return -1; // it's not MP3 in 99,9%
    
    strcpy(finfo->ExtNormal,   ".mp3");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description,  sss);
    FileOffset += fres;
    FOffset = FileOffset+FileSize;

    UnpSize = FileSize;
        
    return FOffset;
}