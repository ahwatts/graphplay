CXXFLAGS=-Wall -ggdb -I/usr/include/libxml2
LDFLAGS=-lGL -lglut -lm -lGLEW -lstdc++ -lxml2
OBJECT_FILES=BasicShader.o Body.o DaeFile.o SphereBody.o World.o graphplay.o

graphplay: ${OBJECT_FILES}

clean:
	rm -f *.o
	rm -f graphplay
