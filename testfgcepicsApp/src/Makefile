TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = testfgcepics
# testfgcepics.dbd will be created and installed
DBD += testfgcepics.dbd

# testfgcepics.dbd will be made up from these files:
testfgcepics_DBD += base.dbd
testfgcepics_DBD += asyn.dbd
testfgcepics_DBD += stream.dbd
testfgcepics_DBD += drvAsynIPPort.dbd
testfgcepics_DBD += devfgcudp.dbd
testfgcepics_DBD += devfgccmd.dbd

# Include dbd files from all support applications:
#testfgcepics_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#testfgcepics_LIBS += xxx

# testfgcepics_registerRecordDeviceDriver.cpp derives from testfgcepics.dbd
testfgcepics_SRCS += testfgcepics_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
testfgcepics_SRCS_DEFAULT += testfgcepicsMain.cpp
testfgcepics_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#testfgcepics_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
testfgcepics_LIBS += devfgcudp devfgccmd
testfgcepics_LIBS += asyn stream
testfgcepics_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

