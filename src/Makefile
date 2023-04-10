CPP=g++
LD_FLAGS=`pkg-config --libs glfw3 glew` 
GLFW_FLAGS=-DUSE_OPENGL
CPP_FLAGS=-std=c++23 -Og -ggdb3 -Wall -I$(HOME)/.local/include
OBJECT_FILES=target/window.o target/main.o target/scluk.a
TARGET=target/out

$(TARGET): $(OBJECT_FILES)
	$(CPP) $(CPP_FLAGS) $(LD_FLAGS) -o $(TARGET) $(OBJECT_FILES)

target/window.o: glfw/window.cpp glfw/window.hpp
	$(CPP) $(CPP_FLAGS) $(GLFW_FLAGS) -c $< -o $@ 

target/main.o: main.cpp glfw/window.hpp
	$(CPP) $(CPP_FLAGS) -c $< -o $@

target/scluk.a:
	make --directory=$(HOME)/.local/include/scluk
	cp $(HOME)/.local/include/scluk/target/scluk.a target/scluk.a

target/%.o: %.cpp
	$(CPP) $(CPP_FLAGS) -c $< -o $@

clean:
	rm target/*.o target/out

run:$(TARGET)
	$(TARGET)
