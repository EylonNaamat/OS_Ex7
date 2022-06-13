//
// Created by eylon on 6/12/22.
//
#include "mystdio.h"


myFILE *myfopen(const char *pathname, const char *mode){
    myFILE *ans = (myFILE*)(malloc(sizeof(myFILE)));
    if(!strcmp(mode, "r")){
        int fd = myopen(pathname, O_RDONLY);
        ans->fd = fd;
    }
    if(!strcmp(mode, "r+")){
        int fd = myopen(pathname, O_RDWR);
        ans->fd = fd;
    }
    if(!strcmp(mode, "a")){
        int fd = myopen(pathname, O_WRONLY | O_CREAT);
        int block_num = get_block_num(fd);
        int len = get_length(block_num);
        mylseek(fd, len, SEEK_SET);
        ans->fd = fd;
    }
    if(!strcmp(mode, "w")){
        int fd = myopen(pathname, O_WRONLY);
        if(fd < 0){
            fd = myopen(pathname, O_WRONLY | O_CREAT);
            ans->fd = fd;
        }else{
            int block_num = get_block_num(fd);
            drain_data(block_num);
            ans->fd = fd;
        }
    }
    return ans;
}

int myfclose(myFILE *stream){
    int ans = myclose(stream->fd);
    return ans;
}

size_t myfread(void *ptr, size_t size, size_t nmemb, myFILE *stream){
    size_t ans = myread(stream->fd, ptr, size*nmemb);
    return ans;
}

size_t myfwrite(void *ptr, size_t size, size_t nmemb, myFILE *stream){
    size_t ans = mywrite(stream->fd, ptr, size*nmemb);
    return ans;
}

int myfseek(myFILE *stream, long offset, int whence){
    int ans = mylseek(stream->fd, offset, whence);
    if(ans < 0) {
        return ans;
    }
    return 0;
}


int myfprintf(myFILE *stream, const char *format, ...){
    va_list arguments;
    va_start(arguments, format);
    char buf[512];
    int j = 0;
    for(int i = 0; i < strlen(format); ++i){
        if(format[i] == '%'){
            if(format[i+1] == 'd'){
                char int_string[MAX_NUM];
                int a = va_arg(arguments, int);
                sprintf(int_string, "%d", a);
                for(int k = 0; k < strlen(int_string); ++k){
                    buf[j] = int_string[k];
                    ++j;
                }
            }
            if(format[i+1] == 'c'){
                char c = va_arg(arguments, int);
                buf[j] = c;
                j++;
            }
            if(format[i+1] == 'f'){
                char float_string[MAX_NUM];
                float a = va_arg(arguments, double);
                sprintf(float_string, "%f", a);
                for(int k = 0; k < strlen(float_string); ++k){
                    buf[j] = float_string[k];
                    ++j;
                }
            }
            ++i;
        }else{
            buf[j] = format[i];
            ++j;
        }
    }
    int ans = myfwrite(buf, 1, strlen(buf), stream);
    return ans;
}

int myfscanf(myFILE *stream, const char *format, ...){
    va_list arguments;
    va_start(arguments, format);
    int count = 0;
    for(int i = 0; i < strlen(format); ++i){
        if(format[i] == '%'){
            if(format[i+1] == 'd'){
                char buf[MAX_NUM];
                strcpy(buf, "");
                int len = read_until(stream->fd, buf);
                int ans = myfseek(stream, len+1, SEEK_CUR);
                if(ans > 0){
                    count++;
                }
                int a = atoi(buf);
                (*va_arg(arguments, int*)) = a;
            }
            if(format[i+1] == 'c'){
                char buf[MAX_NUM];
                strcpy(buf, "");
                int len = read_until(stream->fd, buf);
                int ans = myfseek(stream, len+1, SEEK_CUR);
                if(ans > 0){
                    count++;
                }
                (*va_arg(arguments, char*)) = buf[0];
            }
            if(format[i+1] == 'f'){
                char buf[MAX_NUM];
                strcpy(buf, "");
                int len = read_until(stream->fd, buf);

                int ans = myfseek(stream, len+1, SEEK_CUR);

                if(ans > 0){
                    count++;
                }
                float a = atof(buf);
                (*va_arg(arguments, float*)) = a;
            }
            ++i;
        }
    }
    myfseek(stream, -1, SEEK_CUR);
    return count;
}



//int main(){
//    init_open_files();
//    mymkfs(4096);
//    myFILE * file;
//    myFILE * file2;
////    file = myfopen("eylon", "a");
////    file2 = myfopen("eylon", "r");
//
////    int fd1 = myopen("eylon", O_WRONLY | O_CREAT);
////    int bytes1 = mywrite(fd1, "naamat", 6);
////    file2 = myfopen("eylon", "a");
////    printf("%ld\n", mylseek(file2->fd, 0, SEEK_CUR));
////    int ans = myfclose(file2);
////    printf("%d\n", ans);
////    ans = myclose(fd1);
////    printf("%d\n", ans);
//
//    char buf[256];
//    char stam[256] = "eylon";
//    file2 = myfopen("eylon", "w");
//    int count = myfprintf(file2, "%d %c %f",1, 'e', 2.5);
////    file = myfopen("eylon", "r");
////    int e = myfwrite(stam, 1, 5, file2);
////    printf("%d\n", e);
//    file = myfopen("eylon", "r");
////    int e = myfseek(file, 0, SEEK_CUR);
////    printf("%d\n", e);
////    int r = myfread(buf, 1, 30, file);
////    printf("%s\n", buf);
////    myfseek(file, 0, SEEK_SET);
//    int a;
//    float b;
//    char c;
//    myfscanf(file, "%d %c %f", &a, &c, &b);
//    printf("%d %c %f\n", a, c, b);
////    myfscanf(file, "%d %c %f", &a, &c, &b);
//    myfprintf(file2, "%d %c %f",3, 'n', 3.14);
//    myfscanf(file, "%d %c %f", &a, &c, &b);
//    printf("%d %c %f\n", a, c, b);
//
////    printf("%d %c %f\n", a, c, b);
////    printf("%d\n", file->fd);
////    printf("%d\n", file2->fd);
//}