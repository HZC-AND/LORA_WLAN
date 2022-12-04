


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

/* 网络层数据发送状态 */
typedef enum{
    idle,
    sending_single_data_frame,   //正在发送单帧数据帧
    sending_multiple_data_frame, //正在发送多帧数据帧
    sending_data_ack_frame,
    sending_retransmission_frame,
    sending_location_frame,
    sending_location_ack_frame,
    forwarding_frame,            //正在转发报文
    waiting_ack_frame,      //正在等待ACK
}network_layer_send_state_enum;

/* 网络层数据接收状态 */
typedef enum{
    idle,
    received_single_data_frame,//接收到单帧
    received_multiple_data_frame,//接收到多帧
    received_data_ack_frame,
    received_retransmission_frame,
    received_location_frame,
    received_location_ack_frame,
}network_layer_receive_state_enum;

/* 需要接收ACK的信息 */
// typedef struct {

// }network_layer_wait_ack_info_t;

/* 需要发送的数据，与network_layer_data_frame_send函数参数对应 */
typedef struct
{
    /* data */
    uint8_t* data;
    uint8_t length;
    uint8_t to_mac_address;
}network_layer_from_up_layer_send_data_t;
/* 接收的数据类型，从网络层数据中提取出来的，给到上层的数据 */
typedef struct
{
    /* data */
    uint8_t* data;
    uint8_t length;
    uint8_t from_mac_address;
}network_layer_to_up_layer_receive_data_t;

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

/* 报文转发表处理,栈处理或队列处理 */
// /* 数据报文转发表结构 */

// /* 数据应答报文转发表结构 */

// /* 重传报文转发表结构 */

// /* 定位报文转发表结构 */

// /* 定位应答报文转发表结构 */

/* 统一一种结构，报文转发表结构 */
typedef struct
{
    /* data */
    uint8_t from_mac_address;
    /* 定位报文没有to_mac_address，其值为0 */
    uint8_t to_mac_address;
    /* 对报文所有数据计算crc_8,
    其是判断是否是重复报文的重要依据 */
    uint8_t crc_8; 
    /* 定时器，每增加一秒加1 */  
    uint8_t time_counter;        
}forwarding_table_t;

typedef struct
{
    /* 栈顶索引 */
    uint8_t stack_top_index;
    /* 栈满标志 0->未满，1->满*/
    uint8_t is_full;
    /* 栈大小 */
    uint8_t stack_length;
    /* 每个元素的大小, 即sizeof(forwarding_table_t) */
    uint8_t element_length;
    uint8_t (*push)(stack_t*, uint8_t*);
    uint8_t (*pop)(stack_t*, uint8_t*);
    uint8_t (*delete)(stack_t*, uint8_t*);
    uint8_t (*clear)(stack_t*);
    uint8_t (*serach)(stack_t*, uint8_t*);
    uint8_t** data;
}stack_t;

/* ******************* */




void network_layer_init(void);
uint8_t network_layer_data_frame_send(uint8_t* data, uint8_t length, uint8_t to_mac_address);
void network_layer_location_frame_send(void);
void network_layer_location_ack_frame_send(void);
void network_layer_retransmission_frame_send(void);
void network_layer_data_ack_frame_send(void);

void copy_data_to_send_buffer(uint8_t* buffer, network_layer_data_frame_t* network_layer_data_frame);
// void copy_data_to_receive_buffer(network_layer_data_frame_t* network_layer_data_frame);
uint8_t copy_data_to_receive_frame(uint8_t* data);

uint8_t network_layer_receive_callback(uint8_t* data, uint8_t length);

static uint8_t network_layer_data_frame_send_single_frame(void);
static uint8_t network_layer_data_frame_send_multiple_frame(void);

uint8_t split_frame_to_sub_frame_data_table(uint8_t* data, uint8_t length, uint8_t to_mac_address);
uint8_t combine_sub_frame_data(uint8_t* data, uint8_t length);
network_layer_send_state_enum get_network_layer_send_state(void);

void network_layer_main_function(void);

/* 栈函数 */
uint8_t stack_push(stack_t* this, uint8_t* data); 
uint8_t stack_pop(stack_t* this, uint8_t* data);
uint8_t stack_delete(stack_t* this, uint8_t* data);
uint8_t stack_clear(stack_t* this);
uint8_t stack_serach(stack_t* this, uint8_t* data);
uint8_t stack_create(stack_t* new_stack, uint8_t** data, uint8_t stack_length, uint8_t element_length);
/* ***** */