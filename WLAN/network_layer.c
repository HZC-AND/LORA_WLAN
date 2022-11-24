/* 
网络层的目的是实现两个端系统之间的数据透明传送，具体功能包括寻址和路由选择、连接的建立、保持和终止等。
它提供的服务使传输层不需要了解网络中的数据传输和交换技术。如果您想用尽量少的词来记住网络层，那就是"路径选择、路由及逻辑寻址"。

网络层中涉及众多的协议，其中包括最重要的协议，也是TCP/IP的核心协议——IP协议。IP协议非常简单，仅仅提供不可靠、无连接的传送服务。
IP协议的主要功能有：无连接数据报传输、数据报路由选择和差错控制。
与IP协议配套使用实现其功能的还有地址解析协议ARP、逆地址解析协议RARP、因特网报文协议ICMP、因特网组管理协议IGMP。
 */

#include "network_layer.h"


void network_layer_init(void){

}

void network_layer_data_frame_send(void){}

void network_layer_location_frame_send(void){}

void network_layer_location_ack_frame_send(void){}

void network_layer_retransmission_frame_send(void){}

void network_layer_data_ack_frame_send(void){}

uint8_t network_layer_receive_callback(void){}