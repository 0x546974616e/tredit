# :set noexpandtab

BIN = main

CXXEXT = cpp
CXXSRC = $(wildcard *.$(CXXEXT))
CXXOBJ = $(CXXSRC:.$(CXXEXT)=.o)
CXXDEP = $(CXXSRC:.$(CXXEXT)=.d)

CXX = clang++-19
# TODO: OpenSSF, Visibility
CXXFLAGS = -std=c++23 -O3 -Wall -Wextra -Wconversion -Werror -Wno-unused
CXXDEPS = -MMD -MP -MT $@ -MF $(@:.o=.d)
LDFLAGS =

.PHONY : all clean cleanall mrproper run crun c

all : $(BIN)
	@echo "-->" ./$(notdir $<)

$(BIN) : $(CXXOBJ)
	@echo Generating Code...
	@$(CXX) $^ -o $@ $(LDFLAGS)

%.o : %.$(CXXEXT)
	@echo $(notdir $<)
	@$(CXX) -c $< -o $@ $(CXXFLAGS) $(CXXDEPS)

-include $(CXXDEP)

clean :
	@rm -f $(CXXOBJ)

cleanall : clean
	@rm -f $(CXXDEP)

mrproper : cleanall
	@rm -f $(BIN)

run :
	@./$(BIN)

crun c : all run
