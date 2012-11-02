#include "stdafx.h"
#include "myrtl.h"

#ifdef __cplusplus
namespace crt {
extern "C" {
#endif /* __cplusplus */
void *my_malloc(size_t size)
{
  return HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY, size);
}

void *my_realloc(void *block, size_t size)
{
  if (block)
    return HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,block,size);
  else
    return HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY, size);
}

void my_free(void *block)
{
  HeapFree(GetProcessHeap(),0,block);
}

void *my_memcpy(void *dest, const void *src, size_t n)
{
  size_t i;
  for (i=0;i<n;i++) ((unsigned char *)dest)[i]=((unsigned char *)src)[i];
  return dest;
}
char *my_strdup(const char *src)
{
	int size = lstrlenA(src)+1;
	char * str = (char*)my_malloc(size);
	if (str)
		my_memcpy(str, src, size);
	return str;
}
wchar_t *my_strdupW(const wchar_t *src)
{
	int size = (lstrlen(src)+1)*sizeof(wchar_t);
	wchar_t * str = (wchar_t*)my_malloc(size);
	if (str)
		my_memcpy(str, src, size);
	return str;
}
void *my_memset(void *s, int c, size_t n)
{
  size_t i;
  for (i=0;i<n;i++) ((char *)s)[i]=c;
  return s;
}

int my_strncmp(const char *s1, const char *s2, size_t  maxlen)
{
 if (!maxlen)
   return(0);
 while (--maxlen && *s1 && *s1 == *s2)
 {
    s1++;
    s2++;
  }
  return(*s1 - *s2);
}


const char *my_strchr(const char *s, int c)
{
  while (*s) {
    if (*s==c)
      return s;
    s++;
  }
  return 0;
}

const char *my_strrchr(const char *s, int c)
{
  register const char *ss;
  register size_t i;

  for(i = lstrlenA( s ) + 1, ss = s+i; i; i--)
  {
    if( *(--ss) == (char)c )  return( (char *)ss );
  }
  return( 0 );
}

//void _pure_error_ () {}; // for BC
//int _purecall(void){ return 0; }; // for MS VC

#ifdef __cplusplus
}
}
#endif /* __cplusplus */
