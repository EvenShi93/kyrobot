################################################################################
# Project Makefile
# kyChu@2020-4-15
################################################################################

#target directory
BUILD := build
#target name
MODULE_NAME := libfatfs
#source files
C_SRCS := $(wildcard *.c)
C_SRCS += option/syscall.c
C_SRCS += option/unicode.c
#objects
OBJS := $(addprefix $(BUILD)/,$(C_SRCS:.c=.o))
#ibject directories
OBJ_DIRS := $(sort $(dir $(OBJS)))
#target
all: $(MODULE_NAME).a

$(OBJ_DIRS):
	$(MKDIR) -p $@

$(BUILD)/%.o: %.c | $(OBJ_DIRS)
	$(ECHO) ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"

$(MODULE_NAME).a: $(OBJS)
	$(ECHO) ' AR $@'
	$(AR) -rcs $(BUILD)/$@ $(OBJS)

.PHONY: $(MODULE_NAME).a
