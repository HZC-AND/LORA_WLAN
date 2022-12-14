/**
 * @file network_layer.c
 * 
 * @author HZC (woshihuzhicheng@njfu.edu.cn)
 * @brief 网络层，除TOF和RSSI定位，其余报文为异步发送异步接收
 * @version 0.1
 * @date 2022-12-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include "network_layer.h"
#include "data_link_layer.h"
#include "device_info.h"

#include "algorithm_lib.h"

/* 路由表 */

/* 定位表 */

/* 已转发报文表 */

/* 报文分帧数据 */
network_layer_sub_frame_send_data_t network_layer_sub_frame_send_data;
network_layer_sub_frame_receive_data_t network_layer_sub_frame_receive_data;

/* 网络层状态 */
network_layer_send_state_enum network_layer_send_state = idle;
network_layer_receive_state_enum network_layer_receive_state = idle;
/*************/

/* 接收数据的全局变量 */
network_layer_data_frame_t network_layer_receive_data_frame;
network_layer_data_ack_frame_t network_layer_receive_data_ack_frame;
/* **************** */

/* 报文转发表 */
#define Network_Layer_Data_Frame_Forwarding_Table_Max_Length 10
#define Network_Layer_Data_Frame_Forwarding_Element_Max_Time_Counter 5
forwarding_table_t network_layer_data_frame_forwarding_table[Network_Layer_Data_Frame_Forwarding_Table_Max_Length];/* 数据报文转发表 */
stack_t data_frame_forwarding_table_stack;/* 数据报文转发表栈 */

#define Network_Layer_Data_Ack_Frame_Forwarding_Table_Max_Length 10
#define Network_Layer_Data_Ack_Frame_Forwarding_Table_Max_Time_Counter 5
forwarding_table_t network_layer_data_ack_frame_forwarding_table[Network_Layer_Data_Ack_Frame_Forwarding_Table_Max_Length];/* 数据报文转发表 */
stack_t data_ack_frame_forwarding_table_stack;/* 数据ack报文转发表栈 */
/* ********* */

/* 上层数据的全局变量 */
network_layer_from_up_layer_send_data_t network_layer_send_data;
network_layer_to_up_layer_receive_data_t network_layer_receive_data;//接收到的网络层帧，全局变量
/* **************** */
uint8_t receive_data_buffer[255];//用于保存收到的数据内容，全局变量

/* ACK等待计时器 */
uint8_t waiting_ack_frame_timer_start = 0;//0关闭，1开启
uint8_t waiting_ack_frame_timer_counter = 10;//单位s,向下计数
uint8_t waiting_ack_frame_timer_timeout;//等待超时标志
/* ************ */

/* 转发表计数器 */
/* *********** */

/* TOF相关时间变量 */
/* TODO:考虑us级硬件定时器的使用 */
//TOF发起端
uint64_t t1,t4,t5;
TOF_delta_t_t delta_t_3_2_sender_side;
TOF_delta_t_t delta_t_6_3_serder_side;
//TOF接收端
uint64_t t2,t3,t6;
TOF_delta_t_t delta_t_3_2_receiver_side;
TOF_delta_t_t delta_t_6_3_receiver_side;
/*****************/

/* TOF接收数据报文 */
network_layer_TOF_frame_t network_layer_TOF_frame_received;
/* ************* */

void network_layer_init(void){

    network_layer_receive_data_frame.data = &receive_data_buffer[0];

    /* TODO:network_layer_send_data 初始化 */

    /* network_layer_sub_frame_send_data 初始化 */
    network_layer_sub_frame_send_data.current_frame_number = 1;

    /* 报文转发表，数据报文转发表栈初始化 */
    stack_create(&data_frame_forwarding_table_stack, 
                  network_layer_data_frame_forwarding_table, 
                  Network_Layer_Data_Frame_Forwarding_Table_Max_Length, 
                  sizeof(forwarding_table_t));

    /* 报文转发表，数据Ack报文转发表栈初始化 */
    stack_create(&data_ack_frame_forwarding_table_stack,
                  network_layer_data_ack_frame_forwarding_table,
                  Network_Layer_Data_Ack_Frame_Forwarding_Table_Max_Length,
                  sizeof(forwarding_table_t));
}
/* 
to_mac_address == 0就是广播
 */
