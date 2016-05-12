/*
 * utils.c
 *
 *  Created on: 12.05.2016
 *      Author: wehmeier
 */

#include <stdlib.h>
#include <stdio.h>
int antoi(const char* str, int len)
{
    int i;
    int ret = 0;
    for(i = 0; i < len; ++i)
    {
        ret = ret * 10 + (str[i] - '0');
    }
    return ret;
}
char *strrchr(const char *s, int c)
{
    char* ret=0;
    do {
        if( *s == (char)c )
            ret=s;
    } while(*s++);
    return ret;
}
