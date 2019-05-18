import getopt
import sys
import re
import textwrap
import os
from collections import namedtuple, OrderedDict



# Script arguments description

args_str = "Help: <path>/genIOCScript [-n|--namefile] <name_file> [-u|--udpdirpath] <path_to_udp_conf> [-i|--iochostname] <ioc_host_name> <output>"

namefile_path = None
udpdir = None
iochostname = None
listgws = []


# Regexes

namefile_re = r"([^:]+):([0-9]+):([0-9]+):([^:]+):0x([0-9a-fA-F]{4})"
udpfile_re = r"([^:]+):([0-9]+):([0-9]+):(?:0x)?([0-9a-fA-F]+)"



# Argument parsing

try:
    optlist, args = getopt.getopt(sys.argv[1:], "n:u:i:g:", ["namefile=", "udpdirpath=", "iochostname=", "gateway="])
except getopt.GetoptError as e:
    print("Error: ", e)
    print(args_str)
    sys.exit()

for opt, value in optlist:
    if opt in ["-n", "--namefile"]:
        namefile_path = value
    elif opt in ["-u", "--udpdirpath"]:
        udpdir = value
    elif opt in ["-i", "--iochostname"]:
        iochostname = value.upper()
    elif opt in ["-g", "--gateway"]:
        listgws.append(value.upper())
    else:
        print(f"Error: Argument '{opt}' unknown!")
        print(args_str)
        sys.exit() 

if not namefile_path or not udpdir or not iochostname or len(args) < 1:
    print("Error: Argument(s) is(are) missing!")
    print(args_str)
    sys.exit()



# Read and parse name file

dev_dict = OrderedDict()
gw_to_cmd_port = {}
gateway_names = set()

with open(namefile_path, 'r') as f:
    for line in f.readlines():
        m = re.match(namefile_re, line)

        if m and int(m.group(2)) != 0:

            gw = m.group(1).upper()
  
            # If only a subset of the gateways is required
            if listgws and gw not in listgws:
                continue

            dev_id = int(m.group(2))
            class_id = int(m.group(3))
            dev_name = m.group(4).upper()

            gateway_names.add(gw)

            if gw not in gw_to_cmd_port:
                gw_to_cmd_port[gw] = len(gw_to_cmd_port)

            dev_dict[dev_name] = {
                "gateway":   gw,
                "dev_id":    dev_id,
                "dev_class": class_id,
                "cmd_port":  "cmd_port_{}_{}".format(gw_to_cmd_port[gw], dev_id),
                "pub_port":  None
            }



# Read and parse udp files

filtered_files = []  
pub_dict = OrderedDict()
udp_to_udp_port = {}
                                                                                                                    

for r, d, f in os.walk(udpdir):
    for file in sorted(f):
        if file.upper() in gateway_names:
            filtered_files.append((file, os.path.join(r, file)))

for file, path in filtered_files:
    with open(path, 'r') as f:
        for line in f.readlines():
            m = re.match(udpfile_re, line)
            
            if m and m.group(1).upper().startswith(iochostname.upper()):

                udp_port = int(m.group(2))
                timeout = 4 * float(m.group(3)) / 50
                tag_id = int(m.group(4), 16)

                if udp_port not in udp_to_udp_port:
                    udp_to_udp_port[udp_port] = len(udp_to_udp_port)

                pub_dict[file.upper()] = {
                    "udp_port": udp_port,
                    "pub_port": "udp_port_{}".format(udp_to_udp_port[udp_port]),
                    "timeout":  timeout,  
                    "id":       tag_id
                }


# Update pub ports

for dev, info in dev_dict.items():
    try:
        info["pub_port"] = pub_dict[info["gateway"]]["pub_port"]
    except KeyError:
        info["pub_port"] = None


######## Print output! ########

out_file = open(args[0], 'w')

