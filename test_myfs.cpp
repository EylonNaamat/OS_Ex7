//
// Created by eylon on 6/12/22.
//
#include "myfs.h"

int main(){
    init_open_files();
    mymkfs(4096);
    assert(!strcmp("/", get_inode_name(0)));
    mymount2("test", "/", "naamat", 1, NULL);

    // testing myopen, myread, mywrite, mylseek, myclose
    int fd1 = myopen("eylon", O_RDONLY | O_CREAT); // open for read
    assert(!strcmp(get_name(fd1), "eylon"));
    assert(get_type(fd1) == 0);
    int fd2 = myopen("eylon", O_WRONLY); // open for write
    assert(!strcmp(get_name(fd2), "eylon"));
    assert(get_type(fd2) == 0);

    int bytes1 = mywrite(fd1, "naamat", 6); // should be an error - fd1 is open to read

    assert(bytes1 == -3);

    char buf[256];
    int bytes2 = myread(fd2, buf, 13); // should be an error - fd2 is open to write
    assert(bytes2 == -3);

    bytes2 = mywrite(fd2, "michael", 7);
    assert(bytes2 == 7);

    bytes1 = myread(fd1, buf, 7);
    assert(bytes1 == 7);
    assert(!strcmp("michael", buf));

    int fd3 = myopen("eylon", O_RDWR); // read and write
    assert(!strcmp(get_name(fd3), "eylon"));
    bytes1 = mywrite(fd3, "ao", 2);
    assert(bytes1 == 2);
    char buf2[256];
    mylseek(fd3, 0, SEEK_SET);
    bytes2 = myread(fd3, buf2, 7);
    assert(bytes2 == 7);

    assert(!strcmp("aochael", buf2));

    mylseek(fd2, 0, SEEK_SET);
    bytes1 = myclose(fd2);
    bytes1 = mywrite(fd2, "os_course", 9);
    assert(bytes1 == -2);
    mylseek(fd3, 0, SEEK_SET);
    myread(fd3, buf2, 9);
    assert(!strcmp(buf2, "aochael"));

    bytes2 = mylseek(fd3, -9, 1);
    char buf3[256];
    myread(fd3, buf3, 9);
    assert(!strcmp(buf3, "aochael"));

    bytes1 = mylseek(fd3, -1, SEEK_END);
    bytes1 = mywrite(fd3, "hello", 5);
    bytes1 = mylseek(fd3, -1,SEEK_END);

    char buf4[256];
    myread(fd3, buf4, 5);
    assert(!strcmp(buf4, "hello"));
    myclose(fd1);
    myclose(fd3);

    // testing the dir function - myopendir, myreaddir, myclosedir

    int f1 = myopen("netzer", O_CREAT | O_RDONLY);
    int f2 =myopen("mordechai", O_WRONLY | O_CREAT);
    int f3 =myopen("noam", O_RDWR | O_CREAT);
    int f4 =myopen("OS", O_RDONLY | O_CREAT);
    int f5 =myopen("ex7", O_WRONLY | O_CREAT);

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
        assert(get_flag(i) == -1);
    }

}