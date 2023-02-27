//
// Created by HZC on 2023/2/27.
//

#include "aplication_layer.h"
#include "network_layer.h"
#include "device_info.h"

/*AFLP相关*/
AFLP_state_enum AFLP_state = AFLP_idle;
uint8_t MAX_ID = 3;

uint8_t AFLP_step = 0;
uint8_t AFLP_TOF_counter = 10;
uint8_t AFLP_RSSI_counter = 10;
uint8_t AFLP_to_mac_address = 0;


uint8_t AFLP_pending_timer_time = 50;//初始化单位个，1个AFLP main函数周期

void application_layer_init(void){
    AFLP_to_mac_address = 1;
}

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

/*Automatic fusion location protocol*/
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
            if(get_RSSI_end_flag() | get_TOF_end_flag()){
                /*TODO:获取定位结果并存入表中，使用下层提供的get函数获取数据*/
                AFLP_state = AFLP_start;
                set_TOF_end_flag(0);/*Clear flag*/
                set_RSSI_end_flag(0);/*Clear flag*/
            }
            break;
        case AFLP_end:
            /*处理存储的定位表*/
            break;
        default:
            break;
    }
}


