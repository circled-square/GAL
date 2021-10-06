CPP=g++
LD_FLAGS=`pkg-config --libs glfw3 glew` 
GLFW_FLAGS=-DUSE_OPENGL
CPP_FLAGS=-std=c++20 -Og -ggdb3 -Wall
OBJECT_FILES=target/window.o target/main.o target/scluk.a
TARGET=target/out

$(TARGET): $(OBJECT_FILES)
	$(CPP) $(CPP_FLAGS) $(LD_FLAGS) -o $(TARGET) $(OBJECT_FILES)

target/window.o: glfw/window.cpp glfw/window.hpp
	$(CPP) $(CPP_FLAGS) $(GLFW_FLAGS) -c glfw/window.cpp -o $@ 

target/%.o: %.cpp
	$(CPP) $(CPP_FLAGS) -c $< -o $@

clean:
	rm target/*.o target/out

run:$(TARGET)
	$(TARGET)
