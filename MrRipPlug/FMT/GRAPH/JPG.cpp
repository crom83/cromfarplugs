long FormatCheckJPG(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    unsigned char t[] = {0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46}; //JPEG start signature
    unsigned char c[] = {0xFF, 0xD9};               //JPEG end signature
    unsigned char d[] = {0xFF, 0xED};               //JPEG thumb signature
    long i;

    long fres = func->Find(t, BaseString, 10, readed);
    if (fres == -1) return -1;
    
    if (func->FindInFile(FileName, FileOffset + fres + 10, d, 2) == -1)
    {
        i = func->FindInFile(FileName, FileOffset + fres + 10, c, 2);
        if (i == -1) return -1;
    } else {
        i = func->FindInFile(FileName, FileOffset + fres + 10, c, 2);
        if (i == -1) return -1;
        i = func->FindInFile(FileName, i, c, 2);
        if (i == -1) return -1;
    }


    FileOffset += fres;
    FileSize = i-FileOffset;
    UnpSize  = FileSize;
    strcpy(finfo->ExtNormal, ".jpg");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "JPEG compressed image");

    return i;
}