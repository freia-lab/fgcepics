/*!
 * © Copyright CERN 2019. All rights not expressly granted are reserved.
 * @file   class_base_params.h
 * @brief  parse incoming UDP publication data common to all FGC classes.
 * @author Joao Afonso
 */

#ifndef CLASS_BASE_PARAMS_H
#define CLASS_BASE_PARAMS_H

#include <asynPortDriver.h>
#include "pub_data.h"

#define ASYN_PARAMS_BASE 3



class ClassHandler
{
protected:
    int param_id[ASYN_PARAMS_BASE];
    int dev_id;
    int dev_addr;

public:
    ClassHandler(int id, int addr) : dev_id(id), dev_addr(addr) {};
    virtual void create_class_params(asynPortDriver * drv);
    virtual void update_class_params(asynPortDriver * drv, Pub_data & pub_data);
    virtual void invalidate_class_params(asynPortDriver * drv);
    int get_id()   { return dev_id;   };
    int get_addr() { return dev_addr; };
};



#endif
// EOF
