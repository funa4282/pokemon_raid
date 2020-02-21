all: nms.hpp
	g++ nms.cpp utils.cpp main.cpp -llept -ltesseract -I/usr/local/include -L/usr/local/lib `pkg-config opencv4 --cflags` `pkg-config opencv4 --libs` -std=c++14 -o a.out

run:
	./a.out

clean:
	rm -rf a.out
