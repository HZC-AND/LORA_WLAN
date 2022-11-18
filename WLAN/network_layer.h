


#include "../fake/platform.h"


void network_layer_init(void);
void network_layer_data_frame_send(void);
void network_layer_location_frame_send(void);
void network_layer_location_ack_frame_send(void);
void network_layer_retransmission_frame_send(void);
void network_layer_ack_frame_send(void);

uint8_t network_layer_receive_callback(void);