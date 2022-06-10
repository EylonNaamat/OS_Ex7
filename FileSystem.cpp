//
// Created by eylon on 6/8/22.
//
#include "myfs.h"

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
        inodes[i].type = -1;
        inodes[i].next = -1;
    }

    strcpy(inodes[0].name, "/");
    inodes[0].first_block = 0;
    inodes[0].type = 1;
    inodes[0].next = -1;
    inodes[0].size = 0;

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

int mymount2(const char* source, const char* target, const char* filesystemtype, unsigned long mountflags, const void* data){
    FILE* file_new;
    file_new = fopen(source, "w");
    if(fwrite(superb, sizeof(super_block), 1, file_new) < 1){
        return -1;
    }
    if(fwrite(inodes, sizeof(inode), superb->inodes_num, file_new) < 1){
        return -2;
    }
    if(fwrite(blocks, sizeof(block), superb->blocks_num, file_new) < 1){
        return -3;
    }
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
        if(inodes[i].first_block == -1 && inodes[i].type != 1){

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
        open_files[i].flag = -1;
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
            if(inodes[i].type == 1){
                return -1;
            }
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
        inodes[inode_index].type = 0;
        if(inode_index > 0){
            inodes[inode_index-1].next = inode_index;
        }

        blocks[block_index].in_use = 1;
        blocks[block_index].next_block = -1;
        strcpy(blocks[block_index].data, "");

        new_file.curr_inode = inodes[inode_index];
        new_file.flag = flags;
        new_file.seek_offset = 0;
    }

    int i;
    for(i = 0; i < MAX_FILES; ++i){
        if(open_files[i].flag == -1){
            open_files[i].seek_offset = new_file.seek_offset;
            open_files[i].flag = new_file.flag;
            open_files[i].curr_inode.first_block = new_file.curr_inode.first_block;
            open_files[i].curr_inode.size = new_file.curr_inode.size;
            strcpy(open_files[i].curr_inode.name, new_file.curr_inode.name);
            open_files[i].curr_inode.type = new_file.curr_inode.type;
            open_files[i].curr_inode.next = new_file.curr_inode.next;
            break;
        }
    }
    if(i == MAX_FILES){
        return -3;
    }
    return i;
}

int myclose(int myfd){
    if(myfd > MAX_FILES || myfd < 0){
        return -1;
    }
    if(open_files[myfd].curr_inode.type == 1){
        return -2;
    }
    open_files[myfd].flag = -1;
    return 0;
}


int find_block_seek(int myfd){
    int curr_block = open_files[myfd].seek_offset / 512;
    int first_block = open_files[myfd].curr_inode.first_block;

    for(int i = 0; i < curr_block; ++i) {
        first_block = blocks[first_block].next_block;
        if(first_block == -1){
            return -1;
        }
    }

    return first_block;
}

ssize_t mywrite(int myfd, const void* buf, size_t count){
    if(count < 1){
        return -1;
    }
    if(open_files[myfd].flag == -1){
        return -2;
    }
    if(open_files[myfd].flag != 1 && open_files[myfd].flag != 2){
        return -3;
    }

    int curr_block = find_block_seek(myfd);
    if(curr_block == -1){
        return -4;
    }
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
                return -5;
            }
            blocks[new_block].in_use = 1;
            blocks[curr_block].next_block = new_block;
            open_files[myfd].curr_inode.size += 1;
            curr_seek = 0;
            curr_block = new_block;
        }
    }
    open_files[myfd].seek_offset += num_bytes_written;
    return num_bytes_written;
}


ssize_t myread(int myfd, void* buf, size_t count){
    if(count < 1){
        return -1;
    }
    if(open_files[myfd].flag == -1){
        return -2;
    }
    if(open_files[myfd].flag != 0 && open_files[myfd].flag != 2){
        return -3;
    }

    int curr_block = find_block_seek(myfd);
    if(curr_block == -1){
        return -4;
    }
    int curr_seek = open_files[myfd].seek_offset;
    int how_much_to_read = count;
    int num_bytes_read = 0;

    while(curr_seek > 512){
        curr_seek -= 512;
    }
    int j = 0;

    while(how_much_to_read > 0){
        int i;
        for(i = curr_seek; i < 512; ++i){
            ((char*)buf)[j] = blocks[curr_block].data[i];
            j++;
            how_much_to_read--;
            num_bytes_read++;
            if(how_much_to_read == 0){
                break;
            }
        }
        if(how_much_to_read == 0){
            break;
        }
        if(i == 512 && blocks[curr_block].next_block == -1){
            break;
        }else{
            curr_block = blocks[curr_block].next_block;
            curr_seek = 0;
        }
    }
    open_files[myfd].seek_offset += num_bytes_read;
    return num_bytes_read;
}

off_t mylseek(int myfd, off_t offset, int whence){
    if(open_files[myfd].flag == -1){
        return -1;
    }
    if(whence == 0){
        open_files[myfd].seek_offset = offset;
        return offset;
    }
    if(whence == 1){
        open_files[myfd].seek_offset += offset;
        return open_files[myfd].seek_offset;
    }
    if(whence == 2){
        int curr_block = find_block_seek(myfd);
        int count = 0;
        if(curr_block == -1){
            return -1;
        }
        while(curr_block != -1){
            curr_block = blocks[curr_block].next_block;
            count++;
        }
        open_files[myfd].seek_offset = count*512;
        open_files[myfd].seek_offset += offset;
        return open_files[myfd].seek_offset;
    }
    return -2;
}