uint8_t network_layer_data_frame_send(uint8_t* data, uint8_t length, uint8_t to_mac_address){

    if((Communication_Mode == point_to_point) && (to_mac_address == 0)){
        /* 点播模式不能发送0地址 */
        return 0;
    }

    if(network_layer_send_state == idle){
        if(length <= MAX_DATA_LENGTH){
            /* TODO: 需要添加临界区 */
            network_layer_send_state = sending_single_data_frame;
            network_layer_send_data.data = data;
            network_layer_send_data.length = length;
            network_layer_send_data.to_mac_address = to_mac_address;
            /***********************/
        }else{
            /* 分帧 */
            network_layer_send_state = sending_single_data_frame;
            split_frame_to_sub_frame_data_table(data, length, to_mac_address);
        }
    }else{
        return 0;
    }

    return 1;
}

/* TODO:完善函数 */
void network_layer_location_frame_send(void){
    network_layer_data_frame_t network_layer_data_frame;
    network_layer_data_frame.frame_type = location_frame;
    // data_link_layer_send();
}
/* TODO:完善函数 */
void network_layer_location_ack_frame_send(void){
    network_layer_data_frame_t network_layer_data_frame;
    network_layer_data_frame.frame_type = location_ack_frame;
    // data_link_layer_send();
}
/**
 * @brief 重发送帧发送函数
 * 
 */
/* TODO:完善函数 */
void network_layer_retransmission_frame_send(void){
    network_layer_data_frame_t network_layer_data_frame;
    network_layer_data_frame.frame_type = retransmission_frame;
    // data_link_layer_send();
}
/**
 * @brief data ack 帧发送函数
 * 
 */
void network_layer_data_ack_frame_send(void){
    uint8_t result = 0;
    
    network_layer_data_ack_frame_t network_layer_data_ack_frame;
    network_layer_data_ack_frame.frame_type = data_ack_frame;
    network_layer_data_ack_frame.from_mac_address = Current_MAC_Address;
    network_layer_data_ack_frame.to_mac_address = network_layer_receive_data.from_mac_address;
    result = data_link_layer_send(&network_layer_data_ack_frame);
   
    return result;
}

/**
 * @brief TOF帧发送函数
 * 
 * @param step 
 * @param to_mac_address 
 */
void network_layer_TOF_frame_send(uint8_t step, uint8_t to_mac_address){
    uint8_t result = 0;

    network_layer_TOF_frame_t network_layer_TOF_frame;
    network_layer_TOF_frame.frame_type = TOF_frame;
    network_layer_TOF_frame.from_mac_address = Current_MAC_Address;
    network_layer_TOF_frame.to_mac_address = to_mac_address;
    /* TODO:接收或发送的数组是MSB还是LSB需要验证 */
    switch (step)
    {
        case step1:
            network_layer_TOF_frame.delta_t[4] = 0;
            network_layer_TOF_frame.delta_t[3] = 0;
            network_layer_TOF_frame.delta_t[2] = 0;
            network_layer_TOF_frame.delta_t[1] = 0;
            network_layer_TOF_frame.delta_t[0] = 0;
            break;
        case step2:
            network_layer_TOF_frame.delta_t[4] = delta_t_3_2_receiver_side.time_ns; 
            network_layer_TOF_frame.delta_t[3] = (delta_t_3_2_receiver_side.time_ns >> 8) | (delta_t_3_2_receiver_side.time_us << 2);
            network_layer_TOF_frame.delta_t[2] = (delta_t_3_2_receiver_side.time_us >> 6) | (delta_t_3_2_receiver_side.time_ms << 4);
            network_layer_TOF_frame.delta_t[1] = (delta_t_3_2_receiver_side.time_ms >> 4) | (delta_t_3_2_receiver_side.time_s << 6);
            network_layer_TOF_frame.delta_t[0] = (delta_t_3_2_receiver_side.time_s >> 2) | step;
            break;
        case step3:
            network_layer_TOF_frame.delta_t[4] = 0;
            network_layer_TOF_frame.delta_t[3] = 0;
            network_layer_TOF_frame.delta_t[2] = 0;
            network_layer_TOF_frame.delta_t[1] = 0;
            network_layer_TOF_frame.delta_t[0] = 0;
            break;
        case step4:
            network_layer_TOF_frame.delta_t[4] = delta_t_6_3_receiver_side.time_ns; 
            network_layer_TOF_frame.delta_t[3] = (delta_t_6_3_receiver_side.time_ns >> 8) | (delta_t_6_3_receiver_side.time_us << 2);
            network_layer_TOF_frame.delta_t[2] = (delta_t_6_3_receiver_side.time_us >> 6) | (delta_t_6_3_receiver_side.time_ms << 4);
            network_layer_TOF_frame.delta_t[1] = (delta_t_6_3_receiver_side.time_ms >> 4) | (delta_t_6_3_receiver_side.time_s << 6);
            network_layer_TOF_frame.delta_t[0] = (delta_t_6_3_receiver_side.time_s >> 2) | step;
            break;
        default:
            break;
    }

    result = data_link_layer_send(&network_layer_TOF_frame);

    return result;
}

