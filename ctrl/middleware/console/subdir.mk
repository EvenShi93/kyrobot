################################################################################
# subdir Makefile
# kyChu@2020-5-15
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./middleware/console/console.c \
./middleware/console/commands.c \
./middleware/console/split_argv.c

OBJS += \
$(BuildPath)/middleware/console/console.o \
$(BuildPath)/middleware/console/commands.o \
$(BuildPath)/middleware/console/split_argv.o

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/middleware/console/%.o: ./middleware/console/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
