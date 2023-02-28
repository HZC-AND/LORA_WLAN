/**
 * @file aplication_layer.h
 * @author HZC (woshihuzhicheng@njfu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2023-2-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef NJFUWORK_FINAL_APLICATION_LAYER_H
#define NJFUWORK_FINAL_APLICATION_LAYER_H

#include <stdint.h>

typedef enum{
    AFLP_idle,
    AFLP_start,
    AFLP_pending,
    AFLP_end,
}AFLP_state_enum;

void application_layer_init(void);
uint8_t AFLP_pending_timer(void);
uint8_t AFLP_process(void);
void AFLP_main_function(void);

#endif //NJFUWORK_FINAL_APLICATION_LAYER_H
