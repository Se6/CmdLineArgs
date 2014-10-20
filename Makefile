CXXFLAGS= -std=c++11

all: example test


# Header dependencies:
example.o: CmdLineArgs.h
test.o: CmdLineArgs.h

%.o: %.c
	$(CXX) -o $@ -c $<

example: example.o
	$(CXX) -o $@ $^

test: test.o
	$(CXX) -o $@ $^

clean:
	rm -f *.o example test
	rm -rf html

doc: Doxyfile.in CmdLineArgs.h
	doxygen Doxyfile.in
	