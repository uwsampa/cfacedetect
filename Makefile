
CC = gcc

#SCPATH = /usr/include/libxml2
SCPATH = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/usr/include/libxml2

LIB = /usr/local/lib

OBJS = rgb_image.o parse.o detect.o shrink.o face.o

CFLAGS = -O3 -Wall -g -std=c11

HEADERS = parse.h rgb_image.h shrink.h face.h

LINK = -lxml2 -lm -lfann


all: detect FORCE

detect: $(OBJS) FORCE
	$(CC) $(CFLAGS) -o detect $(OBJS) -I$(SCPATH) -L$(LIB) $(LINK) -I fann/ -I fann/include/ fann/floatfann.c

detect.o: detect.c $(HEADERS) FORCE
	$(CC) $(CFLAGS) -c $< -o detect.o

shrink.o: shrink.c rgb_image.h FORCE
	$(CC) $(CFLAGS) -c $< -o shrink.o 

face.o: face.c face.h FORCE
	$(CC) $(CFLAGS) -c $< -o face.o

rgb_image.o: rgb_image.c rgb_image.h FORCE
	$(CC) $(CFLAGS) -c $< -o rgb_image.o 

parse.o: parse.c parse.h FORCE
	$(CC) $(CFLAGS) -c $< -o parse.o -I$(SCPATH) -L$(LIB) $(LINK)

clean: FORCE
	rm -f *.o detect *.pyc

FORCE: 