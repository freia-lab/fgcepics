TOP=../..
include $(TOP)/configure/CONFIG
#=======================================

# Build devfgcudp as a library for an IOC:
LIBRARY_IOC += devfgccmd

# Library Source files
devfgccmd_SRCS += asubroutine.c

# Link with the asyn and base libraries
devfgccmd_LIBS += streamdevice asyn
devfgccmd_LIBS += $(EPICS_BASE_IOC_LIBS)

# Install .dbd and .db files
DBD += devfgccmd.dbd

#=======================================
include $(TOP)/configure/RULES
