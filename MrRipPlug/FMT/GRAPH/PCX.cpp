long FormatCheckPCX(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, long &UnpSize, struct RIP_FORM_INFO *finfo, long WorkFileSize, RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    long rd, FOffset;       
    long SOffset;
    unsigned char t0[] = {0x0A, 0x00, 0x01}; //PCX 0 signature
    unsigned char t2[] = {0x0A, 0x02, 0x01}; //PCX 2 signature
    unsigned char t3[] = {0x0A, 0x03, 0x01}; //PCX 3 signature
    unsigned char t4[] = {0x0A, 0x04, 0x01}; //PCX 4 signature
    unsigned char t5[] = {0x0A, 0x05, 0x01}; //PCX 5 signature
    unsigned char * buf;

    long fs, i=0; 
    char bpp, Nplanes, ver;
    unsigned char *chr;
    unsigned short Ymax, Ymin, bpl;    

    char msg2[MAX_PATH];

    long fres = func->Find(t5, BaseString, sizeof t5, readed);
    if (fres == -1) ver = -1; else ver = 5;

    if (ver == -1) {
        fres = func->Find(t2, BaseString, sizeof t2, readed);
        if (fres == -1) ver = -1; else ver = 2;
    }

    if (ver == -1) {
        fres = func->Find(t3, BaseString, sizeof t3, readed);
        if (fres == -1) ver = -1; else ver = 3;
    }

    if (ver == -1) {
        fres = func->Find(t4, BaseString, sizeof t4, readed);
        if (fres == -1) ver = -1; else ver = 4;
    }

    if (ver == -1) {
        fres = func->Find(t0, BaseString, sizeof t0, readed);
        if (fres == -1) return -1; else ver = 0;
    }

    SOffset = FileOffset+fres;
    FOffset = SOffset + sizeof t0;
    
    rd = func->ReadFromFile(FileName, FOffset, &bpp, sizeof bpp);
    if (rd < sizeof bpp) return -1;        

    fs = 128;
    switch (bpp){
    case 1:
    case 2:
    case 4:
    case 8:    
        
        FOffset += rd+2;

        rd = func->ReadFromFile(FileName, FOffset, &Ymin, sizeof Ymin);
        if (rd < sizeof Ymin) return -1;        
        FOffset += rd+2;

        rd = func->ReadFromFile(FileName, FOffset, &Ymax, sizeof Ymax);
        if (rd < sizeof Ymax) return -1;        
        FOffset = SOffset + 65;

        if (Ymax < Ymin) return -1;

        rd = func->ReadFromFile(FileName, FOffset, &Nplanes, sizeof Nplanes);
        if (rd < sizeof Nplanes) return -1;        
        FOffset += rd;
        
        if (bpp==8&&ver==5) fs += 769;

        rd = func->ReadFromFile(FileName, FOffset, &bpl, sizeof bpl);
        if (rd < sizeof bpl) return -1;        
        FOffset = SOffset + 0x48;

        /*buf = (unsigned char*)my_malloc(128-0x48);
        rd = func->ReadFromFile(FileName, FOffset, buf, 128-0x48);
        if (rd < 128-0x48) {
            my_free(buf);
            return -1;
        }
        for (i=0;i<128-0x48;i++) {
            if (buf[i] != 0) {
                my_free(buf);
                return -1;
            }
        }

        my_free(buf);*/

        FOffset = SOffset + 128;        

        long l, lsize;
        lsize = (long )bpl * Nplanes * (1 + Ymax - Ymin);
        buf = (unsigned char*)my_malloc(lsize*2);
        rd = func->ReadFromFile(FileName, FOffset, buf, lsize*2);                        
        if (rd < 1) {
            my_free(buf);
            return -1;
        }

        if (!flags->Silent){           
            sprintf(msg2, "PCX decoding...\n%s", FileName);
            func->ShowMessage(FALSE, FALSE, FALSE, TRUE, (const char * const *)msg2, 0, 0);
        }

        chr = buf;
        for (l = 0; l < lsize; ){            
            if (0xC0 == (0xC0 & *chr)){
                l  += 0x3F & *chr;
                chr++;
                chr++;
                fs += 2;                
            } else {
                chr++;
                l  += 1;
                fs += 1;
            }                    
        }
        my_free(buf);

        if ((fs+SOffset<0)||
            (WorkFileSize<fs+SOffset)||
            (fs <= 0)) return -1;
    
        FileSize = fs;
        UnpSize  = FileSize;
        strcpy(finfo->ExtNormal, ".pcx");
        finfo->ExtConvrt[0] = 0;
        strcpy(finfo->Description, "ZSoft image file");    

        FileOffset  = SOffset;
        FOffset     = FileOffset+FileSize;
        break;
    default: return -1;
    }
    return FOffset;    
}
