struct dunepack{
        long Offset;
        char filename[20];
        long filesize;
}d_p;


long PackCheckDUNE2(const char * FileName, PUCHAR BaseString, long readed, long &HeadOffset, long &SomeOffset, long &FileSize, long &FileOffset, struct RIP_FILE_INFO **finfo, long &FilesNumber, long WorkFileSize, struct RIP_SERVICE_FUNC *func, struct RIP_FLAGS *flags)
{       
    vector<dunepack> dp;
    
    long rd, FOffset, lastoff, curitem;

    char ctemp[20];
    long ltemp;
    
    RIP_FILE_INFO           * fi;

    FOffset = 0;
    curitem = -1;

    if (FileOffset>0) return -1;

    do{

        rd =  func->ReadFromFile(FileName, FOffset, &ltemp, sizeof ltemp);
        if (rd < sizeof ltemp) return -1;    
        
        if (ltemp>WorkFileSize||ltemp<0) return -1;
        FOffset += rd;
        
        if (ltemp==0) break;

        rd =  func->ReadFromFile(FileName, FOffset, &ctemp, sizeof ctemp);
        if (rd < 2) return -1;

        if (strlen(ctemp)==0) return -1;

        FOffset += strlen(ctemp)+1;
        
        d_p.Offset   = ltemp;
        d_p.filesize = 0;
        strcpy(d_p.filename, ctemp);
        dp.push_back(d_p);
        curitem++;

        if (curitem>0) dp[curitem-1].filesize = ltemp-lastoff;
        lastoff = ltemp;
    }
    while (FOffset<WorkFileSize);
    if (curitem == -1) return -1;
    dp[curitem].filesize = WorkFileSize-dp[curitem].Offset;

    fi = (RIP_FILE_INFO*)my_malloc(sizeof(RIP_FILE_INFO)*dp.size());
    if (fi == NULL) return -1;
    *finfo = fi;    

    FilesNumber = dp.size();
    for (int i=0; i<dp.size();i++){
        memset(&fi[i], 0, sizeof(RIP_FILE_INFO));

        fi[i].StartOffset = dp[i].Offset;
        fi[i].FileSize    = dp[i].filesize;

        if (fi[i].StartOffset+fi[i].FileSize>WorkFileSize){
            for (int j=0; j<i; j++)
            {
                my_free(fi[j].FullFileName);
                my_free(fi[j].Description);        
            }
            my_free(fi);
            fi = NULL;
            return -1;
        }
        
        fi[i].FullFileName = my_strdup(dp[i].filename);                  
        fi[i].Description = my_strdup("");
    }              
    
    FileOffset = 0;
    FileSize = WorkFileSize;
    return WorkFileSize;    
}