void network_layer_TOF_frame_receive(network_layer_TOF_frame_t network_layer_TOF_frame){
    TOF_delta_t_t TOF_delta_t;

    TOF_delta_t.time_ns = network_layer_TOF_frame.delta_t[4] | (network_layer_TOF_frame.delta_t[3] & 0x3);
    TOF_delta_t.time_us = (network_layer_TOF_frame.delta_t[3] >> 2) | (network_layer_TOF_frame.delta_t[2] & 0xF);
    TOF_delta_t.time_ms = (network_layer_TOF_frame.delta_t[2] >> 4) | (network_layer_TOF_frame.delta_t[1] & 0x3F);
    TOF_delta_t.time_s = (network_layer_TOF_frame.delta_t[1] >> 4) | (network_layer_TOF_frame.delta_t[0] & 0xF);
    TOF_delta_t.step = network_layer_TOF_frame.delta_t[0] >> 4;

    if(TOF_delta_t.step == step2){
        delta_t_3_2_sender_side.step = TOF_delta_t.step;
        delta_t_3_2_sender_side.time_ns = TOF_delta_t.time_ns;
        delta_t_3_2_sender_side.time_us = TOF_delta_t.time_us;
        delta_t_3_2_sender_side.time_ms = TOF_delta_t.time_ms;
        delta_t_3_2_sender_side.time_s = TOF_delta_t.time_s;
    }else if(TOF_delta_t.step == step4){
        delta_t_6_3_serder_side.step = TOF_delta_t.step;
        delta_t_6_3_serder_side.time_ns = TOF_delta_t.time_ns;
        delta_t_6_3_serder_side.time_us = TOF_delta_t.time_us;
        delta_t_6_3_serder_side.time_ms = TOF_delta_t.time_ms;
        delta_t_6_3_serder_side.time_s = TOF_delta_t.time_s;
    }

}

/**
 * @brief 数据接收call_back
 * 
 * @param data 
 * @param length 
 * @return uint8_t 
 */
uint8_t network_layer_receive_callback(uint8_t* data, uint8_t length){
    /* 同步处理报文 */
    switch (data[0])
    {
        case TOF_frame:
            network_layer_TOF_frame_t* network_layer_TOF_frame = (network_layer_TOF_frame_t*)data;
            network_layer_TOF_frame_receive(network_layer_TOF_frame);
            break;
        
        default:
            break;
    }
    /* 异步处理报文 */
    (void)copy_data_to_receive_frame(data);
    return 1;
}

/**
 * @brief copy数据至buffer
 * 
 * @param buffer 
 * @param network_layer_data_frame 
 */
