#include "mainConfig.h"

#ifndef NJFUWORK_FINAL_MAINCONFIG_H
#include "fake/platform.h"
#else
#include <stdint.h>
#endif

#define Current_MAC_Address 1   //1-255
// #define Communication_Mode point_to_point
#define Channel_Selection_Init _413MHZ

typedef enum{
    point_to_point,
    multicast,
    broadcast,
}communication_mode_enum;

typedef enum{
    _413MHZ,
    _424MHZ,
    _434MHZ,
    _445MHZ,
    _454MHZ,
    _463MHZ,
    _474MHZ,
    _484MHZ,
    _495MHZ,
    _503MHZ,
    _514MHZ,
    _523MHZ,
}channel_selection_enum;

extern communication_mode_enum Communication_Mode;
extern communication_mode_enum receive_data_communication_mode;
extern uint8_t channel_selection_table[];