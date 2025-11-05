#ifndef STREXT_H
#define STREXT_H

#ifdef _MSC_VER

#include <stdlib.h>
#include <string.h>

#ifdef __cpluscplus
extern "C" {
#endif

int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
// char *strdup(const char *s);
char *strndup(const char *s, size_t n);

#ifdef __cpluscplus
}
#endif

#else

#include <strings.h>

#endif // _MSVC

#endif // STREXT_H