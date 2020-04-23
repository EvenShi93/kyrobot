################################################################################
# apps/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./apps/src/apps.c \
./apps/src/compass.c \
./apps/src/test_case.c \
./apps/src/att_est_q.c \
./apps/src/cpu_utils.c \
./apps/src/filesystem.c \
./apps/src/filetransfer.c \
./apps/src/mesg.c

C_SRCS += \
./apps/guicfg/gui_drv.c \
./apps/guicfg/GUIConf.c \
./apps/guicfg/LCDConf.c \
./apps/guicfg/gui_x_rtos.c

C_SRCS += \
./apps/ctrl/ctrl_task.c

ifeq ($(CONFIG_STEMWIN_DEMO_ENABLE), y)
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
else
C_SRCS += \
./apps/gui/gui_task.c \
./apps/gui/gui_graph.c \
./apps/gui/gui_iconview.c \
./apps/gui/gui_config_bl.c \
./apps/gui/gui_ctrl_motor.c \
./apps/gui/gui_ctrl_steer.c \
./apps/gui/gui_setting_meu.c
endif

C_SRCS += $(wildcard ./apps/evt/*.c)

OBJS += \
$(BuildPath)/apps/src/apps.o \
$(BuildPath)/apps/src/compass.o \
$(BuildPath)/apps/src/test_case.o \
$(BuildPath)/apps/src/att_est_q.o \
$(BuildPath)/apps/src/cpu_utils.o \
$(BuildPath)/apps/src/filesystem.o \
$(BuildPath)/apps/src/filetransfer.o \
$(BuildPath)/apps/src/mesg.o

OBJS += \
$(BuildPath)/apps/guicfg/gui_drv.o \
$(BuildPath)/apps/guicfg/GUIConf.o \
$(BuildPath)/apps/guicfg/LCDConf.o \
$(BuildPath)/apps/guicfg/gui_x_rtos.o

OBJS += \
$(BuildPath)/apps/ctrl/ctrl_task.o

ifeq ($(STEMWIN_DEMO_ENABLE), y)
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
else
OBJS += \
$(BuildPath)/apps/gui/gui_task.o \
$(BuildPath)/apps/gui/gui_graph.o \
$(BuildPath)/apps/gui/gui_iconview.o \
$(BuildPath)/apps/gui/gui_config_bl.o \
$(BuildPath)/apps/gui/gui_ctrl_motor.o \
$(BuildPath)/apps/gui/gui_ctrl_steer.o \
$(BuildPath)/apps/gui/gui_setting_menu.o
endif

OBJS += \
$(BuildPath)/apps/evt/evt_proc.o

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/apps/%.o: ./apps/%.c | $(OBJ_DIRS)
	$(ECHO) ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
