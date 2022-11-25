


#include "../fake/platform.h"


#define MAX_DATA_LENGTH 10 //这里定义网路层一帧数据携带的上层数据最大长度
#define MAX_SUB_FRAME_NUMBER 10 //这里定义最大的分帧数

typedef enum{
    data_frame,
    data_ack_frame,
    retransmission_frame,
    location_frame,
    location_ack_frame,
}network_layer_frame_type_enum;

typedef enum{
    idle,
    sending_single_frame,   //正在发送单帧
    sending_multiple_frame, //正在发送多帧
    waiting_ack_frame,      //正在等待ACK
}network_layer_send_state_enum;

/* 需要接收ACK的信息 */
// typedef struct {

// }network_layer_wait_ack_info_t;

/* 需要发送的数据，与network_layer_data_frame_send参数对应 */
typedef struct
{
    /* data */
    uint8_t* data;
    uint8_t length;
    uint8_t to_mac_address;
}network_layer_send_data_t;

/* 分帧表结构体 */
typedef struct
{
    /* data */
    uint8_t network_layer_sub_frame_data_table[MAX_SUB_FRAME_NUMBER][MAX_DATA_LENGTH];
    uint8_t length;
    uint8_t to_mac_address;
    uint8_t frame_number;
    uint8_t current_frame_number;//当前发送编号
}network_layer_sub_frame_send_data_t;

/* 分帧表接收结构体 */
typedef struct
{
    /* data */
    uint8_t network_layer_sub_frame_data_table[MAX_SUB_FRAME_NUMBER][MAX_DATA_LENGTH];
    uint8_t length;
    uint8_t from_mac_address;
    uint8_t frame_number;
    uint8_t current_frame_number;//当前接收编号
}network_layer_sub_frame_receive_data_t;


typedef struct
{
    /* data */
    uint8_t frame_type;
    uint8_t message_number;
    uint8_t message_counter;
    uint8_t from_mac_address;
    uint8_t to_mac_address;
    uint8_t data_length;
    uint8_t* data;
}network_layer_data_frame_t;

typedef struct
{
    /* data */
    uint8_t frame_type;
    uint8_t from_mac_address;
    uint8_t to_mac_address;
}network_layer_data_ack_frame_t;

typedef struct
{
    /* data */
    uint8_t frame_type;
    uint8_t message_counter;
    uint8_t from_mac_address;
    uint8_t to_mac_address;
}network_layer_retransmission_frame_t;

typedef struct
{
    /* data */
    uint8_t frame_type;
    uint8_t from_mac_address;
}network_layer_location_frame_t;

typedef struct
{
    /* data */
    uint8_t frame_type;
    uint8_t from_mac_address;
    uint8_t to_mac_address;
    uint8_t rssi_data;
}network_layer_location_ack_frame_t;



void network_layer_init(void);
uint8_t network_layer_data_frame_send(uint8_t* data, uint8_t length, uint8_t to_mac_address);
void network_layer_location_frame_send(void);
void network_layer_location_ack_frame_send(void);
void network_layer_retransmission_frame_send(void);
void network_layer_data_ack_frame_send(void);

void copy_data_to_send_buffer(uint8_t* buffer, network_layer_data_frame_t* network_layer_data_frame);

uint8_t network_layer_receive_callback(uint8_t* data, uint8_t length);

static uint8_t network_layer_data_frame_send_single_frame(void);
static uint8_t network_layer_data_frame_send_multiple_frame(void);

uint8_t split_frame_to_sub_frame_data_table(uint8_t* data, uint8_t length, uint8_t to_mac_address);
network_layer_send_state_enum get_network_layer_send_state(void);

void network_layer_main_function(void);