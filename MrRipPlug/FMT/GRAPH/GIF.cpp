long FormatCheckGIF(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    unsigned char t[4] = {0x47, 0x49, 0x46, 0x38}; //GIF8 start signature
    unsigned char c[2] = {0x00, 0x3B};                     //GIF  end signature
    long MaxEnd, tmp, rd;
    char ch[2];

    long fres = func->Find(t, BaseString, 4, readed);
    if (fres == -1) return -1;

    rd = func->ReadFromFile(FileName, FileOffset + fres + 4, &ch, sizeof ch);
    if (rd<sizeof ch) return -1;

    switch (ch[0]){
    case '7': case '9': if (ch[1] != 'a') return -1;
        break;
    default: return -1;
    }
    

    long i = func->FindInFile(FileName, FileOffset + fres + 4, c, 2);
    if (i == -1) return -1;

    MaxEnd = i;

    while (TRUE){
        tmp = func->FindInFile(FileName, MaxEnd, c, 2);
        if (tmp == -1) break;

        i = func->FindInFile(FileName, MaxEnd, t, 4);
        if (i == -1) MaxEnd = tmp;            
        else {
            if (tmp<i) MaxEnd = tmp;
            else break;
        }
    }

    FileOffset += fres;
    FileSize = MaxEnd-FileOffset;
    UnpSize  = FileSize;
    strcpy(finfo->ExtNormal,".gif");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "CompuServe Graphics Image File");    
        
    return MaxEnd;
}