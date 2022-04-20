/*!
 * © Copyright CERN 2019. All rights not expressly granted are reserved.
 * @file   param_utils.h
 * @brief  helper functions.
 * @author Joao Afonso
 */

#ifndef PARAM_UTILS_H
#define PARAM_UTILS_H

#include <stdint.h>
#include <string>
#include <netinet/in.h>
#include "label.h"



std::string conv_fgc_flags(uint32_t flags, struct sym_name const * mapping);
std::string conv_fgc_enum(uint32_t enum_val, struct sym_name const * mapping);

float ntohf(float);

double timeval_elapsed(struct timeval t_init, struct timeval t_end);



#endif
// EOF
