################################################################################
# gui/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2020-4-10
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./middleware/vscn/vscn.c 

OBJS += \
$(BuildPath)/middleware/vscn/vscn.o 

C_DEPS += \
$(BuildPath)/middleware/vscn/vscn.d 

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/middleware/vscn/%.o: ./middleware/vscn/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
