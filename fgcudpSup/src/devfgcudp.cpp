/*!
 * © Copyright CERN 2019. All rights not expressly granted are reserved.
 * @file   drvFgcUdp.cpp
 * @brief  asynPortDriver subclass definition, to parse incoming UDP publication data from FGC Ether gateway.
 * @author Joao Afonso
 */

#include <iocsh.h>
#include <asynPortDriver.h>

#include <epicsExport.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <cvtFast.h>
#include <drvAsynIPServerPort.h>

#include "pub_data.h"
#include "class_selector.h"
#include "param_utils.h"

#include <map>
#include <set>
#include <string>
#include <errno.h>


//#define MAX_HOSTS         64                           // Max number of hosts suported by a single driver.
//#define NUM_CHANNELS      (FGCD_MAX_DEVS * MAX_HOSTS)  // Max number of channels, one for each FGC in every host.

#define REGISTER_HOST_KEY "regHost"
#define REGISTER_DEV_KEY  "regDev"

#define TIMEOUT_MAX       60.0                         // Default timeout while waiting for UDP/IP packets. 

static const char *driverName = "devfgcudp";



/** PubParser (fix).
  * 'drvAsynIPServerPort' can only read maxSize-1 bytes.
  * Therefore, we must use a structure bigger than Pub_data, containing it followed by a few extra bytes. */
struct PubParser
{
    Pub_data pubData;
    uint64_t  dummyByte;
};



/** HostHandler:
  * Defines how the UDP/IP packets returned by an host should be handled. */
struct HostHandler
{
    ClassHandler * fgcHandler[FGCD_MAX_DEVS]; // Handlers for each one of the active FGCs.
    std::string name;                      // Host name.
    const int    hostId;                   // ID (unique to host) returned with the UDP/IP packet. 
    const double timeout;                  // Timeout for each new incoming packet
    double timeoutCount;                   // Remaining time after last incoming packet, until timeout.

    HostHandler(std::string name, int id, double timeout) : fgcHandler(), name(name), hostId(id), timeout(timeout), timeoutCount(timeout) {}
};



/** FgcUdpDriver:
  * Class required to parse FGC status data, published via UDP/IP. 
  * Subclass of asynPortDriver. */ 
class FgcUdpDriver : public asynPortDriver 
{
public:

    FgcUdpDriver(const char * portName, int udpPortNr);
    ~FgcUdpDriver();

    asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **pptypeName, size_t *psize) override;
    void readUdpThread();

    asynStatus registerHost(const char * hostName, uint32_t hostId, int timeout);
    asynStatus registerDevice(const char * hostName, uint8_t devId, uint8_t classId, const char * devName);

    static FgcUdpDriver * getDrvFromPort(std::string portName);
    static FgcUdpDriver * getDrvFromHost(std::string hostName);

private:

    static void newUdpDataCallback(void *userPvt, asynUser *pasynUser, char *data,size_t numchars, int eomReason);

protected:

    std::map<std::string, uint32_t> hostNameToId; // Maps host names to their (unique) IDs.
    std::map<uint32_t, HostHandler*> hostIdToPtr; // Maps host IDs to their HostHandler pointers.

    std::map<std::string, int> devNameToTag;     // Maps device (FGC) name to their (unique) asyn driver tag.
    std::map<int, ClassHandler*> devTagToPtr;       // Maps asyn driver tag to respective FGC handler object.

    static std::map<std::string, FgcUdpDriver*> hostToDriverMap; // Maps EPICS port name to the respective FgcUdpDriver.
    static std::map<std::string, FgcUdpDriver*> portToDriverMap; // Maps host name (FGC gateway) to FgcUdpDriver that should handle its data.
    static std::set<int> usedUdpPorts;

private:

    asynOctet     * pasynOctet;
    asynInterface * pasynInterface;
    asynUser      * pasynUser;
    void          * registrarPvt;

    Pub_data pubData;        // Structure required to parse UDP/IP data.
    epicsEventId newDataEvt; // Activated by the IPServerPort when a new packet arrives.

    int channel_count = 0;     // Counts the number of devices registered.
};

std::map<std::string, FgcUdpDriver*> FgcUdpDriver::hostToDriverMap;
std::map<std::string, FgcUdpDriver*> FgcUdpDriver::portToDriverMap;
std::set<int> FgcUdpDriver::usedUdpPorts;


