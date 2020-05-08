################################################################################
# periph/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2020-5-8
################################################################################

OBJS += \
$(BuildPath)/driver/periph/periph.o \
$(BuildPath)/driver/periph/usart2.o \
$(BuildPath)/driver/periph/usart6.o

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/periph/%.o: ./driver/periph/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
