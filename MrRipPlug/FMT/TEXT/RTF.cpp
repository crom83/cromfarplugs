long FormatCheckRTF(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    unsigned char t[] = {0x7B, 0x5C, 0x72, 0x74, 0x66}; //RTF start signature
    unsigned char c[] = {0x70, 0x61, 0x72, 0x20, 0x7D}; //RTF  end signature
    long rd;
    unsigned char ch;

    long fres = func->Find(t, BaseString, 5, readed);
    if (fres == -1) return -1;

    long i = func->FindInFile(FileName, FileOffset + fres + 5, c, 5);
    if (i == -1) return -1;

    while (TRUE)
    {
        rd = func->ReadFromFile(FileName, i, &ch, 1);
        if (rd == 0) break;
        if (ch == 0x7D) i++;
        if (ch == 0x7B)
        {              
            i = func->FindInFile(FileName, i, c, 5);
            if (i == -1)return -1;                                      
        }
                if ((ch != 0x7D)&&(ch != 0x7B)) break;          
    }
    FileOffset += fres;
    FileSize = i-FileOffset;
    UnpSize = FileSize;
    strcpy(finfo->ExtNormal,   ".rtf");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "Rich text format");
        
    return i;
}