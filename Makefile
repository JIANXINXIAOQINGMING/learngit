TARGET = $(notdir $(CURDIR))
CC=$(CROSS_COMPILE)gcc
objs := $(patsubst %c, %o, $(shell ls *.c))
$(TARGET)_test:$(objs)
	$(CC) -o $@ $^ -lm
%.o:%.c
	$(CC) -c -o $@ $< -lm
clean:
	rm -f  $(TARGET)_test *.all *.o