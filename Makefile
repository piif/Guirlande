# ARDDUDE_DIR is defined by eclipse CDT config
$(info ARDDUDE_DIR=${ARDDUDE_DIR})

all: bin

include ${ARDDUDE_DIR}/etc/tools.mk

CALLER_DIR := $(call truepath,$(dir $(firstword ${MAKEFILE_LIST})))
$(info CALLER_DIR = ${CALLER_DIR})

UPLOAD_PORT ?= /dev/ttyACM0
TARGET_BOARD ?= uno

include ${ARDDUDE_DIR}/etc/main.mk

ARD_TOOLS_DIR := $(call truepath,../ArduinoTools)
ARD_LIBS_DIR := $(call truepath,../ArduinoLibs)
#LIBRARIES_DIRS := ${ARD_LIBS_DIR} ${ARD_TOOLS_DIR}
INCLUDE_FLAGS_EXTRA += $(addprefix -I,${ARD_LIBS_DIR} ${ARD_TOOLS_DIR} .)
LDFLAGS_EXTRA += -L${ARD_TOOLS_DIR}/target/${TARGET_BOARD} -lArduinoTools
LDFLAGS_EXTRA += -L${ARD_LIBS_DIR}/target/${TARGET_BOARD} -lArduinoLibs
