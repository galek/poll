PROJECT = libpoll.so

SRC := $(wildcard *.cpp) 
OBJECTS = $(SRC:.cpp=.o)
CFLAGS  = -I/usr/include/ -I/usr/include/bullet/ -I../external/include -fPIC -Wall -pedantic -g -std=c++11 -DGLM_FORCE_RADIANS -DNDEBUG=0

all: $(PROJECT)

.cpp.o:
	g++ -c $(CFLAGS) $<

$(PROJECT): $(OBJECTS)
	g++ -shared -o $(PROJECT) $(OBJECTS) -L../external/lib/ -lm -lGLU -lGLEW -lGL -lSDL2 -lassimp -lBulletCollision -lBulletDynamics -lLinearMath -ljsoncpp
	mv $(PROJECT) ../build/.

clean:
	rm -f $(OBJECTS) 

