INCLUDE = ../include
SRC		= ..

OPTKEYS = -o0
DBGFLG	= -g

.PHONY: all clean

all: 	engine.o

clean:	
	rm *.o


windows.o:$(SRC)/windows.cpp
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@

core.o: $(SRC)/core.cpp $(INCLUDE)/engine.h $(INCLUDE)/engine_platform.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@
	
objects.o:$(SRC)/objects.cpp $(INCLUDE)/engine.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@	

geometry.o:$(SRC)/geometry.cpp $(INCLUDE)/geometry.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@
	
physics.o:$(SRC)/physics.cpp $(INCLUDE)/physics.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@

graphics.o:$(SRC)/graphics.cpp $(INCLUDE)/graphics.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@


RENDER_HDS = \
	$(INCLUDE)/engine.h $(INCLUDE)/gui.h $(INCLUDE)/graphics.h \
	$(INCLUDE)/collisions.h $(INCLUDE)/memory_manage.h $(INCLUDE)/png_images.h

render.o: $(SRC)/render.cpp $(RENDER_HDS) 
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@	

collisions.o:$(SRC)/collisions.cpp $(INCLUDE)/collisions.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@
	
png_images.o:$(SRC)/png_images.cpp  $(INCLUDE)/png_images.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@
	
memory_manage.o:$(SRC)/memory_manage.cpp  $(INCLUDE)/memory_manage.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@

GUI_SRC = $(SRC)/gui.cpp
GUI_HDS = $(INCLUDE)/gui.h $(INCLUDE)/graphics.h $(INCLUDE)/collisions.h

gui.o: $(GUI_SRC) $(GUI_HDS) 
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@

ENGINE_OBJECTS = \
	core.o memory_manage.o objects.o geometry.o graphics.o gui.o render.o \
	physics.o collisions.o windows.o png_images.o

engine.o: $(ENGINE_OBJECTS)
	ld -r -o $@ $^ 
