//
// Created by eylon on 6/8/22.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_FILES 10000

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


// global variables
super_block_point superb;
inode_point inodes;
block_point blocks;
struct myopenfile open_files[MAX_FILES];



// functions
void mymkfs(size_t s){
    int super_block_size = sizeof(super_block);
    int block_bytes = (s - super_block_size - 0.1*s);

    superb = (super_block_point)(malloc(sizeof(super_block)));
    inodes = (inode_point)malloc(0.1*s);
    blocks = (block_point)(malloc(block_bytes));

    superb->inodes_num = (0.1*s) / sizeof(inode);
    superb->blocks_num = block_bytes / sizeof(block);
    superb->blocks_size = sizeof(block);


    for(int i = 0; i < superb->inodes_num; ++i){
        inodes[i].size = -1;
        strcpy(inodes[i].name, "");
        inodes[i].first_block = -1;
    }

    for(int i = 0; i < superb->blocks_num; ++i){
        blocks[i].next_block = -1;
        strcpy(blocks[i].data, "");
        blocks[i].in_use = 0;
    }

    FILE* file;
    file = fopen("filesystem", "w+");

    fwrite(superb, sizeof(super_block), 1, file);
    fwrite(inodes, sizeof(inode), superb->inodes_num, file);
    fwrite(blocks, sizeof(block), superb->blocks_num, file);

    fclose(file);
}

int mymount(const char* source, const char* target, const char* filesystemtype, unsigned long mountflags, const void* data){
    FILE* file_from;
    FILE* file_to;

    file_from = fopen(source, "r");
    file_to = fopen(target, "r");

    super_block_point superb_from;
    inode_point inodes_from;
    block_point blocks_from;

    super_block_point superb_to;
    inode_point inodes_to;
    block_point blocks_to;

    superb_from = (super_block_point)(malloc(sizeof(super_block)));
    fread(superb_from, sizeof(super_block), 1, file_from);

    inodes_from = (inode_point)malloc(sizeof(inode)*superb_from->inodes_num);
    blocks_from = (block_point)(malloc(sizeof(block)*superb_from->blocks_num));

    fread(inodes_from, sizeof(inode), superb_from->inodes_num, file_from);
    fread(blocks_from, sizeof(block), superb_from->blocks_num, file_from);

    superb_to = (super_block_point)(malloc(sizeof(super_block)));
    fread(superb_to, sizeof(super_block), 1, file_to);

    inodes_to = (inode_point)malloc(sizeof(inode)*superb_to->inodes_num);
    blocks_to = (block_point)(malloc(sizeof(block)*superb_to->blocks_num));

    fread(inodes_to, sizeof(inode), superb_to->inodes_num, file_to);
    fread(blocks_to, sizeof(block), superb_to->blocks_num, file_to);

    super_block_point superb_new;
    inode_point inodes_new;
    block_point blocks_new;

    superb_new = (super_block_point)(malloc(sizeof(super_block)));
    superb_new->blocks_num = superb_from->blocks_num + superb_to->blocks_num;
    superb_new->inodes_num = superb_from->inodes_num + superb_to->inodes_num;
    superb_new->blocks_size = superb_from->blocks_size + superb_to->blocks_size;

    int k = 0;
    for(int i = 0; i < superb_to->inodes_num; ++i){
        inodes_new[k].size = inodes_to[i].size;
        strcpy(inodes_new[k].name, inodes_to[i].name);
        inodes_new[k].first_block = inodes_to[i].first_block;
        ++k;
    }

    for(int i = 0; i < superb_from->inodes_num; ++i){
        blocks_new[k].next_block = blocks_from[i].next_block;
        strcpy(blocks_new[k].data, blocks_from[i].data);
        blocks_new[k].in_use = blocks_from[i].in_use;
        ++k;
    }

    fclose(file_to);
    fclose(file_from);

    FILE* file_new;
    file_new = fopen(target, "w");
    fwrite(superb_new, sizeof(super_block), 1, file_new);
    fwrite(inodes_new, sizeof(inode), superb_new->inodes_num, file_new);
    fwrite(blocks_new, sizeof(block), superb_new->blocks_num, file_new);

    fclose(file_new);
    return 0;
}


int load_mount(const char* source){
    FILE* file;
    file = fopen(source, "r");

    superb = (super_block_point)(malloc(sizeof(super_block)));
    fread(superb, sizeof(super_block), 1, file);

    inodes = (inode_point)(malloc(sizeof(inode)*superb->inodes_num));
    blocks = (block_point)(malloc(sizeof(block)*superb->blocks_num));

    fread(inodes, sizeof(inode), superb->inodes_num, file);
    fread(blocks, sizeof(block), superb->blocks_num, file);

    fclose(file);
    return 0;
}

