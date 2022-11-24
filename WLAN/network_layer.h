


#include "../fake/platform.h"


typedef enum{
    data_frame,
    data_ack_frame,
    retransmission_frame,
    location_frame,
    location_ack_frame,
}network_layer_frame_type_enum;


void network_layer_init(void);
void network_layer_data_frame_send(void);
void network_layer_location_frame_send(void);
void network_layer_location_ack_frame_send(void);
void network_layer_retransmission_frame_send(void);
void network_layer_data_ack_frame_send(void);

uint8_t network_layer_receive_callback(void);