void copy_data_to_send_buffer(uint8_t* buffer, network_layer_data_frame_t* network_layer_data_frame){
    buffer[0] = network_layer_data_frame->frame_type;
    buffer[1] = network_layer_data_frame->message_number;
    buffer[2] = network_layer_data_frame->message_counter;
    buffer[3] = network_layer_data_frame->from_mac_address;
    buffer[4] = network_layer_data_frame->to_mac_address;
    buffer[5] = network_layer_data_frame->data_length;
    for(uint8_t i = 0;i < network_layer_data_frame->data_length;i++){
        buffer[i + 6] = network_layer_data_frame->data[i];
    }
}

/**
 * @brief copy数据至receive frame,从局部变量到全局变量
 * 
 * @param data 
 * @return uint8_t 
 */
uint8_t copy_data_to_receive_frame(uint8_t* data){

    switch (data[0])
    {
        case data_frame:
            network_layer_data_frame_t* network_layer_data_frame = (network_layer_data_frame_t*)data;
            network_layer_receive_data_frame.frame_type = network_layer_data_frame->frame_type;
            network_layer_receive_data_frame.message_number = network_layer_data_frame->message_number;
            network_layer_receive_data_frame.message_counter = network_layer_data_frame->message_counter;
            network_layer_receive_data_frame.from_mac_address = network_layer_data_frame->from_mac_address;
            network_layer_receive_data_frame.to_mac_address = network_layer_data_frame->to_mac_address;
            for(uint8_t i = 0;i < network_layer_data_frame->data_length;i++){
                receive_data_buffer[i] = network_layer_data_frame->data[i];
            }
            /* 状态机 */
            if(network_layer_data_frame->message_number == 1){
                network_layer_receive_state = received_single_data_frame;
            }else{
                network_layer_receive_state = received_multiple_data_frame;
            }
            /* ***** */
            break;
        case data_ack_frame:
            network_layer_data_ack_frame_t* network_layer_data_ack_frame = (network_layer_data_ack_frame_t*)data;
            network_layer_receive_data_ack_frame.frame_type = network_layer_data_ack_frame->frame_type;
            network_layer_receive_data_ack_frame.from_mac_address = network_layer_data_ack_frame->from_mac_address;
            network_layer_receive_data_ack_frame.to_mac_address = network_layer_data_ack_frame->to_mac_address;
            /* 状态机 */
            network_layer_receive_state = received_data_ack_frame;
            /* ***** */
            break;
        case retransmission_frame:
            /* TODO: */
            network_layer_retransmission_frame_t* network_layer_retransmission_frame = (network_layer_retransmission_frame_t*)data;
            break;
        case location_frame:
            /* TODO: */
            network_layer_location_frame_t* network_layer_location_frame = (network_layer_location_frame_t*)data;
            break;
        case location_ack_frame:
            /* TODO: */
            network_layer_location_ack_frame_t* network_layer_location_ack_frame = (network_layer_location_ack_frame_t*)data;
            break;
        default:
            break;
    }

    return 1;
}

/**
 * @brief 分帧
 * 
 * @param data 
 * @param length 
 * @param to_mac_address 
 * @return uint8_t 
 */
uint8_t split_frame_to_sub_frame_data_table(uint8_t* data, uint8_t length, uint8_t to_mac_address){
    uint8_t frame_number = 0;

    /* 重置 network_layer_sub_frame_send_data */
    // (void)memset(&network_layer_sub_frame_send_data, 0, sizeof(network_layer_sub_frame_send_data_t));

    if(length % MAX_DATA_LENGTH){
        frame_number = length / MAX_DATA_LENGTH + 1;
    }else{
        frame_number = length / MAX_DATA_LENGTH;
    }

    if(frame_number > MAX_SUB_FRAME_NUMBER){
        /* 不支持发送这么多数据 */
        return 0;
    }

    network_layer_sub_frame_send_data.length = length;
    network_layer_sub_frame_send_data.to_mac_address = to_mac_address;
    network_layer_sub_frame_send_data.frame_number = frame_number;
    network_layer_sub_frame_send_data.current_frame_number = 1;

    for(uint8_t i = 0;i < frame_number;i++){
        for(uint8_t j = 0;j < MAX_DATA_LENGTH;j++){
            if((i * MAX_DATA_LENGTH + j) == length){
                break;
            }
            network_layer_sub_frame_send_data.network_layer_sub_frame_data_table[i][j] = data[i * MAX_DATA_LENGTH + j];
        }
    }

    return 1;
}