/** C wrapper for readUdpThread initialization. */
static void readUdpThreadC(void * pPvt)
{
    FgcUdpDriver * pFgcUdpDriver = (FgcUdpDriver*)pPvt;
    pFgcUdpDriver->readUdpThread();
}



/** Constructor for FgcUdpDriver
  * \param[in] portName The name of the fgc udp port driver to be created
  * \param[in] udpPortNr UDP/IP port number used to listen for incoming data */
FgcUdpDriver::FgcUdpDriver(const char *portName, int udpPortNr)
    : asynPortDriver(portName, 1,
      asynFloat64Mask | asynInt32Mask | asynOctetMask | asynDrvUserMask,
      asynFloat64Mask | asynInt32Mask | asynOctetMask | asynDrvUserMask,
      ASYN_CANBLOCK, 1,
      0, 0)
{
    asynStatus status;

    // Check if port is already used
    if(usedUdpPorts.find(udpPortNr) != usedUdpPorts.end())
    {
        printf("UDP port %d already used!\n", udpPortNr);
        return;
    }
    else
    {
        usedUdpPorts.insert(udpPortNr);
    }


    // Configure UDP Server
    char udpServerName[128];
    char udpServerPort[128];
    sprintf(udpServerName, "%s UDP Server", portName);
    sprintf(udpServerPort, ":%d udp", udpPortNr);

    // Initialize driver for UDP packet reception
    drvAsynIPServerPortConfigure(udpServerName, udpServerPort, 1, 0, 0, 0);

    // Create event for new incoming data
    newDataEvt = epicsEventCreate(epicsEventEmpty);

    // Configure asyn user
    pasynUser = pasynManager->createAsynUser(0, 0);
    status = pasynManager->connectDevice(pasynUser, udpServerName, 0);
    if(status != asynSuccess)
    {
        printf("Can't connect to port %s: %s\n", udpServerName, pasynUser->errorMessage);
        return;
    }
    pasynInterface = pasynManager->findInterface(pasynUser, asynOctetType, 1);   
    if(!pasynInterface)
    {
        printf("%s driver not supported\n", asynOctetType);
        return;
    }
    pasynOctet = (asynOctet *)pasynInterface->pinterface;

    // Configure reading thread
    epicsThreadCreate(
        "ReadUDPThread",
        epicsThreadPriorityMedium,
        epicsThreadGetStackSize(epicsThreadStackMedium),
        (EPICSTHREADFUNC) readUdpThreadC,
        this
    );

    // Configure new data callback/event
    status = pasynOctet->registerInterruptUser(
        pasynInterface->drvPvt, 
        pasynUser,
        newUdpDataCallback,
        this,
        &registrarPvt
    );

    // Add port name to static map
    portToDriverMap[portName] = this;
}



