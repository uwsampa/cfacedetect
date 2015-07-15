# Make file for libxml parsing
CC = gcc

SCPATH = /usr/include/libxml2

LIB = /usr/local/lib

OBJS = rgb_image.o parse.o detect.o

CFLAGS = -O3 -Wall

HEADERS = parse.h rgb_image.h

LINK = -lxml2 -lm


all: detect FORCE

#$(TARGET): $(TARGET).c
#	$(CC) $(CFLAGS) -I$(PATH) $(TARGET).c -o $(TARGET) $(LINK)

detect: $(OBJS) FORCE
	$(CC) $(CFLAGS) -o detect $(OBJS)

detect.o: detect.c $(HEADERS) FORCE
	$(CC) $(CFLAGS) -c $< -o detect.o

rgb_image.o: rgb_image.c rgb_image.h FORCE
	$(CC) $(CFLAGS) -c $< -o rgb_image.o 

parse.o: parse.c parse.h FORCE
	$(CC) $(CFLAGS) -c $< -o parse.o -I$(SCPATH) -L$(LIB) $(LINK)

clean: FORCE
	/bin/rm -f *.o *~ detect

FORCE: 