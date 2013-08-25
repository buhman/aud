TARGET = aud
LIBS = -lasound -lFLAC -lvorbisfile -lmagic -loggz -logg -lopusfile
CC = gcc
CFLAGS = -g -Wall -std=c99 -D__STDC_FORMAT_MACROS -D_GNU_SOURCE -I/usr/include/opus

.PHONY: clean all default

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
