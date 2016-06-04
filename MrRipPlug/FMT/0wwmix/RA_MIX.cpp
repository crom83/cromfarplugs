#include "misc/cc_file.cpp"

long PackCheckRAMIX(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
#pragma pack(push, 1)
    struct FATItem{        
        long ID;
        long FileOffset;
        long FileSize;        
    }Item;
#pragma pack(pop)

    long rd, FOffset, SOffset;
    long Flag;
    short nFiles;
    long FSize;
    char s[MAX_PATH];
    char ext[10];
    unsigned char data[1024];
    t_file_type gm = ft_unknown;
    BOOL Encrypted = false;
    BOOL Checksum  = false;

    RIP_FILE_INFO           * fi;

    if (FileOffset>0) return -1;    
    
    FOffset = 0;

    rd =  func->ReadFromFile(FileName, FOffset, &Flag, sizeof Flag);
    if (rd < sizeof Flag) return -1;

    Encrypted = Flag & mix_encrypted;
    Checksum  = Flag & mix_checksum;
    FOffset += rd;

    if (!Encrypted) {
        rd =  func->ReadFromFile(FileName, FOffset, &nFiles, sizeof nFiles);
        if (rd < sizeof nFiles) return -1;
    
        SOffset = nFiles*sizeof Item;
        SOffset += 10;

        if (nFiles < 1||SOffset>WorkFileSize) return -1;
        FOffset += rd;        

        rd =  func->ReadFromFile(FileName, FOffset, &FSize, sizeof FSize);
        if (rd < sizeof FSize) return -1;

        FSize = Checksum?FSize + 20:FSize;

        if (FSize != WorkFileSize-SOffset) return -1;    
        FOffset += rd;

        fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
        if (fi == NULL) return -1;
        *finfo = fi;    

        FilesNumber = nFiles;    
        
        for (int i = 0; i<FilesNumber; i++){    
            rd =  func->ReadFromFile(FileName, FOffset, &Item, sizeof Item);
            if (rd < sizeof Item) return -1;
        
            FOffset += rd;
        
            memset(&fi[i], 0, sizeof(RIP_FILE_INFO));
        
            fi[i].StartOffset = Item.FileOffset+SOffset;
            fi[i].FileSize    = Item.FileSize;
        

            if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
                for (int j=0; j<i; j++)
                {
                    my_free(fi[j].FullFileName);
                    my_free(fi[j].Description);        
                }
                my_free(fi);                
                return -1;
            }

            rd =  func->ReadFromFile(FileName, fi[i].StartOffset, &data, 1024);
            if ((gm = get_file_type(data, rd, (char*)ext, fi[i].FileSize))==ft_unknown) strcpy(ext, "unk");
        
            sprintf(s, "%x", Item.ID);
            strcat(s, ".");strcat(s, ext);


            fi[i].FullFileName = my_strdup(s);
            fi[i].Description = my_strdup("");        
        
        }
    } else {
        unsigned char key_src[80];
        unsigned char key[56];

#pragma pack(push, 1)
        union {
            unsigned char b[8];
            struct{
                short nFiles;
                long  FSize;
            };
        } e;
#pragma pack(pop)

        Cblowfish bf;

        rd =  func->ReadFromFile(FileName, FOffset, &key_src, sizeof key_src);
        if (rd < sizeof key_src) return -1;
        FOffset += rd;

        get_blowfish_key(key_src, key);
        bf.set_key(key, 56);

        rd =  func->ReadFromFile(FileName, FOffset, &e.b, sizeof e.b);
        if (rd < sizeof e.b) return -1;
        FOffset += rd;

        bf.decipher(e.b, e.b, 8);

        nFiles = e.nFiles;
        FSize = e.FSize;        
    
        SOffset = nFiles*sizeof Item;       

        if (nFiles < 1||SOffset>WorkFileSize) return -1;

        const int cb_f = SOffset + 5 & ~7;
        if (WorkFileSize != 92 + cb_f + FSize + (Checksum ? 20 : 0))
            return -1;        

        SOffset = 92 + cb_f;
        
        FSize = Checksum?FSize + 20:FSize;

        fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*nFiles);
        if (fi == NULL) return -1;
        *finfo = fi;    

        FilesNumber = nFiles;    
                
        BYTE* f = (BYTE*)my_malloc(cb_f);
        rd =  func->ReadFromFile(FileName, FOffset, f, cb_f);
        if (rd < cb_f) {
            my_free(f);
            return -1;
        }
        FOffset += rd;
        
        bf.decipher(f, f, cb_f);

        memcpy(&Item, e.b+6, 2);
        memcpy((char*)&Item+2, f, 10);

        memset(&fi[0], 0, sizeof(RIP_FILE_INFO));
        
        fi[0].StartOffset = Item.FileOffset+SOffset;
        fi[0].FileSize    = Item.FileSize;        

        if (fi[0].StartOffset+fi[0].FileSize>WorkFileSize){            
            my_free(fi);
            my_free(f);
            return -1;
        }

        rd =  func->ReadFromFile(FileName, fi[0].StartOffset, &data, 1024);
        if ((gm = get_file_type(data, rd, (char*)ext, fi[0].FileSize))==ft_unknown) strcpy(ext, "unk");
        
        sprintf(s, "%x", Item.ID);
        strcat(s, ".");strcat(s, ext);

        fi[0].FullFileName = my_strdup(s);
        fi[0].Description = my_strdup("");

        for (int i = 0; i<FilesNumber-1; i++){            
            memcpy((char*)&Item, (f+10)+(i*sizeof Item), 12);            
        
            memset(&fi[i+1], 0, sizeof(RIP_FILE_INFO));
        
            fi[i+1].StartOffset = Item.FileOffset+SOffset;
            fi[i+1].FileSize    = Item.FileSize;        

            if (fi[i+1].StartOffset+fi[i+1].FileSize>WorkFileSize){
                for (int j=0; j<i+1; j++)
                {
                    my_free(fi[j].FullFileName);
                    my_free(fi[j].Description);        
                }
                my_free(fi);
                my_free(f);
                return -1;
            }            

            rd =  func->ReadFromFile(FileName, fi[i+1].StartOffset, &data, 1024);
            if ((gm = get_file_type(data, rd, (char*)ext, fi[i+1].FileSize))==ft_unknown) strcpy(ext, "unk");
        
            sprintf(s, "%x", Item.ID);
            strcat(s, ".");strcat(s, ext);

            fi[i+1].FullFileName = my_strdup(s);
            fi[i+1].Description = my_strdup("");        
        
        }
        my_free(f);
    }
    FileOffset = 0;
    FileSize = SOffset+FSize;
    return FileSize;
}