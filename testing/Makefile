SRCDIR1 := ./src
SRCDIR2 := ../src
INCDIR1 := ./include
INCDIR2 := ../include
OBJDIR := ./build/obj
DEPDIR := ./build/deps
BINDIR := .

TARGET := $(BINDIR)/Testing_Function_Plotter_
ifeq ($(RELEASE), 1)
	TARGET := $(TARGET)Release
else
	TARGET := $(TARGET)Debug
endif
TARGET := $(TARGET).exe

MY_PATHS := $(BINDIR) $(INCDIR1) $(INCDIR2)
MY_FLAGS := 

###### complier set-up ######
CC = gcc
CFLAGS = $(MY_FLAGS) -Wextra -Wno-unused-result -std=c17
LD = g++
LDFLAGS = $(CFLAGS)
DEBUGGER = gdb

ifeq ($(RELEASE), 1)
	maketype := RELEASE
	CFLAGS += -O2 -ftree-vectorize -fomit-frame-pointer -march=native
	# Link Time Optimization
	CFLAGS += -flto
else
	maketype := DEBUG
	CFLAGS += -Og -ggdb2 -DDEBUG=1
	# Overflow protection
	CFLAGS += -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fstack-clash-protection -fcf-protection
	CFLAGS += -Wl,-z,defs -Wl,-z,now -Wl,-z,relro
endif

CFLAGS += -MMD -MP -I$(SRCDIR1) -I$(SRCDIR2) $(foreach i,$(MY_PATHS),-I$(i))

SRCS1 := $(wildcard $(SRCDIR1)/*.c)
DEPS := $(patsubst $(SRCDIR1)/%,$(DEPDIR)/%1.d,$(SRCS1))
OBJS := $(patsubst $(SRCDIR1)/%,$(OBJDIR)/%1.$(maketype).o,$(SRCS1))

SRCS2 := $(filter-out $(SRCDIR2)/main.c $(SRCDIR2)/gui_handler.c,$(wildcard $(SRCDIR2)/*.c))
DEPS += $(patsubst $(SRCDIR2)/%,$(DEPDIR)/%2.d,$(SRCS2))
OBJS += $(patsubst $(SRCDIR2)/%,$(OBJDIR)/%2.$(maketype).o,$(SRCS2))

CREATE = mkdir -p $(OBJDIR) $(DEPDIR)
COMPILE = echo CC $(maketype) $(1) "->" $(2) && \
					$(CC) -c $(1) -o $(2) -MF $(DEPDIR)/$(3).d $(CFLAGS)

.PHONY: all
all : $(TARGET)

.PHONY: getTarget
getTarget :
	@echo $(TARGET)

.PHONY: run
run : $(TARGET)
	@$(TARGET) $(ARGUMENTS)
	@echo

.PHONY: init
init :
	-@rm -rf build $(wildcard *.exe)
	@mkdir -p $(SRCDIR1) $(INCDIR1) $(OBJDIR) $(DEPDIR)
	-@for i in $(wildcard *.c); do mv ./$$i $(SRCDIR1)/$$i; done
	-@for i in $(wildcard *.h); do mv ./$$i $(INCDIR1)/$$i; done
	-@$(file >$(SRCDIR1)/.clang_complete)\
		$(foreach i,$(MY_PATHS),\
			$(file >>$(SRCDIR1)/.clang_complete,-I$(i))\
			$(file >>$(SRCDIR1)/.clang_complete,-I../$(i)))

$(TARGET) : $(OBJS)
	-@echo LD $(maketype) "$(<D)/*.o" "->" $@ && \
		$(LD) -o $@ $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.c1.$(maketype).o : $(SRCDIR1)/%.c
	@$(call CREATE)
	-@$(call COMPILE,$<,$@,$(<F))

$(OBJDIR)/%.c2.$(maketype).o : $(SRCDIR2)/%.c
	@$(call CREATE)
	-@$(call COMPILE,$<,$@,$(<F))

.PHONY: clean
clean : 
	-$(RM) $(OBJS) $(DEPS) $(TARGET)

.PHONY: debug
debug : $(TARGET)
	$(DEBUGGER) $(TARGET)

-include $(DEPS)
