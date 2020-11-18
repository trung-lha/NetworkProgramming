CC = gcc
CFLAGS = -I
LIBS = -lm
TARGET = resolver

DEPS = $(wildcard *.h)
src = $(wildcard *.c)
test = $(wildcard *.c)
obj = $(patsubst %.c,%.o,$(src)) $(patsubst %.c,%.o,$(test))

$(TARGET): $(obj) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< 

clean: $(obj)
	rm -rf $(obj) $(TARGET)