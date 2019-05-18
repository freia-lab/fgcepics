/*!
 * © Copyright CERN 2019. All rights not expressly granted are reserved.
 * @file   class_base_params.cpp
 * @brief  parse incoming UDP publication data common to all FGC classes.
 * @author Joao Afonso
 */

#include <string>
#include <asynPortDriver.h>
#include <alarm.h>
#include "pub_data.h"
#include "param_utils.h"
#include "class_base_params.h"



const struct sym_name sym_names_base_data_status[] =
{
    { FGC_DEVICE_IN_DB, "DEVICE_IN_DB" },
    { FGC_DATA_VALID  , "DATA_VALID"   },
    { FGC_CLASS_VALID , "CLASS_VALID"  },
    { 0               , 0              }
};



void ClassHandler::create_class_params(asynPortDriver * drv)
{
    drv->createParam(dev_addr, "DATA_STATUS", asynParamOctet, &param_id[0]);
    drv->createParam(dev_addr, "CLASS_ID"   , asynParamInt32, &param_id[1]);
    drv->createParam(dev_addr, "RUNLOG_BYTE", asynParamInt32, &param_id[2]);
}



void ClassHandler::update_class_params(asynPortDriver * drv, Pub_data & pub_data)
{
    std::string data_status_str = conv_fgc_flags(pub_data.status[dev_id].data_status, sym_names_base_data_status);

    // Reset values
    drv->setStringParam( dev_addr, param_id[0], mod(data_status_str));
    drv->setIntegerParam(dev_addr, param_id[1], mod(pub_data.status[dev_id].class_id));
    drv->setIntegerParam(dev_addr, param_id[2], mod(pub_data.status[dev_id].runlog));

    drv->setStringParam( dev_addr, param_id[0], data_status_str);
    drv->setIntegerParam(dev_addr, param_id[1], pub_data.status[dev_id].class_id);
    drv->setIntegerParam(dev_addr, param_id[2], pub_data.status[dev_id].runlog);

    // Validate params
    for(int i = 0; i < ASYN_PARAMS_BASE; i++)
    {
        drv->setParamStatus(dev_addr, param_id[i], asynSuccess);
    }
}



void ClassHandler::invalidate_class_params(asynPortDriver * drv)
{
    for(int i = 0; i < ASYN_PARAMS_BASE; i++)
    {
        drv->setParamStatus(dev_addr, param_id[i], asynTimeout);
    }
}



// EOF
