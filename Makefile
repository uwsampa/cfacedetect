CC = gcc
CFLAGS = -O3 -Wall -g -std=c11
FANNDIR = ../../fann-snnap/src
SCPATH = /usr/include/libxml2
FANNARGS = -I $(FANNDIR)/ -I $(FANNDIR)/include/ $(FANNDIR)/floatfann.c
LINK = -lxml2 -lm
OBJS = rgb_image.o parse.o detect.o shrink.o face.o
HEADERS = parse.h rgb_image.h shrink.h face.h

all: detect

detect: $(OBJS)
	$(CC) $(CFLAGS) -o detect $(OBJS) $(LINK) $(FANNARGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(DEFINES) -c -I$(SCPATH) -o $@ $<


clean:
	rm -rf *.o detect *.pyc *.data detect.dSYM
