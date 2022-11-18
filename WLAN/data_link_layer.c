/* 
数据链路层在物理层提供的服务的基础上向网络层提供服务，
其最基本的服务是将源自网络层来的数据可靠地传输到相邻节点的目标机网络层。
为达到这一目的，数据链路必须具备一系列相应的功能，主要有：如何将数据组合成数据块，
在数据链路层中称这种数据块为帧（frame），帧是数据链路层的传送单位；
如何控制帧在物理信道上的传输，包括如何处理传输差错，如何调节发送速率以使与接收方相匹配；
以及在两个网络实体之间提供数据链路通路的建立、维持和释放的管理。
数据链路层在不可靠的物理介质上提供可靠的传输。
该层的作用包括：物理地址寻址、数据的成帧、流量控制、数据的检错、重发等。
 */


#include "data_link_layer.h"
#include "../SX1278/SX1278.h"
#include "device_cfg.h"

// uint8_t MAC_Address = 0;
uint8_t channel_selection_table[] = {413,424,434,445,454,463,474,484,495,503.514,523};

// extern SX1278_t SX1278_Module;
SX1278_t SX1278_Module;

data_link_layer_frame_t data_link_layer_frame;

void data_link_layer_init(){
    data_link_layer_frame.function.conmunication_mode = 0b00;//点播
    data_link_layer_frame.function.channel_selection = 0b0000;//413mhz
    data_link_layer_frame.function.multicast_grouping = 0b00;//组播分组1
    data_link_layer_frame.MAC_Address = Current_MAC_Address;

    data_link_layer_frame.network_layer_data = NULL_PTR;
    
}

uint8_t data_link_layer_send(data_link_layer_frame_t* data){
    uint8_t conmunication_mode = data->function.conmunication_mode;
    SX1278_Module.frequency = channel_selection_table[data->function.channel_selection];
    switch (conmunication_mode){
        case 0b00: //点播;
            //SX1278_TX_Once(&SX1278_Module...);
            break;
        case 0b01: //组播；
            // SX1278_TX_Once(&SX1278_Module...);
            break;
        case 0b11: //广播
            // SX1278_TX_Once(&SX1278_Module...);
            break;

        default:
            break;
    }

    return 1;
}

uint8_t data_link_layer_receive_callback(data_link_layer_frame_t* data){
    uint8_t conmunication_mode = data->function.conmunication_mode;
    // SX1278_Module.frequency = channel_selection_table[data->function.channel_selection];
    switch (conmunication_mode){
        case 0b00: //点播;
            //SX1278_TX_Once(&SX1278_Module...);
            break;
        case 0b01: //组播；
            // SX1278_TX_Once(&SX1278_Module...);
            break;
        case 0b11: //广播
            // SX1278_TX_Once(&SX1278_Module...);
            break;

        default:
            break;
    }

    return 1;
}

void crc_8(uint8_t* data){

}