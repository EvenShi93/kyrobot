################################################################################
# subdir Makefile
# kyChu@2020-5-15
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./middleware/shell/shell.c \
./middleware/shell/split_argv.c

OBJS += \
$(BuildPath)/middleware/shell/shell.o \
$(BuildPath)/middleware/shell/split_argv.o

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/middleware/shell/%.o: ./middleware/shell/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
