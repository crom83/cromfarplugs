long WINAPI Find(unsigned char *SearchString, unsigned char *BaseString, long SSsize, long BSsize)
{       
                if (SSsize>BSsize-SSsize) return -1;
        for (long i=0; i<=BSsize-SSsize; i++)
        {
                        if (!memcmp(SearchString, (BaseString + i), SSsize)) return i;                          
        }
        return -1;
}

long WINAPI FindInFile(const char* FileName, long StartOffset, unsigned char *SearchString, long SSsize)
{
    long readed, fres, fpos;                

    int f = _open (FileName, _O_RDONLY|_O_BINARY);
    if (f == -1) return -1;

    _lseek(f, StartOffset, SEEK_SET);
    fpos = StartOffset;
    while (!_eof(f))
    {
        readed = _read(f, buf, BUF_SIZE);
        if (readed <= SSsize) {
            _close(f);
            return -1;
        }
                                
        fres = Find(SearchString, buf, SSsize, readed);         
        if (fres==-1) 
        {
                        fpos = fpos + readed - SSsize;
            _lseek(f, fpos, SEEK_SET);
                }
        else
        {                               
            long FOffset = fpos+fres+SSsize;                        
            _close(f);
            return FOffset;
        }                
    }       
    _close(f);
    return -1;
}

long WINAPI ReadFromFile(const char* FileName, long StartOffset, void *Buffer, long BufSize)
{   
    int f = _open (FileName, _O_RDONLY|_O_BINARY);
    if (f == -1) return -1;

    long r = _lseek(f, StartOffset, SEEK_SET);
    if (r != StartOffset) {
        _close(f);
        return -1;
    }

    r = _read(f, Buffer, BufSize);

    _close(f);

    return r;
}

long WINAPI SaveToFile(const char* FileName, long StartOffset, void *Buffer, long BufSize)
{   
    int f = _open (FileName, _O_CREAT|_O_WRONLY|_O_BINARY, _S_IWRITE);
    if (f == -1) return -1;

    long r = _lseek(f, StartOffset, SEEK_SET);
    if (r != StartOffset) {
        _close(f);
        return -1;
    }

    r = _write(f, Buffer, BufSize);
    _close(f);

    return r;
}

long WINAPI AppendToFile(const char* FileName, void *Buffer, long BufSize)
{   
    int f = _open (FileName, _O_CREAT|_O_APPEND|_O_WRONLY|_O_BINARY, _S_IWRITE);
    if (f == -1) return -1;

    long r = _write(f, Buffer, BufSize);
    _close(f);

    return r;
}

long WINAPI FileToFileCopy(const char* FileNameSrc, const char* FileNameDst, long SrcStart, long SrcLen, long DstStart)
{   
    long readed, fullreaded;
    int f2 = 0;
    int f1 = _open (FileNameSrc, _O_RDONLY|_O_BINARY);
    if (f1 == -1) return -1;

    if (DstStart == -1) {
        f2 = _open (FileNameDst, _O_CREAT|_O_APPEND|_O_WRONLY|_O_BINARY, _S_IWRITE);
        if (f2 == -1) {
            _close(f1);
            return -1;
        }
    } else {
        f2 = _open (FileNameDst, _O_CREAT|_O_WRONLY|_O_BINARY, _S_IWRITE);
        if (f2 == -1) {
            _close(f1);
            return -1;
        }

        if (_lseek(f2, DstStart, SEEK_SET) == -1L) {
            _close(f1);
            _close(f2);
            return -1;
        }
    }
    
    if (_lseek(f1, SrcStart, SEEK_SET) == -1L) {
        _close(f1);
        _close(f2);
        return -1;
    }         

    fullreaded = 0;
    while (!_eof(f1))
    {
        if (fullreaded+BUF_SIZE<SrcLen){
            readed = _read(f1, buf, BUF_SIZE);
            if (readed<BUF_SIZE) {
                _close(f1);
                _close(f2);
                return -1;
            }
            fullreaded += readed;
            _write(f2, &buf, readed);
        } else {
            readed = _read(f1, buf, SrcLen-fullreaded);
            if (readed<SrcLen-fullreaded) {
                _close(f1);
                _close(f2);
                return -1;
            }
            fullreaded += readed;
            _write(f2, &buf, readed);
            break;
        }        
    }       
    _close(f1);
    _close(f2);
    return fullreaded;
}

short WINAPI SwapWord(short Value)
{
  short  b1 =(Value & 0xFF00) >> 8;
  short  b2 =(Value & 0x00FF) << 8;
  return b1 | b2;
}

long WINAPI SwapLong(long Value)
{
  long b1 =(Value & 0xFF000000) >> 24;
  long b2 =(Value & 0x00FF0000) >> 8;
  long b3 =(Value & 0x0000FF00) << 8;
  long b4 =(Value & 0x000000FF) << 24;
  return b1 | b2 | b3 | b4;
}

char* WINAPI ExtractPath(const char *Path)
{    
    char *f, *t, *t2;
    int i = strlen(Path); 
    if (Path[strlen(Path)-1] == '\\') 
         t = my_substr(Path, 0, strlen(Path)-1, 0);
    else t = my_strdup(Path, 0);
    f = strrchr(t, '\\');
    if (f) t2 = my_substr(t, 0, f-t+1, 0);
    else t2 = my_strdup("");
    my_free(t);
    if (strlen(t2)>1) t2[strlen(t2)-1] = 0;    
    return t2;
}

