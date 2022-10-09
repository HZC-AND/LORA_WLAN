

#define RegFifo 0x00        //FIFO读/写访问
#define RegOpMode 0x01     //运行模式&LoRaTM/FSK 选择
/*FSK/OOK 模式*/
#define RegBitrateMsb 0x02 //比特率设置，最高有效位
#define RegBitrateLsb 0x03 //比特率设置，最低有效位
#define RegFdevMsb 0x04    //频率偏移设置，最高有效位
#define RegFdevLsb 0x05    //频率偏移设置，最低有效位
/**************/
#define RegFrfMsb 0x06     //射频载波频率，最高有效位
#define RegFrfMid 0x07     //射频载波频率，中间位
#define RegFrfLsb 0x08     //射频载波频率，最低有效位
#define RegPaConfig 0x09   //PA 选择和输出功率控制
#define RegPaRamp 0x0A     //PA 斜升/斜降时间和低相噪PLL 的控制
#define RegOcp 0x0B         //过流保护控制
#define RegLna 0x0C         //LNA设置
/*FSK/OOK 模式*/
#define RegRxConfig 0x0D   //AFC、AGC、ctrl
#define RegRssiConfig 0x0E //RSSI
#define RegRssiCollision 0x0F //RSSI 冲突检测器
#define RegRssiThresh 0x10 //RSSI 阈值控制
#define RegRssiValue 0x11 //RSSI 值（单位：dBm）
#define RegRxBw 0x12 //信道滤波器带宽控制
#define RegAfcBw 0x13 //AFC 信道滤波器带宽
#define RegOokPeak 0x14 //OOK 解调器
#define RegOokAvg 0x15 //OOK 解调器阈值
#define RegOokAvg2 0x16 //OOK 解调器平均值
#define RegAfcFei 0x1A //AFC 和 FEI控制
#define RegAfcMsb 0x1B //AFC 频率校正值
#define RegAfcLsb 0x1C
#define RegFeiMsb 0x1D //计算出的频率误差值
#define RegFeiLsb 0x1E 
#define RegPreambleDetect 0x1F //前导检测设置
#define RegRxTimeout1 0x20 //超时 Rx 请求和 RSSI
#define RegRxTimeout2 0x21 //超时 RSSI 和PayloadReady
#define RegRxTimeout3 0x22 //超时 RSSI 和SyncAddress
#define RegRxDelay 0x23 //Rx 周期间延迟
#define RegOsc 0x24 //RC 振荡器设置、CLK-OUT 频率
#define RegPreambleMsb 0x25 //前导码长度、最高有效位
#define RegPreambleLsb 0x26 //前导码长度、最低有效位
#define RegSyncConfig 0x27 //同步字识别控制
#define RegSyncValue1_8 0x28 //0x28-0x2F 同步字字节，1~8
#define RegPacketConfig1 0x30 //数据包模式设置
#define RegPacketConfig2 0x31 //数据包模式设置
#define RegPayloadLength 0x32 //负载长度设置
#define RegNodeAdrs 0x33 //节点地址
#define RegBroadcastAdrs 0x34 //广播地址
#define RegFifoThresh 0x35 //Fifo 阈值、Tx起始条件
#define RegSeqConfig1 0x36 //顶级定序器设置
#define RegSeqConfig2 0x37 //顶级定序器设置
#define RegTimerResol 0x38 //定时器 1 和 2分辨率控制
#define RegTimer1Coef 0x39 //定时器 1 设置
#define RegTimer2Coef 0x3A //定时器 2 设置
#define RegImageCal 0x3B //镜像校准引擎控制
#define RegTemp 0x3C //温度传感器值
#define RegLowBat 0x3D //低电量指示设置
#define RegIrqFlags1 0x3E //状态寄存器：PLL 锁定状态、超时、RSSI
#define RegIrqFlags2 0x3F //状态寄存器：FIFO 处理标志、低电量
/**************/
/*lora 模式*/
#define RegFifoAddrPtr 0x0D //FIFO SPI 指针
#define RegFifoTxBaseAddr 0x0E //起始 Tx 数据
#define RegFifoRxBaseAddr 0x0F //起始 Rx 数据
#define FifoRxCurrentAddr 0x10 //最后接收数据包的起始地址
#define RegIrqFlagsMask 0x11 //可选 IRQ 标志屏蔽
#define RegIrqFlags 0x12 //IRQ标志
#define RegRxNbBytes 0x13 //接收到的字节数
#define RegRxHeaderCntValueMsb 0x14 //接收到的有效报头数
#define RegRxHeaderCntValueLsb 0x15
#define RegRxPacketCntValueMsb 0x16 //接收到的有效数据包数
#define RegRxPacketCntValueLsb 0x17
#define RegModemStat 0x18 //LoRa调制解调器现场状态
#define RegPktSnrValue 0x19 //最后一个数据包的信噪比估值
#define RegPktRssiValue 0x1A //最后一个数据包的 RSSI
#define RegRssiValue 0x1B //电流 RSSI
#define RegHopChannel 0x1C //FHSS 起始信道
#define RegModemConfig1 0x1D //调制解调器物理层配置 1
#define RegModemConfig2 0x1E //调制解调器物理层配置 2
#define RegSymbTimeoutLsb 0x1F //接收机超时值
#define RegPreambleMsb 0x20 //前导码长度
#define RegPreambleLsb 0x21 
#define RegPayloadLength 0x22 //LoRa负载长度
#define RegMaxPayloadLength 0x23 //LoRaTM负载长度最大值
#define RegHopPeriod 0x24 //FHSS 跳频周期
#define RegFifoRxByteAddr 0x25 //FIFO 中最后写入字节的地址
#define RegModemConfig3 0x26 //调制解调器物理层配置 3
/**************/
#define RegDioMapping1 0x40 //DIO0 到 DIO3 引脚映射
#define RegDioMapping2 0x41 //DIO4 到 DIO5 引脚映射、ClkOut 频率
#define RegVersion 0x42 //芯片版本--相关升特 ID
/*FSK/OOK 模式*/
#define RegPllHop 0x44 //控制快速跳频模式
/*************/
#define RegTcxo 0x4B //TCXO 或 XTAL 输入设置
#define RegPaDac 0x4D //PA 更大功率设置
#define RegFormerTemp 0x5B //IQ 校准期间存储温度
/*FSK/OOK 模式*/
#define RegBitRateFrac 0x5D //比特率分配比小数部分
/*************/
#define RegAgcRef 0x61 //AGC 阈值调整
#define RegAgcThresh1 0x62
#define RegAgcThresh2 0x63
#define RegAgcThresh3 0x64
#define RegPll 0x70


