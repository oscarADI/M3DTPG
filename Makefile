output: circuit.o parser.o main.o 
	g++ circuit.o parser.o main.o -o test.o -L. -lgurobi_c++ -lgurobi90

main.o: main.cpp
	g++ -std=c++11 -c main.cpp

parser.o: parser.cpp parser.hpp
	g++ -std=c++11 -c parser.cpp

circuit.o: circuit.cpp circuit.hpp
	g++ -std=c++11 -c circuit.cpp 

clean: 
	rm -f *.o





