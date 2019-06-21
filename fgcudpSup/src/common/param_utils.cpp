/*!
 * © Copyright CERN 2019. All rights not expressly granted are reserved.
 * @file   param_utils.cpp
 * @brief  helper functions.
 * @author Joao Afonso
 */

#include <cstdint>
#include <string>
#include <param_utils.h>
#include <netinet/in.h>
#include "label.h"



std::string conv_fgc_flags(uint32_t flags, struct sym_name const * mapping)
{
    std::string output;    

    while(mapping->type != 0  || mapping->label != 0)
    {
        if(mapping->type & flags)
        {
            if(!output.empty())
            {
                output += " ";
            }
            output += mapping->label;
        }
        ++mapping;
    }

    return output;
}



std::string conv_fgc_enum(uint32_t enum_val, struct sym_name const * mapping)
{
     while(mapping->type != 0  || mapping->label != 0)
    {
        if(enum_val == mapping->type)
        {
            return std::string(mapping->label);
        }
        ++mapping;
    }

    return std::string("Unknown");
}



float ntohf(float input)
{
    float output;
    uint8_t *input_d  = reinterpret_cast<uint8_t*>(&input);
    uint8_t *output_d = reinterpret_cast<uint8_t*>(&output);
    output_d[0] = input_d[3];
    output_d[1] = input_d[2];
    output_d[2] = input_d[1];
    output_d[3] = input_d[0];
    return output;
}



double timeval_elapsed(struct timeval t_init, struct timeval t_end)
{
    double diff_sec  = t_end.tv_sec - t_init.tv_sec;
    double diff_usec = ((double) (t_end.tv_usec - t_init.tv_usec)) / 1000000;

    return diff_sec + diff_usec;
}


// EOF
