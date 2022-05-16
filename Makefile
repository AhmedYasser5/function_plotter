empty :=
space := $(empty) $(empty)

SRCDIR := ./src
INCDIR := ./include
OBJDIR := ./build/obj
DEPDIR := ./build/deps
BINDIR := .
TARGET := $(BINDIR)/$(subst $(space),_,$(shell basename "${PWD}")).exe

MY_PATHS := $(BINDIR) $(INCDIR)
MY_FLAGS := 

###### extra variables #######
MY_PATHS += $(shell cat .my_paths 2>/dev/null)
MY_FLAGS += $(shell cat .my_flags 2>/dev/null)

###### complier set-up ######
CC = gcc
CFLAGS = $(MY_FLAGS) -Wextra
CXX = g++
CXXFLAGS = $(CFLAGS)
LD = g++
LDFLAGS = $(CXXFLAGS)
DEBUGGER = gdb

maketype :=

ifeq ($(RELEASE), 1)
	maketype += RELEASE
	CFLAGS += -O3 -march=native
	CXXFLAGS += -std=c++17
	LDFLAGS += -flto=full
else ifeq ($(DEBUG), 1)
	maketype += DEBUG
	CFLAGS += -O0 -g
else
	maketype += NORMAL
	CFLAGS += -O0
	CXXFLAGS += -O0 -std=c++14
endif

CFLAGS += -MMD -MP -I$(SRCDIR) $(foreach i,$(MY_PATHS),-I$(i))

SRCS := $(wildcard $(SRCDIR)/**/*.cpp)
SRCS += $(wildcard $(SRCDIR)/*.cpp)
SRCS += $(wildcard $(SRCDIR)/**/*.c)
SRCS += $(wildcard $(SRCDIR)/*.c)
DEPS := $(patsubst $(SRCDIR)/%,$(DEPDIR)/%.d,$(SRCS))
OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%.o,$(SRCS))

.PHONY: all
all : $(TARGET)

.PHONY: init
init :
	-@rm -rf build $(wildcard *.exe)
	@mkdir -p $(SRCDIR) $(INCDIR) $(OBJDIR) $(DEPDIR)
	-@for i in $(wildcard *.cpp) $(wildcard *.c) $(wildcard *.tpp); do mv ./$$i $(SRCDIR)/$$i; done
	-@for i in $(wildcard *.h); do mv ./$$i $(INCDIR)/$$i; done
	-@echo -e "-DDEBUG$(foreach i,$(MY_PATHS),\n-I../$(i)\n-I$(i))" >| src/.clang_complete

$(TARGET): $(OBJS)
	-@echo LD $(maketype) "$(<D)/*.o" "->" $@ && \
		$(LD) -o $@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.cpp.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR) $(DEPDIR)
	-@echo CXX $(maketype) $< "->" $@ && \
		$(CXX) -c $< -o $@ -MF $(DEPDIR)/$(<F).d $(CXXFLAGS)

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR) $(DEPDIR)
	-@echo CC $(maketype) $< "->" $@ && \
		$(CC) -c $< -o $@ -MF $(DEPDIR)/$(<F).d $(CFLAGS)

.PHONY: clean
clean: 
	-$(RM) $(OBJS) $(DEPS) $(TARGET)

.PHONY: debug
debug: $(TARGET)
	$(DEBUGGER) $(TARGET)

-include $(DEPS)
