/*
Eugene Kotlyarov <ek@oris.ru>
*/

#define heapNEW GetProcessHeap()

//extern "C" void *          __cdecl _alloca(size_t);
//#define alloca  _alloca
//#pragma intrinsic(_alloca)
#ifdef _DEBUG
typedef struct {
    DWORD address;
    DWORD size;
    char  file[64];
    char  buf[16];
    DWORD line;
} ALLOC_INFO;

typedef list<ALLOC_INFO*> AllocList;

AllocList allocList;

void AddTrack(DWORD addr,  DWORD asize,  const char *fname, DWORD lnum)
{
    ALLOC_INFO *info;    

    //info = (ALLOC_INFO*)malloc(sizeof(ALLOC_INFO));
    info = (ALLOC_INFO*)HeapAlloc(heapNEW, HEAP_ZERO_MEMORY, sizeof(ALLOC_INFO));
    info->address = addr;
    strncpy(info->file, fname, 63);
    strncpy(info->buf, (char*)addr, 15);
    info->buf[16] = 0;
    info->line = lnum;
    info->size = asize;
    allocList.insert(allocList.begin(), info);
};

void RemoveTrack(DWORD addr)
{
    AllocList::iterator i;

    if(!(allocList.size()>0))
        return;
    for(i = allocList.begin(); i != allocList.end(); i++)
    {
        if((*i)->address == addr)
        {
            //free(*i);
            HeapFree(heapNEW,0,*i);
            allocList.erase(i);
            break;
        }
    }
};

void DumpUnfreed()
{
    AllocList::iterator i;
    DWORD totalSize = 0;
    char buf[1024];    
    
    for(i = allocList.begin(); i != allocList.end(); i++) {
        sprintf(buf, "%s:\tLINE %d,\tADDRESS %d\t%d unfreed %s\n",
        (*i)->file, (*i)->line, (*i)->address, (*i)->size, (*i)->buf);
        OutputDebugString(buf);

        SaveToLog("DumpUnfreed", buf, lstrlen(buf)-lstrlen("\n"), FALSE, FALSE, FALSE);

        totalSize += (*i)->size;
    }
    sprintf(buf, "--------------------------------------------------\n");
    OutputDebugString(buf);
    sprintf(buf, "Total Unfreed: %d bytes\n", totalSize);
    OutputDebugString(buf);

    SaveToLog("DumpUnfreed", buf, lstrlen(buf)-lstrlen("\n"), TRUE, FALSE, FALSE);
};
#endif

#ifdef _DEBUG
void * __cdecl my_malloc(size_t size, const char *file, int line)
{
    //char buf[128];
    //void *ptr = malloc(size);
    void *ptr = HeapAlloc(heapNEW, HEAP_ZERO_MEMORY, size);
    AddTrack((DWORD)ptr, size, file, line);
    //sprintf(buf, "add %x, size %d, buf: %-15s; file: %s, line: %d", ptr, size, (char*)&ptr, file, line);
    //SaveToLog("my_malloc", buf, lstrlen(buf), TRUE, FALSE, FALSE);
    return ptr;
};

void WINAPI my_free(void *block)
{
    //char buf[128];
    RemoveTrack((DWORD)block);
    //free(block);
    HeapFree(heapNEW,0,block);
    //block = NULL;
    //sprintf(buf, "add %x, buf: %-15s", block, (char*)&block);
    //SaveToLog("my_free", buf, lstrlen(buf), TRUE, FALSE, FALSE);
};

void * __cdecl my_realloc(void *block, size_t size)
{
  if (block)
//    return realloc(block, size);
    return HeapReAlloc(heapNEW,HEAP_ZERO_MEMORY,block,size);
  else
    //return malloc(size);
    return HeapAlloc(heapNEW,HEAP_ZERO_MEMORY, size);
}

#else
void * __cdecl my_malloc(size_t size)
{
  //return malloc(size);  
  return HeapAlloc(heapNEW, HEAP_ZERO_MEMORY, size);
}

void * __cdecl my_realloc(void *block, size_t size)
{
  if (block)
//    return realloc(block, size);
    return HeapReAlloc(heapNEW,HEAP_ZERO_MEMORY,block,size);
  else
    //return malloc(size);
    return HeapAlloc(heapNEW,HEAP_ZERO_MEMORY, size);
}

void WINAPI my_free(void *block)
{
//  free(block);
  HeapFree(heapNEW,0,block);
  //block = NULL;
}

#endif

#ifdef _DEBUG
#define DEBUG_NEW(size) my_malloc(size, __FILE__, __LINE__)
#else
#define DEBUG_NEW my_malloc
#endif
#define my_malloc(size) DEBUG_NEW(size)

char * my_strdup(const char *Str, int AddBytes = 0)
{
    char * ret;
    ret = (char*)my_malloc(strlen(Str)+AddBytes+1);
    lstrcpy(ret, Str);
    return ret;
}

char * my_substr(const char *Str, int FromPos, int ToPos, int AddBytes = 0)
{
    char * ret;
    if (ToPos <= FromPos) return my_strdup("");
    ret = (char*)my_malloc(ToPos-FromPos+AddBytes+1);
    strncpy(ret, Str+FromPos, ToPos-FromPos);
    ret[ToPos-FromPos] = 0;
    return ret;
}


/*#ifdef __cplusplus
void * __cdecl operator new(size_t size)
{
  return my_malloc(size);
}

void __cdecl operator delete(void *block)
{
  my_free(block);
}
#endif*/

void _pure_error_ () {};
