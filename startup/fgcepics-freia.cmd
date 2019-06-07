######## Set env variables ########

#----------------------------------------------------------------------------------#
# Notes:                                                                           #
#  - Max array size must be increased, to accomodate large FGC commands/responses. #
#----------------------------------------------------------------------------------#

#epicsEnvSet("STREAM_PROTOCOL_PATH", "$(TOP)/db")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES", "1000000")
epicsEnvSet("PV_PREFIX", "MAG-FGC")



######## Register support components ########

#dbLoadDatabase "dbd/testfgcepics.dbd"
#testfgcepics_registerRecordDeviceDriver pdbbase



######## Open command/response connection ########

#-------------------------------------------------------------#
# Notes:                                                      #
#  - Open 1 connection per each FGC (for maximum performance).#
#  - Recomend using the 'drvAsynIPPortConfigure' driver.      #
#-------------------------------------------------------------#

drvAsynIPPortConfigure ("fgc_port_1" ,"localhost:1906", 0, 0, 0)
#drvAsynIPPortConfigure ("fgc_port_2" ,"localhost:1906", 0, 0, 0)
#drvAsynIPPortConfigure ("fgc_port_3" ,"localhost:1906", 0, 0, 0)
#drvAsynIPPortConfigure ("fgc_port_4" ,"localhost:1906", 0, 0, 0)



######## Start UDP published data driver ########

#--------------------#
# Parameters:        #
#  1) port_name      #
#  2) local_udp_port #
#  3) timeout        #
#--------------------#

devFgcUdpConfig("fgc_udp", 2906)
devFgcUdpRegisterHost("fgc_udp", "cbox1-0", "0x00ff", 5.0)


######## Configure UDP published data driver ########

#----------------------------------------------------------------------------#
# Notes:                                                                     #
#  - Call 1 per each FGC.                                                    #
#  - Class #0 is a placeholder for the properties that common to all classes.#
#                                                                            #
# Parameters:                                                                #
#  1) port_name                                                              #
#  2) fgc_id                                                                 #
#  3) fgc_class                                                              #
#----------------------------------------------------------------------------#

devFgcUdpRegisterDev("cbox1-0", 2, 63, "FGC.2")
#devFgcUdpRegisterDev("pcte23185", 4, 0,  "FGC_4")



######## Debug ########

#asynSetTraceMask("fgc_udp",-1,0xff)
#asynSetTraceIOMask("fgc_udp",-1,0x2)



######## Load record instances - FGC udp ########

dbLoadRecords("fgcudp_class_63.db",  "PORT=fgc_udp,HOST=MAG-,DEV=FGC1:,FGC=FGC.2")
#dbLoadRecords("fgcudp_class_base.db","PORT=fgc_udp,HOST=,DEV=FGC_4:,FGC=FGC_4")


######## Load record instances - FGc cmd ########

dbLoadRecords("fgccmd_class_63.db",  "PORT=fgc_port_1,HOST=MAG-,DEV=FGC1:,FGC=FGC.2")
#dbLoadRecords("fgccmd_class_63.db",  "PORT=fgc_port_2,HOST=,DEV=FGC_2:,FGC=2")
#dbLoadRecords("fgccmd_class_63.db",  "PORT=fgc_port_3,HOST=,DEV=FGC_3:,FGC=3")
#dbLoadRecords("fgccmd_class_63.db",  "PORT=fgc_port_4,HOST=,DEV=FGC_4:,FGC=4")


######## Start IOC ########

