######## Set env variables ########

#----------------------------------------------------------------------------------#
# Notes:                                                                           #
#  - Max array size must be increased, to accomodate large FGC commands/responses. #
#----------------------------------------------------------------------------------#

#epicsEnvSet("STREAM_PROTOCOL_PATH", "$(TOP)/db")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES", "1000000")
epicsEnvSet("PV_PREFIX", "MAG-RCH")



######## Register support components ########

#dbLoadDatabase "dbd/testfgcepics.dbd"
#testfgcepics_registerRecordDeviceDriver pdbbase



######## Open command/response connection ########

#-------------------------------------------------------------#
# Notes:                                                      #
#  - Open 1 connection per each FGC (for maximum performance).#
#  - Recomend using the 'drvAsynIPPortConfigure' driver.      #
#-------------------------------------------------------------#

# Example:
#
# - 2 hosts / 4 devices
#   * Host #0:
#     ** FGC #1 : cmd_port_0_1
#   * Host #1:
#     ** FGC #1 : cmd_port_1_1
#     ** FGC #2 : cmd_port_1_2
#     ** FGC #3 : cmd_port_1_3

#drvAsynIPPortConfigure("cmd_port_0_1" ,"host_0_addr:1906", 0, 0, 0)
#drvAsynIPPortConfigure("cmd_port_1_1" ,"host_1_addr:1906", 0, 0, 0)
#drvAsynIPPortConfigure("cmd_port_1_2" ,"host_1_addr:1906", 0, 0, 0)
#drvAsynIPPortConfigure("cmd_port_1_3" ,"host_1_addr:1906", 0, 0, 0)

drvAsynIPPortConfigure ("fgc_port_1" ,"localhost:1906", 0, 0, 0)
drvAsynIPPortConfigure ("fgc_port_2" ,"localhost:1906", 0, 0, 0)
drvAsynIPPortConfigure ("fgc_port_3" ,"localhost:1906", 0, 0, 0)
#drvAsynIPPortConfigure ("fgc_port_4" ,"localhost:1906", 0, 0, 0)



######## Start UDP published data driver ########

#--------------------------------------------------------#
# Parameters:                                            #
#  1) Asyn port name                                     #
#  2) UDP IP port                                        #
#  3) Timeout                                            #
#                                                        #
# NOTE: Default UDP port values may have to be modified. #
#       Different gateways can use different UDP ports.  #
#--------------------------------------------------------#       

# Example:
#
# - 2 hosts, sending to two different UDP ports.
# - Alternatively, they could use the same UDP port.
#   * Host #0:
#     ** udp_port_0, ip-port 2906
#   * Host #1:
#     ** udp_port_1, ip-port 2905

#devFgcUdpConfig("udp_port_0" , 2906)
#devFgcUdpConfig("udp_port_1" , 2905)

devFgcUdpConfig("fgc_udp", 2906)

######## Configure published data driver - hosts ########

#----------------------------------------------------------------------------#
# Notes:                                                                     #
#  - Call 1 per each Host.                                                   #
#                                                                            #
# Parameters:                                                                #
#  1) Asyn port name                                                         #
#  2) Host name                                                              #
#  3) Host publication ID                                                    #
#  4) Timeout                                                                #
#----------------------------------------------------------------------------#

# Example:
#
# - 2 hosts, with different names and IDs:
#   * Host #0:
#     ** EPICS port: udp_port_0
#     ** Host name:  name_host_0
#     ** ID:         0x00ff
#     ** Timeout:    5.0
#   * Host #1:
#     ** EPICS port: udp_port_1
#     ** Host name:  name_host_1
#     ** ID:         0x00fe
#     ** Timeout:    10.0

#devFgcUdpRegisterHost("udp_port_0" , "name_host_0", "0x00ff", 5.0)
#devFgcUdpRegisterHost("udp_port_1" , "name_host_1", "0x00fe", 10.0)

devFgcUdpRegisterHost("fgc_udp", "cbox1-0", "0x00ff", 5.0)

######## Configure published data driver - devices ########

#----------------------------------------------------------------------------#
# Notes:                                                                     #
#  - Call 1 per each FGC.                                                    #
#                                                                            #
# Parameters:                                                                #
#  1) Host name                                                              #
#  2) FGC ID (dongle)                                                        #
#  3) FGC class                                                              #
#  4) FGC name (name file notation)                                          #
#----------------------------------------------------------------------------#

# Example:
#
# - 2 hosts / 4 devices
#   * Host #0:
#     ** FGC #1 : dev.name.0
#   * Host #1:
#     ** FGC #1 : dev.name.1
#     ** FGC #2 : dev.name.2
#     ** FGC #3 : dev.name.3

