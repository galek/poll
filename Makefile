ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

#DIRS = src demos/common demos/01_skeletal_animation demos/02_physics_simple demos/03_cube_map demos/04_normal_mapping demos/05_game_demo demos/06_sponza demos/07_showcase
DIRS = src demos/plugins demos/02_physics_simple/ demos/05_game_demo demos/06_sponza 

all : libpoll.so

libpoll.so : force_look
	-@for d in $(DIRS); do (cd $$d; $(MAKE) $(MFLAGS) ); done

clean :
	-@for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done
	-@rm build/*.so

check :
	-@for d in $(DIRS); do (cppcheck --enable=all --std=posix --std=c++11 $$d ); done

force_look :
	-@true
