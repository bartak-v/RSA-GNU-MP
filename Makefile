# Projekt: RSA and integer factorization
# Autor:   Vít Barták
# Datum:   10.03.2022 -11.2 # cxxflags -std=c++20

CXX=g++
XLOGINXX=xbarta47

OBJ=kry.cpp
BIN=kry

CXXFLAGS:=-Wall -Wextra -Wsuggest-override -Wnull-dereference -Wshadow -Wold-style-cast -pedantic -lgmp 

LINK.o = $(LINK.cpp)

all: CXXFLAGS += -Ofast -march=native -flto
all: kry

debug: CXXFLAGS += -g3 -fsanitize=address,undefined -fno-omit-frame-pointer
debug: kry

debug_functional: CXXFLAGS += -g3
debug_functional: kry 

kry: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJ) -o $(BIN)

pack:
	zip $(XLOGINXX).zip *.cpp *.hpp  Makefile doc.pdf

dep:
	g++ *.cpp -MM >> Makefile