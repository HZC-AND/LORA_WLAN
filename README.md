# LORA_WLAN

## 介绍
一个轻量级的Lora无线通信协议，本协议基于SX1278芯片开发。

## 使用（举例）
> Demo可参考 https://gitee.com/huzhicheng666/NJFUWork_Final.git 工程
1. 初始化
- SX1278硬件及Lora WLAN初始化
```c
//SX1278初始化
SX1278_hw_t SX1278_hw;//定义为全局
SX1278_t SX1278;//定义为全局

SX1278_hw.dio0.port = LORA_DIO0_GPIO_Port;
SX1278_hw.dio0.pin = LORA_DIO0_Pin;
SX1278_hw.nss.port = LORA_NSS_GPIO_Port;
SX1278_hw.nss.pin = LORA_NSS_Pin;
SX1278_hw.reset.port = LORA_RESET_GPIO_Port;
SX1278_hw.reset.pin = LORA_RESET_Pin;
SX1278_hw.spi = &hspi1;

SX1278.hw = &SX1278_hw;

SX1278_H_Init(&SX1278, 413000000, SX1278_POWER_17DBM,SX1278_LORA_SF_7,
              SX1278_LORA_BW_500KHZ, SX1278_LORA_CR_4_5,SX1278_LORA_CRC_EN, 10);
SX1278.Running_Status = SX1278_RUNNING_IDLE;
SX1278_H_RX_Once(&SX1278, 50, 200);

//Lora WLAN初始化
#if(LORA_WLAN_AFLP == STD_ON && LORA_WLAN == STD_ON)
    application_layer_init();
#endif

#if (LORA_WLAN == STD_ON)
    data_link_layer_init();
    network_layer_init();
#endif
```
- 将network_layer_main_function()放置于周期task或者while(1)中
```c
taskENTER_CRITICAL();//临界区，这里以freertos为例
network_layer_main_function();
taskEXIT_CRITICAL();
```

2. 发送
```c
taskENTER_CRITICAL();//临界区，这里以freertos为例
if (get_network_layer_send_state() == sending_idle) {
    network_layer_data_frame_send(&Tx_Data[0], 10, To_MAC_Address);//(发送的数据Buffer,长度，接收节点MAC地址)
}
taskEXIT_CRITICAL();
```


3. 接收（放置在IO中断回调函数中）
```c
if (GPIO_Pin == LORA_DIO0_Pin) {
    switch (SX1278.Running_Status) {
        case SX1278_RUNNING_TX_PENDING:
            SX1278.Running_Status = SX1278_RUNNING_IDLE;
            SX1278_H_RX_Once(&SX1278, 50, 200);//(SX1278_t *module, uint8_t length, uint32_t timeout)
            break;
        case SX1278_RUNNING_RX_PENDING:
            SX1278.Running_Status = SX1278_RUNNING_RX_DONE;
            SX1278_H_RX_Once(&SX1278, 50, 200);
            break;
        case SX1278_RUNNING_RX_DONE:
            SX1278_H_RX_Once(&SX1278, 50, 200);
        default:
            break;
    }
}
```

## 功能
> 待更新


