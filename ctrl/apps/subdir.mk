################################################################################
# apps/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./apps/src/apps.c \
./apps/src/f9pconfig.c \
./apps/src/gnss.c \
./apps/src/server.c \
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

OBJS += \
$(BuildPath)/apps/src/apps.o \
$(BuildPath)/apps/src/f9pconfig.o \
$(BuildPath)/apps/src/gnss.o \
$(BuildPath)/apps/src/server.o \
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

C_DEPS += \
$(BuildPath)/apps/src/apps.d \
$(BuildPath)/apps/src/f9pconfig.d \
$(BuildPath)/apps/src/gnss.d \
$(BuildPath)/apps/src/server.d \
$(BuildPath)/apps/src/compass.d \
$(BuildPath)/apps/src/gui_task.d \
$(BuildPath)/apps/src/test_case.d \
$(BuildPath)/apps/src/att_est_q.d \
$(BuildPath)/apps/src/cpu_utils.d \
$(BuildPath)/apps/src/filesystem.d \
$(BuildPath)/apps/src/filetransfer.d \
$(BuildPath)/apps/src/mesg.d

C_DEPS += \
$(BuildPath)/apps/gui/gui_drv.d \
$(BuildPath)/apps/gui/GUIConf.d \
$(BuildPath)/apps/gui/LCDConf.d \
$(BuildPath)/apps/gui/gui_x_rtos.d

C_DEPS += \
$(BuildPath)/apps/demo/GUIDEMO_Automotive.d \
$(BuildPath)/apps/demo/GUIDEMO_Bitmap.d \
$(BuildPath)/apps/demo/GUIDEMO.d \
$(BuildPath)/apps/demo/GUIDEMO_ColorBar.d \
$(BuildPath)/apps/demo/GUIDEMO_Conf.d \
$(BuildPath)/apps/demo/GUIDEMO_Cursor.d \
$(BuildPath)/apps/demo/GUIDEMO_Graph.d \
$(BuildPath)/apps/demo/GUIDEMO_IconView.d \
$(BuildPath)/apps/demo/GUIDEMO_Intro.d \
$(BuildPath)/apps/demo/GUIDEMO_Listview.d \
$(BuildPath)/apps/demo/GUIDEMO_Resource.d \
$(BuildPath)/apps/demo/GUIDEMO_Speed.d \
$(BuildPath)/apps/demo/GUIDEMO_Start.d \
$(BuildPath)/apps/demo/GUIDEMO_TransparentDialog.d \
$(BuildPath)/apps/demo/GUIDEMO_Treeview.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/apps/%.o: ./apps/%.c | $(OBJ_DIRS)
	$(ECHO) ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
