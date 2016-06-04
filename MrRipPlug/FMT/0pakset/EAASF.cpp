struct SCDLChunk{
  ULONG Start;
  ULONG Len;
};

vector<SCDLChunk> SCDLChunks;

ULONG CurChunk = 0;

LONG EATable[]=
{
  0x00000000,
  0x000000F0,
  0x000001CC,
  0x00000188,
  0x00000000,
  0x00000000,
  0xFFFFFF30,
  0xFFFFFF24,
  0x00000000,
  0x00000001,
  0x00000003,
  0x00000004,
  0x00000007,
  0x00000008,
  0x0000000A,
  0x0000000B,
  0x00000000,
  0xFFFFFFFF,
  0xFFFFFFFD,
  0xFFFFFFFC
};

struct ASFBlockHeader
{
  char  szBlockID[4];
  DWORD dwSize;
};

struct ASFSCClBlockHeader
{
  char  szBlockID[4];
  DWORD dwSize;
  DWORD SCClNumber;
};

struct ASFSplitPCMChunkHeader
{
  DWORD dwOutSize;
  DWORD dwLeftChannelOffset;
  DWORD dwRightChannelOffset;
};

struct ASFChunkHeader
{
  DWORD dwOutSize;
  SHORT lCurSampleLeft;
  SHORT lPrevSampleLeft;
  SHORT lCurSampleRight;
  SHORT lPrevSampleRight;
};

struct ASFSplitChunkHeader
{
  DWORD dwOutSize;
  DWORD dwLeftChannelOffset;
  DWORD dwRightChannelOffset;
};

short lCurSampleLeft;
short lPrevSampleLeft;
BYTE* bLeftChannelData; // compressed data for left channel goes here...
short lCurSampleRight;
short lPrevSampleRight;
BYTE* bRightChannelData; // compressed data for right channel goes here...

struct TMplHeader
{
  char  szID[4];
  BYTE  bUnknown1;
  BYTE  bBits;
  BYTE  bChannels;
  BYTE  bCompression;
  WORD  wUnknown2;
  WORD  wSampleRate;
  DWORD dwNumSamples; // ???
  BYTE  bUnknown3[20];
};

DWORD ReadBytes(const char * FileName, long &SOffset, BYTE count)
{
  BYTE  i, byte;
  DWORD result, rd;

  result=0L;
  for (i=0;i<count;i++)
  {
    rd = func->ReadFromFile(FileName, SOffset, &byte, 1);
    if (rd < 1) return 0L;
    SOffset++;
    result<<=8;
    result+=byte;
  }

  return result;
}

DWORD dwSampleRate;
DWORD dwChannels;
DWORD dwCompression;
DWORD dwNumSamples;
DWORD dwDataStart;
DWORD dwLoopOffset;
DWORD dwLoopLength;
DWORD dwBytesPerSample;
BYTE  bSplit;
BYTE  bSplitCompression;

void ParsePTHeader(const char * FileName, long &SOffset)
{
  DWORD rd;
  BYTE byte;
  BOOL bInHeader, bInSubHeader;

  bInHeader=TRUE;
  while (bInHeader)
  {

    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
    switch (byte) // parse header code
    {
      case 0xFF: // end of header
           bInHeader=FALSE;
      case 0xFE: // skip
      case 0xFC: // skip
           break;
      case 0xFD: // subheader starts...
           bInSubHeader=TRUE;
           while (bInSubHeader)
           {
             rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
             switch (byte) // parse subheader code
             {
               case 0x82:
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    dwChannels=ReadBytes(FileName, SOffset, byte);
                    break;
               case 0x83:
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    dwCompression=ReadBytes(FileName, SOffset, byte);
                    break;
               case 0x84:
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    dwSampleRate=ReadBytes(FileName, SOffset, byte);
                    break;
               case 0x85:
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    dwNumSamples=ReadBytes(FileName, SOffset, byte);
                    break;
               case 0x86:
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    dwLoopOffset=ReadBytes(FileName, SOffset, byte);
                    break;
               case 0x87:
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    dwLoopLength=ReadBytes(FileName, SOffset, byte);
                    break;
               case 0x88:
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    dwDataStart=ReadBytes(FileName, SOffset, byte);
                    break;
               case 0x92:
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    dwBytesPerSample=ReadBytes(FileName, SOffset, byte);
                    break;
               case 0x80: // ???
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    bSplit=(BYTE)ReadBytes(FileName, SOffset, byte);
                    break;
               case 0xA0: // ???
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    bSplitCompression=(BYTE)ReadBytes(FileName, SOffset, byte);
                    break;
               case 0xFF:
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    bInSubHeader=FALSE;
                    bInHeader=FALSE;
                    break;
               case 0x8A: // end of subheader
                    bInSubHeader=FALSE;
               default: // ???
                    rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
                    SOffset += byte;
             }
           }
           break;
      default:
           rd = func->ReadFromFile(FileName, SOffset++, &byte, 1);
           if (byte==0xFF) SOffset += 4;
           SOffset += byte;
    }
  }
}