/**
 * @brief 组帧
 * 
 * @param data 
 * @param length 
 * @return uint8_t 
 */
uint8_t combine_sub_frame_data(uint8_t* data, uint8_t length){
    /* TODO: */
    //存入表中
    //当完整收到所有数据后将表组合成一个网络层数据。
}

network_layer_send_state_enum get_network_layer_send_state(void){
    return network_layer_send_state;
}

static uint8_t network_layer_data_frame_send_single_frame(void){
    uint8_t result = 0;

    network_layer_data_frame_t network_layer_data_frame;
    network_layer_data_frame.frame_type = data_frame;

    network_layer_data_frame.data_length = network_layer_send_data.length;
    network_layer_data_frame.message_number = 1;
    network_layer_data_frame.message_counter = 1;
    network_layer_data_frame.from_mac_address = Current_MAC_Address;
    network_layer_data_frame.to_mac_address = network_layer_send_data.to_mac_address;
    network_layer_data_frame.data = network_layer_send_data.data;
    /*copy数据至buffer*/
    uint8_t network_layer_data_buffer[MAX_DATA_LENGTH + 6];
    copy_data_to_send_buffer(&network_layer_data_buffer[0],&network_layer_data_frame);
    /*****************/
    result = data_link_layer_send(&network_layer_data_buffer[0]);

    return result;
}

static uint8_t network_layer_data_frame_send_single_frame_forwarding(network_layer_data_frame_t* network_layer_data_frame){
    uint8_t result = 0;

    /*copy数据至buffer*/
    uint8_t network_layer_data_buffer[MAX_DATA_LENGTH + 6];
    copy_data_to_send_buffer(&network_layer_data_buffer[0],network_layer_data_frame);
    /*****************/
    result = data_link_layer_send(&network_layer_data_buffer[0]);

    return result;
}

static uint8_t network_layer_data_frame_send_multiple_frame(void){
    uint8_t result = 0;
    uint8_t current_frame_data_length = MAX_DATA_LENGTH;
    
    network_layer_data_frame_t network_layer_data_frame;
    network_layer_data_frame.frame_type = data_frame;

    // network_layer_sub_frame_send_data
    if(network_layer_sub_frame_send_data.current_frame_number 
        == network_layer_sub_frame_send_data.frame_number){
        current_frame_data_length = network_layer_sub_frame_send_data.length 
                                    - (network_layer_sub_frame_send_data.current_frame_number - 1) * MAX_DATA_LENGTH;
    }

    network_layer_data_frame.data_length = current_frame_data_length;
    network_layer_data_frame.message_number = network_layer_sub_frame_send_data.frame_number;
    network_layer_data_frame.message_counter = network_layer_sub_frame_send_data.current_frame_number;
    network_layer_data_frame.from_mac_address = Current_MAC_Address;
    network_layer_data_frame.to_mac_address = network_layer_sub_frame_send_data.to_mac_address;
    network_layer_data_frame.data = &network_layer_sub_frame_send_data.network_layer_sub_frame_data_table[network_layer_sub_frame_send_data.current_frame_number - 1];
    /*copy数据至buffer*/
    uint8_t network_layer_data_buffer[MAX_DATA_LENGTH + 6];
    copy_data_to_send_buffer(&network_layer_data_buffer[0],&network_layer_data_frame);
    /*****************/
    result = data_link_layer_send(&network_layer_data_buffer[0]);

    if(result){
        network_layer_sub_frame_send_data.current_frame_number++;
    }

    return result;
}

/**
 * @brief 报文转发表定时器管理
 * 
 */
