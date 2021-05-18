CC = clang++                                                                    
CFLAGS = -Wall -Wextra -c -std=c++11 -O2                                        
DEPS = naiveTree.hpp lcaMultilevel.hpp generateRandTrees.hpp lcaTree.hpp

%.o: %.cpp $(DEPS)                                                              
		$(CC) -o $@ $< $(CFLAGS)

lca: test.o lcaMultilevel.o generateRandTrees.o lcaTree.o
	$(CC) -o lca test.o lcaMultilevel.o generateRandTrees.o lcaTree.o

demo: demo.o lcaMultilevel.o generateRandTrees.o lcaTree.o
	$(CC) -o demo demo.o lcaMultilevel.o generateRandTrees.o lcaTree.o

timing: timingTest.o naiveTree.o lcaMultilevel.o generateRandTrees.o lcaTree.o
	$(CC) -o timing timingTest.o naiveTree.o lcaMultilevel.o generateRandTrees.o lcaTree.o

clean:                                                                          
		rm -f *.o core* *~ er
