.PHONY: all run clean

SOURCES := $(shell find src -name '*.cpp' -print)

all: network_os

network_os:
	g++ -std=c++17 -Wall -Wextra -g -fsanitize=address $(SOURCES) -o network_os

run: network_os
	./network_os

clean:
	rm -f network_os
	find . -type f \( -name '*.o' -o -name '*.out' \) -delete
