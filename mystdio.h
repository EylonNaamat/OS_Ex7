//
// Created by eylon on 6/11/22.
//
#pragma once
#include "myfs.h"
#include <stdarg.h>
#define MAX_NUM 100

typedef struct myFILE{
    int inode_num;
    char *data;
    int fd;
}myFILE;


myFILE *myfopen(const char *pathname, const char *mode);
int myfclose(myFILE *stream);

size_t myfread(void *ptr, size_t size, size_t nmemb, myFILE *stream);
size_t myfwrite(void *ptr, size_t size, size_t nmemb, myFILE *stream);
int myfseek(myFILE *stream, long offset, int whence);

int myfscanf(myFILE *stream, const char *format, ...);
int myfprintf(myFILE *stream, const char *format, ...);