/*lora 模式*/
typedef struct
{
    /*
    器件模式
    000->睡眠
    001->待机
    010->频率合成发送（FSTx）
    011->发送（Tx）
    100->频率合成接收（FSRx）
    101->持续接收（RXCONTINUOUS）
    110->单次接收（RXSINGLE）
    111->信道活动检测（CAD）
    */
    uint8_t Mode : 3;
    /*
    访问低频模式寄存器
    0->高频模式（访问高频测试寄存器的权限）
    1->低频模式（访问低频测试寄存器的权限）
    */
    uint8_t LowFrequencyModeOn : 1;
    uint8_t reserved : 2;
    /*
    该位在器件处于睡眠模式时工
    作；如果设置了该位，则允许在
    LoRa 模式下访问位于地址空间
    （0x0D:0x3F）的 FSK 寄存器页面
    0->访 问 LoRa 寄 存 器 页 面0x0D:0x3F
    1->（在 LoRa 模式下）访问 FSK寄存器页面 0x0D:0x3F
    */
    uint8_t AccessSharedReg : 1;
    /*
    0->FSK/OOK 模式
    1->LoRa模式
    仅可在睡眠模式下修改该位。其
    他器件模式下的写操作将被忽
    略。
    */
    uint8_t LongRangeMode : 1;
}RegOpMode_t;

typedef struct
{
    /*RF 载波频率最低有效位*/
    uint8_t RegFrLsbValue;
    /*RF 载波频率中间有效位*/
    uint8_t RegFrMidValue;
    /*RF 载波频率最高有效位*/
    uint8_t RegFrMsbValue;
}RegFr_t;

typedef struct
{
    /*
    若 PaSelect=0（RFO 引脚），则
    Pout=Pmax-(15-OutputPower)
    若 PaSelect=1（PA_BOOST 引
    脚 ） ， 则
    Pout=17-(15-OutputPower)
    */
    uint8_t OutputPower : 4;
    /*
    选 择 最 大 输 出 功 率 ：
    Pmax=10.8+0.6 x MaxPower
    【dBm】
    */
    uint8_t MaxPower : 3;
    /*
    选择 PA 输出引脚
    0->RFO 引脚。输出功率不得超
    过+14dBm。
    1->PA_BOOST 引脚。输出功率
    不得超过+20dBm
    */
    uint8_t PaSelect : 1;
}RegPaConfig_t;

