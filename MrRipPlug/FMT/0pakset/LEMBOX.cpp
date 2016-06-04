long PackCheckLEMBOX(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(1)    
    struct FATInfo2{
        long Len;
        char FN[MAX_PATH];        
    }FATItem2;    
#pragma pack(8)

    long rd, FOffset, FOffset2, FOffset3, SOffset;
    ULONG nFiles, dirsize, Offset, Size, z=0;
    bool WasError = false;
    int errorfrom;
    
    RIP_FILE_INFO           * fi;
    
    unsigned char           t[] = "LEMBOX";     
    
    long fres = func->Find(t, BaseString, sizeof t-1, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;
    FOffset = SOffset+sizeof t-1;

    rd =  func->ReadFromFile(FileName, FOffset, &nFiles, sizeof nFiles);
    if (rd < sizeof nFiles) return -1;    
    FOffset += rd;

    if (nFiles*8>WorkFileSize) return -1;
    if (nFiles<1) return -1;

    rd =  func->ReadFromFile(FileName, FOffset, &dirsize, sizeof dirsize);
    if (rd < sizeof dirsize) return -1;    
    FOffset += rd;

    if (dirsize>WorkFileSize) return -1;    

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = nFiles;
    
    FOffset2 = SomeOffset+14+dirsize+5;
    FOffset3 = FOffset2+nFiles*4+4;

    for (int i = 0; i<FilesNumber; i++){
        rd =  func->ReadFromFile(FileName, FOffset, &FATItem2, sizeof FATItem2);
        if (rd < sizeof FATItem2.Len) {
            WasError = true;
            errorfrom = i;
            break;
        }
        FATItem2.FN[FATItem2.Len]=0;
        FOffset += FATItem2.Len+4;
        
        rd =  func->ReadFromFile(FileName, FOffset2, &Offset, sizeof Offset);
        if (rd < sizeof Offset){
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset2 += 4;

        rd =  func->ReadFromFile(FileName, FOffset3, &Size, sizeof Size);
        if (rd < sizeof Size) {
            WasError = true;
            errorfrom = i;
            break;
        }
        FOffset3 += 4;

        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));        
        

        fi[i].StartOffset = Offset;
        fi[i].FileSize    = Size;        
        if ((lstrlen(FATItem2.FN)==0)||Offset+Size>WorkFileSize||Size<=0){
            WasError = true;
            errorfrom = i;
            break;
        }
        if (z < Offset+Size) z = Offset+Size;
        if (FATItem2.FN[0]!='.')
             fi[i].FullFileName = my_strdup(FATItem2.FN);
        else fi[i].FullFileName = my_substr(FATItem2.FN, 2, lstrlen(FATItem2.FN));
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

    FileOffset = SOffset;
    FileSize = z-FileOffset;
    return FileSize;    
}