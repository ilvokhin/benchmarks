CXX ?= c++
CXXFLAGS ?= -std=c++20 -pedantic -Wall -O3
LDFLAGS = -lbenchmark

BIN = bin
SOURCE = src

SOURCES = $(wildcard $(SOURCE)/*.cpp)
BINARIES = $(patsubst $(SOURCE)/%.cpp,$(BIN)/%,$(SOURCES))

.PHONY: all clean

all: $(BINARIES)

$(BIN)/%: $(SOURCE)/%.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

$(BIN):
	mkdir -p $(BIN)

clean:
	rm -rf $(BIN)
