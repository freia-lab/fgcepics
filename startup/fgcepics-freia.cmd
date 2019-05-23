######## Set env variables ########

#----------------------------------------------------------------------------------#
# Notes:                                                                           #
#  - Max array size must be increased, to accomodate large FGC commands/responses. #
#----------------------------------------------------------------------------------#

#epicsEnvSet("STREAM_PROTOCOL_PATH", "$(TOP)/db")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES", "1000000")



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

#iocInit

# EOF
