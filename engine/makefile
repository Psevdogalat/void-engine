INCLUDE = -Iinclude
OPTKEYS = -o0
DBGFLG	= -g -fmax-errors=5
CXX_FLAGS = 
MODULES = core.o memoryManage.o objects.o geometry.o graphics.o gui.o \
	render.o physics.o collisions.o windows.o pngImage.o


.PHONY: all clean

all: engine.o

clean:	
	rm *.o

.cpp.o:
	g++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE) -c -o $@

.c.o:
	gcc -x c $(DBGFLG) $(OPTKEYS) $< $(INCLUDE) -c -o $@

engine.o: $(MODULES)
	ld -r -o $@ $^ 
