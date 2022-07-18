SRCDIR := ./src
INCDIR := ./include
OBJDIR := ./build/obj
DEPDIR := ./build/deps
BINDIR := .

TARGET := $(BINDIR)/Function_Plotter_
ifeq ($(RELEASE), 1)
	TARGET := $(TARGET)Release
else
	TARGET := $(TARGET)Debug
endif
TARGET := $(TARGET).exe

MY_PATHS := $(BINDIR) $(INCDIR) $(shell cat .my_paths 2>/dev/null)
MY_FLAGS := -rdynamic $(shell pkg-config --cflags --libs gtk+-3.0)

###### complier set-up ######
CC = gcc
CFLAGS = $(MY_FLAGS) -Wextra
CXX = g++
CXXFLAGS = $(CFLAGS)
LD = g++
LDFLAGS = $(CXXFLAGS)
DEBUGGER = gdb

ifeq ($(RELEASE), 1)
	maketype := RELEASE
	CFLAGS += -O3 -march=native
	CXXFLAGS += -std=c++17
	LDFLAGS += -flto=full
else
	maketype := DEBUG
	CFLAGS += -O0 -g -DDEBUG=1
	CXXFLAGS += -std=c++17
endif

CFLAGS += -MMD -MP -I$(SRCDIR) $(foreach i,$(MY_PATHS),-I$(i))

SRCS := $(wildcard $(SRCDIR)/**/*.cpp)
SRCS += $(wildcard $(SRCDIR)/*.cpp)
SRCS += $(wildcard $(SRCDIR)/**/*.c)
SRCS += $(wildcard $(SRCDIR)/*.c)
DEPS := $(patsubst $(SRCDIR)/%,$(DEPDIR)/%.d,$(SRCS))
OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%.$(maketype).o,$(SRCS))

.PHONY: all
all : $(TARGET)

.PHONY: run
run : $(TARGET)
	@$(TARGET)
	@echo

.PHONY: init
init :
	-@rm -rf build $(wildcard *.exe)
	@mkdir -p $(SRCDIR) $(INCDIR) $(OBJDIR) $(DEPDIR)
	-@for i in $(wildcard *.cpp) $(wildcard *.c) $(wildcard *.tpp); do mv ./$$i $(SRCDIR)/$$i; done
	-@for i in $(wildcard *.h) $(wildcard *.hpp); do mv ./$$i $(INCDIR)/$$i; done
	-@echo -e "$(foreach i,$(MY_PATHS),-I../$(i)\n-I$(i)\n)" >| src/.clang_complete

$(TARGET): $(OBJS)
	-@echo LD $(maketype) "$(<D)/*.o" "->" $@ && \
		$(LD) -o $@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.cpp.$(maketype).o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR) $(DEPDIR)
	-@echo CXX $(maketype) $< "->" $@ && \
		$(CXX) -c $< -o $@ -MF $(DEPDIR)/$(<F).d $(CXXFLAGS)

$(OBJDIR)/%.c.$(maketype).o: $(SRCDIR)/%.c
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
