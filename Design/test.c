#pragma section "my_section"
#pragma Location = "my_section" //Flash "my_sction .data .ro .rw .text"
#pragma default_function_attributes = __root
void aaaa(){

}
void bbbb(){
    
}
#pragma default_function_attributes =
#pragma Location =



#pragma pack(1)
typedef struct test
{
    /* data */
};
#pragma pack()


void aa(){
    ...
    ...
    ..
    __DSB();
    ...
}

.intvec{

}


////
不完全前置条件：内核了解：Arm

第一天：13K
1.编译流程，链接流程，启动流程。


第二天：15K+
SBC:(CAN,LIN,睡眠唤醒)
    9262,TJA1145A,

DCDC/LDO:
    供电

FBL:
    UDS刷写

睡眠唤醒：


