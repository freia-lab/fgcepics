// ../../sw/inc/classes/63/sub_defsyms.h - This file is automatically generated by `parser/Output/C/Sub_defsyms.pm`  DO NOT EDIT
#ifndef SUB_63_DEFSYMS_H
#define SUB_63_DEFSYMS_H

#include <classes/63/sub_defconst.h>
#include <label.h>
#include <deftypes.h>
#include <fgc_stat.h>

// OP

CONST struct sym_name sym_names_63_op[]
        =
{
    { SUB_63_OP_BOOT                     , "BOOT"                      },
    { SUB_63_OP_CALIBRATING              , "CALIBRATING"               },
    { SUB_63_OP_NORMAL                   , "NORMAL"                    },
    { SUB_63_OP_PROGRAMMING              , "PROGRAMMING"               },
    { SUB_63_OP_SIMULATION               , "SIMULATION"                },
    { SUB_63_OP_TEST                     , "TEST"                      },
    { SUB_63_OP_UNCONFIGURED             , "UNCONFIGURED"              },
    { 0                            , 0                           }
}
;

// PC

CONST struct sym_name sym_names_63_pc[]
        =
{
    { SUB_63_PC_ABORTING                 , "ABORTING"                  },
    { SUB_63_PC_ARMED                    , "ARMED"                     },
    { SUB_63_PC_BLOCKING                 , "BLOCKING"                  },
    { SUB_63_PC_CYCLING                  , "CYCLING"                   },
    { SUB_63_PC_DIRECT                   , "DIRECT"                    },
    { SUB_63_PC_ECONOMY                  , "ECONOMY"                   },
    { SUB_63_PC_FLT_OFF                  , "FLT_OFF"                   },
    { SUB_63_PC_FLT_STOPPING             , "FLT_STOPPING"              },
    { SUB_63_PC_IDLE                     , "IDLE"                      },
    { SUB_63_PC_OFF                      , "OFF"                       },
    { SUB_63_PC_ON_STANDBY               , "ON_STANDBY"                },
    { SUB_63_PC_PAUSED                   , "PAUSED"                    },
    { SUB_63_PC_POL_SWITCHING            , "POL_SWITCHING"             },
    { SUB_63_PC_RUNNING                  , "RUNNING"                   },
    { SUB_63_PC_SLOW_ABORT               , "SLOW_ABORT"                },
    { SUB_63_PC_STARTING                 , "STARTING"                  },
    { SUB_63_PC_STOPPING                 , "STOPPING"                  },
    { SUB_63_PC_TO_CYCLING               , "TO_CYCLING"                },
    { SUB_63_PC_TO_STANDBY               , "TO_STANDBY"                },
    { 0                            , 0                           }
}
;

// PLL

CONST struct sym_name sym_names_63_pll[]
        =
{
    { SUB_63_PLL_CAPTURE                  , "CAPTURE"                   },
    { SUB_63_PLL_FAILED                   , "FAILED"                    },
    { SUB_63_PLL_FAST_SLEW                , "FAST_SLEW"                 },
    { SUB_63_PLL_LOCKED                   , "LOCKED"                    },
    { SUB_63_PLL_NO_SYNC                  , "NO_SYNC"                   },
    { 0                            , 0                           }
}
;

// VS

CONST struct sym_name sym_names_63_vs[]
        =
{
    { SUB_63_VS_BLOCKED                  , "BLOCKED"                   },
    { SUB_63_VS_FASTPA_OFF               , "FASTPA_OFF"                },
    { SUB_63_VS_FAST_STOP                , "FAST_STOP"                 },
    { SUB_63_VS_FLT_OFF                  , "FLT_OFF"                   },
    { SUB_63_VS_INVALID                  , "INVALID"                   },
    { SUB_63_VS_NONE                     , "NONE"                      },
    { SUB_63_VS_OFF                      , "OFF"                       },
    { SUB_63_VS_READY                    , "READY"                     },
    { SUB_63_VS_STARTING                 , "STARTING"                  },
    { SUB_63_VS_STOPPING                 , "STOPPING"                  },
    { 0                            , 0                           }
}
;

// ADC_STATUS

CONST struct sym_name sym_names_63_adc_status[]
        =
{
    { SUB_63_ADC_STATUS_CAL_ACTIVE               , "CAL_ACTIVE"                },
    { SUB_63_ADC_STATUS_CAL_FAILED               , "CAL_FAILED"                },
    { SUB_63_ADC_STATUS_IN_USE                   , "IN_USE"                    },
    { SUB_63_ADC_STATUS_SIGNAL_STUCK             , "SIGNAL_STUCK"              },
    { SUB_63_ADC_STATUS_V_MEAS_OK                , "V_MEAS_OK"                 },
    { 0                            , 0                           }
}
;

// DCCT

CONST struct sym_name sym_names_63_dcct[]
        =
{
    { SUB_63_DCCT_CAL_FLT                  , "CAL_FLT"                   },
    { SUB_63_DCCT_FAULT                    , "FAULT"                     },
    { SUB_63_DCCT_MEAS_OK                  , "MEAS_OK"                   },
    { 0                            , 0                           }
}
;

// FLT

