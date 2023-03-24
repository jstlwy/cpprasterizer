# macOS note:
# You may need to run the following command:
# codesign -f -s - SDL2.framework
#
# Alternatively, in Security & Privacy > Privacy > Developer Tools,
# allow Terminal.app to run software locally
# that does not meet the system's security policy.

CXX := clang++
sdlf := -framework SDL2
sdlh := -I /Library/Frameworks/SDL2.framework/Headers
CXXFLAGS := -std=c++17 -Wall $(sdlh)
LDFLAGS := $(sdlf) -F /Library/Frameworks

# Declare names that indicate recipes, not files 
.PHONY: all clean

srcdir := ./src
objdir := ./obj
src := $(wildcard $(srcdir)/*.cpp)
headers := $(patsubst %.cpp, %.h, $(filter-out $(srcdir)/main.cpp, $(src)))
obj := $(patsubst $(srcdir)/%.cpp, $(objdir)/%.o, $(src))
dep := $(addsuffix .d, $(basename $(obj)))
bin := rasterizer

all: $(bin)

$(bin): $(obj)
	$(CXX) $(LDFLAGS) $^ -o $@

# Generic object file creation rule
$(objdir)/%.o: $(srcdir)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	rm -f $(obj) $(dep) $(bin)