#define HINIBBLE(byte) ((byte) >> 4)
#define LONIBBLE(byte) ((byte) & 0x0F)

LONG Clip16BitSample(LONG sample)
{
  if (sample>32767)
     return 32767;
  else if (sample<-32768)
     return (-32768);
  else
     return sample;
}

void Output(const char * FileName, short Left, short Right)
{
  long wr = func->AppendToFile(FileName, &Left, sizeof Left);
  wr = func->AppendToFile(FileName, &Right, sizeof Right);
}

DWORD dwOutSize; // outsize value from the ASFChunkHeader

/*void DecodeASF()
{
  BYTE  InputBuffer[InputBufferSize]; // buffer containing audio data of "SCDl" block
  BYTE  bInput;
  DWORD i, bCount, sCount;
  LONG  c1left,c2left,c1right,c2right,left,right;
  BYTE  dleft,dright;

  DWORD dwSubOutSize=0x1c;

  i=0;

  dwOutSize = 10;

  // process integral number of (dwSubOutSize) samples
  for (bCount=0;bCount<(dwOutSize/dwSubOutSize);bCount++)
  {
    bInput=InputBuffer[i++];
    c1left=EATable[HINIBBLE(bInput)];   // predictor coeffs for left channel
    c2left=EATable[HINIBBLE(bInput)+4];
    c1right=EATable[LONIBBLE(bInput)];  // predictor coeffs for right channel
    c2right=EATable[LONIBBLE(bInput)+4];
    bInput=InputBuffer[i++];
    dleft=HINIBBLE(bInput)+8;   // shift value for left channel
    dright=LONIBBLE(bInput)+8;  // shift value for right channel
    for (sCount=0;sCount<dwSubOutSize;sCount++)
    {
      bInput=InputBuffer[i++];
      left=HINIBBLE(bInput);  // HIGHER nibble for left channel
      right=LONIBBLE(bInput); // LOWER nibble for right channel
      left=(left<<0x1c)>>dleft;
      right=(right<<0x1c)>>dright;
      left=(left+lCurSampleLeft*c1left+lPrevSampleLeft*c2left+0x80)>>8;
      right=(right+lCurSampleRight*c1right+lPrevSampleRight*c2right+0x80)>>8;
      left=Clip16BitSample(left);
      right=Clip16BitSample(right);
      lPrevSampleLeft=lCurSampleLeft;
      lCurSampleLeft=(short)left;
      lPrevSampleRight=lCurSampleRight;
      lCurSampleRight=(short)right;

      // Now we've got lCurSampleLeft and lCurSampleRight which form one stereo
      // sample and all is set for the next input byte...
      Output((SHORT)lCurSampleLeft,(SHORT)lCurSampleRight); // send the sample to output
    }
  }

  // process the rest (if any)
  if ((dwOutSize % dwSubOutSize) != 0)
  {
    bInput=InputBuffer[i++];
    c1left=EATable[HINIBBLE(bInput)];   // predictor coeffs for left channel
    c2left=EATable[HINIBBLE(bInput)+4];
    c1right=EATable[LONIBBLE(bInput)];  // predictor coeffs for right channel
    c2right=EATable[LONIBBLE(bInput)+4];
    bInput=InputBuffer[i++];
    dleft=HINIBBLE(bInput)+8;   // shift value for left channel
    dright=LONIBBLE(bInput)+8;  // shift value for right channel
    for (sCount=0;sCount<(dwOutSize % dwSubOutSize);sCount++)
    {
      bInput=InputBuffer[i++];
      left=HINIBBLE(bInput);  // HIGHER nibble for left channel
      right=LONIBBLE(bInput); // LOWER nibble for right channel
      left=(left<<0x1c)>>dleft;
      right=(right<<0x1c)>>dright;
      left=(left+lCurSampleLeft*c1left+lPrevSampleLeft*c2left+0x80)>>8;
      right=(right+lCurSampleRight*c1right+lPrevSampleRight*c2right+0x80)>>8;
      left=Clip16BitSample(left);
      right=Clip16BitSample(right);
      lPrevSampleLeft=lCurSampleLeft;
      lCurSampleLeft=(short)left;
      lPrevSampleRight=lCurSampleRight;
      lCurSampleRight=(short)right;

      // Now we've got lCurSampleLeft and lCurSampleRight which form one stereo
      // sample and all is set for the next input byte...
      Output((SHORT)lCurSampleLeft,(SHORT)lCurSampleRight); // send the sample to output
    }
  }
}*/

