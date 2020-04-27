################################################################################
# bsp/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

OBJS += \
$(BuildPath)/driver/bsp/callback.o \
$(BuildPath)/driver/bsp/comif.o \
$(BuildPath)/driver/bsp/ec20.o \
$(BuildPath)/driver/bsp/hal_msp.o \
$(BuildPath)/driver/bsp/mcu_irq.o \
$(BuildPath)/driver/bsp/gpio_irq.o \
$(BuildPath)/driver/bsp/rtcmif.o \
$(BuildPath)/driver/bsp/timebase.o \
$(BuildPath)/driver/bsp/ubloxa.o \
$(BuildPath)/driver/bsp/remoterif.o \
$(BuildPath)/driver/bsp/leds.o \
$(BuildPath)/driver/bsp/gpios.o \
$(BuildPath)/driver/bsp/buttons.o \
$(BuildPath)/driver/bsp/voltage.o \
$(BuildPath)/driver/bsp/imuif.o \
$(BuildPath)/driver/bsp/magif.o \
$(BuildPath)/driver/bsp/disp.o \
$(BuildPath)/driver/bsp/dispif.o \
$(BuildPath)/driver/bsp/steerif.o \
$(BuildPath)/driver/bsp/motorif.o \
$(BuildPath)/driver/bsp/flashif.o \
$(BuildPath)/driver/bsp/usbdif.o \
$(BuildPath)/driver/bsp/ist83xx.o \
$(BuildPath)/driver/bsp/icm42605.o \
$(BuildPath)/driver/bsp/w25qxx.o \
$(BuildPath)/driver/bsp/usb_cdcif.o \
$(BuildPath)/driver/bsp/w25_diskio.o

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/bsp/%.o: ./driver/bsp/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
