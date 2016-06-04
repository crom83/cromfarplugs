long PackCheckLOD(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{
#pragma pack(1)
    struct {
        char Name[16];
        long Offset;
        long RealSize;
        long Type;
        long PackedSize;        
    }FATItem;
#pragma pack(8)

    unsigned char           t[] = "LOD";

    long rd, SOffset, FOffset, filestotal, x=0;    
    
    RIP_FILE_INFO           * fi;    

    long fres = func->Find(t, BaseString, sizeof t, readed);
    if (fres == -1) return -1;

    SOffset = FileOffset+fres;

    FOffset = SOffset+sizeof t+4;        

    rd =  func->ReadFromFile(FileName, FOffset, &filestotal, 4);
    if (rd < 4) return -1;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*filestotal);
    if (fi == NULL) return -1;

    *finfo = fi;
    FilesNumber = filestotal;

    FOffset = SOffset+0x5c;

    for (int i = 0; i<FilesNumber; i++){
        rd =  func->ReadFromFile(FileName, FOffset, &FATItem, sizeof FATItem);
        if (rd < sizeof FATItem) return -1;
        
        FOffset += rd;
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = FATItem.Offset;
        if (FATItem.PackedSize==0) 
            fi[i].FileSize    = FATItem.RealSize;
        else
            fi[i].FileSize    = FATItem.PackedSize;
        fi[i].UnpSize     = FATItem.RealSize;

        if (FATItem.PackedSize==0)
            x = FATItem.Offset+FATItem.RealSize>x?FATItem.Offset+FATItem.RealSize:x;        
        else 
            x = FATItem.Offset+FATItem.PackedSize>x?FATItem.Offset+FATItem.PackedSize:x;                    
        
        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);                
            }
            my_free(fi);
            return -1;
        }        

        fi[i].FullFileName = my_strdup(FATItem.Name);                
        if (FATItem.Type==1&&FATItem.PackedSize==0)
            fi[i].Description = my_strdup("GZip stream");
        else if (FATItem.Type==2&&FATItem.PackedSize==0)
            fi[i].Description = my_strdup("Text file");
        else
            fi[i].Description = my_strdup("");
    }              
    
    FileOffset = SOffset;
    FileSize = x;    
    return SOffset+FileSize;
}