void network_layer_forwarding_table_timer_management(void){
    /* TODO:放入OS周期task中 */
    /* 数据 */
    for(uint8_t i = 0;i <= data_frame_forwarding_table_stack.stack_top_index;i++){
        network_layer_data_frame_forwarding_table[i].time_counter--;
        if(network_layer_data_frame_forwarding_table[i].time_counter == 0){
            data_frame_forwarding_table_stack.delete(&data_frame_forwarding_table_stack, &network_layer_data_frame_forwarding_table[i]);
        }
    }

    /* 数据Ack */
    for(uint8_t i = 0;i <= data_ack_frame_forwarding_table_stack.stack_top_index;i++){
        network_layer_data_ack_frame_forwarding_table[i].time_counter--;
        if(network_layer_data_ack_frame_forwarding_table[i].time_counter == 0){
            data_ack_frame_forwarding_table_stack.delete(&data_ack_frame_forwarding_table_stack, &network_layer_data_ack_frame_forwarding_table[i]);
        }
    }
}

/**
 * @brief ACK定时器管理
 * 
 */
void network_layer_ack_timer_management(void){
    /* TODO:放入OS周期task中 */
    if(waiting_ack_frame_timer_start){
        waiting_ack_frame_timer_counter--;
        if(waiting_ack_frame_timer_counter == 0){
            waiting_ack_frame_timer_timeout = 1;
        }else{
            waiting_ack_frame_timer_timeout = 0;
        }
    }else{
        waiting_ack_frame_timer_timeout = 0;
        waiting_ack_frame_timer_counter = 10;
    }
}

/**
 * @brief 网络层主函数
 * 
 */
