PROJECT = 02_physics_simple

SRC := $(wildcard *.cpp)  
OBJECTS = $(SRC:.cpp=.o) 
CFLAGS  = -I/usr/include/bullet/ -I../../external/include -I../plugins/ -Wall -pedantic -g -std=c++11 -DGLM_FORCE_RADIANS

all: $(PROJECT)

.cpp.o:
	g++ -c -I../../src/ $(CFLAGS) $<

$(PROJECT): $(OBJECTS)
	g++ -o $(PROJECT) $(OBJECTS) -L../../external/lib/ -L../../build/ -lpoll -lpoll_plugins -lassimp -lGLU -lGLEW -lGL -lSDL2 -lBulletDynamics -lBulletCollision -lLinearMath -ljsoncpp

clean:
	rm -f $(OBJECTS) $(PROJECT)



