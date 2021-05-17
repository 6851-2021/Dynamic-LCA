CC = clang++                                                                    
CFLAGS = -Wall -Wextra -c -std=c++11 -O2                                        
DEPS = lcaMultilevel.hpp generateRandTrees.hpp lcaTree.hpp

%.o: %.cpp $(DEPS)                                                              
		$(CC) -o $@ $< $(CFLAGS)

lca: test.o lcaMultilevel.o generateRandTrees.o lcaTree.o
	$(CC) -o lca test.o lcaMultilevel.o generateRandTrees.o lcaTree.o

clean:                                                                          
		rm -f *.o core* *~ er
