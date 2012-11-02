#ifndef __CRTDLL_H
#define __CRTDLL_H

#ifdef __cplusplus
extern "C" {
#endif
/* stdlib.h */ 
void *my_malloc(size_t size);
void *my_realloc(void *block, size_t size);
void my_free(void *block);
/* mem.h */
void *my_memcpy(void *dest, const void *src, size_t n);
char *my_strdup(const char *src);
wchar_t *my_strdupW(const wchar_t *src);
void *my_memset(void *s, int c, size_t n);
/* string.h */
int my_strncmp(const char *s1, const char *s2, size_t  maxlen);
const char *my_strchr(const char *s, int c);
const char *my_strrchr(const char *s, int c);

#define my_strdupF my_strdupW

#ifdef __cplusplus
}
#endif
#endif //__CRTDLL_H
