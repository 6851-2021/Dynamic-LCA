CC = clang++                                                                    
CFLAGS = -Wall -Wextra -c -std=c++11 -O2                                        
DEPS = generateRandTrees.hpp lcaTree.hpp

%.o: %.cpp $(DEPS)                                                              
		$(CC) -o $@ $< $(CFLAGS)

lca: test.o generateRandTrees.o lcaTree.o
	$(CC) -o lca test.o generateRandTrees.o lcaTree.o

clean:                                                                          
		rm -f *.o core* *~ er