out_file.write(
    textwrap.dedent("""\
        #!../../bin/linux-x86_64/testfgcepics

        < envPaths
        cd "${TOP}"
        
        
        ######## Set env variables ########
        
        #----------------------------------------------------------------------------------#
        # Notes:                                                                           #
        #  - Max array size must be increased, to accomodate large FGC commands/responses. #
        #----------------------------------------------------------------------------------#
        
        epicsEnvSet("STREAM_PROTOCOL_PATH", "$(TOP)/db")
        epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES", "1000000")
        
        
        ######## Register support components ########
        
        dbLoadDatabase "dbd/testfgcepics.dbd"
        testfgcepics_registerRecordDeviceDriver pdbbase
        
        
        ######## Open command/response connection ########
        
        #-------------------------------------------------------------#
        # Notes:                                                      #
        #  - Open 1 connection per each FGC (for maximum performance).#
        #  - Recomend using the 'drvAsynIPPortConfigure' driver.      #
        #-------------------------------------------------------------#

        """)
) 

######## Open command/response connections,for each FGC ########

for dev, info in dev_dict.items():
    out_file.write(f"drvAsynIPPortConfigure(\"{info['cmd_port']}\" ,\"{info['gateway']}:1906\", 0, 0, 0)\n")

################################################################

out_file.write(
    textwrap.dedent("""\


        ######## Start published data driver ########
        
        #--------------------------------------------------------#
        # Parameters:                                            #
        #  1) Asyn port name                                     #
        #  2) UDP IP port                                        #
        #  3) Timeout                                            #
        #                                                        #
        # NOTE: Default UDP port values may have to be modified. #
        #       Different gateways may use different UDP ports.  #
        #--------------------------------------------------------#       

        """)
)

######## Open udp ports for published data ########

for pub_port, udp_port in sorted({ (val["pub_port"], val["udp_port"]) for key, val in pub_dict.items()}):
    out_file.write(f"devFgcUdpConfig(\"{pub_port}\" , {udp_port})\n")

###################################################

out_file.write(
    textwrap.dedent("""\


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

        """)
)

######## Setup driver for each publishing host ########

for host, info in pub_dict.items():
    out_file.write(f"devFgcUdpRegisterHost(\"{info['pub_port']}\" , \"{host}\", \"0x{info['id']:04x}\", {info['timeout']})\n")

#######################################################

out_file.write(
    textwrap.dedent("""\


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

        """)
)

######## Setup driver for each device ########

for dev, info in dev_dict.items():
    if info['gateway'] in pub_dict:
        out_file.write(f"devFgcUdpRegisterDev(\"{info['gateway']}\" , {info['dev_id']}, {info['dev_class']}, \"{dev}\")\n")

##############################################

out_file.write(
    textwrap.dedent("""\


        ######## Debug #######")

        """)
) 

######## Configure debugging ########

for pub_port in sorted({ val["pub_port"] for key, val in pub_dict.items()}):
    out_file.write(f"#asynSetTraceMask(\"{pub_port}\",-1,0xff)\n")

for cmd_port in sorted({ val["cmd_port"] for key, val in dev_dict.items()}):
    out_file.write(f"#asynSetTraceMask(\"{cmd_port}\",-1,0xff)\n")

out_file.write("\n")

for pub_port in sorted({ val["pub_port"] for key, val in pub_dict.items()}):
    out_file.write(f"#asynSetTraceIOMask(\"{pub_port}\",-1,0x2)\n")

for cmd_port in sorted({ val["cmd_port"] for key, val in dev_dict.items()}):
    out_file.write(f"#asynSetTraceIOMask(\"{cmd_port}\",-1,0x2)\n")

#####################################

out_file.write(
    textwrap.dedent("""\


        ######## Load record instances - FGC udp #######

        cd "${TOP}/db"

        """)
)

######## Load udp published records ########

for dev, info in dev_dict.items():
    if info['pub_port']: 
        out_file.write(f"dbLoadRecords(\"fgcudp_class_{info['dev_class']}.db\", \"PORT={info['pub_port']},HOST=,DEV={dev.replace('.','_')}:,FGC={dev}\")\n")

############################################

out_file.write(
    textwrap.dedent("""\


        ######## Load record instances - FGC cmd #######

        cd "${TOP}/db"

        """)
)

######## Load command/response records ########

for dev, info in dev_dict.items():
    out_file.write(f"dbLoadRecords(\"fgccmd_class_{info['dev_class']}.db\", \"PORT={info['cmd_port']},HOST=,DEV={dev.replace('.','_')}:,FGC={dev}\")\n")

###############################################

######## Finalize ########

out_file.write(
    textwrap.dedent("""\


        ######## Start IOC #######

        cd "${TOP}/iocBoot/${IOC}"
        iocInit

        # EOF
        """)
)

# EOF