/** Destructor for FgcUdpDriver */
FgcUdpDriver::~FgcUdpDriver()
{
    // Delete allocated HostHandler structures
    for(auto idPtrPair : hostIdToPtr)
    {
        delete idPtrPair.second;
    }

    // Delete entries in static table 'hostToDriverMap'
    for(auto iter = hostToDriverMap.begin(); iter != hostToDriverMap.end(); )
    {
        if((*iter).second == this)
        {
            iter = hostToDriverMap.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    // Delete entries in static table 'portToDriverMap'
    for(auto iter = portToDriverMap.begin(); iter != portToDriverMap.end(); )
    {
        if((*iter).second == this)
        {
            iter = portToDriverMap.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}



/** Given an asyn port name, returns a pointer to the respective FgcUdpDriver.
  * \param[in] portName The name of the asyn port driver
  * \return Returns a pointer to a FgcUdpDriver */
FgcUdpDriver * FgcUdpDriver::getDrvFromPort(std::string portName)
{
    auto iter = portToDriverMap.find(portName);
    if(iter == portToDriverMap.end())
    {
        return NULL;
    }
    return iter->second;
}



/** Given an host (FGC gateway) name, returns a pointer to the FgcUdpDriver that handles it.
  * \param[in] hostName The name of the host (FGC gateway)
  * \return Returns a pointer to a FgcUdpDriver */
FgcUdpDriver * FgcUdpDriver::getDrvFromHost(std::string hostName)
{
    auto iter = hostToDriverMap.find(hostName);
    if(iter == hostToDriverMap.end())
    {
        return NULL;
    }
    return iter->second;
}



/** Overridden method - check base class documentation. */
asynStatus FgcUdpDriver::drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **pptypeName, size_t *psize)
{
    static const char *functionName = "drvUserCreate";

    char fgcName[64];
    strncpy(fgcName, drvInfo, 64);
    asynStatus status;

    // Split FGC name from parameter
    // <drvInfo> ==(parse)==> <fgc_name>:<parameter>  
    const char * param_name = NULL;

    for(char * ptr = fgcName; *ptr != 0; ptr++)
    {
        if(*ptr == ':')
        {
            *ptr = 0;
            param_name = ptr+1;
            break;
        }
    }

    if(param_name == NULL)
    {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "%s:%s: drvInfo has wrong format %s (should be '<FGC_NAME>:<PARAMETER>')\n",
                  driverName, functionName, drvInfo);
        return(asynError);
    }

    auto iter = devNameToTag.find(fgcName);
    if(iter == devNameToTag.end())
    {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "%s:%s: cannot find FGC with name %s, name not registered\n",
                  driverName, functionName, fgcName);
        return(asynError);
    }

    int devTag = iter->second;
    int index;

    // Check parameter index
    char param_name_buf[128];
    snprintf(param_name_buf, 128, "%d_%s", devTag, param_name);
    status = findParam(param_name_buf, &index);

    if (status) {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "%s:%s: cannot find parameter %s\n",
                  driverName, functionName, param_name_buf);
        return(status);
    }

    // Configure pasynUser with parameter location
    pasynUser->drvUser = static_cast<void*>(devTagToPtr[devTag]);
    pasynUser->reason  = index;

    asynPrint(pasynUser, ASYN_TRACE_FLOW,
              "%s:%s: drvInfo=%s, param=%s, index=%d\n",
              driverName, functionName, drvInfo, param_name_buf, index);

    return(asynSuccess);
}



/** Registers a new host (FGC gateway) with this driver.
  * \param[in] hostName The name of the host (FGC gateway)
  * \param[in] hostId Unique ID identifying this host UDP/IP packets
  * \param[in] timeout Timeout for incoming UDP/IP packets from this host */
asynStatus FgcUdpDriver::registerHost(const char * hostName, uint32_t hostId, int timeout)
{
    static const char *functionName = "registerHost";

    if(hostNameToId.find(hostName) != hostNameToId.end())
    {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "%s:%s: host name %s already registered with port %s\n",
                  driverName, functionName, hostName, portName);
        return(asynError);
    }

    if(hostIdToPtr.find(hostId) != hostIdToPtr.end())
    {
        // IDs must be unique to a single host!
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "%s:%s: udp id %d is already registered with a different host\n",
                  driverName, functionName, hostId);
        return(asynError);
    }

    hostNameToId[hostName] = hostId;
    hostIdToPtr[hostId] = new HostHandler(hostName, hostId, timeout);
    hostToDriverMap[hostName] = this;

    return asynSuccess;
}



/** Registers a new device (FGC) with this driver.
  * Parameters should match each one of the name-file fields.
  * \param[in] hostName The name of the host (FGC gateway)
  * \param[in] devId Device ID (FGC dongle)
  * \param[in] classId Class ID (FGC class)
  * \param[in] devName Device (FGC) name*/
asynStatus FgcUdpDriver::registerDevice(const char * hostName, uint8_t devId, uint8_t classId, const char * devName)
{
    static const char *functionName = "registerDevice";

    if(devNameToTag.find(devName) != devNameToTag.end())
    {
        // Device names must be unique!
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "%s:%s: device name %s already registered\n",
                  driverName, functionName, devName);
        return(asynError);
    }

    auto iter = hostNameToId.find(hostName);
    if(iter == hostNameToId.end())
    {
        // Each host must be registered before respective devices
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "%s:%s: host name %s not registered\n",
                  driverName, functionName, hostName);
        return(asynError);
    }

    HostHandler * hostPtr = hostIdToPtr[iter->second];

    if(hostPtr->fgcHandler[devId] != 0)
    {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "%s:%s: device ID %d already registered with host %s\n",
                  driverName, functionName, devId, hostName);
        return(asynError);
    }

    int devTag = channel_count;
    channel_count++;

    ClassHandler * newFgc = createFGChandler(devTag, devId, classId);
    if(newFgc == NULL)
    {
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                      "%s: unable to create handler for device #%d of class %d", driverName, devId, classId);
        return asynError;
    }

    devNameToTag[devName] = devTag;
    devTagToPtr[devTag]  = newFgc;

    hostPtr->fgcHandler[devId] = newFgc;
    hostPtr->fgcHandler[devId]->create_class_params(this);

    return asynSuccess;
}



