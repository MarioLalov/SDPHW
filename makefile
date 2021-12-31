test: interface.o tests.o
	g++ interface.o tests.o -o test

interface.o: interface.cpp
	g++ -c interface.cpp

tests.o: tests.cpp
	g++ -c tests.cpp