typedef struct
{
    /*
    LORA 模式下斜升/斜降时间
    0000->3.4ms
    0001->2ms
    0010->1ms
    0011->500us
    0100->250us
    0101->125us
    0110->100us
    0111->62us
    1000->50us
    1001->40us
    1010->31us
    1011->25us
    1100->20us
    1101->15us
    1110->12us
    1111->10us
    */
    uint8_t PaRamp : 4;
    uint8_t reserved : 4;
}RegPaRamp_t;

typedef struct
{
    /*
    OCP 电流微调：
    若 OcpTrim<=15（120mA），则
    Imax=45+5 x OcpTrim【mA】；若 15<OcpTrim<=27
    （130~240mA），则 Imax=-30+10 x OcpTrim【mA】；
    更高设置值时，Imax=240mA；缺省 Imax=100mA
    */
    uint8_t OcpTrim : 5;
    /*
    为 PA 开启过流保护（OCP）:
    0->OCP 关闭
    1->OCP 开启
    */
    uint8_t OcpOn : 1;
    uint8_t reserved : 2;
}RegOcp_t;

typedef struct
{
    /*
    高频（RFI_HF）LNA 电流调整
    00->缺省 LNA 电流
    11->提升灵敏度，150%LNA 电流
    */
    uint8_t LnaBoostHf : 2;
    uint8_t reserved : 1;
    /*
    低频（RFI_LF）LNA 电流调整
    00->缺省 LNA 电流
    Other->预留
    */
    uint8_t LnaBoostLf : 2;
    /*
    LNA 增益设置：
    000->未使用
    001->G1=最大增益
    010->G2
    011->G3
    100->G4
    101->G5
    110->G6=最小增益
    111->未使用
    */
    uint8_t LnaGain : 3;
}RegLna_t;

typedef struct
{
    /*
    检测到 CAD 中断屏蔽：设置该
    位将屏蔽 RegIrqFlags 中对应的
    IRQ
    */
    uint8_t CadDetectedMask : 1;
    /*
    FHSS 改变信道中断屏蔽：设置
    该位将屏蔽 RegIrqFlags 中对应
    的 IRQ
    */
    uint8_t FhssChangeChannelMask : 1;
    /*
    CAD 完成中断屏蔽：设置该位将
    屏蔽 RegIrqFlags 中对应的 IRQ
    */
    uint8_t CadDoneMask : 1;
    /*
    FIFO 负载发送完成中断屏蔽：设
    置该位将屏蔽 RegIrqFlags 中对
    应的 IRQ
    */
    uint8_t TxDoneMask : 1;
    /*
    Rx 模式下接收到的有效报头屏
    蔽 ： 设 置 该 位 将 屏 蔽
    RegIrqFlags 中对应的 IRQ
    */
    uint8_t ValidHeaderMask : 1;
    /*
    负载 CRC 错误中断屏蔽：设置
    该位将屏蔽 RegIrqFlags 中对应
    的 IRQ
    */
    uint8_t PayloadCrcErrorMask : 1;
    /*
    数据包接收完成中断屏蔽：设置
    该位将屏蔽 RegIrqFlags 中对应
    的 IRQ
    */
    uint8_t RxDoneMask : 1;
    /*
    超时中断屏蔽：设置该位将屏蔽
    RegIrqFlags 中对应的 IRQ
    */
    uint8_t RxTimeoutMask : 1;
}RegIrqFlagsMask_t;

typedef struct
{
    /*
    CAD 操作中检测到有效 Lora 信
    号：一次写操作清除 IRQ
    */
    uint8_t CadDetected : 1;
    /*
    FHSS 改变信道中断：一次写操
    作清除 IRQ
    */
    uint8_t FhssChangeChannel : 1;
    /*  
    CAD 完成：通过写操作清除：一
    次写操作清除 IRQ
    */
    uint8_t CadDone : 1;
    /*
    FIFO 负载发送完成中断：一次写
    操作清除 IRQ
    */
    uint8_t TxDone : 1;
    /*
    Rx 模式下接收到的有效报头：一
    次写操作清除 IRQ
    */
    uint8_t ValidHeader : 1;
    /*
    负载 CRC 错误中断：一次写操
    作清除 IRQ
    */
    uint8_t PayloadCrcError : 1;
    /*
    数据包接收完成中断：一次写操
    作清除 IRQ
    */
    uint8_t RxDone : 1;
    /*
    超时中断：一次写操作清除 IRQ
    */
    uint8_t RxTimeout : 1;
}RegIrqFlags_t;