char* WINAPI ExtractName(const char* Path)
{   
    char  *t, *t2;
    char  *f;

    if (Path[strlen(Path)-1] == '\\') {
        t = my_substr(Path, 0, lstrlen(Path)-1);
        f = strrchr(t, '\\');
        if (f) t2 = my_substr(t, f-t+1, lstrlen(t),1);
        else t2 = my_strdup(t, 1);
        my_free(t); 
        lstrcat(t2, "\\");
        return t2;
    } else {        
        f = strrchr(Path, '\\');
        if (f) t = my_substr(Path, f-Path+1, lstrlen(Path));
        else t = my_strdup(Path);
        return t;
    }
    return NULL;
}

char* WINAPI ExtractRoot(const char* Path)
{   
    char  *t;
    char  *f;

    f = strchr(Path, '\\');
    if (f) t = my_substr(Path, 0, f-Path);
    else t = my_strdup(Path);
            
    return t;
}

BOOL WINAPI FileInDir(const char *Dir, const char *Path)
{
    BOOL b;    
    char *t;
    char *temp;    
    char *n;

    if (Dir[strlen(Dir)-1] == '\\') n = my_substr(Dir, 0, strlen(Dir)-1);
    else n = my_strdup(Dir);
    if (Path[strlen(Path)-1] == '\\'){
        t = my_substr(Path, 0, lstrlen(Path)-1);        
        temp = ExtractPath(t);        
        my_free(t);
        b = (lstrcmp(temp, n)==0);
        my_free(temp);
        my_free(n);
        return b;
    } else {
        temp = ExtractPath(Path);
        b = (lstrcmp(temp, n)==0);
        my_free(temp);
        my_free(n);
        return b;
    }    
}

int WINAPI MyShowMessage(BOOL Warning,
						 BOOL KeepBG,
						 BOOL LeftAlign,
						 BOOL AllInOne,
						 const char * const *Items, 
						 int ItemsNumber, 
						 int ButtonsNumber)
{
	unsigned int flags = 0;
	
	if (Warning)   flags |= FMSG_WARNING;
	if (KeepBG)    flags |= FMSG_KEEPBACKGROUND;
	if (LeftAlign) flags |= FMSG_LEFTALIGN;
	if (AllInOne)  flags |= FMSG_ALLINONE;

	return Info.Message(Info.ModuleNumber,
               flags,
               NULL,
               Items,
               ItemsNumber,
               ButtonsNumber);

}

long WINAPI Char2Ver(const char* ver){
    long v1, v2, v3=0, tp;
    char t[10];
    FSF.sscanf(ver, "%d.%d %s %d", &v1, &v2, &t, &v3);
    if (t[0]=='.')
        FSF.sscanf(ver, "%d.%d.%d", &v1, &v2, &v3);

    if      (lstrcmp(t, "prealpha")==0) tp = 1;
    else if (lstrcmp(t, "alpha")==0) tp = 2;
    else if (lstrcmp(t, "prebeta")==0) tp = 3;
    else if (lstrcmp(t, "beta")==0) tp = 4;
    else if (lstrcmp(t, "pregamma")==0) tp = 5;
    else if (lstrcmp(t, "gamma")==0) tp = 6;
    else if (lstrcmp(t, "RC")==0) tp = 7;
    else if (lstrcmp(t, "demo")==0) tp = 8;
    else if (lstrcmp(t, "prerelease")==0) tp = 9;
    else if (lstrcmp(t, "release")==0) tp = 10;
    else if (lstrcmp(t, "build")==0) tp = 11;
    else tp = 0;
    return (v1 << 24)|(v2 << 16)|(tp << 8)|v3;
}

BOOL WINAPI Ver2Char(const long ver, char* v){
    char t[10];

    int v1 = (ver & 0xFF000000) >> 24;  
    int v2 = (ver & 0x00FF0000) >> 16; 
    int v3 = (ver & 0x0000FF00) >> 8;     
    int v4 = (ver & 0x000000FF);     
    switch(v3){
    case 1  : lstrcpy(t, "prealpha");
        break;
    case 2  : lstrcpy(t, "alpha");
        break;
    case 3  : lstrcpy(t, "prebeta");
        break;
    case 4  : lstrcpy(t, "beta");
        break;
    case 5  : lstrcpy(t, "pregamma");
        break;
    case 6  : lstrcpy(t, "gamma");
        break;
    case 7  : lstrcpy(t, "RC");
        break;
    case 8  : lstrcpy(t, "demo");
        break;
    case 9  : lstrcpy(t, "prerelease");
        break;
    case 0xA: lstrcpy(t, "release");
        break;
    case 0xB: lstrcpy(t, "build");
        break;
    default : lstrcpy(t, "");
    }

    if (lstrlen(t)!=0) FSF.sprintf(v, "%d.%d %s %d", v1, v2, t, v4);
    else               FSF.sprintf(v, "%d.%d.%d", v1, v2, v4); 
    
    return NULL;
}

BOOL WINAPI CheckForKey(const int key)
{
//  return CheckForKey(VK_ESCAPE);
  int ExitCode=FALSE;
  while (1)
  {
    INPUT_RECORD rec;
    HANDLE hConInp=GetStdHandle(STD_INPUT_HANDLE);
    DWORD ReadCount;
    PeekConsoleInput(hConInp,&rec,1,&ReadCount);
    if (ReadCount==0)
      break;
    ReadConsoleInput(hConInp,&rec,1,&ReadCount);
    if (rec.EventType==KEY_EVENT)
      if (rec.Event.KeyEvent.wVirtualKeyCode==key &&
          rec.Event.KeyEvent.bKeyDown)
        ExitCode=TRUE;
  }
  return ExitCode;
}