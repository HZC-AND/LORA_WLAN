#include "fake/platform.h"

#define Current_MAC_Address 1   //1-255
#define Communication_Mode_Init point_to_point
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

extern communication_mode_enum global_communication_mode;
extern uint8_t channel_selection_table[];