myDIR *myopendir(const char* name){
    for(int i = 0; i < superb->inodes_num; ++i){
        if(!strcmp(inodes[i].name, name)){
            if(inodes[i].type == 1){
                myDIR * ans = (myDIR*)(malloc(sizeof(myDIR)));
                ans->inode_num = i;
                ans->inode_next = i;
                strcpy(ans->name, name);
                for(int j = 0; j < MAX_FILES; ++j){
                    if(open_files[j].flag == -1){
                        open_files[j].flag = 0;
                        open_files[j].curr_inode.first_block = inodes[ans->inode_num].first_block;
                        open_files[j].curr_inode.size = inodes[ans->inode_num].size;
                        strcpy(open_files[j].curr_inode.name, inodes[ans->inode_num].name);
                        open_files[j].curr_inode.type = inodes[ans->inode_num].type;
                        open_files[j].curr_inode.next = inodes[ans->inode_num].next;
                        ans->fd = j;
                        break;
                    }
                }
                return ans;
            }
        }
    }
    return NULL;
}

struct mydirent *myreaddir(myDIR *dirp){
    if(open_files[dirp->fd].flag == -1){
        return NULL;
    }

    dirp->inode_next = inodes[dirp->inode_next].next;

    if(dirp->inode_next == -1){
        return NULL;
    }

    struct mydirent* ans = (struct mydirent*)(malloc(sizeof(struct mydirent)));
    strcpy(ans->d_name, inodes[dirp->inode_next].name);
    ans->inode_num = dirp->inode_num;
    return ans;
}

int myclosedir(myDIR * dirp){
    if(dirp->fd < 0 || dirp->fd > MAX_FILES){
        return -1;
    }
    if(open_files[dirp->fd].flag == -1){
        return -2;
    }
    open_files[dirp->fd].flag = -1;
    return 0;
}



int main(){
    init_open_files();
    mymkfs(4096);
    assert(!strcmp("/", inodes[0].name));
    mymount2("test", "/", "naamat", 1, NULL);

    // testing myopen, myread, mywrite, mylseek, myclose
    int fd1 = myopen("eylon", 0); // open for read
    assert(!strcmp(open_files[fd1].curr_inode.name, "eylon"));
    assert(open_files[fd1].curr_inode.type == 0);
    int fd2 = myopen("eylon", 1); // open for write
    assert(!strcmp(open_files[fd2].curr_inode.name, "eylon"));
    assert(open_files[fd2].curr_inode.type == 0);

    int bytes1 = mywrite(fd1, "naamat", 6); // should be an error - fd1 is open to read
    assert(bytes1 == -3);

    char buf[256];
    int bytes2 = myread(fd2, buf, 256); // should be an error - fd2 is open to write
    assert(bytes2 == -3);

    bytes2 = mywrite(fd2, "michael", 7);
    assert(bytes2 == 7);

    bytes1 = myread(fd1, buf, 7);
    assert(bytes1 == 7);
    assert(!strcmp("michael", buf));

    int fd3 = myopen("eylon", 2); // read and write
    assert(!strcmp(open_files[fd3].curr_inode.name, "eylon"));
    bytes1 = mywrite(fd3, "ao", 2);
    assert(bytes1 == 2);
    char buf2[256];
    mylseek(fd3, 0, 0);
    bytes2 = myread(fd3, buf2, 7);
    assert(bytes2 == 7);

    assert(!strcmp("aochael", buf2));

    mylseek(fd2, 0, 0);
    bytes1 = myclose(fd2);
    bytes1 = mywrite(fd2, "os_course", 9);
    assert(bytes1 == -2);
    mylseek(fd3, 0, 0);
    myread(fd3, buf2, 9);
    assert(!strcmp(buf2, "aochael"));

    bytes2 = mylseek(fd3, -9, 1);
    char buf3[256];
    myread(fd3, buf3, 9);
    assert(!strcmp(buf3, "aochael"));

    bytes1 = mylseek(fd3, -1, 2);
    bytes1 = mywrite(fd3, "hello", 5);
    bytes1 = mylseek(fd3, -1,2);

    char buf4[256];
    myread(fd3, buf4, 5);
    assert(!strcmp(buf4, "hello"));
    myclose(fd1);
    myclose(fd3);

    // testing the dir function - myopendir, myreaddir, myclosedir

    int f1 = myopen("netzer", 0);
    int f2 =myopen("mordechai", 1);
    int f3 =myopen("noam", 2);
    int f4 =myopen("OS", 0);
    int f5 =myopen("ex7", 1);

    myclose(f1);
    myclose(f2);
    myclose(f3);
    myclose(f4);
    myclose(f5);


    myDIR * dir = myopendir("/");
    struct mydirent* sd;
    int counter = 0;
    sd = myreaddir(dir);
    assert(!strcmp(sd->d_name, "eylon"));

    sd = myreaddir(dir);
    assert(!strcmp(sd->d_name, "netzer"));

    sd = myreaddir(dir);
    assert(!strcmp(sd->d_name, "mordechai"));

    sd = myreaddir(dir);
    assert(!strcmp(sd->d_name, "noam"));

    sd = myreaddir(dir);
    assert(!strcmp(sd->d_name, "OS"));

    sd = myreaddir(dir);
    assert(!strcmp(sd->d_name, "ex7"));

    sd = myreaddir(dir);
    assert(sd == NULL);



    myclosedir(dir);

    for(int i = 0; i < MAX_FILES; ++i){
        assert(open_files[i].flag == -1);
    }
}

