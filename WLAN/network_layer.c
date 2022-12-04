/* 
网络层的目的是实现两个端系统之间的数据透明传送，具体功能包括寻址和路由选择、连接的建立、保持和终止等。
它提供的服务使传输层不需要了解网络中的数据传输和交换技术。如果您想用尽量少的词来记住网络层，那就是"路径选择、路由及逻辑寻址"。

网络层中涉及众多的协议，其中包括最重要的协议，也是TCP/IP的核心协议——IP协议。IP协议非常简单，仅仅提供不可靠、无连接的传送服务。
IP协议的主要功能有：无连接数据报传输、数据报路由选择和差错控制。
与IP协议配套使用实现其功能的还有地址解析协议ARP、逆地址解析协议RARP、因特网报文协议ICMP、因特网组管理协议IGMP。
 */

#include "network_layer.h"
#include "data_link_layer.h"
#include "device_info.h"

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
/* ********* */

network_layer_from_up_layer_send_data_t network_layer_send_data;
network_layer_to_up_layer_receive_data_t network_layer_receive_data;//接收到的网络层帧，全局变量
uint8_t receive_data_buffer[255];//用于保存收到的数据内容，全局变量

void network_layer_init(void){

    network_layer_receive_data_frame.data = &receive_data_buffer[0];

    /* network_layer_send_data 初始化 */

    /* network_layer_sub_frame_send_data 初始化 */
    network_layer_sub_frame_send_data.current_frame_number = 1;
}

uint8_t network_layer_data_frame_send(uint8_t* data, uint8_t length, uint8_t to_mac_address){
    if(network_layer_send_state == idle){
        if(length <= MAX_DATA_LENGTH){
            /* TODO: 需要添加临界区 */
            network_layer_send_state = sending_single_frame;
            network_layer_send_data.data = data;
            network_layer_send_data.length = length;
            network_layer_send_data.to_mac_address = to_mac_address;
            /***********************/
        }else{
            /* 分帧 */
            network_layer_send_state = sending_multiple_frame;
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
void copy_data_to_receive_frame(uint8_t* data){

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

/* 栈处理逻辑代码,测试完成后提出去成为一个单独的模块 */
uint8_t stack_push(stack_t* this, uint8_t* data){
    if(this->is_full == 1){
        /* 栈满 */
        return 0;
    }
    this->stack_top_index++;
    if(this->stack_top_index == (this->stack_length - 1)){
        this->is_full = 1;
    }
    // this->data[this->i]->from_mac_address = data->from_mac_address;
    // this->data[this->i]->to_mac_address = data->to_mac_address;
    // this->data[this->i]->crc_8 = data->crc_8;
    for(uint8_t i = 0;i < this->element_length;i++){
        this->data[this->stack_top_index][i] = data[i];
    }
    
    return 1;
} 

uint8_t stack_pop(stack_t* this, uint8_t* data){
    if(this->stack_top_index == 0){
        /* 栈空 */
        return 0;
    }
    for(uint8_t i = 0;i < this->element_length;i++){
        data[i] = this->data[this->stack_top_index][i];
    }
    this->stack_top_index--;
    if(this->stack_top_index == 0){
        this->is_full = 0;
    }

    return 1;
}

uint8_t stack_delete(stack_t* this, uint8_t* data){
    uint8_t find_index = this->stack_length;//初始为超出范围值
    uint8_t delete_state = 0;
    for(uint8_t i = 0;i <= this->stack_top_index;i++){
        for(uint8_t j = 0;j < this->element_length;j++){
            if(data[j] == this->data[i][j]){
                find_index = i;
            }else{
                find_index = this->stack_length;
                break;
            }
        }

        if(find_index <= this->stack_top_index){
            for(uint8_t j = 0;j <= (this->stack_top_index - 1);j++){
                for(uint8_t k = 0;k < this->element_length;k++){
                    /* 后面元素覆盖前面元素 */
                    this->data[j][k] = this->data[j+1][k];
                }
            }
            delete_state = 1;
            /* 删除操作后，重置find_index */
            find_index = this->stack_length;
            /* 栈数据减去1 */
            this->stack_top_index--;
        }
    }
    
    if(delete_state == 0){
        /* 删除错误，栈中未找到指定数据 */
        return 0;
    }
    return 1;
}

uint8_t stack_clear(stack_t* this){
    this->stack_top_index = 0;
    this->is_full = 0;
    return 1;
}

uint8_t stack_serach(stack_t* this, uint8_t* data){
    return 1;
}
/* ************* */

void network_layer_main_function(void){
    if(network_layer_receive_state != idle){
        /* 处理接收 */
        switch(network_layer_receive_state){
            case received_single_data_frame:
                /* 处理转发逻辑 */
                /* *********** */

                /* 确认为本机接收报文 */
                /* **************** */

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