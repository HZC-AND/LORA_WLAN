/**
 * @file aplication_layer.c
 * @author HZC (woshihuzhicheng@njfu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2023-2-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "aplication_layer.h"
#include "network_layer.h"
#include "device_info.h"
#include "algorithm_lib.h"

#include "FreeRTOS.h"
#include "task.h"

/*AFLP相关*/
AFLP_state_enum AFLP_state = AFLP_idle;
uint8_t MAX_ID = 3;

uint8_t AFLP_step = 0;
uint8_t AFLP_TOF_counter = 10;
uint8_t AFLP_RSSI_counter = 10;
uint8_t AFLP_to_mac_address = 0;

/*TOF_result_array[0]记录本节点ID，其余元素下标及表示相对应节点id，值为计算出的距离值*/
uint64_t TOF_result_array[255];
uint8_t RSSI_result_array[255];
uint64_t TOF_result_temp[10];
uint8_t RSSI_result_temp[10];


uint8_t AFLP_pending_timer_time = 50;//初始化单位个，1个AFLP main函数周期


/* 最近邻路由表建立机制相关变量 */
uint8_t min_distance_target_mac_address = 0;/* 存放最近邻节点MAC地址，即ID */
uint64_t min_distance_target_distance = 0xFFFFFFFFFFFFFFFF;/* 存放与最近邻节点的距离值 */

uint8_t own_mac_address_is_target_mac_address = 0 /* 设置本届点为目标路由节点的节点MAC地址 */
/* ************************* */
/**
 * @brief application_layer_init
 * 
 */
void application_layer_init(void){
    AFLP_to_mac_address = 1;
    TOF_result_array[0] = Current_MAC_Address;
    RSSI_result_array[0] = Current_MAC_Address;
}

/**
 * @brief AFLP_pending_timer
 * 
 * @return uint8_t 
 */
uint8_t AFLP_pending_timer(void){
    uint8_t AFLP_pending_timer_notify = 0;
    /*需要放到20ms中调用*/
    if(AFLP_state == AFLP_pending) {
        AFLP_pending_timer_time--;
        AFLP_pending_timer_notify = 0;
    }

    if(AFLP_pending_timer_time == 0){
//        AFLP_state = AFLP_start;
        AFLP_pending_timer_time = 50;
        AFLP_pending_timer_notify = 1;
    }

    return AFLP_pending_timer_notify;
}

/**
 * @brief AFLP_process
 * 
 * @return uint8_t 
 */
