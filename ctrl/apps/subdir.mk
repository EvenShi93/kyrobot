################################################################################
# apps/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OBJS += \
$(BuildPath)/apps/src/apps.o \
$(BuildPath)/apps/src/compass.o \
$(BuildPath)/apps/src/test_case.o \
$(BuildPath)/apps/src/att_est_q.o \
$(BuildPath)/apps/src/cpu_utils.o \
$(BuildPath)/apps/src/filesystem.o \
$(BuildPath)/apps/src/filetransfer.o \
$(BuildPath)/apps/src/mesg.o \
$(BuildPath)/apps/src/sd_diskio.o

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
$(BuildPath)/apps/evt/evt_proc.o \
$(BuildPath)/apps/evt/remote_task.o

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/apps/%.o: ./apps/%.c | $(OBJ_DIRS)
	$(ECHO) ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
