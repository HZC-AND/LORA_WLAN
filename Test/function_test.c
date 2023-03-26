//
// Created by HZC on 2022/12/19.
//

#include "function_test.h"
#include "device_info.h"
#include "SX1278.h"

#include "data_link_layer.h"
#include "network_layer.h"

#include "usart.h"

extern SX1278_t SX1278;
uint8_t SX1278_test_tx_buffer[10] = {0};

/**
 * @brief 测试重写的sx1278的初始化
 */
void SX1278_test_init() {
    SX1278_H_Init(&SX1278, 413000000, SX1278_POWER_17DBM, SX1278_LORA_SF_7,
                  SX1278_LORA_BW_500KHZ, SX1278_LORA_CR_4_5, SX1278_LORA_CRC_EN, 10);
}

void SX1278_test_tx() {
}

/**
 * @brief 测试CAD模式
 */
void SX1278_test_rx() {

}

void SX1278_test_main_function() {

}
/********* 性能测试 **********/
/*丢包率测试*/
#if (Packet_Loss_Rate_Test == STD_ON)
uint8_t packet_loss_rate_test_start_flag = 0;

uint8_t packet_loss_rate_test_send_data[10];
uint8_t receive_counter = 0;
uint8_t receive_counter_number_in_packet = 0;
#endif

void packet_loss_rate_test_send_function(uint8_t mode) {
#if (Packet_Loss_Rate_Test == STD_ON)
    switch (mode) {
        case 1:
            /* 点对点单节点测试 */
            packet_loss_rate_test_send_data[0] = 0xFF;
            packet_loss_rate_test_send_data[1] += 1;
            if(packet_loss_rate_test_send_data[1] == 0){
                packet_loss_rate_test_send_data[1] = 1;
            }
            if (get_network_layer_send_state() == sending_idle) {
                network_layer_data_frame_send(&packet_loss_rate_test_send_data[0], 10, To_MAC_Address);//TODO:应该使用立即发送接口
                network_layer_data_frame_send_single_frame();
                set_network_layer_send_state(sending_idle);
            }
            break;

        default:
            break;
    }
#endif
}

void packet_loss_rate_test_receive_function(uint8_t *data) {
#if (Packet_Loss_Rate_Test == STD_ON)
    uint8_t temp_data[3];
    receive_counter++;
    receive_counter_number_in_packet = data[1];
    temp_data[0] = 0xFF;
    temp_data[1] = receive_counter;
    temp_data[2] = receive_counter_number_in_packet;
    HAL_UART_Transmit(&huart1, &temp_data[0], 3, 0xFFFF);
    if(receive_counter_number_in_packet == 0xFF){
        receive_counter = 0;
    }
#endif
}
/**************/
/* ********************** */
