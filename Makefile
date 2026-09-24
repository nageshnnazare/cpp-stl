# Build every example in this directory.
#
#   make
#   make test      # run them all
#   make 01_vector_timeline && ./build/01_vector_timeline

CXX      ?= c++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic -O2
LDFLAGS  ?=

SRC      := $(wildcard examples/*.cpp)
BINS     := $(patsubst examples/%.cpp,build/%,$(SRC))

.PHONY: all test clean
all: $(BINS)

build/%: examples/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

test: all
	@set -e; \
	for b in $(BINS); do \
	  echo "===== $$b ====="; \
	  "./$$b"; \
	done

clean:
	rm -rf build