ULONG LeftSize, RightSize;

void DecodeASFSplit(const char * FileName)
{
  BYTE  bInput;
  DWORD i, j, bCount, sCount;
  LONG  c1left,c2left,c1right,c2right,left,right, templ, tempr;
  BYTE  dleft,dright;

  DWORD dwSubOutSize=0x0E;

  i=j=0;

  // process integral number of (dwSubOutSize) samples
  for (bCount=0;bCount<(dwOutSize/dwSubOutSize);bCount++)
  {
    bInput = bLeftChannelData[i++];
    c1left = EATable[HINIBBLE(bInput)];   // predictor coeffs
    c2left = EATable[HINIBBLE(bInput)+4];
    dleft  = LONIBBLE(bInput)+8;  // shift value

    bInput = bRightChannelData[j++];
    c1right= EATable[HINIBBLE(bInput)];   // predictor coeffs
    c2right= EATable[HINIBBLE(bInput)+4];
    dright = LONIBBLE(bInput)+8;  // shift value

    for (sCount=0;sCount<dwSubOutSize;sCount++)
    {
      bInput=bLeftChannelData[i++];
      left=HINIBBLE(bInput);
      left=(left<<0x1c)>>dleft;
      left=(left+lCurSampleLeft*c1left+lPrevSampleLeft*c2left+0x80)>>8;
      left=Clip16BitSample(left);
      lPrevSampleLeft=lCurSampleLeft;
      lCurSampleLeft=(short)left;
      templ = left;

      left=LONIBBLE(bInput);
      left=(left<<0x1c)>>dleft;
      left=(left+lCurSampleLeft*c1left+lPrevSampleLeft*c2left+0x80)>>8;
      left=Clip16BitSample(left);
      lPrevSampleLeft=lCurSampleLeft;
      lCurSampleLeft=(short)left;

      bInput=bRightChannelData[j++];
      right=HINIBBLE(bInput);
      right=(right<<0x1c)>>dright;
      right=(right+lCurSampleRight*c1right+lPrevSampleRight*c2right+0x80)>>8;
      right=Clip16BitSample(right);
      lPrevSampleRight=lCurSampleRight;
      lCurSampleRight=(short)right;
      tempr = right;

      right=LONIBBLE(bInput);
      right=(right<<0x1c)>>dright;
      right=(right+lCurSampleRight*c1right+lPrevSampleRight*c2right+0x80)>>8;
      right=Clip16BitSample(right);
      lPrevSampleRight=lCurSampleRight;
      lCurSampleRight=(short)right;


      Output(FileName, (SHORT)templ,(SHORT)tempr); // send the sample to output
      Output(FileName, (SHORT)lCurSampleLeft,(SHORT)lCurSampleRight); // send the sample to output
    }
  }

  // process the rest (if any)
  if ((dwOutSize % dwSubOutSize) != 0)
  {
    bInput = bLeftChannelData[i];
    c1left = EATable[HINIBBLE(bInput)];   // predictor coeffs
    c2left = EATable[HINIBBLE(bInput)+4];
    dleft  = LONIBBLE(bInput)+8;  // shift value

    bInput = bRightChannelData[i++];
    c1right= EATable[HINIBBLE(bInput)];   // predictor coeffs
    c2right= EATable[HINIBBLE(bInput)+4];
    dright = LONIBBLE(bInput)+8;  // shift value

    for (sCount=0;sCount<(dwOutSize % dwSubOutSize);sCount++)
    {
      bInput=bLeftChannelData[i++];
      left=HINIBBLE(bInput);
      left=(left<<0x1c)>>dleft;
      left=(left+lCurSampleLeft*c1left+lPrevSampleLeft*c2left+0x80)>>8;
      left=Clip16BitSample(left);
      lPrevSampleLeft=lCurSampleLeft;
      lCurSampleLeft=(short)left;
      templ = left;

      left=LONIBBLE(bInput);
      left=(left<<0x1c)>>dleft;
      left=(left+lCurSampleLeft*c1left+lPrevSampleLeft*c2left+0x80)>>8;
      left=Clip16BitSample(left);
      lPrevSampleLeft=lCurSampleLeft;
      lCurSampleLeft=(short)left;

      bInput=bRightChannelData[j++];
      right=HINIBBLE(bInput);
      right=(right<<0x1c)>>dright;
      right=(right+lCurSampleRight*c1right+lPrevSampleRight*c2right+0x80)>>8;
      right=Clip16BitSample(right);
      lPrevSampleRight=lCurSampleRight;
      lCurSampleRight=(short)right;
      tempr = right;

      right=LONIBBLE(bInput);
      right=(right<<0x1c)>>dright;
      right=(right+lCurSampleRight*c1right+lPrevSampleRight*c2right+0x80)>>8;
      right=Clip16BitSample(right);
      lPrevSampleRight=lCurSampleRight;
      lCurSampleRight=(short)right;

      Output(FileName, (SHORT)templ,(SHORT)tempr); // send the sample to output
      Output(FileName, (SHORT)lCurSampleLeft,(SHORT)lCurSampleRight); // send the sample to output
    }
  }
}

