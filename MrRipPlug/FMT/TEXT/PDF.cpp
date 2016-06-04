long FormatCheckPDF(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    unsigned char t[] = {0x25, 0x50, 0x44, 0x46, 0x2D}; //PDF start signature
    unsigned char c[] = {0x25, 0x25, 0x45, 0x4F, 0x46, 0x0D}; //PDF end signature

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    long i = func->FindInFile(FileName, FileOffset + fres + sizeof t, c, sizeof c);
    if (i == -1) return -1;     

    FileOffset += fres;
    FileSize = i-FileOffset;
    UnpSize = FileSize;
    strcpy(finfo->ExtNormal, ".pdf"); 
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "Adobe Acrobate PDF file");

    return i;
}