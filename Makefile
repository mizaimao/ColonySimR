CXX = clang++
CXXFLAGS = -Wall -std=c++20 -fcolor-diagnostics -g
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs) -lSDL2_ttf

all:
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) main.cpp -I . -o main $(SDL_LIBS)

run: all
	./main

clean:
	rm -f main

.PHONY: all run clean
