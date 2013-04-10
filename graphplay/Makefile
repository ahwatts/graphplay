CPPFLAGS=-Wall -ggdb $(shell xml2-config --cflags)
LDFLAGS=-lGL -lglut -lm -lGLEW -lstdc++ $(shell xml2-config --libs)
SOURCES=$(wildcard *.cpp)

graphplay: $(SOURCES:.cpp=.o)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.d
	rm -f *.d.*
	rm -f graphplay

%.d: %.cpp
	@set -e
	rm -f $@
	$(CXX) -M $(CPPFLAGS) $< > $@.tmp
	sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' < $@.tmp > $@
	rm -f $@.tmp

include $(SOURCES:.cpp=.d)
