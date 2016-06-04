/*
Eugene Kotlyarov <ek@oris.ru>
*/

#define heapNEW GetProcessHeap()

extern "C" void *          __cdecl _alloca(size_t);
#define alloca  _alloca
#pragma intrinsic(_alloca)
#ifdef _DEBUG
typedef struct {
    DWORD address;
    DWORD size;
    char  file[64];
    DWORD line;
} ALLOC_INFO;

typedef vector<ALLOC_INFO*> AllocList;

AllocList allocList;

void AddTrack(DWORD addr,  DWORD asize,  const char *fname, DWORD lnum)
{
    ALLOC_INFO *info;    

    info = (ALLOC_INFO*)HeapAlloc(heapNEW, HEAP_ZERO_MEMORY, sizeof(ALLOC_INFO));;
    info->address = addr;
    strncpy(info->file, fname, 63);
    info->line = lnum;
    info->size = asize;
    allocList.push_back(info);
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

    if(!(allocList.size()>0)){
        OutputDebugString("All freed\n");
        return;
    }

    for(i = allocList.begin(); i != allocList.end(); i++) {
        sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS %d\t%d unfreed\n",
        (*i)->file, (*i)->line, (*i)->address, (*i)->size);
        OutputDebugString(buf);
        totalSize += (*i)->size;
    }
    sprintf(buf, "--------------------------------------------------\n");
    OutputDebugString(buf);
    sprintf(buf, "Total Unfreed: %d bytes\n", totalSize);
    OutputDebugString(buf);
};
#endif

#ifdef _DEBUG
void * __cdecl my_malloc(size_t size, const char *file, int line)
{
      void *ptr = HeapAlloc(heapNEW, HEAP_ZERO_MEMORY, size);
      AddTrack((DWORD)ptr, size, file, line);
      return ptr;
};

void __cdecl my_free(void *block)
{
    RemoveTrack((DWORD)block);
    HeapFree(heapNEW,0,block);
};
#else
void * __cdecl my_malloc(size_t size)
{
//  return malloc(size);  
    return HeapAlloc(heapNEW, HEAP_ZERO_MEMORY, size);
}

void * __cdecl my_realloc(void *block, size_t size)
{
  if (block)
//    return realloc(block, size);
    return HeapReAlloc(heapNEW,HEAP_ZERO_MEMORY,block,size);
  else
//    return malloc(size);
    return HeapAlloc(heapNEW,HEAP_ZERO_MEMORY, size);
}

void __cdecl my_free(void *block)
{
//  free(block);
  HeapFree(heapNEW,0,block);
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
    strcpy(ret, Str);
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
