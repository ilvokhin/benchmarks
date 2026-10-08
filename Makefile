CXX=c++
CXXFLAGS=-std=c++20 -pedantic -Wall -O3
LDFLAGS=-lbenchmark

BIN=bin
SOURCE=src

OS=$(shell uname)
ifeq ($(OS),Darwin)
	HOMEBREW=$(shell brew --prefix 2>/dev/null)
	CXXFLAGS+=-I$(HOMEBREW)/include
	LDFLAGS+=-L$(HOMEBREW)/lib
endif

SOURCES=$(wildcard $(SOURCE)/*.cpp)
BINARIES=$(patsubst $(SOURCE)/%.cpp,$(BIN)/%,$(SOURCES))

.PHONY: all clean

all: $(BINARIES)

$(BIN)/%: $(SOURCE)/%.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

$(BIN):
	mkdir -p $(BIN)

clean:
	rm -rf $(BIN)
