#CC     = arm-linux-gnueabihf-g++
CC = gcc

VPATH  = 
LIB_DIR = 
INCLUDES = 
CFLAGS  = -Wall -g 
LIBS    = -lpthread -lrt -lm

OBJS    = sl.o wave.o

TARGET  = sl wv

all:$(TARGET)

sl:sl.o 
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)

wv:wave.o 
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)

%.o:%.c
	$(CC) -c $< -o $@ $(INCLUDES) $(CFLAGS)

.PHONY:
clean:
	rm -f $(TARGET) *.o 

