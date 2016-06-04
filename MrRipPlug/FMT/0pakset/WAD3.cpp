BOOL GetFileWAD3(const char * PackName, const char * FileName, const char * PathName, long StartOffset, long FileSize, long UnpSize, struct RIP_SERVICE_FUNC* func, struct RIP_FLAGS *flags)
{
#pragma pack(1)
    struct TGARec{
        long Junk1;
        long Junk2;
        long Junk3;
        WORD Width;
        WORD Height;
        WORD Junk4;
    }TGA;

    struct RGBRec{
        BYTE R;
        BYTE G;
        BYTE B;
    };
#pragma pack(8)
  
    RGBRec TGAPalette[256];

    long _Width,_Height,Mip1,Mip2,Mip3,Mip4;
    short PalSize;
    long FOffset = StartOffset, SOffset;
        
    long rd, sv;
    char * dstpath = my_strdup(PathName, strlen(FileName)+2);

    if (UnpSize != 0) return FALSE;

    strcat(dstpath, "\\");
    strcat(dstpath, FileName);

    char msg[MAX_PATH] = "Decoding\n";
    char msg2[MAX_PATH];

    if (!flags->Silent){
        strcat(msg, FileName);
        strcat(msg, "\n");              
    }

    TGA.Junk1=0x00010100;
    TGA.Junk2=0x18010000;
    TGA.Junk3=0x00000000;
    TGA.Junk4=0x0808;    
         
    FOffset += 16;
    rd = func->ReadFromFile(PackName, FOffset, &_Width,sizeof _Width);
    if (rd < sizeof _Width) {
        my_free(dstpath);
        return FALSE;
    }
    FOffset += rd;

    rd = func->ReadFromFile(PackName, FOffset, &_Height,sizeof _Height);
    if (rd < sizeof _Height) {
        my_free(dstpath);
        return FALSE;
    }
    FOffset += rd;

    rd = func->ReadFromFile(PackName, FOffset, &Mip1,sizeof Mip1);
    if (rd < sizeof Mip1) {
        my_free(dstpath);
        return FALSE;
    }
    FOffset += rd;
    rd = func->ReadFromFile(PackName, FOffset, &Mip2,sizeof Mip2);
    if (rd < sizeof Mip2) {
        my_free(dstpath);
        return FALSE;
    }
    FOffset += rd;
    rd = func->ReadFromFile(PackName, FOffset, &Mip3,sizeof Mip3);
    if (rd < sizeof Mip3) {
        my_free(dstpath);
        return FALSE;
    }
    FOffset += rd;
    rd = func->ReadFromFile(PackName, FOffset, &Mip4,sizeof Mip4);
    if (rd < sizeof Mip4) {
        my_free(dstpath);
        return FALSE;
    }
    FOffset += rd;

    TGA.Width =_Width;
    TGA.Height=_Height;

    sv = func->AppendToFile(dstpath, &TGA, sizeof TGA);
    if (sv<sizeof TGA){
        my_free(dstpath);
        return FALSE;
    }
    SOffset = FOffset;

    FOffset += (Mip4-40)+(Mip4-Mip3) / 4;
       
    rd = func->ReadFromFile(PackName, FOffset, &PalSize,sizeof PalSize);
    if (rd < sizeof PalSize) {
        my_free(dstpath);
        return FALSE;
    }
    FOffset += rd;

    memset(TGAPalette, 0, 256);
    
    for (int j=0; j<PalSize;j++){
        rd = func->ReadFromFile(PackName, FOffset++, &TGAPalette[j].B,sizeof TGAPalette[j].B);
        rd += func->ReadFromFile(PackName, FOffset++, &TGAPalette[j].G,sizeof TGAPalette[j].G);
        rd += func->ReadFromFile(PackName, FOffset++, &TGAPalette[j].R,sizeof TGAPalette[j].R);
        if (rd < sizeof TGAPalette[j]) {
            my_free(dstpath);
            return FALSE;
        }       
    }
    
    sv = func->AppendToFile(dstpath, TGAPalette, sizeof TGAPalette);
    if (sv<sizeof TGAPalette){
        my_free(dstpath);
        return FALSE;
    }    

    for (j=_Height-1;j>=0;j--){      
        FOffset = SOffset+j*_Width;
        func->FileToFileCopy(PackName, dstpath, FOffset, _Width, -1);
        if (!flags->Silent){           
            sprintf(msg2, "%s%d/%d", msg, FOffset-StartOffset, FileSize);
            func->ShowMessage(FALSE, FALSE, FALSE, TRUE, (const char * const *)msg2, 0, 0);
        }
    }
    
    my_free(dstpath);
    return TRUE;
}

long PackCheckWAD3(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)
    struct DoomInfo{
        long FilePos;
        long FileSize;
        long Res1;
        long Res2;
        char FileName[16];        
    }Doom;
#pragma pack(8)

    char FName[16];
    long rd, FOffset, SOffset, DirOffset, nFiles, z;
    unsigned char           t[] = "WAD3";
    bool WasError=false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;

    long fres = func->Find(t, BaseString, sizeof t-1, readed);
    if (fres == -1) return -1;
    
    SOffset = FileOffset+fres;

    FOffset = SOffset+sizeof t-1;    

    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, 4);
    if (rd < 4) return -1;
    FOffset += rd;

    rd =  func->ReadFromFile(FileName, FOffset, &DirOffset, 4);
    if (rd < 4) return -1;
    FOffset += rd;
 
    z = DirOffset+nFiles*sizeof Doom+SOffset;
    if (z>WorkFileSize)
        return -1;
        
    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;

    FOffset = DirOffset;
    for (int i = 0; i<FilesNumber; i++){    
        rd =  func->ReadFromFile(FileName, FOffset, &Doom, sizeof Doom);
        if (rd < sizeof Doom)  {
            WasError = true;
            errorfrom = i;
            break;
        }
        
        FOffset += rd;
        
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = Doom.FilePos+SOffset;
        fi[i].FileSize    = Doom.FileSize;        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize) {
            WasError = true;
            errorfrom = i;
            break;
        }
        rd =  func->ReadFromFile(FileName, fi[i].StartOffset, &FName, sizeof FName);
        if (rd < sizeof FName) {
            WasError = true;
            errorfrom = i;
            break;
        }

        fi[i].FullFileName = my_substr(Doom.FileName,0,16,5);
        fi[i].FullFileName[16]=0;

        fi[i].ConvFullFileName = my_substr(Doom.FileName,0,16,5);
        fi[i].ConvFullFileName[16]=0;

        if (lstrcmpi(FName, fi[i].ConvFullFileName)==0){
            strcat(fi[i].ConvFullFileName, ".tga");
            fi[i].UnpSize    = 0;
        } else fi[i].UnpSize    = fi[i].FileSize;
        fi[i].Description = my_strdup("");
    }

    if (WasError){
        for (int j=0; j<errorfrom; j++){
            my_free(fi[j].FullFileName);                
            my_free(fi[j].Description);        
        }
        my_free(fi);
        fi = NULL;
        return -1;
    }
    
    FileOffset = FileOffset+fres;
    FileSize = z-FileOffset;
    return z;    
}