#devFgcUdpRegisterDev("host_name_0" , 1, 63, "dev.name.0")
#devFgcUdpRegisterDev("host_name_1" , 1, 63, "dev.name.1")
#devFgcUdpRegisterDev("host_name_1" , 2, 63, "dev.name.2")
#devFgcUdpRegisterDev("host_name_1" , 3, 63, "dev.name.3")

devFgcUdpRegisterDev("cbox1-0", 2, 63, "RFBGN.FREIA.SPARE.2")
devFgcUdpRegisterDev("cbox1-0", 13, 63, "RPABR.FREIA.RCH1.2KA")
devFgcUdpRegisterDev("cbox1-0", 14, 63, "RPABR.FREIA.RCH2.2KA")
#devFgcUdpRegisterDev("pcte23185", 4, 0,  "FGC_4")



######## Debug ########

#asynSetTraceMask("fgc_udp",-1,0xff)
#asynSetTraceIOMask("fgc_udp",-1,0x2)



######## Load record instances - FGC udp ########

dbLoadRecords("fgcudp_class_63.db",  "PORT=fgc_udp,HOST=MAG-,DEV=SPARE:,FGC=RFBGN.FREIA.SPARE.2")
dbLoadRecords("fgcudp_class_63.db",  "PORT=fgc_udp,HOST=MAG-,DEV=RCH1:,FGC=RPABR.FREIA.RCH1.2KA")
dbLoadRecords("fgcudp_class_63.db",  "PORT=fgc_udp,HOST=MAG-,DEV=RCH2:,FGC=RPABR.FREIA.RCH2.2KA")
#dbLoadRecords("fgcudp_class_base.db","PORT=fgc_udp,HOST=,DEV=FGC_4:,FGC=FGC_4")


######## Load record instances - FGc cmd ########

dbLoadRecords("fgccmd_class_63.db",  "PORT=fgc_port_1,HOST=MAG-,DEV=RCH1:,FGC=RPABR.FREIA.RCH1.2KA")
dbLoadRecords("fgccmd_class_63.db",  "PORT=fgc_port_2,HOST=MAG-,DEV=RCH2:,FGC=RPABR.FREIA.RCH2.2KA")
dbLoadRecords("fgccmd_class_63.db",  "PORT=fgc_port_3,HOST=MAG-,DEV=SPARE:,FGC=RFBGN.FREIA.SPARE.2")
#dbLoadRecords("fgccmd_class_63.db",  "PORT=fgc_port_2,HOST=,DEV=FGC_2:,FGC=2")
#dbLoadRecords("fgccmd_class_63.db",  "PORT=fgc_port_3,HOST=,DEV=FGC_3:,FGC=3")
#dbLoadRecords("fgccmd_class_63.db",  "PORT=fgc_port_4,HOST=,DEV=FGC_4:,FGC=4")


######## Load record instances - FGc help records ########

dbLoadRecords("fgcepics-freia-gen.db",  "HOST=MAG-,DEV=RCH1:")
dbLoadRecords("fgcepics-freia-gen.db",  "HOST=MAG-,DEV=RCH2:")
dbLoadRecords("fgcepics-freia-gen.db",  "HOST=MAG-,DEV=SPARE:")


######## Start IOC ########

iocInit
dbpf $(PV_PREFIX)1:REF:TABLE:FUNCTION:G.PROC 1
#dbpf $(PV_PREFIX)1:REF:TABLE:FUNCTION:G:READ.PROC 1
#dbpf $(PV_PREFIX)1:REF:TABLE:FUNCTION:REF.PROC 1
#dbpf $(PV_PREFIX)1:REF:TABLE:FUNCTION:REF.TIME 1
#dbpf $(PV_PREFIX)1::S.FLNK $(PV_PREFIX)1::G.PROC

dbpf $(PV_PREFIX)2:REF:TABLE:FUNCTION:G.PROC 1
#dbpf $(PV_PREFIX)2:REF:TABLE:FUNCTION:G:READ.PROC 1
#dbpf $(PV_PREFIX)2:REF:TABLE:FUNCTION:REF.PROC 1
#dbpf $(PV_PREFIX)2:REF:TABLE:FUNCTION:REF.TIME 1
#dbpf $(PV_PREFIX)2::S.FLNK $(PV_PREFIX)2::G.PROC

dbpf MAG-SPARE:REF:TABLE:FUNCTION:G.PROC 1
#dbpf MAG-SPARE:REF:TABLE:FUNCTION:G:READ.PROC 1
#dbpf MAG-SPARE:REF:TABLE:FUNCTION:REF.PROC 1
#dbpf MAG-SPARE:REF:TABLE:FUNCTION:REF.TIME 1
#dbpf MAG-SPARE::S.FLNK $(PV_PREFIX)3::G.PROC


# EOF