/** Callback for when new UDP/IP data is available. */
void FgcUdpDriver::newUdpDataCallback(void *userPvt, asynUser *pasynUser, char *data, size_t numchars, int eomReason)
{
      FgcUdpDriver * fgcUdpPvt = (FgcUdpDriver*) userPvt;
      epicsEventSignal(fgcUdpPvt->newDataEvt);
}



/** Thread that reads, parses and processes incoming UDP/IP packets. */
void FgcUdpDriver::readUdpThread()
{
    PubParser pubParser;
    Pub_data * pubDataPtr;
    size_t nRead;
    int eomReason;
    asynStatus status;

    struct timeval pubTime;
    struct timeval lastPubTime;
    epicsTimeStamp pubTimeEpics;

    // Get current timestamp
    updateTimeStamp();
    getTimeStamp(&pubTimeEpics);
    epicsTimeToTimeval(&lastPubTime, &pubTimeEpics);

    while(1)
    {
        int hostId = 0;

        // Check what is the next host to suffer a timeout, from all non-expired hosts.
        // Use remaining time as the timeout value.
        double timeout = TIMEOUT_MAX;
        double elapsedTime;

        for(auto & iter : hostIdToPtr)
        {
            HostHandler * hostPtr = iter.second;
            if(timeout > hostPtr->timeoutCount && hostPtr->timeoutCount > 0)
            {
                timeout = hostPtr->timeoutCount;
            }
        }

        // Wait for data, or timeout
        epicsEventWaitStatus evt_status = epicsEventWaitWithTimeout(newDataEvt, timeout);

        lock();

        if(evt_status != epicsEventWaitTimeout)
        {
            pasynOctet->read(pasynInterface->drvPvt, pasynUser, (char *) &pubParser, sizeof(pubParser), &nRead, &eomReason);

            pubDataPtr = &pubParser.pubData;

            // Update timestamp, using received data timestamp
            pubTime.tv_sec  = ntohl(pubDataPtr->time_sec);
            pubTime.tv_usec = ntohl(pubDataPtr->time_usec);

            elapsedTime = timeval_elapsed(lastPubTime, pubTime);
            lastPubTime = pubTime;

            epicsTimeFromTimeval(&pubTimeEpics, &pubTime);
            setTimeStamp(&pubTimeEpics);

            int validData = 1;
            HostHandler * hostPtr = NULL;

            if(nRead < sizeof(Pub_data))
            {
                printf("UDP publication wrong size (%ld/%ld bytes).\n", nRead, sizeof(Pub_data));
                validData = 0;
            }
            else
            {
                // Identify host
                hostId = ntohl(pubDataPtr->header.id);
                auto hostIter = hostIdToPtr.find(hostId);
                if(hostIter == hostIdToPtr.end())
                {
                    printf("UDP publication received from unknown host.\n");
                    validData = 0;
                }
                hostPtr = hostIter->second;
            }

            if(validData)
            {
                // Handle incoming data, for each device
                for(int i = 0; i < FGCD_MAX_DEVS; ++i)
                {
                    if(hostPtr->fgcHandler[i] != NULL)
                    {
                        hostPtr->fgcHandler[i]->update_class_params(this, *pubDataPtr);
                    }
                }

                // Reset timeout
                hostPtr->timeoutCount = hostPtr->timeout;
            }
        }
        else
        {
            // Update current timestamp
            updateTimeStamp(&pubTimeEpics);
            epicsTimeToTimeval(&pubTime, &pubTimeEpics);
            elapsedTime = timeval_elapsed(lastPubTime, pubTime);
            lastPubTime = pubTime;
        }

        // Reduce timeout of other hosts, and do callbacks if necessary
        //updateTimeStamp();

        // Invalidate (timeout error) only the devices of the hosts which suffered a timeout
        for(auto & iter : hostIdToPtr)
        {
            HostHandler * hostPtr = iter.second;

            if(hostPtr->hostId != hostId || evt_status == epicsEventWaitTimeout)
            {
                // Exclude if it was already timed out
                // Exclude if it had not timed out yet
                if((hostPtr->timeoutCount > 0) && (hostPtr->timeoutCount - elapsedTime <= 0))
                {
                    for(int i = 0; i < FGCD_MAX_DEVS; ++i)
                    {
                        if(hostPtr->fgcHandler[i] != NULL)
                        {
                            hostPtr->fgcHandler[i]->invalidate_class_params(this);
                        }
                    }
                }

                hostPtr->timeoutCount -= elapsedTime;
            }
        }

        // Trigger all callbacks, for modified parameters
        status = (asynStatus) callParamCallbacks(0);
        if (status)
        {
            printf("Callback Error\n");
        }

        unlock();
    }
}



