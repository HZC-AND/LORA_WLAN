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
#include "device_info.h"

#include "network_layer.h"

// extern SX1278_t SX1278_Module;
SX1278_t SX1278_Module;

data_link_layer_frame_t data_link_layer_frame;

uint8_t data_buffer_for_sx1278[255];

void data_link_layer_init(){
    data_link_layer_frame.function.conmunication_mode = Communication_Mode_Init;//点播
    data_link_layer_frame.function.channel_selection = Channel_Selection_Init;//413mhz
    data_link_layer_frame.function.multicast_grouping = 0b00;//组播分组1
    data_link_layer_frame.MAC_Address = Current_MAC_Address;

    data_link_layer_frame.network_layer_data = NULL_PTR;
    
}

uint8_t data_link_layer_send(uint8_t* data){
    uint8_t data_length = 0;

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
    data_buffer_for_sx1278[0] = (uint8_t)(data_link_layer_frame.function);
    data_buffer_for_sx1278[1] = (uint8_t)(data_link_layer_frame.data_length);
    for (uint8_t i = 0; i < data_link_layer_frame.data_length; i++)
    {
        data_buffer_for_sx1278[i + 2] = data_link_layer_frame.network_layer_data[i];
    }
    /* 计算CRC */
    data_link_layer_frame.CRC_8 = crc_8(&data_buffer_for_sx1278[0], data_length - 1);
    /**********/
    data_buffer_for_sx1278[data_length - 1] = (uint8_t)(data_link_layer_frame.CRC_8);
    /********************/
    

    SX1278_Module.frequency = channel_selection_table[data_link_layer_frame.function.channel_selection];

    SX1278_TX_Once(&SX1278_Module,&data_buffer_for_sx1278[0],data_length,200);

    return 1;
}

uint8_t data_link_layer_receive_callback(data_link_layer_frame_t* data){
    uint8_t conmunication_mode = data->function.conmunication_mode;
    // SX1278_Module.frequency = channel_selection_table[data->function.channel_selection];
    switch (conmunication_mode){
        case 0b00: //点播;
            global_communication_mode = point_to_point;
            break;
        case 0b01: //组播；
            global_communication_mode = multicast;
            break;
        case 0b11: //广播
            global_communication_mode = broadcast;
            break;
        default:
            break;
    }

    return 1;
}

// #define FACTOR (0x107 & 0xFF) //多项式因子(取低8bit)

uint8_t crc_8(uint8_t* data, uint8_t length){
    //G(X) = X8+X2+X+1
    uint8_t crc = 0x00;
    uint8_t FACTOR = (0x107 & 0xFF);//多项式因子(取低8bit)
    while(length--)
	{
        crc ^= (*data++);//前一字节计算CRC后的结果异或上后一字节，再次计算CRC
        for (uint8_t i=8; i>0; i--)
        {
            if (crc & 0x80)//高位为1，需要异或；否则，不需要
            {
                crc = (crc << 1) ^ FACTOR;
            }
            else
            {
            	crc = (crc << 1);
			}
        }
    }

    return crc;
}