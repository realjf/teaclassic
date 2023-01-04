#ifndef _TC_STRING_H_
#define _TC_STRING_H_

#include <stddef.h>

char *tc_strtok_r(char *str, const char *delim, char **saveptr);
char *tc_strdup(const char *str);
char *tc_strapp(char *str, const char *append);
size_t tc_strlcpy(char *dest, const char *src, size_t size);
int tc_snprintf(char *str, size_t size, const char *format, ...);
int tc_endswith(const char *str, const char *end);
char *tc_strlcat(char *dest, const char *src, size_t size);

#endif /* _TC_STRING_H_ */
