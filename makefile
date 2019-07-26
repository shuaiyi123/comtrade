#CC     = arm-linux-gnueabihf-g++
CC = gcc

VPATH  = 
LIB_DIR = 
INCLUDES = 
CFLAGS  = -Wall -g 
LIBS    = -lpthread -lrt -lm

OBJS    = sl.o wave.o aly.o

TARGET  = sl wv aly

all:$(TARGET)

sl:sl.o 
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)

wv:wave.o 
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)

aly:aly.o 
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)

%.o:%.c
	$(CC) -c $< -o $@ $(INCLUDES) $(CFLAGS)

.PHONY:
clean:
	rm -f $(TARGET) *.o

