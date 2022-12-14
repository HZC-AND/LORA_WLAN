/**
 * @file data_link_layer.h
 * @author HZC (woshihuzhicheng@njfu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-12-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* 数据分帧，流控，地址寻址，重发*/

#include "mainConfig.h"

#ifndef NJFUWORK_FINAL_MAINCONFIG_H
#include "fake/platform.h"
#else
#include <stdint.h>
#endif
typedef struct
{
    /* data */
    uint8_t conmunication_mode : 2;
    uint8_t channel_selection : 4;
    uint8_t multicast_grouping : 2;
}data_link_layer_function_t;


typedef struct
{
    /* data */
    data_link_layer_function_t function;
    uint8_t MAC_Address;
    uint8_t data_length;
    uint8_t* network_layer_data;
    uint8_t CRC_8;
}data_link_layer_frame_t;


void data_link_layer_init();

uint8_t data_link_layer_send(uint8_t* data);
uint8_t data_link_layer_receive_callback(uint8_t* data, uint8_t length);