void network_layer_main_function(void){
    if(network_layer_receive_state != idle){
        /* 处理接收 */
        switch(network_layer_receive_state){
            case received_single_data_frame:
                if(network_layer_receive_data_frame.to_mac_address != Current_MAC_Address){
                    /* 处理转发逻辑 */
                    uint8_t stack_serach_result;
                    forwarding_table_t forwarding_table_data;
                    forwarding_table_data.from_mac_address = network_layer_receive_data_frame.from_mac_address;
                    forwarding_table_data.to_mac_address = network_layer_receive_data_frame.to_mac_address;
                    forwarding_table_data.time_counter = Network_Layer_Data_Frame_Forwarding_Element_Max_Time_Counter;//不参与搜索比较
                    forwarding_table_data.crc_8 = crc_8((uint8_t*)network_layer_receive_data_frame.data, 
                                                         network_layer_receive_data_frame.data_length);
                    stack_serach_result = data_frame_forwarding_table_stack.serach(&data_frame_forwarding_table_stack,
                                                                                    (uint8_t*)&forwarding_table_data);
                    if(stack_serach_result == data_frame_forwarding_table_stack.stack_top_index){
                        /* 没有搜索到，push进表，并转发 */
                        data_frame_forwarding_table_stack.push(&data_frame_forwarding_table_stack,
                                                                (uint8_t*)&forwarding_table_data);//push进表
                        network_layer_data_frame_send_single_frame_forwarding(&network_layer_receive_data_frame);//转发报文
                    }else{
                        /* 不做处理，丢弃该报文 */
                    }
                    /* *********** */
                }else{
                    /* 确认为本机接收报文 */
                    network_layer_receive_data.from_mac_address = network_layer_receive_data_frame.from_mac_address;
                    network_layer_receive_data.data = network_layer_receive_data_frame.data;
                    network_layer_receive_data.length = network_layer_receive_data_frame.data_length;
                    //发送ACK报文
                    network_layer_data_ack_frame_send();
                    /* TODO:调用上层CallBack */

                    /* **************** */
                }

                network_layer_receive_state = idle;
                break;
            case received_multiple_data_frame:
                network_layer_receive_state = idle;
                break;
            case received_data_ack_frame:
                if(network_layer_receive_data_ack_frame.to_mac_address != Current_MAC_Address){
                    /* 处理转发逻辑 */
                    uint8_t stack_serach_result;
                    forwarding_table_t forwarding_table_data;
                    forwarding_table_data.from_mac_address = network_layer_receive_data_ack_frame.from_mac_address;
                    forwarding_table_data.to_mac_address = network_layer_receive_data_ack_frame.to_mac_address;
                    forwarding_table_data.time_counter = Network_Layer_Data_Ack_Frame_Forwarding_Table_Max_Time_Counter;//不参与搜索比较
                    forwarding_table_data.crc_8 = 0;
                    stack_serach_result = data_ack_frame_forwarding_table_stack.serach(&data_ack_frame_forwarding_table_stack,
                                                                                    (uint8_t*)&forwarding_table_data);
                    if(stack_serach_result == data_ack_frame_forwarding_table_stack.stack_top_index){
                        /* 没有搜索到，push进表，并转发 */
                        data_ack_frame_forwarding_table_stack.push(&data_ack_frame_forwarding_table_stack,
                                                                (uint8_t*)&forwarding_table_data);//push进表
                        network_layer_data_frame_send_single_frame_forwarding(&network_layer_receive_data_ack_frame);//转发报文
                    }else{
                        /* 不做处理，丢弃该报文 */
                    }
                    /* *********** */
                }else{
                    /* 确认为本机接收报文 */
                    if(network_layer_send_state == waiting_single_data_ack_frame){
                        network_layer_send_state = idle;
                    }else if(network_layer_send_state == waiting_multiple_data_ack_frame){
                        network_layer_send_state = sending_multiple_data_frame;/* 通知继续发送多帧 */
                    }
                    waiting_ack_frame_timer_start = 0;//关闭ACK超时定时器
                    /* TODO:调用上层CallBack */
                    /* **************** */
                }
                network_layer_receive_state = idle;
                break;
            case received_retransmission_frame:
                /* TODO:处理转发逻辑 */
                /* *********** */

                /* TODO:确认为本机接收报文 */
                /* **************** */
                network_layer_receive_state = idle;
                break;
            case received_location_frame:
                /* TODO:处理转发逻辑 */
                /* *********** */

                /* TODO:确认为本机接收报文 */
                /* **************** */
                network_layer_receive_state = idle;
                break;
            case received_location_ack_frame:
                /* TODO:处理转发逻辑 */
                /* *********** */

                /* TODO:确认为本机接收报文 */
                /* **************** */
                network_layer_receive_state = idle;
                break;
            case received_TOF_frame:
                break;
            default:

                network_layer_receive_state = idle;
                break;

        }
    }else{
        /* 处理发送 */
        switch (network_layer_send_state)
        {
            case sending_single_data_frame:
                /* code */
                if(network_layer_data_frame_send_single_frame()){
                    if(Communication_Mode == point_to_point){
                        /* 点播模式，需要ACK */
                        network_layer_send_state = waiting_single_data_ack_frame;
                        waiting_ack_frame_timer_start = 1;//开启ACK计时器
                    }else{
                        network_layer_send_state = idle;
                    }
                }else{
                    /* 此次发送失败 */
                }
                break;
            case sending_multiple_data_frame:
                /* code */
                if(network_layer_sub_frame_send_data.current_frame_number <= network_layer_sub_frame_send_data.frame_number){
                    if(network_layer_data_frame_send_multiple_frame()){
                        if(Communication_Mode == point_to_point){
                            /* 点播模式，需要ACK */
                            // network_layer_send_state = waiting_ack_frame;
                        }
                    }else{
                        /* 此次发送失败 */
                        network_layer_send_state = idle;
                    }
                }else{
                    network_layer_send_state = idle;
                }
                break;
            case sending_data_ack_frame:
                break;
            case sending_retransmission_frame:
                break;
            case sending_location_frame:
                break;
            case sending_location_ack_frame:
                break;
            case waiting_single_data_ack_frame:
                /* code */
                break;
            case waiting_multiple_data_ack_frame:
                break;
            
            default:
                network_layer_send_state = idle;
                break;
        }
    }
}