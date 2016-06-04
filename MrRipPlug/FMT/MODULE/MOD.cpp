long FormatCheckMOD(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    union mod{
        unsigned char c[2]; 
        unsigned short i;
    };

#pragma pack(1)
    struct mod_ins{
        char  name[22];
        mod   length;
        char  other[6];
    } mod_instr;
#pragma pack(8)

    long rd, FOffset, instr_size = 0;
    unsigned char temp;

    unsigned char t[] = {0x4D, 0x2E, 0x4B, 0x2E}; //MOD start signature         

    long fres = func->Find(t, BaseString, 4, readed);
    if (fres == -1) return -1;

    FOffset = (FileOffset+fres)-1060; // to the start of instruments

    for (int i=0; i!=30; i++)
    {
        rd = func->ReadFromFile(FileName, FOffset, &mod_instr, sizeof (struct mod_ins));        
        if (rd < sizeof (struct mod_ins)) return -1;                
        FOffset += rd;
        temp = mod_instr.length.c[1];
        mod_instr.length.c[1] = mod_instr.length.c[0];
        mod_instr.length.c[0] = temp;
        instr_size = instr_size + (mod_instr.length.i*2);
    }

    FOffset = (FileOffset+fres)-128; // to the start of patterns

    char max_pat = 0;
    for (i=0; i!=127; i++)
    {                       
        rd = func->ReadFromFile(FileName, FOffset++, &temp, 1);
        if (rd < 1)return -1;                        
        if ((temp+1) > max_pat) max_pat = temp+1;
    }

    FileSize = instr_size+(max_pat*1024)+1084;

    if ((FileSize+FileOffset+fres-1080<0)||
        (WorkFileSize<(FileSize+FileOffset+fres-1080))||
        (FileSize<=0)) return -1;        

    strcpy(finfo->ExtNormal,   ".mod");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "ProTracker module");

    FileOffset = FileOffset + (fres-1080);
    FOffset    = FileOffset+FileSize;
    HeadOffset = 1080; //!!!! for full scan
    UnpSize = FileSize;
         
    return FOffset;
}