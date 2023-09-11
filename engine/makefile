INCLUDE = -Iinclude
OPTKEYS = -o0
DBGFLG	= -g -fmax-errors=5
CXX_FLAGS = $(DBGFLG) $(OPTKEYS) $(INCLUDE)
MODULES = geometryC.o collisionC.o core.o memoryManage.o objects.o geometry.o graphics.o gui.o \
	render.o physics.o collisions.o windows.o pngImage.o


.PHONY: all clean

all: engine.o

clean:	
	rm *.o

.cpp.o:
	g++ $(CXX_FLAGS) $< -c -o $@

.c.o:
	gcc -x c $(CXX_FLAGS) $< -c -o $@

geometryC.o: geometry.c include/geometry.h
	gcc -x c $(CXX_FLAGS) geometry.c -c -o $@ 

collisionC.o: collision.c include/geometry.h include/collision.h
	gcc -x c $(CXX_FLAGS) collision.c -c -o $@ 

engine.o: $(MODULES)
	ld -r -o $@ $^ 
