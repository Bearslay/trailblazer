default:
	@g++ -c tileset.cpp ../src/RenderWindow.cpp ../src/Texture.cpp ../src/Utilities.cpp -std=c++14 -m64 -g -Wall -I ../include
	@g++ *.o -o tileset -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
	@./tileset