int find_inode(){
    for(int i = 0; i < superb->inodes_num; ++i){
        if(inodes[i].first_block == -1){
            return i;
        }
    }
    return -1;
}

int find_block(){
    for(int i = 0; i < superb->blocks_num; ++i){
        if(blocks[i].in_use == 0){
            return i;
        }
    }
    return -1;
}

void init_open_files(){
    for(int i = 0; i < MAX_FILES; ++i){
        open_files[i].seek_offset = -1;
    }
}


int myopen(const char *pathname, int flags){
    int exist = 0;
    struct myopenfile new_file;

    if(flags != 0 && flags != 1 && flags != 2){
        return -1;
    }

    for(int i = 0; i < superb->inodes_num; ++i){
        if(!strcmp(inodes[i].name, pathname)){
            new_file.curr_inode = inodes[i];
            new_file.flag = flags;
            new_file.seek_offset = 0;
            exist = 1;
            break;
        }
    }
    if(!exist){
        int inode_index = find_inode();
        int block_index = find_block();

        if(inode_index == -1 || block_index == -1){
            return -2;
        }

        strcpy(inodes[inode_index].name, pathname);
        inodes[inode_index].first_block = block_index;
        inodes[inode_index].size = 1;

        blocks[block_index].in_use = 1;
        blocks[block_index].next_block = -1;
        strcpy(blocks[block_index].data, "");

        new_file.curr_inode = inodes[inode_index];
        new_file.flag = flags;
        new_file.seek_offset = 0;
    }

    int i;
    for(i = 0; i < MAX_FILES; ++i){
        if(open_files[i].seek_offset == -1){
            open_files[i].seek_offset = new_file.seek_offset;
            open_files[i].flag = new_file.flag;
            open_files[i].curr_inode.first_block = new_file.curr_inode.first_block;
            open_files[i].curr_inode.size = new_file.curr_inode.size;
            strcpy(open_files[i].curr_inode.name, new_file.curr_inode.name);
            break;
        }
    }
    if(i == MAX_FILES){
        return -3;
    }
    return i;
}

int find_block_seek(int myfd){
    int curr_block = open_files[myfd].seek_offset / 512;
    int first_block = open_files[myfd].curr_inode.first_block;

    for(int i = 0; i < curr_block; ++i) {
        first_block = blocks[first_block].next_block;
    }

    return first_block;
}

ssize_t mywrite(int myfd, const void* buf, size_t count){
    if(count < 1){
        return -1;
    }
    if(open_files[myfd].seek_offset == -1){
        return -2;
    }
    if(open_files[myfd].flag != 1 && open_files[myfd].flag != 2){
        return -3;
    }

    int curr_block = find_block_seek(myfd);
    int curr_seek = open_files[myfd].seek_offset;
    int how_much_to_write = count;
    int num_bytes_written = 0;
    while(curr_seek > 512){
        curr_seek -= 512;
    }
    int j = 0;
    while(how_much_to_write > 0){
        int i;
        for(i = curr_seek; i < 512; ++i){
            blocks[curr_block].data[i] = ((char*)buf)[j];
            j++;
            how_much_to_write--;
            num_bytes_written++;
            if(how_much_to_write == 0){
                break;
            }
        }
        if(i == 512 && blocks[curr_block].next_block == -1){
            int new_block = find_block();
            if(new_block == -1){
                return -4;
            }
            blocks[new_block].in_use = 1;
            blocks[curr_block].next_block = new_block;
            curr_seek = 0;
        }
    }
    open_files[myfd].seek_offset += count;
    return num_bytes_written;
}


//ssize_t myread(int myfd, void* buf, size_t count){
//    if(count < 1){
//        return -1;
//    }
//    if(open_files[myfd].seek_offset == -1){
//        return -2;
//    }
//    if(open_files[myfd].flag != 0 && open_files[myfd].flag != 2){
//        return -3;
//    }
//
//
//}





int main(){
    init_open_files();
    mymkfs(4096);
    printf("done\n");
    int fd = myopen("eylon", 1);
//    printf("%d\n", fd);
//    printf("%s\n", open_files[fd].curr_inode.name);
//    printf("%d\n", open_files[fd].curr_inode.first_block);
//    printf("%d\n", open_files[fd].curr_inode.size);
    int bytes = mywrite(fd, "eylon", 5);
    printf("1   %s\n", blocks[0].data);

    int bytes2 = mywrite(fd, "michael", 7);
    printf("2   %s\n", blocks[0].data);
    printf("%d\n", bytes2);


//    int fd2 = myopen("naamat", 0);
////    printf("%d\n", fd2);
//    printf("%s\n", open_files[fd2].curr_inode.name);
//    printf("%d\n", open_files[fd2].curr_inode.first_block);
//    printf("%d\n", open_files[fd2].curr_inode.size);
}

