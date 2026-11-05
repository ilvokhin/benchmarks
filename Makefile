CXX ?= c++
CXXFLAGS ?= -std=c++20 -pedantic -Wall -O3
LDFLAGS = -lbenchmark

BIN = bin
SOURCE = src

SOURCES = $(wildcard $(SOURCE)/*.cpp)
BINARIES = $(patsubst $(SOURCE)/%.cpp,$(BIN)/%,$(SOURCES))

.PHONY: all
all: $(BINARIES)

$(BIN)/%: $(SOURCE)/%.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

$(BIN):
	mkdir -p $(BIN)

.PHONY: clean
clean:
	rm -rf $(BIN)
