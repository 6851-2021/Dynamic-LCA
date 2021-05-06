CC = clang++                                                                    
CFLAGS = -Wall -Wextra -c -std=c++11 -O2                                        
DEPS = lcaTree.hpp

%.o: %.cpp $(DEPS)                                                              
		$(CC) -o $@ $< $(CFLAGS)

lca: lcaTree.o
	$(CC) -o lca lcaTree.o

clean:                                                                          
		rm -f *.o core* *~ er