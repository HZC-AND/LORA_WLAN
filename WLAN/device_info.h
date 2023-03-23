/**
 * @file device_info.h
 * @author HZC (woshihuzhicheng@njfu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-12-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "mainConfig.h"

#ifndef NJFUWORK_FINAL_MAINCONFIG_H
#include "fake/platform.h"
#else
#include <stdint.h>
#endif

#define Packet_Loss_Rate_Test STD_OFF //Packet_Loss_Rate_Test 开关

#define Current_MAC_Address 1   //1-255
#define To_MAC_Address 2 //1-255
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
extern uint64_t channel_selection_table[];