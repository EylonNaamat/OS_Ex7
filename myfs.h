//
// Created by eylon on 6/10/22.
//
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_FILES 10000
#define O_CREAT 0100
#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR 02

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// structs

typedef struct _SuperBlock{
    int blocks_num;
    int inodes_num;
    int blocks_size;
}super_block, *super_block_point;

typedef struct _Inode{
    char name[16];
    int size;
    int first_block;
    int type;
    int next;
}inode, *inode_point;

typedef struct _Block{
    int next_block;
    char data[512];
    int in_use; // 0 means not in use, 1 is in use
}block, *block_point;


// 0 is read only, 1 is write only, 2 is read and write mode
struct myopenfile{
    inode curr_inode;
    int seek_offset;
    int flag;
};

struct mydirent{
    int inode_num;
    char d_name[16];
};

typedef struct myDIR{
    int inode_num;
    int inode_next;
    char name[16];
    int fd;
}myDIR;


// functions
void mymkfs(size_t s);
int mymount(const char* source, const char* target, const char* filesystemtype, unsigned long mountflags, const void* data);
int mymount2(const char* source, const char* target, const char* filesystemtype, unsigned long mountflags, const void* data);

int load_mount(const char* source);
int find_inode();
int find_block();
void init_open_files();

int myopen(const char *pathname, int flags);
int myclose(int myfd);

int find_block_seek(int myfd);

ssize_t mywrite(int myfd, const void* buf, size_t count);
ssize_t myread(int myfd, void* buf, size_t count);
off_t mylseek(int myfd, off_t offset, int whence);

myDIR *myopendir(const char* name);
struct mydirent *myreaddir(myDIR *dirp);
int myclosedir(myDIR * dirp);

char* get_inode_name(int index);
char* get_name(int fd);
int get_type(int fd);
int get_flag(int fd);
int get_seek(int fd);
int get_length(int block_num);
int get_block_num(int fd);
void drain_data(int block_num);

int length_to_read(int fd);

int read_until(int fd, char* buf);


//// global variables
//super_block_point superb;
//inode_point inodes;
//block_point blocks;
//struct myopenfile open_files[MAX_FILES];



