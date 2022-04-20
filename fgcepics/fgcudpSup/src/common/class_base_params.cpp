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

#define CREATE_PARAM(id, type, name) do { \
    snprintf(name_buffer, NAME_BUFFER_LEN, "%d_%s", dev_tag, (name)); \
    drv->createParam(name_buffer, (type), (id)); \
    } while(0)

    CREATE_PARAM(&param_id[0], asynParamOctet        , "DATA_STATUS__STR");
    CREATE_PARAM(&param_id[1], asynParamUInt32Digital, "DATA_STATUS__RAW");
    CREATE_PARAM(&param_id[2], asynParamInt32        , "CLASS_ID__INT");
    CREATE_PARAM(&param_id[3], asynParamUInt32Digital, "RUNLOG_BYTE__RAW");

    drv->setStringParam(      param_id[0], "");
    drv->setUIntDigitalParam( param_id[1], 0, 0xFFFFFFFF);
    drv->setIntegerParam(     param_id[2], 0);
    drv->setUIntDigitalParam( param_id[3], 0, 0xFFFFFFFF);
}



void ClassHandler::update_class_params(asynPortDriver * drv, Pub_data & pub_data)
{
    std::string data_status_str = conv_fgc_flags(pub_data.status[dev_id].data_status, sym_names_base_data_status);

    // DATA_STATUS should allways be valid and show values
    drv->setStringParam(      param_id[0], data_status_str);
    drv->setParamStatus(      param_id[0], asynParamUndefined); // Force callback
    drv->setParamStatus(      param_id[0], asynSuccess);

    drv->setUIntDigitalParam( param_id[1], pub_data.status[dev_id].data_status, 0xFFFF);
    drv->setParamStatus(      param_id[1], asynParamUndefined); // Force callback
    drv->setParamStatus(      param_id[1], asynSuccess);

    //Check if common data is valid
    if(
        !(pub_data.status[dev_id].data_status & FGC_DATA_VALID)
    )
    {
        drv->setParamStatus(param_id[2], asynSuccess); // Force callback
        drv->setParamStatus(param_id[2], asynParamUndefined);

        drv->setParamStatus(param_id[3], asynSuccess); // Force callback
        drv->setParamStatus(param_id[3], asynParamUndefined);
        return;
    }


    drv->setIntegerParam(     param_id[2], pub_data.status[dev_id].class_id);
    drv->setParamStatus(      param_id[2], asynParamUndefined); // Force callback
    drv->setParamStatus(      param_id[2], asynSuccess);

    drv->setUIntDigitalParam( param_id[3], pub_data.status[dev_id].runlog, 0xFFFF);
    drv->setParamStatus(      param_id[3], asynParamUndefined); // Force callback
    drv->setParamStatus(      param_id[3], asynSuccess);

}



void ClassHandler::invalidate_class_params(asynPortDriver * drv)
{
    for(int i = 0; i < ASYN_PARAMS_BASE; i++)
    {
        drv->setParamStatus(param_id[i], asynTimeout);
    }
}



// EOF
