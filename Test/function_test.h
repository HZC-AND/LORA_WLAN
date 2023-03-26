//
// Created by HZC on 2022/12/19.
//

#ifndef NJFUWORK_FINAL_FUNCTION_TEST_H
#define NJFUWORK_FINAL_FUNCTION_TEST_H


#include "mainConfig.h"

#ifndef NJFUWORK_FINAL_MAINCONFIG_H
#include "fake/platform.h"
#else
#include <stdint.h>
#endif

void SX1278_test_init();
void SX1278_test_main_function();

void packet_loss_rate_test_send_function(uint8_t mode);
void packet_loss_rate_test_receive_function(uint8_t *data);
void set_packet_loss_rate_test_start_flag(uint8_t flag);
uint8_t get_packet_loss_rate_test_start_flag(void);

#endif //NJFUWORK_FINAL_FUNCTION_TEST_H
