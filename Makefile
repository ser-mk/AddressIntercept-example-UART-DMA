# App name
#-------------------------------------------------------------------------------
TARGET  = test

# peripherial driver
#-------------------------------------------------------------------------------
# PERIPHDRIVERS += stm32f10x_adc
# PERIPHDRIVERS += stm32f10x_bkp
# PERIPHDRIVERS += stm32f10x_can
# PERIPHDRIVERS += stm32f10x_cec
# PERIPHDRIVERS += stm32f10x_crc
# PERIPHDRIVERS += stm32f10x_dbgmcu
# PERIPHDRIVERS += stm32f10x_exti
# PERIPHDRIVERS += stm32f10x_flash
# PERIPHDRIVERS += stm32f10x_fsmc
PERIPHDRIVERS += stm32f10x_gpio
# PERIPHDRIVERS += stm32f10x_i2c
# PERIPHDRIVERS += stm32f10x_iwdg
# PERIPHDRIVERS += stm32f10x_pwr
PERIPHDRIVERS += stm32f10x_rcc
# PERIPHDRIVERS += stm32f10x_rtc
# PERIPHDRIVERS += stm32f10x_sdio
# PERIPHDRIVERS += stm32f10x_spi
# PERIPHDRIVERS += stm32f10x_tim
PERIPHDRIVERS += stm32f10x_usart
# PERIPHDRIVERS += stm32f10x_wwdg
PERIPHDRIVERS += stm32f10x_dma
# PERIPHDRIVERS += misc.c

# USE DEFINES
#-------------------------------------------------------------------------------
DEFINES += USE_STDPERIPH_DRIVER
DEFINES += STM32F10X_MD

CC = gcc
LD = gcc
CP = objcopy
SZ = size


RM = rm
# PATH to CMSIS and StdPeriph Lib
#-------------------------------------------------------------------------------
CMSIS_PATH         = cmsis
STDPERIPH_INC_PATH = stdperiph/inc
STDPERIPH_SRC_PATH = stdperiph/src

# startup file
#-------------------------------------------------------------------------------

# PATH SOURCE
#-------------------------------------------------------------------------------
SOURCEDIRS := src
SOURCEDIRS += $(CMSIS_PATH)

# PATH HEADERS
#-------------------------------------------------------------------------------
INCLUDES += .
INCLUDES += $(SOURCEDIRS) 
INCLUDES += $(CMSIS_PATH)
INCLUDES += $(STDPERIPH_INC_PATH)

# EXTRA LIBRARY
#-------------------------------------------------------------------------------
LIBPATH +=
LIBS    +=

# TOOLCHAIN OPTIONS
#-------------------------------------------------------------------------------
CFLAGS += -m32
CFLAGS += -g
CFLAGS += -Wall -pedantic         # Выводить все предупреждения
CFLAGS += -Os                     # Оптимизация
CFLAGS += -ggdb                   # Генерировать отладочную информацию для gdb

CFLAGS += $(addprefix -I, $(INCLUDES))
CFLAGS += $(addprefix -D, $(DEFINES))

LDFLAGS += $(addprefix -L, $(LIBPATH))
LDFLAGS += $(LIBS)
LDFLAGS += -m32

# OBJECT FILES
#-------------------------------------------------------------------------------
OBJS += $(patsubst %.c, %.o, $(wildcard  $(addsuffix /*.c, $(SOURCEDIRS))))
OBJS += $(addprefix $(STDPERIPH_SRC_PATH)/, $(addsuffix .o, $(PERIPHDRIVERS)))
OBJS += $(patsubst %.s, %.o, $(STARTUP))

# list of directories that make should search
#-------------------------------------------------------------------------------
VPATH := $(SOURCEDIRS)

# temp file
#-------------------------------------------------------------------------------
TOREMOVE += *.elf *.hex
TOREMOVE += $(addsuffix /*.o, $(SOURCEDIRS))
TOREMOVE += $(addsuffix /*.d, $(SOURCEDIRS))
TOREMOVE += $(STDPERIPH_SRC_PATH)/*.o
TOREMOVE += $(patsubst %.s, %.o, $(STARTUP))
TOREMOVE += $(TARGET)

# general build
#-------------------------------------------------------------------------------
all: $(TARGET).elf size

# Очистка
#-------------------------------------------------------------------------------
clean:
	@$(RM) -f $(TOREMOVE)  

# show size
#-------------------------------------------------------------------------------
size:
	@echo "---------------------------------------------------"
	@$(SZ) $(TARGET).elf

# linking
#------------------------------------------------------------------------------- 
$(TARGET).elf: $(OBJS)
	@$(LD) $(LDFLAGS) $^ -o $@

# compile
#------------------------------------------------------------------------------- 
%.o: %.c
	@$(CC) $(CFLAGS) -MD -c $< -o $@
	
%.o: %.s
	@$(AS) $(AFLAGS) -c $< -o $@

# create d-files
#-------------------------------------------------------------------------------
include $(wildcart *.d)
