//
// Created by HZC on 2022/12/19.
//

#include "function_test.h"
#include "SX1278.h"

extern SX1278_t SX1278;
uint8_t SX1278_test_tx_buffer[10] = {0};
/**
 * @brief 测试重写的sx1278的初始化
 */
void SX1278_test_init(){
    SX1278_H_Init(&SX1278, 413000000, SX1278_POWER_17DBM, SX1278_LORA_SF_7,
                  SX1278_LORA_BW_500KHZ, SX1278_LORA_CR_4_5, SX1278_LORA_CRC_EN, 10);
}

void SX1278_test_tx(){
    SX1278_TX_Once(&SX1278,&SX1278_test_tx_buffer[0],10,200);
}
/**
 * @brief 测试CAD模式
 */
void SX1278_test_rx(){

}

void SX1278_test_main_function(){

}
