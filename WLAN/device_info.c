/**
 * @file device_info.c
 * @author HZC (woshihuzhicheng@njfu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-12-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "device_info.h"

communication_mode_enum Communication_Mode = point_to_point;
communication_mode_enum receive_data_communication_mode = point_to_point;

uint64_t channel_selection_table[] = {413,424,434,445,454,463,474,484,495,503,514,523};