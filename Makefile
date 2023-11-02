OS := $(shell uname)
WFLAGS := -Wall -Wextra -Werror
ifeq ($(OS), Darwin)
CXX := clang++
CXXFLAGS := -std=c++17 -F/Library/Frameworks $(WFLAGS)
LDFLAGS := -F/Library/Frameworks -framework SDL2 -rpath /Library/Frameworks
else
CXX := g++
CXXFLAGS := -std=c++17 $(WFLAGS)
LDFLAGS := -lSDL2
endif

srcdir := ./src
objdir := ./obj
src := $(wildcard $(srcdir)/*.cpp)
hdr := $(wildcard $(srcdir)/*.h)
obj := $(patsubst $(srcdir)/%.cpp, $(objdir)/%.o, $(src))
dep := $(addsuffix .d, $(basename $(obj)))
bin := rasterizer

.PHONY: all clean

all: $(bin)

$(bin): $(obj)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(objdir)/%.o: $(srcdir)/%.cpp
	$(CXX) -c $(CXXFLAGS) -MMD $< -o $@

clean:
	rm -f $(obj) $(dep) $(bin)
