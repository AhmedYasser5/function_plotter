SRCDIR := src
INCDIR := include
OBJDIR := build/obj
DEPDIR := build/deps
BINDIR1 := .
BINDIR2 := ..

TARGET := $(BINDIR1)/Testing_Function_Plotter_
ifeq ($(RELEASE), 1)
	TARGET := $(TARGET)Release
else
	TARGET := $(TARGET)Debug
endif
TARGET := $(TARGET).exe

MY_FLAGS := -I$(BINDIR1) -I$(INCDIR) -I$(BINDIR2)/$(INCDIR)

CC = clang
CFLAGS = $(MY_FLAGS) -Wextra -Wno-unused-result -Wno-unused-command-line-argument -std=c17
LD = clang++
LDFLAGS = $(CFLAGS)
DEBUGGER = lldb

ifeq ($(RELEASE), 1)
	maketype := RELEASE
	CFLAGS += -O2 -ftree-vectorize -fomit-frame-pointer -march=native
	# Link Time Optimization
	CFLAGS += -flto
else
	maketype := DEBUG
	CFLAGS += -Og -ggdb3 -DDEBUG=1
	# Overflow protection
	CFLAGS += -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fstack-clash-protection -fcf-protection
	CFLAGS += -Wl,-z,defs -Wl,-z,now -Wl,-z,relro
	CFLAGS += -fsanitize=address -fsanitize=undefined -fsanitize=leak
endif

CFLAGS += -MMD -MP

SRCS := $(wildcard $(BINDIR2)/$(SRCDIR)/*.c)
SRCS += $(wildcard $(BINDIR2)/$(SRCDIR)/**/*.c)
SRCS := $(filter-out $(BINDIR2)/$(SRCDIR)/main.c $(BINDIR2)/$(SRCDIR)/gui_handler.c,$(SRCS))

OBJS := $(patsubst $(BINDIR2)/$(SRCDIR)/%,$(BINDIR2)/$(OBJDIR)/%.$(maketype).o,$(SRCS))
DEPS := $(patsubst $(BINDIR2)/$(SRCDIR)/%,$(BINDIR2)/$(DEPDIR)/%.d,$(SRCS))

SRCS := $(wildcard $(SRCDIR)/*.c)
SRCS += $(wildcard $(SRCDIR)/**/*.c)

OBJS += $(patsubst $(SRCDIR)/%,$(OBJDIR)/%.$(maketype).o,$(SRCS))
DEPS += $(patsubst $(SRCDIR)/%,$(DEPDIR)/%.d,$(SRCS))

.PHONY: all
all : $(TARGET)

.PHONY: getTarget
getTarget :
	@echo $(TARGET)

.PHONY: run
run : $(TARGET)
	@echo --------------------------------------------------
	@\time -f "\n--------------------------------------------------\nElapsed Time: %e sec\nCPU Percentage: %P\n"\
		$(TARGET) $(ARGUMENTS)

.PHONY: init
init :
	-@rm -rf build $(wildcard *.exe)
	@mkdir -p $(SRCDIR) $(INCDIR) $(OBJDIR) $(DEPDIR)
	@mkdir -p $(BINDIR2)/$(SRCDIR) $(BINDIR2)/$(INCDIR) $(BINDIR2)/$(OBJDIR) $(BINDIR2)/$(DEPDIR)
	@for i in $(wildcard *.c); do mv ./$$i $(SRCDIR)/$$i; done
	@for i in $(wildcard *.h); do mv ./$$i $(INCDIR)/$$i; done
	@$(file >compile_flags.txt)
	@$(foreach i,$(CFLAGS),$(file >>compile_flags.txt,$(i)))

$(TARGET) : $(OBJS)
	-@echo LD $(maketype) "ALL ->" $@ && \
		$(LD) -o $@ $(OBJS) $(LDFLAGS)

CREATE = mkdir -p $(1) $(dir $(2))
COMPILE = echo CC $(maketype) $(1) "->" $(2) && \
						$(CC) -c $(1) -o $(2) -MF $(3) $(CFLAGS)

$(OBJDIR)/%.c.$(maketype).o : $(SRCDIR)/%.c
	@$(eval CUR_DEP := $(patsubst $(SRCDIR)/%,$(DEPDIR)/%.d,$<))
	@$(call CREATE,$(@D),$(CUR_DEP))
	-@$(call COMPILE,$<,$@,$(CUR_DEP))

$(BINDIR2)/$(OBJDIR)/%.c.$(maketype).o : $(BINDIR2)/$(SRCDIR)/%.c
	@$(eval CUR_DEP := $(patsubst $(BINDIR2)/$(SRCDIR)/%,$(BINDIR2)/$(DEPDIR)/%.d,$<))
	@$(call CREATE,$(@D),$(CUR_DEP))
	-@$(call COMPILE,$<,$@,$(CUR_DEP))

.PHONY: clean
clean : 
	-$(RM) $(OBJS) $(DEPS) $(TARGET)

.PHONY: debug
debug : $(TARGET)
	@export ASAN_OPTIONS=detect_leaks=0; \
		$(DEBUGGER) $(TARGET)

-include $(DEPS)
