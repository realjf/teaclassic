
#include "public/tc_string.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

/*****************************************************************************/
/* EXTERN FUNCTIONS                                                          */
/*****************************************************************************/

char *tc_strtok_r(char *str, const char *delim, char **saveptr)
{
    if(str == NULL)
        str = *saveptr;

    if(*str == '\0') {
        *saveptr = str;
        return NULL;
    }

    str += strspn(str, delim);
    if(*str == '\0') {
        *saveptr = str;
        return NULL;
    }

    char *end = str + strcspn(str, delim);
    if(*end == '\0') {
        *saveptr = end;
        return str;
    }

    *end = '\0';
    *saveptr = end + 1;
    return str;
}

char *tc_strdup(const char *str)
{
    char *ret = malloc(strlen(str) + 1);
    if(ret)
        strcpy(ret, str);
    return ret;
}

char *tc_strapp(char *str, const char *append)
{
    size_t len = strlen(str) + strlen(append) + 1;
    char *ret = realloc((void*)str, len);
    if(!ret)
        return NULL;
    strcat(ret, append);
    assert(ret[len-1] == '\0');
    return ret;
}

size_t tc_strlcpy(char *dest, const char *src, size_t size)
{
    if(!size)
        return 0;

    size_t srclen = strlen(src);
    size_t ret = (srclen > size-1) ? size-1 : srclen;
    memcpy(dest, src, ret);
    dest[ret] = '\0';
    return ret;
}

int tc_snprintf(char *str, size_t size, const char *format, ...)
{
    int ret;
    va_list args;

    va_start(args, format);
    ret = vsnprintf(str, size, format, args);
    va_end(args);

    str[size-1] = '\0';
    return ret;
}

int tc_endswith(const char *str, const char *end)
{
    size_t slen = strlen(str);
    size_t elen = strlen(end);

    if(elen > slen)
        return 0;

    return (0 == strcmp(str + (slen - elen), end));
}

char *tc_strlcat(char *dest, const char *src, size_t size)
{
    size_t srclen = strlen(src);
    size_t dstlen = strlen(dest);
    if(size < 2)
        return dest;
    if(dstlen >= size-1)
        return dest;
    size_t left = size - 1 - dstlen;
    size_t ncpy = left > srclen ? srclen : left;
    memcpy(dest + dstlen, src, ncpy);
    dest[dstlen + ncpy] = '\0';
    return dest;
}


