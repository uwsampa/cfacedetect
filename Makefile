# Make file for libxml parsing
CC = gcc

PATH = /usr/include/libxml2

LIB = /usr/local/lib

TARGET = debug

CFLAGS = -O3 -Wall

LINK = -lxml2 -lm


all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -I$(PATH) -L$(LIB) $(TARGET).c -o $(TARGET) $(LINK)