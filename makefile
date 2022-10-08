OUT = ./example

CXX = g++
CXX_VER = c++17
CXX_FLAGS = -O3 -std=$(CXX_VER) -Wall -Wextra -Werror \
            -pedantic -Wno-deprecated-declarations

example: ./bin ./flag.hh ./example.cc
	$(CXX) $(CXX_FLAGS) -o $(OUT) ./example.cc

./bin:
	mkdir -p bin

clean:
	rm -r $(OUT)

all:
	@echo example, clean
