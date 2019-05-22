EXCLUDE_ARCHS += eldk
EXCLUDE_VERSIONS += 3.14

include $(EPICS_ENV_PATH)/module.Makefile

USR_DEPENDENCIES += asyn,4.33.0-konrad
USR_DEPENDENCIES += streamdevice,2.7.11+

USR_CXXFLAGS += -std=c++11
USR_CFLAGS += -std=c99

#SOURCES = -none-
HEADERS += fgcudpSup/src/common/inc/pub_data.h

MISCS = $(AUTOMISCS)
#MISCS += misc/ppt-freia.req
STARTUPS = $(wildcard startup/*.cmd)

OPIS = opi
