
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
forwarding_table_t network_layer_data_frame_forwarding_table[Network_Layer_Data_Frame_Forwarding_Table_Max_Length];/* 数据报文转发表 */
stack_t data_frame_forwarding_table_stack;/* 数据报文转发表栈 */
/* ********* */

/* 上层数据的全局变量 */
network_layer_from_up_layer_send_data_t network_layer_send_data;
network_layer_to_up_layer_receive_data_t network_layer_receive_data;//接收到的网络层帧，全局变量
/* **************** */
uint8_t receive_data_buffer[255];//用于保存收到的数据内容，全局变量

void network_layer_init(void){

    network_layer_receive_data_frame.data = &receive_data_buffer[0];

    /* network_layer_send_data 初始化 */

    /* network_layer_sub_frame_send_data 初始化 */
    network_layer_sub_frame_send_data.current_frame_number = 1;

    /* 报文转发表，数据报文转发表栈初始化 */
    stack_create(&data_frame_forwarding_table_stack, 
                  network_layer_data_frame_forwarding_table, 
                  Network_Layer_Data_Frame_Forwarding_Table_Max_Length, 
                  sizeof(forwarding_table_t));
}

uint8_t network_layer_data_frame_send(uint8_t* data, uint8_t length, uint8_t to_mac_address){
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

void network_layer_location_frame_send(void){
    network_layer_data_frame_t network_layer_data_frame;
    network_layer_data_frame.frame_type = location_frame;
    // data_link_layer_send();
}

void network_layer_location_ack_frame_send(void){
    network_layer_data_frame_t network_layer_data_frame;
    network_layer_data_frame.frame_type = location_ack_frame;
    // data_link_layer_send();
}

void network_layer_retransmission_frame_send(void){
    network_layer_data_frame_t network_layer_data_frame;
    network_layer_data_frame.frame_type = retransmission_frame;
    // data_link_layer_send();
}

void network_layer_data_ack_frame_send(void){
    network_layer_data_frame_t network_layer_data_frame;
    network_layer_data_frame.frame_type = data_ack_frame;
    // data_link_layer_send();
}

uint8_t network_layer_receive_callback(uint8_t* data, uint8_t length){
    (void)copy_data_to_receive_frame(data);
    return 1;
}

/* copy数据至buffer */
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

/* copy数据至receive frame,从局部变量到全局变量 */
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
            network_layer_retransmission_frame_t* network_layer_retransmission_frame = (network_layer_retransmission_frame_t*)data;
            break;
        case location_frame:
            network_layer_location_frame_t* network_layer_location_frame = (network_layer_location_frame_t*)data;
            break;
        case location_ack_frame:
            network_layer_location_ack_frame_t* network_layer_location_ack_frame = (network_layer_location_ack_frame_t*)data;
            break;
        default:
            break;
    }

    return 1;
}

 /* 分帧 */
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

/* 组帧 */
uint8_t combine_sub_frame_data(uint8_t* data, uint8_t length){
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

void network_layer_main_function(void){
    if(network_layer_receive_state != idle){
        /* 处理接收 */
        switch(network_layer_receive_state){
            case received_single_data_frame:
                if(network_layer_receive_data_frame.to_mac_address != Current_MAC_Address){
                    /* 处理转发逻辑 */
                    uint8_t stack_serach_result;
                    forwarding_table_t* forwarding_table_data = (forwarding_table_t*)malloc(sizeof(forwarding_table_t));
                    forwarding_table_data->from_mac_address = network_layer_receive_data_frame.from_mac_address;
                    forwarding_table_data->to_mac_address = network_layer_receive_data_frame.to_mac_address;
                    forwarding_table_data->time_counter = 0;//不参与搜索比较
                    forwarding_table_data->crc_8 = crc_8((uint8_t*)network_layer_receive_data_frame.data, 
                                                         network_layer_receive_data_frame.data_length);
                    stack_serach_result = data_frame_forwarding_table_stack.serach(&data_frame_forwarding_table_stack,
                                                                                    (uint8_t*)forwarding_table_data);
                    if(stack_serach_result == data_frame_forwarding_table_stack.stack_top_index){
                        /* 没有搜索到，push进表，并转发 */
                        data_frame_forwarding_table_stack.push(&data_frame_forwarding_table_stack,
                                                                (uint8_t*)forwarding_table_data);//push进表
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
                    //调用上层CallBack
                    /* **************** */
                }

                network_layer_receive_state = idle;
                break;
            case received_multiple_data_frame:
                /* 处理转发逻辑 */
                /* *********** */

                /* 确认为本机接收报文 */
                /* **************** */
                network_layer_receive_state = idle;
                break;
            case received_data_ack_frame:
                /* 处理转发逻辑 */
                /* *********** */

                /* 确认为本机接收报文 */
                /* **************** */
                network_layer_receive_state = idle;
                break;
            case received_retransmission_frame:
                /* 处理转发逻辑 */
                /* *********** */

                /* 确认为本机接收报文 */
                /* **************** */
                network_layer_receive_state = idle;
                break;
            case received_location_frame:
                /* 处理转发逻辑 */
                /* *********** */

                /* 确认为本机接收报文 */
                /* **************** */
                network_layer_receive_state = idle;
                break;
            case received_location_ack_frame:
                /* 处理转发逻辑 */
                /* *********** */

                /* 确认为本机接收报文 */
                /* **************** */
                network_layer_receive_state = idle;
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
                    if(network_layer_send_data.to_mac_address != 0){
                        /* 点播模式，需要ACK */
                        network_layer_send_state = waiting_ack_frame;
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
                        if(network_layer_send_data.to_mac_address != 0){
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
            case waiting_ack_frame:
                /* code */
                break;
            
            default:
                network_layer_send_state = idle;
                break;
        }
    }
}