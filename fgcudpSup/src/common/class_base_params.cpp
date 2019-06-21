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

#define NAME_BUFFER_LEN 256

const struct sym_name sym_names_base_data_status[] =
{
    { FGC_DEVICE_IN_DB, "DEVICE_IN_DB" },
    { FGC_DATA_VALID  , "DATA_VALID"   },
    { FGC_CLASS_VALID , "CLASS_VALID"  },
    { 0               , 0              }
};



void ClassHandler::create_class_params(asynPortDriver * drv)
{
    int dev_tag = get_tag();
    char name_buffer[NAME_BUFFER_LEN];

    snprintf(name_buffer, NAME_BUFFER_LEN, "%d_%s", dev_tag, "DATA_STATUS");
    drv->createParam(name_buffer, asynParamOctet, &param_id[0]);

    snprintf(name_buffer, NAME_BUFFER_LEN, "%d_%s", dev_tag, "CLASS_ID");
    drv->createParam(name_buffer, asynParamInt32, &param_id[1]);

    snprintf(name_buffer, NAME_BUFFER_LEN, "%d_%s", dev_tag, "RUNLOG_BYTE");
    drv->createParam(name_buffer, asynParamInt32, &param_id[2]);

    drv->setStringParam( param_id[0], "");
    drv->setIntegerParam(param_id[1], 0);
    drv->setIntegerParam(param_id[2], 0);
}



void ClassHandler::update_class_params(asynPortDriver * drv, Pub_data & pub_data)
{
    std::string data_status_str = conv_fgc_flags(pub_data.status[dev_id].data_status, sym_names_base_data_status);

    // DATA_STATUS should allways be valid and show values
    drv->setStringParam(param_id[0], data_status_str);
    drv->setParamStatus(param_id[0], asynParamUndefined); // Force callback
    drv->setParamStatus(param_id[0], asynSuccess);

    //Check if common data is valid
    if(
        !(pub_data.status[dev_id].data_status & FGC_DATA_VALID)
    )
    {
        drv->setParamStatus(param_id[1], asynSuccess); // Force callback
        drv->setParamStatus(param_id[1], asynParamUndefined);

        drv->setParamStatus(param_id[2], asynSuccess); // Force callback
        drv->setParamStatus(param_id[2], asynParamUndefined);
        return;
    }


    drv->setIntegerParam(param_id[1], pub_data.status[dev_id].class_id);
    drv->setParamStatus( param_id[1], asynParamUndefined); // Force callback
    drv->setParamStatus( param_id[1], asynSuccess);

    drv->setIntegerParam(param_id[2], pub_data.status[dev_id].runlog);
    drv->setParamStatus( param_id[2], asynParamUndefined); // Force callback
    drv->setParamStatus( param_id[2], asynSuccess);

}



void ClassHandler::invalidate_class_params(asynPortDriver * drv)
{
    for(int i = 0; i < ASYN_PARAMS_BASE; i++)
    {
        drv->setParamStatus(param_id[i], asynTimeout);
    }
}



// EOF
