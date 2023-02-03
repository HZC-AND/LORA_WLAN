/**
 * @file data_link_layer.c
 * @author HZC (woshihuzhicheng@njfu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-12-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include "data_link_layer.h"
#include "../SX1278/SX1278.h"
#include "device_info.h"

#include "network_layer.h"

#include "algorithm_lib.h"

extern SX1278_t SX1278;

data_link_layer_frame_t data_link_layer_frame;
data_link_layer_frame_t data_link_layer_frame_receive;//接收的数据

uint8_t data_send_buffer_for_sx1278[255];
uint8_t network_layer_frame_receive[255];

void data_link_layer_init(){
    // data_link_layer_frame.function.conmunication_mode = Communication_Mode;//点播
    // data_link_layer_frame.function.channel_selection = Channel_Selection_Init;//413mhz
    // data_link_layer_frame.function.multicast_grouping = 0b00;//组播分组1
    // data_link_layer_frame.MAC_Address = Current_MAC_Address;

    data_link_layer_frame.network_layer_data = NULL_PTR;
    
}

uint8_t data_link_layer_send(uint8_t* data){
    uint8_t data_length = 0;

    data_link_layer_frame.function.conmunication_mode = Communication_Mode;
    data_link_layer_frame.function.channel_selection = Channel_Selection_Init;//413mhz
    data_link_layer_frame.function.multicast_grouping = 0b00;//组播分组1
    data_link_layer_frame.MAC_Address = Current_MAC_Address;

    data_link_layer_frame.network_layer_data = data;
    uint8_t conmunication_mode = data_link_layer_frame.function.conmunication_mode;

    switch (data_link_layer_frame.network_layer_data[0]){
        case data_frame:
            data_link_layer_frame.data_length = data_link_layer_frame.network_layer_data[5] + 6;
            break;
        case data_ack_frame:
            data_link_layer_frame.data_length = 3;
            break;
        case retransmission_frame:
            data_link_layer_frame.data_length = 3;
            break;
        case location_frame:
            data_link_layer_frame.data_length = 2;
            break;
        case location_ack_frame:
            data_link_layer_frame.data_length = 4;
            break;
        default:
            break;
    }

    data_length = data_link_layer_frame.data_length + 4;

    /* Copy数据到buffer */
    data_send_buffer_for_sx1278[0] = (uint8_t)(*(uint8_t*)(&data_link_layer_frame.function));
    data_send_buffer_for_sx1278[1] = data_link_layer_frame.MAC_Address;
    data_send_buffer_for_sx1278[2] = data_link_layer_frame.data_length;
    for (uint8_t i = 0; i < data_link_layer_frame.data_length; i++)
    {
        data_send_buffer_for_sx1278[i + 3] = data_link_layer_frame.network_layer_data[i];
    }
    /* 计算CRC */
    data_link_layer_frame.CRC_8 = crc_8(&data_send_buffer_for_sx1278[0], data_length - 1);
    /**********/
    data_send_buffer_for_sx1278[data_length - 1] = (uint8_t)(data_link_layer_frame.CRC_8);
    /********************/


    SX1278.frequency = channel_selection_table[data_link_layer_frame.function.channel_selection];

    //SX1278_TX_Once(&SX1278,&data_send_buffer_for_sx1278[0],data_length,200);//HZC Modified in 2023-2-2
    uint8_t result = SX1278_H_TX_Once(&SX1278,&data_send_buffer_for_sx1278[0],data_length,200);

    return result;
}

uint8_t data_link_layer_receive_callback(uint8_t* data, uint8_t length){
//    uint8_t network_layer_frame_receive[255];
    /* 将数据取出 */
    if(data[2] != (length - 4)){
        //接收数据长度不正确
        return 0;
    }
    if(data[length - 1] != (crc_8(data, length - 1))){
        //crc验证不正确
        return 0;
    }
    data_link_layer_frame_receive.function.conmunication_mode = data[0] & 0x3;
    data_link_layer_frame_receive.function.channel_selection = (data[0] >> 2) & 0xF;
    data_link_layer_frame_receive.function.multicast_grouping = data[0] >> 6;
    data_link_layer_frame_receive.MAC_Address = data[1];
    data_link_layer_frame_receive.data_length = data[2];
    data_link_layer_frame_receive.CRC_8 = data[length - 1];
    for(uint8_t i = 0;i < data_link_layer_frame_receive.data_length;i++){
        network_layer_frame_receive[i] = data[i + 3];
    }
    data_link_layer_frame_receive.network_layer_data = &network_layer_frame_receive[0];
    /*************/
    
    switch (data_link_layer_frame_receive.function.conmunication_mode){
        case 0b00: //点播;
            receive_data_communication_mode = point_to_point;
            break;
        case 0b01: //组播；
            receive_data_communication_mode = multicast;
            break;
        case 0b10: //广播
            receive_data_communication_mode = broadcast;
            break;
        default:
            break;
    }

    /* 调用网络层callback */
    (void)network_layer_receive_callback(data_link_layer_frame_receive.network_layer_data, data_link_layer_frame_receive.data_length);

    return 1;
}