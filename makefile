myfs.o : myfs.cpp
	gcc -fPIC -c myfs.cpp

mystdio.o : mystdio.cpp
	gcc -fPIC -c mystdio.cpp

test_myfs.o : test_myfs.cpp
	gcc -fPIC -c test_myfs.cpp

test_mystdio.o : test_mystdio.cpp
	gcc -fPIC -c test_mystdio.cpp

libmyfs.so : myfs.o
	gcc --shared -fPIC myfs.o -o libmyfs.so

libmylibc.so : mystdio.o myfs.o
	gcc --shared -fPIC mystdio.o myfs.o -o libmylibc.so

test_myfs : libmyfs.so test_myfs.o
	gcc -fPIC test_myfs.o ./libmyfs.so -o test_myfs

test_mystdio : libmylibc.so test_mystdio.o
	gcc -fPIC test_mystdio.o ./libmylibc.so -o test_mystdio

all : libmyfs.so libmylibc.so test_myfs test_mystdio

clean:
	rm *.o *.so test_myfs test_mystdio


