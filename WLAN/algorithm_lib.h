#include "fake/platform.h"


typedef struct
{
    /* 栈顶索引 */
    uint8_t stack_top_index;
    /* 栈满标志 0->未满，1->满*/
    uint8_t is_full;
    /* 栈大小 */
    uint8_t stack_length;
    /* 每个元素的大小, 即sizeof(forwarding_table_t) */
    uint8_t element_length;
    uint8_t (*push)(stack_t*, uint8_t*);
    uint8_t (*pop)(stack_t*, uint8_t*);
    uint8_t (*delete)(stack_t*, uint8_t*);
    uint8_t (*clear)(stack_t*);
    uint8_t (*serach)(stack_t*, uint8_t*);
    uint8_t** data;
}stack_t;

/* 栈函数 */
uint8_t stack_push(stack_t* this, uint8_t* data); 
uint8_t stack_pop(stack_t* this, uint8_t* data);
uint8_t stack_delete(stack_t* this, uint8_t* data);
uint8_t stack_clear(stack_t* this);
uint8_t stack_serach(stack_t* this, uint8_t* data);
uint8_t stack_create(stack_t* new_stack, uint8_t** data, uint8_t stack_length, uint8_t element_length);
/* ***** */

/* CRC */
uint8_t crc_8(uint8_t* data, uint8_t length);
/* *** */