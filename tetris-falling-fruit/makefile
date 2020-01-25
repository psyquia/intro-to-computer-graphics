CXX = g++
CXXFLAGS = -Wall -g

p1: p1.o game.o shapes.o drawings.o
	$(CXX) $(CXXFLAGS) -o FruitTetris p1.o game.o shapes.o drawings.o -lopengl32 -lglew32 -lfreeglut -lglu32
	
p1.o: p1.cpp game.h 
	$(CXX) $(CXXFLAGS) -c p1.cpp
	
game.o: game.h shapes.h  

shapes.o: shapes.h drawings.h

drawings.o: drawings.h
	$(CXX) $(CXXFLAGS) -c drawings.cpp -lopengl32 -lglew32 -lfreeglut -lglu32

	




	

	
