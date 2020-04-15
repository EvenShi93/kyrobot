################################################################################
# apps/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./apps/src/apps.c \
./apps/src/compass.c \
./apps/src/gui_task.c \
./apps/src/test_case.c \
./apps/src/att_est_q.c \
./apps/src/cpu_utils.c \
./apps/src/filesystem.c \
./apps/src/filetransfer.c \
./apps/src/mesg.c

C_SRCS += \
./apps/gui/gui_drv.c \
./apps/gui/GUIConf.c \
./apps/gui/LCDConf.c \
./apps/gui/gui_x_rtos.c

C_SRCS += \
./apps/demo/GUIDEMO_Automotive.c \
./apps/demo/GUIDEMO_Bitmap.c \
./apps/demo/GUIDEMO.c \
./apps/demo/GUIDEMO_ColorBar.c \
./apps/demo/GUIDEMO_Conf.c \
./apps/demo/GUIDEMO_Cursor.c \
./apps/demo/GUIDEMO_Graph.c \
./apps/demo/GUIDEMO_IconView.c \
./apps/demo/GUIDEMO_Intro.c \
./apps/demo/GUIDEMO_Listview.c \
./apps/demo/GUIDEMO_Resource.c \
./apps/demo/GUIDEMO_Speed.c \
./apps/demo/GUIDEMO_Start.c \
./apps/demo/GUIDEMO_TransparentDialog.c \
./apps/demo/GUIDEMO_Treeview.c

C_SRCS += $(wildcard ./apps/evt/*.c)

OBJS += \
$(BuildPath)/apps/src/apps.o \
$(BuildPath)/apps/src/compass.o \
$(BuildPath)/apps/src/gui_task.o \
$(BuildPath)/apps/src/test_case.o \
$(BuildPath)/apps/src/att_est_q.o \
$(BuildPath)/apps/src/cpu_utils.o \
$(BuildPath)/apps/src/filesystem.o \
$(BuildPath)/apps/src/filetransfer.o \
$(BuildPath)/apps/src/mesg.o

OBJS += \
$(BuildPath)/apps/gui/gui_drv.o \
$(BuildPath)/apps/gui/GUIConf.o \
$(BuildPath)/apps/gui/LCDConf.o \
$(BuildPath)/apps/gui/gui_x_rtos.o

OBJS += \
$(BuildPath)/apps/demo/GUIDEMO_Automotive.o \
$(BuildPath)/apps/demo/GUIDEMO_Bitmap.o \
$(BuildPath)/apps/demo/GUIDEMO.o \
$(BuildPath)/apps/demo/GUIDEMO_ColorBar.o \
$(BuildPath)/apps/demo/GUIDEMO_Conf.o \
$(BuildPath)/apps/demo/GUIDEMO_Cursor.o \
$(BuildPath)/apps/demo/GUIDEMO_Graph.o \
$(BuildPath)/apps/demo/GUIDEMO_IconView.o \
$(BuildPath)/apps/demo/GUIDEMO_Intro.o \
$(BuildPath)/apps/demo/GUIDEMO_Listview.o \
$(BuildPath)/apps/demo/GUIDEMO_Resource.o \
$(BuildPath)/apps/demo/GUIDEMO_Speed.o \
$(BuildPath)/apps/demo/GUIDEMO_Start.o \
$(BuildPath)/apps/demo/GUIDEMO_TransparentDialog.o \
$(BuildPath)/apps/demo/GUIDEMO_Treeview.o

OBJS += \
$(BuildPath)/apps/evt/evt_proc.o

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/apps/%.o: ./apps/%.c | $(OBJ_DIRS)
	$(ECHO) ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
