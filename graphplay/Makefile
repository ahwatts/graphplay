CPPFLAGS=-Wall -ggdb -pg
CXXFLAGS=
LDFLAGS=-Wl,--as-needed -ggdb -pg
LDLIBS=-lGL -lglut -lm -lGLEW -lstdc++
SOURCES=$(wildcard *.cpp)

graphplay: $(SOURCES:.cpp=.o)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.d
	rm -f *.d.*
	rm -f graphplay
	rm -f graphplay.exe

%.d: %.cpp
	@set -e
	rm -f $@
	$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< > $@.tmp
	sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' < $@.tmp > $@
	rm -f $@.tmp

include $(SOURCES:.cpp=.d)