typedef struct
{
    /*
    最后一次转换至 Rx 模式后接收
    的有效报头数。报头和数据包计数器在
    睡眠模式下复位。
    */
    uint8_t RegRxHeaderCntValueLsb;
    uint8_t RegRxHeaderCntValueMsb;
}RegRxHeaderCntValue_t;

typedef struct
{
    /*
    最后一次转换至 Rx 模式后接收
    的有效报头数。报头和数据包计数器在
    睡眠模式下复位。
    */
    uint8_t RegRxPacketCntValueLsb;
    uint8_t RegRxPacketCntValueMsb;
}RegRxPacketCntValue_t;

//状态寄存器
typedef struct
{
    /*
    ex:10000
    1->调制解调器清零
    0->报头信息有效
    0->RX 进行中
    0->信号已同步
    0->检测到信号
    */
    uint8_t ModemStatus : 5;
    /*
    最后一次接收到的报头的编码率
    */
    uint8_t RxCodingRate : 2;
}RegModemStat_t;

typedef struct
{
    /*
    使用中的调频当前信道值
    */
    uint8_t FhssPresentChannel : 6;
    /*
    从接收到的数据包报头中提取的CRC 信息
    0->报头显示 CRC 关闭
    1->报头显示 CRC 开启*/
    uint8_t RxPayloadCrcOn : 1;
    /*
    尝试 Tx/Rx/CAD 操作时 PLL 锁定失败
    0->PLL 未锁定
    1->PLL 锁定
    */
    uint8_t PllTimeout : 1;
}RegHopChannel_t;

typedef struct
{
    /*
    0->显式报头模式
    1->隐式报头模式
    */
    uint8_t mplicitHeaderModeOn : 1;
    /*
    纠错编码率
    001->4/5
    010->4/6
    011->4/7
    100->4/8
    所有其他值->预留
    隐式报头模式下应在接收机上设
    置该位，以确定
    */
    uint8_t CodingRate : 3;
    /*
    信号带宽：
    0000->7.8kHz
    0001->10.4kHz
    0010->15.6kHz
    0011->20.8kHz
    0100->31.25kHz
    0101->41.7kHz
    0110->62.5kHz
    0111->125kHz
    1000->250kHz
    1001->500kHz
    其他值->预留
    低频段（169MHz）不支持信号
    带宽 8&9
    */
    uint8_t Bw : 4;
}RegModemConfig1_t;

typedef struct
{
    /*
    RX 超时最高有效位
    */
    uint8_t SymbTimeout_9_8 : 2;
    /*
    发送端 CRC 信息
    0->CRC 关闭
    1->CRC 开启
    */
    uint8_t RxPayloadCrcOn : 1;
    /*
    0->正常模式，发送单个数据包
    1->持续模式，通过 FIFO 发送多
    个数据包（用于频谱分析）
    */
    uint8_t TxContinuousMode : 1;
    /*
    SF 值（以 2 基对数表示）
    6->64 码片/符号
    7->128 码片/符号
    8->256 码片/符号
    9->512 码片/符号
    10->1024 码片/符号
    11->2048 码片/符号
    12->4096 码片/符号
    其他值预留。
    */
    uint8_t SpreadingFactor : 4;
}RegModemConfig2_t;

typedef struct
{
    /*
    RX 超时最低有效位
    RX 操作超时值以符号数表示：
    TimeOut=SymbTimeout·Ts
    */
    uint8_t SymbTimeout_7_0;
    uint8_t SymbTimeout_9_8;
}RegSymbTimeout_t;

/*
前导码长度
*/
typedef struct
{
    uint8_t RegPreambleLsb;
    uint8_t RegPreambleMsb;
}RegPreamble_t;

typedef struct
{
    uint8_t reserved1 : 2;
    /*
    0->寄存器 LnaGain 设置的 LNA增益
    1->内部 AGC 环路设置的 LNA增益
    */
    uint8_t AgcAutoOn : 1;
    /*
    0->关闭
    1->开启；符号长度超过 16ms时必须打开
    */
    uint8_t LowDataRateOptimize : 1;
    uint8_t reserved2 : 4;
}RegModemConfig3_t;




/*************/


void SX1278_Init();