long DecodeASF(const char * FileName, const char * DestName, long &SOffset){
  ASFBlockHeader ASFbh;
  ASFSplitChunkHeader ASFsch;
  ASFSCClBlockHeader ASFSCClbh;
  SCDLChunk chunk;
  ULONG pos = 0;

  SOffset += 8;

  dwCompression = 7;
  dwSampleRate  = 22050;
  dwChannels    = 2;
  ParsePTHeader(FileName, SOffset);

  if (bSplit) {
    rd = func->ReadFromFile(FileName, FOffset, &ASFSCClbh, sizeof ASFSCClbh);
    if (rd < sizeof ASFSCClbh)return -1;
    SOffset += rd;

    while (TRUE){
      rd = func->ReadFromFile(FileName, FOffset, &ASFbh, sizeof ASFbh);
      if (rd < sizeof ASFbh)return -1;
      SOffset += rd;

      if (!memcmp(ASFbh.szBlockID, "SCDl", 4)){
        chunk.Len   = ASFbh.dwSize;
        chunk.Start = SOffset - 8;
        SCDLChunks.push_back(chunk);
      } else {
        if (!memcmp(ASFbh.szBlockID, "SCEl", 4))
          break;
      }
    }
  }

  rd = func->ReadFromFile(FileName, FOffset, &ASFsch, sizeof ASFsch);
  if (rd < sizeof ASFsch)return -1;
  SOffset += rd;

  dwOutSize = ASFsch.dwOutSize/2;
  pos = SOffset;
  SOffset = pos+ASFsch.dwLeftChannelOffset;

  rd = func->ReadFromFile(FileName, FOffset, &lCurSampleLeft, sizeof lCurSampleLeft);
  if (rd < sizeof lCurSampleLeft)return -1;
  SOffset += rd;

  rd = func->ReadFromFile(FileName, FOffset, &lPrevSampleLeft, sizeof lPrevSampleLeft);
  if (rd < sizeof lPrevSampleLeft)return -1;
  SOffset += rd;

  LeftSize = (ASFsch.dwRightChannelOffset-ASFsch.dwLeftChannelOffset)-4;
  bLeftChannelData = (BYTE*)my_malloc(LeftSize);

  rd = func->ReadFromFile(FileName, FOffset, bLeftChannelData, LeftSize);
  if (rd < LeftSize){
     my_free(bLeftChannelData);
     return -1;
  }
  SOffset += rd;

  SOffset = pos+ASFsch.dwRightChannelOffset;

  rd = func->ReadFromFile(FileName, FOffset, &lCurSampleRight, sizeof lCurSampleRight);
  if (rd < sizeof lCurSampleRight){
     my_free(bLeftChannelData);
     return -1;
  }
  SOffset += rd;

  rd = func->ReadFromFile(FileName, FOffset, &lPrevSampleRight, sizeof lPrevSampleRight);
  if (rd < sizeof lPrevSampleRight){
     my_free(bLeftChannelData);
     return -1;
  }
  SOffset += rd;

  RightSize = (ASFbh.dwSize-ASFsch.dwRightChannelOffset)-24;
  bRightChannelData = (BYTE*)my_malloc(RightSize);

  rd = func->ReadFromFile(FileName, FOffset, bRightChannelData, RightSize);
  if (rd < LeftSize){
     my_free(bLeftChannelData);
     my_free(bRightChannelData);
     return -1;
  }

  DecodeASFSplit(DestFile);
  my_free(bLeftChannelData);
  my_free(bRightChannelData);

  return 1;
}

