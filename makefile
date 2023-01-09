INCLUDE 		= include
OBJ				= objects

ENGINE_INCLUDE 	= engine/include
ENGINE_OBJ		= engine/objects/engine.o

INCLUDE_FLAGS 	= -I$(INCLUDE) -I$(ENGINE_INCLUDE)
LIBS_FLAGS		= \
	-static -lcppfs -static -lstdc++  -lgdi32  -lopengl32 \
	-lglew32.dll -static -lpng -static -lz
	
OPTKEYS			= -o0
#DBGFLG			= -Wl,--strip-all
DBGFLG			= -g -fmax-errors=5

.PHONY: all clean $(ENGINE_OBJ)

all: game.exe raycast_test.exe test.exe gjk_test.exe pathfinder_test.exe

clean:
	cd engine; make clean
	rm $(OBJ)/*.o
	rm *.exe

$(OBJ):
	mkdir -p $(OBJ)

$(OBJ)/game_module.o: game_module.cpp $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@
	
$(OBJ)/test_module.o: test_module.cpp $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@
	
$(OBJ)/gjk_test.o: gjk_test.cpp $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@
	
$(OBJ)/raycast_test.o: raycast_test.cpp $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@

$(OBJ)/objects.o: objects.cpp  $(INCLUDE)/objects.h $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@
	
$(OBJ)/vector_utilits.o: vector_utilits.cpp  $(INCLUDE)/vector_utilits.h $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@	
	
$(OBJ)/utilits.o: utilits.cpp  $(INCLUDE)/utilits.h $(ENGINE_INCLUDE)/graphics.h $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@	
	
$(OBJ)/keys.o: keys.cpp  $(INCLUDE)/keys.h $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@	

$(OBJ)/controllers.o: controllers.cpp  $(INCLUDE)/controllers.h $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@	

$(OBJ)/collisions2.o: collisions2.cpp  $(INCLUDE)/collisions2.h $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@	

UIBE_MODULE_HDS = $(ENGINE_INCLUDE)/engine.h $(INCLUDE)/uibe.h  \
	$(INCLUDE)/keys.h $(INCLUDE)/objects.h $(INCLUDE)/utilits.h
 
$(OBJ)/uibe_module.o: uibe_module.cpp $(UIBE_MODULE_HDS) $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@		

$(OBJ)/pathfind.o: pathfind.cpp $(INCLUDE)/pathfind.h \
	$(ENGINE_INCLUDE)/engine.h $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@		

$(ENGINE_OBJ): 
	cd engine; make

COMMON_OBJS = \
	$(ENGINE_OBJ) $(OBJ)/uibe_module.o $(OBJ)/objects.o \
	$(OBJ)/vector_utilits.o $(OBJ)/utilits.o $(OBJ)/keys.o

GAME_OBJS = \
	$(OBJ)/game_module.o  $(OBJ)/controllers.o $(COMMON_OBJS)

game.exe: $(GAME_OBJS)
	gcc  $^ $(LIBS_FLAGS) -o $@
	
TEST_OBJS = \
	$(OBJ)/test_module.o $(OBJ)/collisions2.o $(COMMON_OBJS)

test.exe: $(TEST_OBJS) 
	gcc $(DBGFLG) $^ $(LIBS_FLAGS) -o $@

GJK_TEST_OBJS = \
	$(OBJ)/gjk_test.o $(OBJ)/collisions2.o $(COMMON_OBJS)

gjk_test.exe: $(GJK_TEST_OBJS)
	gcc $(DBGFLG) $^ $(LIBS_FLAGS) -o $@
	

raycast_test.exe: $(OBJ)/raycast_test.o $(COMMON_OBJS)
	gcc $(DBGFLG) $^ $(LIBS_FLAGS) -o $@

$(OBJ)/visualizers.o: visualizers.cpp  $(INCLUDE)/visualizers.h $(OBJ) 
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@ 

$(OBJ)/pathfinder_test.o: pathfinder_test.cpp $(OBJ) 
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@ 

pathfinder_test.exe: $(OBJ)/pathfinder_test.o $(OBJ)/visualizers.o \
        $(OBJ)/pathfind.o $(COMMON_OBJS)
	gcc $(DBGFLG) $^ $(LIBS_FLAGS) -o $@
	

