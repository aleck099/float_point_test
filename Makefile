OFLAGS:=-Ofast -march=native
#OFLAGS:=-g
CXXFLAGS:=-std=c++20 -Wall -isystem ./fast_io/include $(OFLAGS)
LDFLAGS:=$(OFLAGS)
TARGETS=gen legacy s2 s3

.PHONY: all clean

all: $(TARGETS)

clean:
	-rm -- $(TARGETS)
	-rm -- *.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $<

gen.o: gen.cpp

legacy.o: legacy.cpp

v2.o: v2.cpp

v3.o: v3.cpp

gen: gen.o
	$(CXX) $^ -o $@ $(LDFLAGS)

legacy: legacy.o
	$(CXX) $^ -o $@ $(LDFLAGS)

s2: v2.o
	$(CXX) $^ -o $@ $(LDFLAGS)

s3: v3.o
	$(CXX) $^ -o $@ $(LDFLAGS)