uint8_t AFLP_process(void){
    if(AFLP_to_mac_address == Current_MAC_Address){
        AFLP_to_mac_address++;
    }
    if(AFLP_step == 1){
        /*10次TOF，去掉最大值和最小值后求平均*/
        /*TODO:定义定位表，直接获取底层结果数据存入表中*/
        if(AFLP_TOF_counter > 0) {
            network_layer_TOF_frame_send(step1, AFLP_to_mac_address);
            AFLP_TOF_counter--;
        }else{
            AFLP_step = 2;
        }
    }else if(AFLP_step == 2){
        /*10次RSSI，去掉最大值和最小值后求平均*/
        /*TODO:定义定位表，直接获取底层结果数据存入表中*/
        if(AFLP_RSSI_counter > 0) {
            network_layer_special_function_frame_send_asyn(location_frame, AFLP_to_mac_address);
            AFLP_RSSI_counter--;
        }else{
            AFLP_step = 0;
        }
    }else{
        AFLP_TOF_counter = 10;
        AFLP_RSSI_counter = 10;
        /*上述动作每执行一次后，目标ID+1，直至ID到达已知的最后一个节点ID*/
        if(AFLP_to_mac_address < MAX_ID) {
            AFLP_to_mac_address++;
        }else{
            AFLP_to_mac_address = 1;/*Reset*/
            AFLP_step = 1;/*Reset*/
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Automatic fusion location protocol
 * 
 */
void AFLP_main_function(void){
    switch (AFLP_state) {
        case AFLP_start:
            /*开始定位流程*/
            if(AFLP_process()){
                AFLP_pending_timer_time = 50;
                AFLP_state = AFLP_pending;
            }else{
                AFLP_state = AFLP_end;
            }
            break;
        case AFLP_pending:
//            /*定位操作间隔等待*/
//            if(AFLP_pending_timer() == 1){
//                AFLP_state = AFLP_start;
//            }
            if(get_TOF_end_flag()){
                TOF_result_temp[AFLP_TOF_counter] = get_TOF_result();
                set_TOF_end_flag(0);/*Clear flag*/
            }else if(get_RSSI_end_flag()){
                RSSI_result_temp[AFLP_RSSI_counter] = get_RSSI_result();
                set_RSSI_end_flag(0);/*Clear flag*/
            }
            if(AFLP_TOF_counter == 0){
                uint8_t index_max = 0;
                uint8_t index_min = 0;
                uint64_t sum = 0;
                get_max_min_value_in_array((uint8_t*)&TOF_result_temp[0],8,10,&index_max,&index_min);
                for(uint8_t i = 0;i < 10;i++){
                    if((i == index_max) || (i == index_min)){
                        continue;
                    }
                    sum += TOF_result_temp[i];
                }
                TOF_result_array[AFLP_to_mac_address] = sum/8;
            }else if(AFLP_RSSI_counter == 0){
                uint8_t index_max = 0;
                uint8_t index_min = 0;
                uint16_t sum = 0;
                get_max_min_value_in_array((uint8_t*)&RSSI_result_temp[0],8,10,&index_max,&index_min);
                for(uint8_t i = 0;i < 10;i++){
                    if((i == index_max) || (i == index_min)){
                        continue;
                    }
                    sum += RSSI_result_temp[i];
                }
                RSSI_result_array[AFLP_to_mac_address] = (uint8_t)(sum/8);
            }
            AFLP_state = AFLP_start;
            break;
        case AFLP_end:
            /*处理存储的定位表,主动发送出去*/
            if(Current_MAC_Address != 1){
                if (get_network_layer_send_state() == sending_idle) {
                    taskENTER_CRITICAL();
                    /* TODO:向sink node发送定位结果数据 */
                    /* Comment this line of code for adaptation to the next version of hardware. */
                    // network_layer_data_frame_send(&Tx_Data[0], 10, 1);
                    taskEXIT_CRITICAL();
                }
            }else{
                /* TODO:直接使用uart向上位机输出结果 */
            }
            break;
        default:
            break;
    }
}

/* 最近邻路由表建立机制 Nearest neighbor routing(NNR)
目的：保证存在一条通信路径的同时极大的减少对flash和ram的空间占用
 */

//该函数应在AFLP定位距离计算函数中调用
/**
 * @brief 更新最近邻信息
 * 
 * @param mac_address 
 * @param distance 
 */
void NNR_update_distace(uint8_t mac_address,uint64_t distance){
    if(mac_address != own_mac_address_is_target_mac_address){
        if(distance < min_distance_target_distance ){
            min_distance_target_mac_address = mac_address;
            min_distance_target_distance = distance;
        }else{
            /* Do Nothing */
        }
    }else{
        /* Do Nothing */
    }
}

//由数据接收函数调用，用于设置own_mac_address_is_target_mac_address
/**
 * @brief 保存将本节点作为目标路由节点的节点ID，用途是避免路由闭锁
 * 
 * @param mac_address 
 */
void NNR_set_is_target_mac_address(uint8_t mac_address){
    own_mac_address_is_target_mac_address = mac_address;
}

//该函数周期调用
/**
 * @brief 主函数
 * 
 */
void NNR_main_function(){
    if(AFLP_state == AFLP_end){
        /* TODO:主动向目标节点发送其被标记为本节点的路由节点的报文，用于更新own_mac_address_is_target_mac_address，
        目的是避免路由闭锁的发生。
         */

        /* Comment this line of code for adaptation to the next version of hardware. */
        // network_layer_data_frame_send(&Tx_Data[0], 10, min_distance_target_mac_address);
    }else{
        /* Do Nothing */
    }
}

/**
 * @brief 获取min_distance_target_mac_address
 * 
 * @return uint8_t 
 */
uint8_t NNR_get_min_distance_target_mac_address(){
    return min_distance_target_mac_address;
}

/**
 * @brief 获取NNR_get_min_distance_target_distance
 * 
 * @return uint64_t 
 */
uint64_t NNR_get_min_distance_target_distance(){
    return min_distance_target_distance;
}
/* ****************** */


