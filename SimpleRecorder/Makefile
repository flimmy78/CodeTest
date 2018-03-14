SOURCE=$(wildcard *.c)
SDL-DIR = /usr/local/SDL_arm
OBJS=$(patsubst %.c,%.o,$(SOURCE))
CC=arm-none-linux-gnueabi-gcc
CPP=arm-none-linux-gnueabi-g++
CFLAGS  = -I$(SDL-DIR)/include
LIBS	= -L$(SDL-DIR)/lib -lSDL

all: simplerecorder

simplerecorder: $(OBJS)
	$(CPP) -Wall -o $@ $^ $(LIBS) -static -pthread -L. -lv4lconvert -lm -lrt ./linux_lib/libcedarv_osal.a ./linux_lib/libcedarxalloc.a ./linux_lib/libh264enc.a ./linux_lib/libcedarv.a
%.o: %.c
	$(CC) -Wall -c $< -o $@ $(CFLAGS)

clean:
	rm $(OBJS) simplerecorder
