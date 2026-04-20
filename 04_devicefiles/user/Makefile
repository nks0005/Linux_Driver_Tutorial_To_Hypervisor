CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -Werror

TARGET := hello_user

all: $(TARGET)

$(TARGET): hello_user.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