CONST struct sym_name sym_names_63_flt[]
        =
{
    { SUB_63_FLT_FAST_ABORT               , "FAST_ABORT"                },
    { SUB_63_FLT_FGC_HW                   , "FGC_HW"                    },
    { SUB_63_FLT_FGC_STATE                , "FGC_STATE"                 },
    { SUB_63_FLT_I_MEAS                   , "I_MEAS"                    },
    { SUB_63_FLT_I_RMS_LIM                , "I_RMS_LIM"                 },
    { SUB_63_FLT_LIMITS                   , "LIMITS"                    },
    { SUB_63_FLT_NO_PC_PERMIT             , "NO_PC_PERMIT"              },
    { SUB_63_FLT_POL_SWITCH               , "POL_SWITCH"                },
    { SUB_63_FLT_REG_ERROR                , "REG_ERROR"                 },
    { SUB_63_FLT_SLAVE                    , "SLAVE"                     },
    { SUB_63_FLT_VS_EXTINTLOCK            , "VS_EXTINTLOCK"             },
    { SUB_63_FLT_VS_FAULT                 , "VS_FAULT"                  },
    { SUB_63_FLT_VS_RUN_TO                , "VS_RUN_TO"                 },
    { SUB_63_FLT_VS_STATE                 , "VS_STATE"                  },
    { SUB_63_FLT_V_ERROR                  , "V_ERROR"                   },
    { 0                            , 0                           }
}
;

// LAT

CONST struct sym_name sym_names_63_lat[]
        =
{
    { SUB_63_LAT_DAC_FLT                  , "DAC_FLT"                   },
    { SUB_63_LAT_DALLAS_FLT               , "DALLAS_FLT"                },
    { SUB_63_LAT_DCCT_PSU_FAIL            , "DCCT_PSU_FAIL"             },
    { SUB_63_LAT_DIMS_EXP_FLT             , "DIMS_EXP_FLT"              },
    { SUB_63_LAT_DIM_SYNC_FLT             , "DIM_SYNC_FLT"              },
    { SUB_63_LAT_DIM_TRIG_FLT             , "DIM_TRIG_FLT"              },
    { SUB_63_LAT_DSP_FLT                  , "DSP_FLT"                   },
    { SUB_63_LAT_FGC_PSU_FAIL             , "FGC_PSU_FAIL"              },
    { SUB_63_LAT_ID_FLT                   , "ID_FLT"                    },
    { SUB_63_LAT_SCIVS_EXP_FLT            , "SCIVS_EXP_FLT"             },
    { SUB_63_LAT_SPIVS_FLT                , "SPIVS_FLT"                 },
    { SUB_63_LAT_VDC_FAIL                 , "VDC_FAIL"                  },
    { SUB_63_LAT_VS_COMMS                 , "VS_COMMS"                  },
    { 0                            , 0                           }
}
;

// UNL

CONST struct sym_name sym_names_63_unl[]
        =
{
    { SUB_63_UNL_FIELDBUS_WRN             , "FIELDBUS_WRN"              },
    { SUB_63_UNL_I_MEAS_DIFF              , "I_MEAS_DIFF"               },
    { SUB_63_UNL_LOG_PLEASE               , "LOG_PLEASE"                },
    { SUB_63_UNL_LOW_CURRENT              , "LOW_CURRENT"               },
    { SUB_63_UNL_NOMINAL_LOAD             , "NOMINAL_LOAD"              },
    { SUB_63_UNL_PC_DISCH_RQ              , "PC_DISCH_RQ"               },
    { SUB_63_UNL_PC_PERMIT                , "PC_PERMIT"                 },
    { SUB_63_UNL_PC_REG_OK                , "PC_REG_OK"                 },
    { SUB_63_UNL_POL_SWI_NEG              , "POL_SWI_NEG"               },
    { SUB_63_UNL_POL_SWI_POS              , "POL_SWI_POS"               },
    { SUB_63_UNL_POST_MORTEM              , "POST_MORTEM"               },
    { SUB_63_UNL_PWR_FAILURE              , "PWR_FAILURE"               },
    { SUB_63_UNL_REF_RT_ACTIVE            , "REF_RT_ACTIVE"             },
    { SUB_63_UNL_SYNC_PLEASE              , "SYNC_PLEASE"               },
    { SUB_63_UNL_SYNC_REGFGC3             , "SYNC_REGFGC3"              },
    { SUB_63_UNL_VS_POWER_ON              , "VS_POWER_ON"               },
    { 0                            , 0                           }
}
;

// WRN

CONST struct sym_name sym_names_63_wrn[]
        =
{
    { SUB_63_WRN_CONFIG                   , "CONFIG"                    },
    { SUB_63_WRN_EXT_SYNC_LOST            , "EXT_SYNC_LOST"             },
    { SUB_63_WRN_FGC_HW                   , "FGC_HW"                    },
    { SUB_63_WRN_FGC_PSU                  , "FGC_PSU"                   },
    { SUB_63_WRN_I_MEAS                   , "I_MEAS"                    },
    { SUB_63_WRN_I_RMS_LIM                , "I_RMS_LIM"                 },
    { SUB_63_WRN_REF_LIM                  , "REF_LIM"                   },
    { SUB_63_WRN_REF_RATE_LIM             , "REF_RATE_LIM"              },
    { SUB_63_WRN_REG_ERROR                , "REG_ERROR"                 },
    { SUB_63_WRN_SIMULATION               , "SIMULATION"                },
    { SUB_63_WRN_SUBCONVTR_FLT            , "SUBCONVTR_FLT"             },
    { SUB_63_WRN_TEMPERATURE              , "TEMPERATURE"               },
    { SUB_63_WRN_TIMING_EVT               , "TIMING_EVT"                },
    { SUB_63_WRN_V_ERROR                  , "V_ERROR"                   },
    { 0                            , 0                           }
}
;

// DATA_STATUS

CONST struct sym_name sym_names_63_data_status[]
        =
{
    { FGC_DEVICE_IN_DB, "DEVICE_IN_DB" },
    { FGC_DATA_VALID  , "DATA_VALID"   },
    { FGC_CLASS_VALID , "CLASS_VALID"  },
    { 0               , 0                }
}
;

#endif