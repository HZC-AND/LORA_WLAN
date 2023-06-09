/**
 * @file algorithm_lib.c
 * @author HZC (woshihuzhicheng@njfu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-12-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include "algorithm_lib.h"

/* 栈处理逻辑代码 */
uint8_t stack_push(struct stack* this, uint8_t* data){
    if(this->is_full == 1){
        /* 栈满 */
        return 0;
    }
    this->stack_top_index++;
    if(this->stack_top_index == (this->stack_length - 1)){
        this->is_full = 1;
    }
    // this->data[this->i]->from_mac_address = data->from_mac_address;
    // this->data[this->i]->to_mac_address = data->to_mac_address;
    // this->data[this->i]->crc_8 = data->crc_8;
    for(uint8_t i = 0;i < this->element_length;i++){
//        this->data[this->stack_top_index][i] = data[i];
        *(this->data + this->stack_top_index * this->element_length + i) = data[i];
    }
    
    return 1;
} 

uint8_t stack_pop(struct stack* this, uint8_t* data){
    if(this->stack_top_index == 0){
        /* 栈空 */
        return 0;
    }
    for(uint8_t i = 0;i < this->element_length;i++){
//        data[i] = this->data[this->stack_top_index][i];
        data[i] = *(this->data + this->stack_top_index * this->element_length + i);
    }
    this->stack_top_index--;
    if(this->stack_top_index == 0){
        this->is_full = 0;
    }

    return 1;
}

uint8_t stack_delete(struct stack* this, uint8_t* data){
    uint8_t find_index = this->stack_length;//初始为超出范围值
    uint8_t delete_state = 0;
    /* 遍历搜索，后续可尝试使用更高效的查找法，比如二分法 */
    for(uint8_t i = 0;i <= this->stack_top_index;i++){
        for(uint8_t j = 0;j < this->element_length;j++){
            if(data[j] == *(this->data + i * this->element_length + j)){
                find_index = i;
            }else{
                find_index = this->stack_length;
                break;
            }
        }

        if(find_index <= this->stack_top_index){
            for(uint8_t j = 0;j <= (this->stack_top_index - 1);j++){
                for(uint8_t k = 0;k < this->element_length;k++){
                    /* 后面元素覆盖前面元素 */
//                    this->data[j][k] = this->data[j+1][k];
                    *(this->data + j * this->element_length + k) = *(this->data + (i + 1) * this->element_length + k);
                }
            }
            delete_state = 1;
            /* 删除操作后，重置find_index */
            find_index = this->stack_length;
            /* 栈数据减去1 */
            this->stack_top_index--;
        }
    }
    
    if(delete_state == 0){
        /* 删除错误，栈中未找到指定数据 */
        return 0;
    }
    return 1;
}

uint8_t stack_clear(struct stack* this){
    this->stack_top_index = 0;
    this->is_full = 0;
    return 1;
}

uint8_t stack_search(struct stack* this, uint8_t* data){
    uint8_t find_index = this->stack_length;//初始为超出范围值
    uint8_t delete_state = 0;
    /* 遍历搜索，后续可尝试使用更高效的查找法，比如二分法 */
    for(uint8_t i = 0;i <= this->stack_top_index;i++){
        for(uint8_t j = 0;j < this->element_length;j++){
            if(j != 2){//特殊处理，第三个字节不比较，time_counter
//                if(data[j] == this->data[i][j]){
                if(data[j] == *(this->data + i * this->element_length + j)){
                    find_index = i;
                }else{
                    find_index = this->stack_length;
                    break;
                }
            }
        }

        /* 搜索到一个就退出外层循环 */
        if(find_index <= this->stack_top_index){
            break;
        }
    }
    
    if(find_index == this->stack_length){
        /* 栈中未找到指定数据 */
        return this->stack_top_index;
    }
    return find_index;
}

uint8_t stack_create(struct stack* new_stack, uint8_t* data, uint8_t stack_length, uint8_t element_length){
    new_stack->stack_top_index = 0;
    new_stack->is_full = 0;
    new_stack->stack_length = stack_length;
    new_stack->element_length = element_length;
    new_stack->push = stack_push;
    new_stack->pop = stack_pop;
    new_stack->delete = stack_delete;
    new_stack->clear = stack_clear;
    new_stack->search = stack_search;
    new_stack->data = data;

    return 1;
}
/* ************* */

/* CRC处理 */
// #define FACTOR (0x107 & 0xFF) //多项式因子(取低8bit)

uint8_t crc_8(uint8_t* data, uint8_t length){
    //G(X) = X8+X2+X+1
    uint8_t crc = 0x00;
    uint8_t FACTOR = (0x107 & 0xFF);//多项式因子(取低8bit)
    while(length--)
	{
        crc ^= (*data++);//前一字节计算CRC后的结果异或上后一字节，再次计算CRC
        for (uint8_t i=8; i>0; i--)
        {
            if (crc & 0x80)//高位为1，需要异或；否则，不需要
            {
                crc = (crc << 1) ^ FACTOR;
            }
            else
            {
            	crc = (crc << 1);
			}
        }
    }

    return crc;
}
/* ****** */

/* 数组处理 */
void get_max_min_value_in_array(uint8_t* array,uint8_t element_length,uint8_t array_length,uint8_t* index_max,uint8_t* index_min){
    switch (element_length)
    {
        case 1:{
            uint8_t max_temp_value = 0;
            uint8_t min_temp_value = 0xFF;
            for(uint8_t i = 0;i < array_length;i++){
                if(array[i] > max_temp_value){
                    max_temp_value = array[i];
                    *index_max = i;
                }
                if(array[i] < min_temp_value){
                    min_temp_value = array[i];
                    *index_min = i;
                }
            }
        }
            break;
        case 2:{
            uint16_t max_temp_value = 0;
            uint16_t min_temp_value = 0xFFFF;
            for(uint8_t i = 0;i < array_length;i++){
                if(((uint16_t*)array)[i] > max_temp_value){
                    max_temp_value = ((uint16_t*)array)[i];
                    *index_max = i;
                }
                if(((uint16_t*)array)[i] < min_temp_value){
                    min_temp_value = ((uint16_t*)array)[i];
                    *index_min = i;
                }
            }
        }
            break;
        case 3:
            break;
        case 8:{
            uint64_t max_temp_value = 0;
            uint64_t min_temp_value = 0xFFFFFFFFFFFFFFFF;
            for(uint8_t i = 0;i < array_length;i++){
                if(((uint64_t*)array)[i] > max_temp_value){
                    max_temp_value = ((uint64_t*)array)[i];
                    *index_max = i;
                }
                if(((uint64_t*)array)[i] < min_temp_value){
                    min_temp_value = ((uint64_t*)array)[i];
                    *index_min = i;
                }
            }
        }
            break;
        default:
            break;
    }
}
/* ******* */