CXX := clang++
sdlf := -framework SDL2
sdlh := -I /Library/Frameworks/SDL2.framework/Headers
CXXFLAGS := -std=c++17 -Wall $(sdlh)
LDFLAGS := $(sdlf) -F /Library/Frameworks/

# Declare names that indicate recipes, not files 
.PHONY: all clean

src := $(wildcard *.cpp)
#headers := $(filter-out main.h, $(patsubst %.cpp, %.h, $(src)))
obj := $(patsubst %.cpp, %.o, $(src))
dep := $(patsubst %.cpp, %.d, $(src))
bin := rasterizer

all: $(bin)

$(bin): $(obj)
	$(CXX) $(LDFLAGS) $^ -o $@

# Generic object file creation rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	rm -f $(obj) $(dep) $(bin)
