CPPFLAGS=-Wall -ggdb -pg
CXXFLAGS=-std=c++11
LDFLAGS=-Wl,--as-needed -ggdb -pg
LDLIBS=-lstdc++ -lm -lglfw -lGL -lGLEW
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