iocInit
dbpf $(PV_PREFIX)1:REF:TABLE:FUNCTION:G.PROC 1
#dbpf $(PV_PREFIX)1:REF:TABLE:FUNCTION:G:READ.PROC 1
#dbpf $(PV_PREFIX)1:REF:TABLE:FUNCTION:REF.PROC 1
#dbpf $(PV_PREFIX)1:REF:TABLE:FUNCTION:REF.TIME 1
dbpf $(PV_PREFIX)1:REF:VCV:VALUE:S.FLNK $(PV_PREFIX)1:REF:VCV:VALUE:G.PROC
dbpf $(PV_PREFIX)1:REF:BCV:VALUE:S.FLNK $(PV_PREFIX)1:REF:BCV:VALUE:G.PROC
dbpf $(PV_PREFIX)1:REF:CCV:VALUE:S.FLNK $(PV_PREFIX)1:REF:CCV:VALUE:G.PROC
dbpf $(PV_PREFIX)1:REF:PLEP:ACCELERATION:S.FLNK $(PV_PREFIX)1:REF:PLEP:ACCELERATION:G.PROC
dbpf $(PV_PREFIX)1:REF:PLEP:EXP_FINAL:S.FLNK $(PV_PREFIX)1:REF:PLEP:EXP_FINAL:G.PROC
dbpf $(PV_PREFIX)1:REF:PLEP:EXP_TC:S.FLNK $(PV_PREFIX)1:REF:PLEP:EXP_TC:G.PROC
dbpf $(PV_PREFIX)1:REF:PLEP:FINAL:S.FLNK $(PV_PREFIX)1:REF:PLEP:FINAL:G.PROC
dbpf $(PV_PREFIX)1:REF:PLEP:INITIAL:S.FLNK $(PV_PREFIX)1:REF:PLEP:INITIAL:G.PROC
dbpf $(PV_PREFIX)1:REF:PLEP:LINEAR_RATE:S.FLNK $(PV_PREFIX)1:REF:PLEP:LINEAR_RATE:G.PROC
dbpf $(PV_PREFIX)1:REF:PPPL:ACCELERATION1:S.FLNK $(PV_PREFIX)1:REF:PPPL:ACCELERATION1:G.PROC
dbpf $(PV_PREFIX)1:REF:PPPL:ACCELERATION2:S.FLNK $(PV_PREFIX)1:REF:PPPL:ACCELERATION2:G.PROC
dbpf $(PV_PREFIX)1:REF:PPPL:ACCELERATION3:S.FLNK $(PV_PREFIX)1:REF:PPPL:ACCELERATION3:G.PROC
dbpf $(PV_PREFIX)1:REF:PPPL:DURATION4:S.FLNK $(PV_PREFIX)1:REF:PPPL:DURATION4:G.PROC
dbpf $(PV_PREFIX)1:REF:PPPL:INITIAL:S.FLNK $(PV_PREFIX)1:REF:PPPL:INITIAL:G.PROC
dbpf $(PV_PREFIX)1:REF:PPPL:RATE2:S.FLNK $(PV_PREFIX)1:REF:PPPL:RATE2:G.PROC
dbpf $(PV_PREFIX)1:REF:PPPL:RATE4:S.FLNK $(PV_PREFIX)1:REF:PPPL:RATE4:G.PROC
dbpf $(PV_PREFIX)1:REF:PPPL:REF4:S.FLNK $(PV_PREFIX)1:REF:PPPL:REF4:G.PROC
dbpf $(PV_PREFIX)1:REF:RAMP:ACCELERATION:S.FLNK $(PV_PREFIX)1:REF:RAMP:ACCELERATION:G.PROC
dbpf $(PV_PREFIX)1:REF:RAMP:DECELERATION:S.FLNK $(PV_PREFIX)1:REF:RAMP:DECELERATION:G.PROC
dbpf $(PV_PREFIX)1:REF:RAMP:FINAL:S.FLNK $(PV_PREFIX)1:REF:RAMP:FINAL:G.PROC
dbpf $(PV_PREFIX)1:REF:RAMP:INITIAL:S.FLNK $(PV_PREFIX)1:REF:RAMP:INITIAL:G.PROC
dbpf $(PV_PREFIX)1:REF:RAMP:LINEAR_RATE:S.FLNK $(PV_PREFIX)1:REF:RAMP:LINEAR_RATE:G.PROC
dbpf $(PV_PREFIX)1:REF:TEST:AMPLITUDE:S.FLNK $(PV_PREFIX)1:REF:TEST:AMPLITUDE:G.PROC
dbpf $(PV_PREFIX)1:REF:TEST:INITIAL:S.FLNK $(PV_PREFIX)1:REF:TEST:INITIAL:G.PROC
dbpf $(PV_PREFIX)1:REF:TEST:PERIOD:S.FLNK $(PV_PREFIX)1:REF:TEST:PERIOD:G.PROC
dbpf $(PV_PREFIX)1:REF:TRIM:DURATION:S.FLNK $(PV_PREFIX)1:REF:TRIM:DURATION:G.PROC
dbpf $(PV_PREFIX)1:REF:TRIM:FINAL:S.FLNK $(PV_PREFIX)1:REF:TRIM:FINAL:G.PROC
dbpf $(PV_PREFIX)1:REF:TRIM:INITIAL:S.FLNK $(PV_PREFIX)1:REF:TRIM:INITIAL:G.PROC
dbpf $(PV_PREFIX)1:DCCT:SELECT:S.FLNK $(PV_PREFIX)1:DCCT:SELECT:G.PROC
dbpf $(PV_PREFIX)1:MODE:PC_SIMPLIFIED:S.FLNK $(PV_PREFIX)1:MODE:PC_SIMPLIFIED:G.PROC
dbpf $(PV_PREFIX)1:REF:FUNC:REG_MODE:S.FLNK $(PV_PREFIX)1:REF:FUNC:REG_MODE:G.PROC
dbpf $(PV_PREFIX)1:REF:FUNC:TYPE:S.FLNK $(PV_PREFIX)1:REF:FUNC:TYPE:G.PROC
dbpf $(PV_PREFIX)1:REF:TEST:EXP_DECAY:S.FLNK $(PV_PREFIX)1:REF:TEST:EXP_DECAY:G.PROC
dbpf $(PV_PREFIX)1:REF:TEST:WINDOW:S.FLNK $(PV_PREFIX)1:REF:TEST:WINDOW:G.PROC
dbpf $(PV_PREFIX)1:REF:TEST:NUM_PERIODS:S.FLNK $(PV_PREFIX)1:REF:TEST:NUM_PERIODS:G.PROC
#dbpf $(PV_PREFIX)1::S.FLNK $(PV_PREFIX)1::G.PROC



# EOF
