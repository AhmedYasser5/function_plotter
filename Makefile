empty :=
space := $(empty) $(empty)

SRCDIR := ./src
INCDIR := ./include
OBJDIR := ./build/obj
DEPDIR := ./build/deps
BINDIR := .
TARGET := $(BINDIR)/$(subst $(space),_,$(shell basename "${PWD}")).exe

###### extra variables #######

###### complier set-up ######
CC = gcc
CFLAGS = -Wextra
CXX = g++
CXXFLAGS = $(CFLAGS)
LD = g++
LDFLAGS = $(CXXFLAGS)
DEBUGGER = gdb

ifdef RELEASE
	maketype := RELEASE
	CFLAGS += -O3 -march=native
	CXXFLAGS += -std=c++17
	LDFLAGS += -flto=full
else ifdef DEBUG
	maketype := DEBUG
	CFLAGS += -O0 -g
else ifdef THREADS
	maketype := THREADS
	CFLAGS += -O2 -march=native -pthread
	CXXFLAGS += -std=c++17
	LDFLAGS += -flto=full
else
	maketype := NORMAL
	CFLAGS += -O0
	CXXFLAGS += -O0 -std=c++14
endif

OUTPUT_OPTION = -I $(INCDIR) -I $(SRCDIR) -I $(BINDIR) -MMD -MP

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
	-@echo -e "-DDEBUG\n-I../$(BINDIR)\n-I../$(INCDIR)" >| src/.clang_complete
	-@cp /home/ahmed/opt/compile_and_run.sh ./

$(TARGET): $(OBJS)
	-@echo LD $(maketype) "$(<D)/*.o" "->" $@ && $(LD) $(LDFLAGS) $(OBJS) -o $@

$(OBJDIR)/%.cpp.o: $(SRCDIR)/%.cpp
	-@echo CXX $(maketype) $< "->" $@ && \
		$(CXX) $(CXXFLAGS) -c $< $(OUTPUT_OPTION) \
		-o $@ -MF $(DEPDIR)/$(<F).d

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c
	-@echo CC $(maketype) $< "->" $@ && \
		$(CC) $(CFLAGS) -c $< $(OUTPUT_OPTION) \
		-o $@ -MF $(DEPDIR)/$(<F).d

.PHONY: clean
clean: 
	-$(RM) $(OBJS) $(DEPS) $(TARGET)

.PHONY: debug
debug: $(TARGET)
	$(DEBUGGER) $(TARGET)

-include $(DEPS)