int GetFileGTAVCADF(const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize, long UnpSize, struct RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{
    char buf [16384];
    long rd, sv;
    char * dstpath = my_strdup(PathName, strlen(FileName)+2);
    strcat(dstpath, "\\");
    strcat(dstpath, FileName);
    char msg[MAX_PATH] = "Decoding\n";
    char msg2[MAX_PATH];

    if (!flags->Silent){
        strcat(msg, FileName);
        strcat(msg, "\n");
    }

    for (int i = StartOffset; i<FileSize+StartOffset;){
        rd = func->ReadFromFile(PackName, i, buf, sizeof buf);
        if (rd == -1) {
            my_free(dstpath);
            return FALSE;
        }
        if (rd > 0) {
            i += rd;
            for (int j = 0; j<rd; j++) buf[j] ^= 0x22;
            sv = func->AppendToFile(dstpath, buf, rd);
            if (sv < rd) {
                my_free(dstpath);
                return FALSE;
            }
        }

        if (func->CheckKey(VK_ESCAPE)){
            my_free(dstpath);
            return 2;
        }
        if (!flags->Silent){
            sprintf(msg2, "%s%d/%d", msg, i-StartOffset, FileSize);
            func->ShowMessage(FALSE, FALSE, FALSE, TRUE, (const char * const *)msg2, 0, 0);
        }
    }
    my_free(dstpath);
    return TRUE;
}

long FormatCheckEAASF(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{




    /*if ((FileSize+FileOffset+fres<0)||
        (WorkFileSize<(FileSize+FileOffset+fres))||
        (FileSize<=0)) return -1;*/

    if ((w[0] == 0x50)&&
        (w[1] == 0x54)){

        strcpy(finfo->ExtNormal,   ".asf");
        strcpy(finfo->ExtConvrt,   ".wav");
        strcpy(finfo->Description, "EA ASF");

        return FOffset;
    }
    return -1;
}

long PackCheckEAASF(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
    long rd, FOffset;
    unsigned char t[4] = {0x53, 0x43, 0x48, 0x6C}; //"SCHl" signature
    unsigned char w[2]; //PT signature
    char fname[MAX_PATH];
    char fext[3];
    char* temp;

    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, 4, readed);
    if (fres == -1) return -1;

    FOffset = FileOffset+fres+4+4;

    rd = func->ReadFromFile(FileName, FOffset, &w, 2);
    if (rd < 2)return -1;

    FOffset += rd+4;

    temp = func->ExtractName(FileName);

    strcpy(fname, temp);
    func->FreeMem(temp);
    strcpy(fext, fname+(strlen(fname)-3));

    if (strcmp(fext, "adf") != 0)
        if (strcmp(fext, "ADF") != 0) return -1;

    fname[strlen(fname)-3] = 0;
    strcat(fname, "mp3");

    FilesNumber = 1;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*FilesNumber);
    if (fi == NULL) return -1;
    *finfo = fi;

    memset(&fi[0], 0, sizeof(RIP_FILE_INFO));
    fi[0].StartOffset = 0;
    fi[0].FileSize = WorkFileSize;
    fi[0].FullFileName = func->ExtractName(FileName);
    fi[0].ConvFullFileName = my_strdup(fname);
    fi[0].Description = my_strdup("Crypted GTA VC radio file");

    FileOffset = 0;
    FileSize = WorkFileSize;
    return FileSize;
}