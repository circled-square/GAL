CPP=g++
LD_FLAGS=`pkg-config --libs glfw3`
CPP_FLAGS=-fPIC -std=c++2a -Og -ggdb3 -DUSE_OPENGL

all: window.o lib/libwindow.so

lib/libwindow.so: window.o
	$(CPP) -shared -o lib/libwindow.so window.o $(CPP_FLAGS) $(LD_FLAGS)

window.o: window.cpp window.hpp
	$(CPP) -o window.o -c window.cpp $(CPP_FLAGS)

clean:
	rm window.o lib/libwindow.so
