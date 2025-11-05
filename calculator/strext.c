#ifdef _MSC_VER

#include "strext.h"
#include <ctype.h> // 用于 tolower
#include <stdio.h>
#include <string.h>

int strcasecmp(const char *s1, const char *s2)
{
    size_t n1 = strlen(s1);
    size_t n2 = strlen(s2);
    size_t n = n1 < n2 ? n1 : n2;
    return strncasecmp(s1, s2, n);
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    size_t i = 0;
    while (i < n && s1[i] && s2[i]) {
        int c1 = tolower((unsigned char)s1[i]);
        int c2 = tolower((unsigned char)s2[i]);
        if (c1 < c2)
            return -1;
        if (c1 > c2)
            return 1;
        i++;
    }
    return (s1[i] - s2[i]); // 处理长度差异
}

char *strndup(const char *s, size_t n) {
    size_t len = strnlen(s, n);
    char *new_str = (char *)malloc(len + 1);
    if (new_str == NULL) {
        return NULL;
    }
    memcpy(new_str, s, len);
    new_str[len] = '\0';
    return new_str;
}

// char* strdup(const char* s) {
    // return strndup(s, strlen(s));
// }

#endif // _MSVC