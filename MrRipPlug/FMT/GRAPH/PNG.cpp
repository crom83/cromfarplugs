long FormatCheckPNG(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    unsigned char t[4] = {0x89, 0x50, 0x4E, 0x47}; //PNG start signature
    unsigned char c[3] = {0x42, 0x60, 0x82};       //PNG  end signature

    long fres = func->Find(t, BaseString, 4, readed);
    if (fres == -1)     return -1;      
    
    func->SaveToFile("test", 0, BaseString, readed);
    long i = func->FindInFile(FileName, FileOffset + fres + 4, c, 3);
    if (i == -1) return -1;     

    FileOffset += fres;
    FileSize = i-FileOffset;
    UnpSize  = FileSize;

    strcpy(finfo->ExtNormal,   ".png");
    finfo->ExtConvrt[0] = 0;
    strcpy(finfo->Description, "Portable Network Graphics");
            
    return i;
}
