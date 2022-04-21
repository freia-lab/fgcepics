/*!
 * © Copyright CERN 2019. All rights not expressly granted are reserved.
 * @file   asubroutine.c
 * @brief  aSub record - associated subroutines definition.
 * @author Joao Afonso
 */

#include <stdlib.h>
#include <errlog.h>
#include <registryFunction.h>
#include <menuFtype.h>
#include <aSubRecord.h>
#include <epicsExport.h>
#include <stdio.h>

/**
 * Given two arrays of doubles (time and ref), merges them and prints to a single char array.
 *
 * Format:  [<time>|<ref>,]*<time>|<ref>
 *
 * Example: 
 *     Input 1:  [0.01, 0.02, 0.03]
 *     Input 2:  [0.123, 0.456, 0.789]
 *     Output:   "0.01|0.123,0.02|0.456,0.03|0.789"
 *
 * @param prec->fta          type of input buffer A, must be DOUBLE
 * @param prec->ftb          type of input buffer B, must be DOUBLE
 * @param prec->ftva         type of output buffer A, must be CHAR
 * @param prec->a            pointer to input buffer A
 * @param prec->b            pointer to input buffer B
 * @param prec->vala         pointer to output buffer A
 * @param prec->nea          length of data in input buffer A
 * @param prec->neb          length of data in input buffer B
 * @param prec->nova         size in output buffer A
 *
 * @return                   error code, or zero
 */
static long time_val_write(aSubRecord * prec)
{
    if(prec->fta != menuFtypeDOUBLE)
    {
        errlogSevPrintf(errlogMajor, "%s incorrect input argument type [A]", prec->name);
        return 1;
    }

    if(prec->ftb != menuFtypeDOUBLE)
    {
        errlogSevPrintf(errlogMajor, "%s incorrect input argument type [B]", prec->name);
        return 1;
    }
    
    if(prec->ftva != menuFtypeCHAR)
    {
        errlogSevPrintf(errlogMajor, "%s incorrect output argument type [VALA]", prec->name);
        return 1;
    }   

    double *in_a, *in_b;
    in_a = (double *) prec->a;
    in_b = (double *) prec->b;

    if(prec->nea != prec->neb)
    {
        errlogSevPrintf(errlogMajor, "%s input arrays [A,B] have different lengths", prec->name);       
        return 1;
    }

    int write_offset  = 0;
    int available_len = prec->nova;
    char* out_buffer  = prec->vala;
    long i;

    // Read two arrays, merge and print values
    for(i = 0; i < prec->nea; i++)
    {
        int num_read = snprintf(&out_buffer[write_offset], available_len, "%f|%f,", in_a[i], in_b[i]);
        if(num_read > available_len)
        {
            errlogSevPrintf(errlogMajor, "%s output buffer is full, unable to write more [VALA]", prec->name);
            return 1;
        }

        write_offset  += num_read;
        available_len -= num_read;
    }

    // Remove trailing comma
    int last_null = write_offset > 0 ? write_offset-1 : 0;
    out_buffer[last_null] = '\0';

    return 0;
}



/**
 * Given a char array containing a list of pairs <time, ref>, splits these values into two arrays,
 * one for the time values, the other for reference values.
 *
 * Format input:  [<time>|<value>,]*<time>|<value>
 *
 * Example: 
 *     Input:     "0.01|0.123,0.02|0.456,0.03|0.789"
 *     Output 1:  [0.01, 0.02, 0.03]
 *     Output 2:  [0.123, 0.456, 0.789]
 *
 * @param prec->fta          type of input buffer A, must be CHAR
 * @param prec->ftva         type of output buffer A, must be DOUBLE
 * @param prec->ftvb         type of output buffer B, must be DOUBLE
 * @param prec->a            pointer to input buffer A
 * @param prec->vala         pointer to output buffer A
 * @param prec->valb         pointer to output buffer B
 * @param prec->noa          size of input buffer A
 * @param prec->nova         size of output buffer A
 * @param prec->novb         size of output buffer B
 *
 * @return                   error code, or zero
 */
static long time_val_read(aSubRecord * prec)
{
    if(prec->fta != menuFtypeCHAR)
    {
        errlogSevPrintf(errlogMajor, "%s incorrect input argument type [A]", prec->name);
        return 1;
    }

    if(prec->ftva != menuFtypeDOUBLE)
    {
        errlogSevPrintf(errlogMajor, "%s incorrect output argument type [VALA]", prec->name);
        return 1;
    }
    
    if(prec->ftvb != menuFtypeDOUBLE)
    {
        errlogSevPrintf(errlogMajor, "%s incorrect output argument type [VALB]", prec->name);
        return 1;
    }   

    char* in_a = (char *) prec->a;
    int read_idx  = 0;
    int write_idx = 0;

    int read_offset;
    double time_val, ref_val;

    while(read_idx < prec->nea && sscanf(&in_a[read_idx], "%lf|%lf%n", &time_val, &ref_val, &read_offset) == 2)
    { 
        if(write_idx >= prec->nova)
        {
            errlogSevPrintf(errlogMajor, "%s output buffer is full, unable to write more [VALA]", prec->name);
            return 1;
        }       
        if(write_idx >= prec->novb)
        {
            errlogSevPrintf(errlogMajor, "%s output buffer is full, unable to write more [VALB]", prec->name);
            return 1;
        }      

        ((double *)prec->vala)[write_idx] = time_val; 
        ((double *)prec->valb)[write_idx] = ref_val; 

        write_idx++;
        read_idx += read_offset;
        read_idx++; // To skip comma ','
    }

    prec->neva = write_idx;
    prec->nevb = write_idx;

    return 0;
}



// Register methods with EPICS framework
epicsRegisterFunction(time_val_read);
epicsRegisterFunction(time_val_write);



// EOF
