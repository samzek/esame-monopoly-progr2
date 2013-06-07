OBJ = mono_main.o manip.o fileIO.o imp_prob.o
#DEBUG_FLAGS=-D DEBUG_MODE
CXXFLAGS=$(DEBUG_FLAGS) -Wall `pkg-config --cflags gtk+-3.0`
LDFLAGS=-export-dynamic `pkg-config --libs gtk+-3.0`

Monopoly: $(OBJ)
	g++ -o Monopoly  $(OBJ) $(LDFLAGS)

-include dependencies

.PHONY: depend clean cleanall debug

depend:
	g++ -MM *.cpp > dependencies
clean:
	rm *.o
cleanall:
	rm Monopoly *.o *~
debug:
	g++ -D DEBUG_MODE -o Monopoly $(OBJ) $(LDFLAGS) 
