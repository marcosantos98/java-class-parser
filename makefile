CC=clang++

CFLAGS=-Wextra -Werror -Wall -Wpedantic

.njava:
	@mkdir -p build
	$(CC) $(CFLAGS) -o ./build/njava main.cpp

all: .njava