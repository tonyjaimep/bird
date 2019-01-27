bird: main.cpp
	g++ -c main.cpp
	g++ main.o -o bird -lsfml-graphics -lsfml-window -lsfml-system
	rm main.o
