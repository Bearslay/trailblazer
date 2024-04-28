debug:
	@mkdir bin -p
	@mkdir bin/debug -p
	@g++ -c src/*.cpp -std=c++14 -m64 -g -Wall -I include
	@g++ *.o -o bin/debug/main -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
	@./bin/debug/main
release:
	@mkdir bin -p
	@mkdir bin/release -p
	@g++ -c src/*.cpp -std=c++14 -m64 -O3 -Wall -I include
	@g++ *.o -o bin/release/main -s -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
	@./bin/release/main
