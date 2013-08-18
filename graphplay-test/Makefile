CPPFLAGS=-Wall -ggdb -pg -I../../graphplay-deps/include -DGLEW_STATIC -DGTEST_HAS_PTHREAD=0
CXXFLAGS=-std=gnu++11
LDFLAGS=-Wl,--as-needed -ggdb -pg -L../../graphplay-deps/lib/mingw
LDLIBS=-lgtest -lgtest_main -lstdc++ -lglew32 -lglfw3 -lopengl32 -lglu32 -lgdi32
GP_SOURCES=$(filter-out ../graphplay/graphplay.cpp,$(wildcard ../graphplay/*.cpp))
GP_OBJECTS=$(GP_SOURCES:.cpp=.o)
SOURCES=$(wildcard *.cpp)

graphplay-test: $(SOURCES:.cpp=.o)
	$(MAKE) -C ../graphplay
	$(LINK.o) $^ $(GP_OBJECTS) $(LOADLIBS) $(LDLIBS) -o $@

.PHONY: clean

clean:
	$(MAKE) -C ../graphplay clean
	rm -f *.o
	rm -f *.d
	rm -f *.d.*
	rm -f graphplay-test
	rm -f graphplay-test.exe

%.d: %.cpp
	@set -e
	rm -f $@
	$(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< > $(basename $@).d.tmp
	echo $(subst /,\/,$(subst .,\.,$*))
	sed 's/\($(subst /,\/,$(subst .,\.,$*))\)\.o[ :]*/\1.o $@ : /g' < $@.tmp > $@
	rm -f $@.tmp

include $(SOURCES:.cpp=.d)
