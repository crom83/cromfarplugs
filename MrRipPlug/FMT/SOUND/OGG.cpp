long FormatCheckOGG(const char * FileName, unsigned char* BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long rd, SOffset, FOffset, framesize;
    unsigned char blocks, blocksize;
    BOOL fnd = FALSE;    
    unsigned long oggs = 0x5367674F; //OggS start signature
    unsigned char oggs_c[] = {0x4f, 0x67, 0x67, 0x53}; //OggS start signature
    unsigned char vorbis[] = "vorbis";
    unsigned long temp;    
    char v[7];
    
    long fres = func->Find(vorbis, BaseString, sizeof vorbis, readed);    
    if (fres == -1) return -1;                             

    SOffset = FileOffset+fres - 0x1D;    
    FOffset = SOffset; 
    
    rd = func->ReadFromFile(FileName, FOffset, &temp, sizeof temp);
    if (rd < sizeof temp) return -1;

    if(temp!=oggs) return -1; // not ogg vorbis

    FileSize = 0;

    while (FOffset<WorkFileSize)
    {       
        FOffset = func->FindInFile(FileName, FOffset, oggs_c, sizeof oggs_c);
        if (FOffset <= -1) break;
        else FOffset -= sizeof oggs_c;
        rd = func->ReadFromFile(FileName, FOffset, &temp, sizeof temp);
        if (rd < sizeof temp) 
            break;
        if(temp!=oggs) 
            break; // end vorbis file

        FileSize = FOffset-SOffset;

        FOffset += 0x1D;

        rd = func->ReadFromFile(FileName, FOffset, &v, sizeof v);
        if (rd < sizeof v) break;

        if ((strcmp(v,(char*)vorbis)==0)&&            
            (FileSize>0)){

            FOffset -= 0x1D;
            break; //start
        }

        FOffset -= 3;

        framesize = 0;

        rd = func->ReadFromFile(FileName, FOffset, &blocks, sizeof blocks);
        if (rd < sizeof blocks) 
            break;
        
        FOffset += rd;                

        for (int i=0; i<blocks;i++){
            rd = func->ReadFromFile(FileName, FOffset, &blocksize, sizeof blocksize);
            if (rd < sizeof blocksize) 
                break;
            FOffset += rd;            
            framesize += blocksize;
        }

        FOffset += framesize;

        FileSize += framesize + 27+blocks;

        if (WorkFileSize<FileSize+SOffset) 
        {
            FileSize -= framesize;
            break;
        }
    }

    if (FileSize <= 0) return -1;        
    
    strcpy(finfo->ExtNormal,   ".ogg");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "Ogg Vorbis audio file");
    FileOffset = SOffset;
    HeadOffset = 0x1D;
    FOffset = FileOffset+FileSize;
    UnpSize = FileSize;
        
    return FOffset;
}