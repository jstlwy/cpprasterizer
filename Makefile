CXX := clang++
sdlf := -framework SDL2
sdlh := -I /Library/Frameworks/SDL2.framework/Headers
CXXFLAGS := -std=c++17 -Wall $(sdlh)
LDFLAGS := $(sdlf) -F /Library/Frameworks/

# Declare names that indicate recipes, not files 
.PHONY: all clean

src := $(wildcard *.cpp)
headers := $(patsubst %.cpp, %.h, $(filter-out main.cpp, $(src)))
#obj := $(patsubst $(srcdir)/%.cpp, $(objdir)/%.o, $(src))
obj := $(addsuffix .o, $(basename $(src)))
dep := $(addsuffix .d, $(basename $(obj)))
bin := rasterizer

all: $(bin)

$(bin): $(obj)
	$(CXX) $(LDFLAGS) $^ -o $@

# Generic object file creation rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	rm -f $(obj) $(dep) $(bin)