/** Define IOC shell command for adding a new host (FGC gateway) */
extern "C" int drvFgcUdpRegisterHost(const char *portName, const char * hostName, const char * hexId, double timeout)
{
    FgcUdpDriver * drvPtr = FgcUdpDriver::getDrvFromPort(portName);
    if(!drvPtr) 
    {
        printf("Failed to identify driver from port name.\n");
        return -1;
    }

    int hostId = (int) strtol(hexId, NULL, 16);
    if(!hostId and errno)
    {
        printf("Failed to read host ID '%s'.\n", hexId);
        return -1;
    }

    return drvPtr->registerHost(hostName, hostId, timeout);
}

static const iocshArg regHostArg0 = {"Asyn port name",        iocshArgString};
static const iocshArg regHostArg1 = {"FGC gateway host name", iocshArgString};
static const iocshArg regHostArg2 = {"FGC gateway hex ID",    iocshArgString};
static const iocshArg regHostArg3 = {"Timeout",               iocshArgDouble};

static const iocshArg * const regHostArgs[] = {&regHostArg0,
                                               &regHostArg1,
                                               &regHostArg2,
                                               &regHostArg3};

static const iocshFuncDef registerHostFuncDef = {"devFgcUdpRegisterHost", 4, regHostArgs};



static void registerHostCallFunc(const iocshArgBuf * args)
{
    drvFgcUdpRegisterHost(args[0].sval, args[1].sval, args[2].sval, args[3].dval);
}


/** Define IOC shell command for adding a new FGC */
extern "C" int drvFgcUdpRegisterDev(const char *hostName, int fgcId, int fgcClass, const char *devName)
{
    FgcUdpDriver * drvPtr = FgcUdpDriver::getDrvFromHost(hostName);
    if(!drvPtr)
    {
        printf("Failed to identify driver from host name.\n");
        return -1;
    }

    // TODO: Double check input types of this func
    return drvPtr->registerDevice(hostName, fgcId, fgcClass, devName);
}

static const iocshArg regDevArg0 = {"Gateway host name", iocshArgString};
static const iocshArg regDevArg1 = {"FGC ID",            iocshArgInt};
static const iocshArg regDevArg2 = {"FGC class",         iocshArgInt};
static const iocshArg regDevArg3 = {"FGC name",          iocshArgString};

static const iocshArg * const regDevArgs[] = {&regDevArg0,
                                              &regDevArg1,
                                              &regDevArg2,
                                              &regDevArg3};

static const iocshFuncDef registerDevFuncDef = {"devFgcUdpRegisterDev", 4, regDevArgs};



static void registerDevCallFunc(const iocshArgBuf * args)
{
    drvFgcUdpRegisterDev(args[0].sval, args[1].ival, args[2].ival, args[3].sval);
}



/** Define IOC shell command for driver initialization */
extern "C" int devFgcUdpConfig(const char *portName, int udpPortNr)
{
    new FgcUdpDriver(portName, udpPortNr);
    return asynSuccess;
}


static const iocshArg configArg0 = {"Asyn port name",  iocshArgString};
static const iocshArg configArg1 = {"UDP port number", iocshArgInt};

static const iocshArg * const configArgs[] = {&configArg0,
                                              &configArg1};

static const iocshFuncDef configFuncDef = {"devFgcUdpConfig", 2, configArgs};



static void configCallFunc(const iocshArgBuf * args)
{
    devFgcUdpConfig(args[0].sval, args[1].ival);
}



// Register both IOC shell commands
void devFgcUdpRegister(void)
{
    iocshRegister(&configFuncDef,       configCallFunc);
    iocshRegister(&registerDevFuncDef,  registerDevCallFunc);
    iocshRegister(&registerHostFuncDef, registerHostCallFunc);
}



extern "C" 
{
    epicsExportRegistrar(devFgcUdpRegister);
}



// EOF
