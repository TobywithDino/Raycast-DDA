all:
	g++ -Iinclude -Llib/SDL2 -o main src/*.cpp -lmingw32 -lSDL2main -lSDL2