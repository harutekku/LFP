# Compiler & Linker
#-----------------------------------------------------------
export CC        := gcc 
export LD        := gcc # Don't use LD unless you need to

# Flags
#-----------------------------------------------------------
export CFLAGS    := -c -g -std=gnu2x -Wall -Wextra -Wpedantic 
export LDFLAGS   := 
CPPFLAGS         := -I include
DEPFLAGS          = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

# Directories
#-----------------------------------------------------------
INCDIR           := include
SRCDIR           := src
OBJDIR           := bin
DEPDIR           := dep

# File extensions
#-----------------------------------------------------------
INCEXT           := h
SRCEXT           := c

# Files
#-----------------------------------------------------------
SRCFILES         := main.c Args.c Client.c Error.c IO.c Protocol.c Server.c Utility.c # Source files
export OBJFILES  := $(SRCFILES:%.$(SRCEXT)=%.o)
DEPFILES         := $(SRCFILES:%.$(SRCEXT)=$(DEPDIR)/%.d)

# Target
#-----------------------------------------------------------
export TARGET    := program # Target name

# Final command
#-----------------------------------------------------------
COMPILE.c         = $(CC) $(CPPFLAGS) $(DEPFLAGS) $(CFLAGS) 
export LINK       = $(LD) $(LDFLAGS)

# Paths
#-----------------------------------------------------------
vpath %.$(INCEXT)   $(INCDIR) 
vpath %.$(SRCEXT)   $(SRCDIR) 
vpath %.o           $(OBJDIR)

# Commands
#-----------------------------------------------------------
$(TARGET): $(OBJFILES)
	make --directory=$(OBJDIR)/

%.o: %.$(SRCEXT)
%.o: %.$(SRCEXT) $(DEPDIR)/%.d | $(DEPDIR)
	@echo Compiling $@
	@$(COMPILE.c) -o $(OBJDIR)/$@ $<

$(DEPDIR): ; @mkdir -p $@

$(DEPFILES):
include $(wildcard $(DEPFILES))
