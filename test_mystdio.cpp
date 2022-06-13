//
// Created by eylon on 6/13/22.
//
#include "mystdio.h"

int main(){
    init_open_files();
    mymkfs(4096);

    // test myfopen myfclose
    myFILE* file1 = myfopen("eylon", "r"); // fd should be -4 because file dont exist
    assert(file1->fd == -4);

    file1 = myfopen("eylon", "w"); // fd should be 0 or more because file should be created
    assert(file1->fd >= 0);

    myFILE* file2 = myfopen("michael", "a"); // file should be created
    assert(file2->fd >= 0);

    int a = myfclose(file1); // a should be 0
    assert(a == 0);
    assert(get_flag(file1->fd) == -1);

    a = myfclose(file2);
    assert(a == 0);
    assert(get_flag(file2->fd) == -1);

    // testing myfwrite, myfread, myfseek
    file1 = myfopen("eylon", "r+"); // writing naamat into eylon file
    char buf[256] = "naamat";
    a = myfwrite(buf, 1, 6, file1);
    assert(a == 6);
    strcpy(buf, "");
    assert(!strcmp(buf, ""));
    myfseek(file1, 0, SEEK_SET);
    a = myfread(buf, 1, 6, file1);
    assert(!strcmp(buf, "naamat"));

    strcpy(buf, "matveev");
    myfwrite(buf, 1, 7, file1);
    strcpy(buf, "");
    assert(!strcmp(buf, ""));
    myfseek(file1, -7, SEEK_CUR);
    myfread(buf, 1, 7, file1);
    assert(!strcmp(buf, "matveev"));

    file2 = myfopen("eylon", "a");
    assert(get_seek(file2->fd) == 13);

    strcpy(buf, "netzer");
    myfwrite(buf, 1, 6, file2);
    strcpy(buf, "");
    assert(!strcmp(buf, ""));

    myfclose(file1);
    file1 = myfopen("eylon", "r");
    myfread(buf, 1, 19, file1);
    assert(!strcmp(buf, "naamatmatveevnetzer"));

    myfseek(file2, -5, SEEK_CUR);
    myfseek(file1, 0, SEEK_SET);
    strcpy(buf, "mordi");
    myfwrite(buf, 1, 5, file2);
    strcpy(buf, "");
    assert(!strcmp(buf, ""));
    myfread(buf, 1, 19, file1);
    assert(!strcmp(buf, "naamatmatveevnmordi"));

    myfclose(file1);
    myfclose(file2);


    //testing myfprintf, myfscanf
    file1 = myfopen("test_mystdio", "w"); // should create this file
    assert(file1->fd >= 0);

    int count = myfprintf(file1, "%d %c",1, 'e'); // the length of the string we print to file1 is 3


    file2 = myfopen("test_mystdio", "r");
    int d;
    float b;
    char c;

    myfscanf(file2, "%d %c", &d, &c);
    assert(d == 1);
    assert(c == 'e');


    myfprintf(file1, "%d %c",12, 'n');

    myfscanf(file2, "%d %c", &d, &c);

    assert(d == 12);
    assert(c == 'n');

    myfclose(file1);
    myfclose(file2);

    file1 = myfopen("amit", "w");
    myfprintf(file1, "%f", 2.5);

    file2 = myfopen("amit", "r");

    float x;

    myfscanf(file2, "%f", &x);

    assert(x == 2.500000);

    myfclose(file1);
    myfclose(file2);

}