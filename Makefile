WFLAGS := -Wall -Wextra -Werror
CXXFLAGS := -std=c++17 $(WFLAGS)

OS := $(shell uname)
ifeq ($(OS), Darwin)
CXX := clang++
CXXFLAGS += -F/Library/Frameworks
LDFLAGS := -F/Library/Frameworks -framework SDL2 -rpath /Library/Frameworks
else
CXX := g++
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
