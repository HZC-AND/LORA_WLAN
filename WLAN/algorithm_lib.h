/**
 * @file algorithm_lib.h
 * @author HZC (woshihuzhicheng@njfu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-12-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "mainConfig.h"

#ifndef NJFUWORK_FINAL_MAINCONFIG_H
#include "fake/platform.h"
#else
#include <stdint.h>
#endif


typedef struct stack
{
    /* 栈顶索引 */
    uint8_t stack_top_index;
    /* 栈满标志 0->未满，1->满*/
    uint8_t is_full;
    /* 栈大小 */
    uint8_t stack_length;
    /* 每个元素的大小, 即sizeof(forwarding_table_t) */
    uint8_t element_length;
    uint8_t (*push)(struct stack*, uint8_t*);
    uint8_t (*pop)(struct stack*, uint8_t*);
    uint8_t (*delete)(struct stack*, uint8_t*);
    uint8_t (*clear)(struct stack*);
    uint8_t (*search)(struct stack*, uint8_t*);
    uint8_t* data;
}stack_t;

/* 栈函数 */
uint8_t stack_push(struct stack* this, uint8_t* data);
uint8_t stack_pop(struct stack* this, uint8_t* data);
uint8_t stack_delete(struct stack* this, uint8_t* data);
uint8_t stack_clear(struct stack* this);
uint8_t stack_search(struct stack* this, uint8_t* data);
uint8_t stack_create(struct stack* new_stack, uint8_t* data, uint8_t stack_length, uint8_t element_length);
/* ***** */

/* CRC */
uint8_t crc_8(uint8_t* data, uint8_t length);
/* *** */