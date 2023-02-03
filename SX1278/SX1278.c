/**
 * @file SX1278.c
 * @author HZC (woshihuzhicheng@njfu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-12-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* TODO:1.尽可能不使用hw_delay 2.发送成功使用中断反馈 */

#include "SX1278.h"
#include <string.h>
#include "data_link_layer.h"

extern SX1278_t SX1278;

uint8_t SX1278_SPIRead(SX1278_t *module, uint8_t addr)
{
    uint8_t tmp;
    SX1278_hw_SPICommand(module->hw, addr);
    tmp = SX1278_hw_SPIReadByte(module->hw);
    SX1278_hw_SetNSS(module->hw, 1);
    return tmp;
}

void SX1278_SPIWrite(SX1278_t *module, uint8_t addr, uint8_t cmd)
{
    SX1278_hw_SetNSS(module->hw, 0);
    SX1278_hw_SPICommand(module->hw, addr | 0x80);
    SX1278_hw_SPICommand(module->hw, cmd);
    SX1278_hw_SetNSS(module->hw, 1);
}

void SX1278_SPIBurstRead(SX1278_t *module, uint8_t addr, uint8_t *rxBuf,
                         uint8_t length)
{
    uint8_t i;
    if (length <= 1)
    {
        return;
    }
    else
    {
        SX1278_hw_SetNSS(module->hw, 0);
        SX1278_hw_SPICommand(module->hw, addr);
        for (i = 0; i < length; i++)
        {
            *(rxBuf + i) = SX1278_hw_SPIReadByte(module->hw);
        }
        SX1278_hw_SetNSS(module->hw, 1);
    }
}

void SX1278_SPIBurstWrite(SX1278_t *module, uint8_t addr, uint8_t *txBuf,
                          uint8_t length)
{
    unsigned char i;
    if (length <= 1)
    {
        return;
    }
    else
    {
        SX1278_hw_SetNSS(module->hw, 0);
        SX1278_hw_SPICommand(module->hw, addr | 0x80);
        for (i = 0; i < length; i++)
        {
            SX1278_hw_SPICommand(module->hw, *(txBuf + i));
        }
        SX1278_hw_SetNSS(module->hw, 1);
    }
}

void SX1278_config(SX1278_t *module)
{
    SX1278_sleep(module); // Change modem mode Must in Sleep mode
    SX1278_hw_DelayMs(15);

    SX1278_entryLoRa(module);
    // SX1278_SPIWrite(module, 0x5904); //?? Change digital regulator form 1.6V to 1.47V: see errata note

    uint64_t freq = ((uint64_t)module->frequency << 19) / 32000000;
    uint8_t freq_reg[3];
    freq_reg[0] = (uint8_t)(freq >> 16);
    freq_reg[1] = (uint8_t)(freq >> 8);
    freq_reg[2] = (uint8_t)(freq >> 0);
    SX1278_SPIBurstWrite(module, LR_RegFrMsb, (uint8_t *)freq_reg, 3); // setting  frequency parameter

    SX1278_SPIWrite(module, RegSyncWord, 0x34);

    // setting base parameter
    SX1278_SPIWrite(module, LR_RegPaConfig, SX1278_Power[module->power]); // Setting output power parameter

    SX1278_SPIWrite(module, LR_RegOcp, 0x0B); // RegOcp,Close Ocp
    SX1278_SPIWrite(module, LR_RegLna, 0x23); // RegLNA,High & LNA Enable
    if (SX1278_SpreadFactor[module->LoRa_SF] == 6)
    { // SFactor=6
        uint8_t tmp;
        SX1278_SPIWrite(module,
                        LR_RegModemConfig1,
                        ((SX1278_LoRaBandwidth[module->LoRa_BW] << 4) + (SX1278_CodingRate[module->LoRa_CR] << 1) + 0x01)); // Implicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)

        SX1278_SPIWrite(module,
                        LR_RegModemConfig2,
                        ((SX1278_SpreadFactor[module->LoRa_SF] << 4) + (SX1278_CRC_Sum[module->LoRa_CRC_sum] << 2) + 0x03));

        tmp = SX1278_SPIRead(module, 0x31);
        tmp &= 0xF8;
        tmp |= 0x05;
        SX1278_SPIWrite(module, 0x31, tmp);
        SX1278_SPIWrite(module, 0x37, 0x0C);
    }
    else
    {
        SX1278_SPIWrite(module,
                        LR_RegModemConfig1,
                        ((SX1278_LoRaBandwidth[module->LoRa_BW] << 4) + (SX1278_CodingRate[module->LoRa_CR] << 1) + 0x00)); // Explicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)

        SX1278_SPIWrite(module,
                        LR_RegModemConfig2,
                        ((SX1278_SpreadFactor[module->LoRa_SF] << 4) + (SX1278_CRC_Sum[module->LoRa_CRC_sum] << 2) + 0x00)); // SFactor &  LNA gain set by the internal AGC loop
    }

    SX1278_SPIWrite(module, LR_RegModemConfig3, 0x04);
    SX1278_SPIWrite(module, LR_RegSymbTimeoutLsb, 0x08); // RegSymbTimeoutLsb Timeout = 0x3FF(Max)
    SX1278_SPIWrite(module, LR_RegPreambleMsb, 0x00);    // RegPreambleMsb
    SX1278_SPIWrite(module, LR_RegPreambleLsb, 8);       // RegPreambleLsb 8+4=12byte Preamble
    SX1278_SPIWrite(module, REG_LR_DIOMAPPING2, 0x01);   // RegDioMapping2 DIO5=00, DIO4=01
    module->readBytes = 0;
    SX1278_standby(module); // Entry standby mode
}

void SX1278_standby(SX1278_t *module)
{
    SX1278_SPIWrite(module, LR_RegOpMode, 0x09);
    module->status = STANDBY;
}

void SX1278_sleep(SX1278_t *module)
{
    SX1278_SPIWrite(module, LR_RegOpMode, 0x08);
    module->status = SLEEP;
}

void SX1278_entryLoRa(SX1278_t *module)
{
    SX1278_SPIWrite(module, LR_RegOpMode, 0x88);
}

void SX1278_clearLoRaIrq(SX1278_t *module)
{
    SX1278_SPIWrite(module, LR_RegIrqFlags, 0xFF);
}

int SX1278_LoRaEntryRx(SX1278_t *module, uint8_t length, uint32_t timeout)
{
    uint8_t addr;

    module->packetLength = length;

    SX1278_config(module);                             // Setting base parameter
    SX1278_SPIWrite(module, REG_LR_PADAC, 0x84);       // Normal and RX
//    SX1278_SPIWrite(module, LR_RegHopPeriod, 0xFF);    // No FHSS
    SX1278_SPIWrite(module, LR_RegHopPeriod, 0x00);    // No FHSS
    SX1278_SPIWrite(module, REG_LR_DIOMAPPING1, 0x01); // DIO=00,DIO1=00,DIO2=00, DIO3=01
    SX1278_SPIWrite(module, LR_RegIrqFlagsMask, 0x3F); // Open RxDone interrupt & Timeout
    SX1278_clearLoRaIrq(module);
    SX1278_SPIWrite(module, LR_RegPayloadLength, length); // Payload Length 21byte(this register must difine when the data long of one byte in SF is 6)
    addr = SX1278_SPIRead(module, LR_RegFifoRxBaseAddr);  // Read RxBaseAddr
    SX1278_SPIWrite(module, LR_RegFifoAddrPtr, addr);     // RxBaseAddr->FiFoAddrPtr
    SX1278_SPIWrite(module, LR_RegOpMode, 0x8d);          // Mode//Low Frequency Mode
    // SX1278_SPIWrite(module, LR_RegOpMode,0x05);	//Continuous Rx Mode //High Frequency Mode
    module->readBytes = 0;

    while (1)
    {
        if ((SX1278_SPIRead(module, LR_RegModemStat) & 0x04) == 0x04)
        { // Rx-on going RegModemStat
            module->status = RX;
            return 1;
        }
        if (--timeout == 0)
        {
            SX1278_hw_Reset(module->hw);
            SX1278_config(module);
            return 0;
        }
        //		SX1278_hw_DelayMs(1);
    }
}

uint8_t SX1278_LoRaRxPacket(SX1278_t *module)
{
    unsigned char addr;
    unsigned char packet_size;

    if (SX1278_hw_GetDIO0(module->hw))
    {
        memset(module->rxBuffer, 0x00, SX1278_MAX_PACKET);

        addr = SX1278_SPIRead(module, LR_RegFifoRxCurrentaddr); // last packet addr
        SX1278_SPIWrite(module, LR_RegFifoAddrPtr, addr);       // RxBaseAddr -> FiFoAddrPtr

        if (module->LoRa_SF == SX1278_LORA_SF_6)
        { // When SpreadFactor is six,will used Implicit Header mode(Excluding internal packet length)
            packet_size = module->packetLength;
        }
        else
        {
            packet_size = SX1278_SPIRead(module, LR_RegRxNbBytes); // Number for received bytes
        }

        SX1278_SPIBurstRead(module, 0x00, module->rxBuffer, packet_size);
        module->readBytes = packet_size;
        SX1278_clearLoRaIrq(module);
    }
    return module->readBytes;
}

int SX1278_LoRaEntryTx(SX1278_t *module, uint8_t length, uint32_t timeout)
{
    uint8_t addr;
    uint8_t temp;

    module->packetLength = length;

    SX1278_config(module);                             // setting base parameter
    SX1278_SPIWrite(module, REG_LR_PADAC, 0x87);       // Tx for 20dBm
    SX1278_SPIWrite(module, LR_RegHopPeriod, 0x00);    // RegHopPeriod NO FHSS
    SX1278_SPIWrite(module, REG_LR_DIOMAPPING1, 0x41); // DIO0=01, DIO1=00,DIO2=00, DIO3=01
    SX1278_clearLoRaIrq(module);
    SX1278_SPIWrite(module, LR_RegIrqFlagsMask, 0xF7);    // Open TxDone interrupt
    SX1278_SPIWrite(module, LR_RegPayloadLength, length); // RegPayloadLength 21byte
    addr = SX1278_SPIRead(module, LR_RegFifoTxBaseAddr);  // RegFiFoTxBaseAddr
    SX1278_SPIWrite(module, LR_RegFifoAddrPtr, addr);     // RegFifoAddrPtr

    while (1)
    {
        temp = SX1278_SPIRead(module, LR_RegPayloadLength);
        if (temp == length)
        {
            module->status = TX;
            return 1;
        }

        if (--timeout == 0)
        {
            SX1278_hw_Reset(module->hw);
            SX1278_config(module);
            return 0;
        }
    }
}

int SX1278_LoRaTxPacket(SX1278_t *module, uint8_t *txBuffer, uint8_t length,
                        uint32_t timeout)
{
    SX1278_SPIBurstWrite(module, 0x00, txBuffer, length);
    SX1278_SPIWrite(module, LR_RegOpMode, 0x8b); // Tx Mode
    while (1)
    {
        if (SX1278_hw_GetDIO0(module->hw))
        { // if(Get_NIRQ()) //Packet send over
            SX1278_SPIRead(module, LR_RegIrqFlags);
            SX1278_clearLoRaIrq(module); // Clear irq
            SX1278_standby(module);      // Entry Standby mode
            return 1;
        }

        if (--timeout == 0)
        {
            SX1278_hw_Reset(module->hw);
            SX1278_config(module);
            return 0;
        }
        SX1278_hw_DelayMs(1);
    }
}

void SX1278_init(SX1278_t *module, uint64_t frequency, uint8_t power,
                 uint8_t LoRa_SF, uint8_t LoRa_BW, uint8_t LoRa_CR,
                 uint8_t LoRa_CRC_sum, uint8_t packetLength)
{
    SX1278_hw_init(module->hw);
    module->frequency = frequency;
    module->power = power;
    module->LoRa_SF = LoRa_SF;
    module->LoRa_BW = LoRa_BW;
    module->LoRa_CR = LoRa_CR;
    module->LoRa_CRC_sum = LoRa_CRC_sum;
    module->packetLength = packetLength;
    SX1278_config(module);
}

int SX1278_transmit(SX1278_t *module, uint8_t *txBuf, uint8_t length,
                    uint32_t timeout)
{
    if (SX1278_LoRaEntryTx(module, length, timeout))
    {
        return SX1278_LoRaTxPacket(module, txBuf, length, timeout);
    }
    return 0;
}

int SX1278_receive(SX1278_t *module, uint8_t length, uint32_t timeout)
{
    return SX1278_LoRaEntryRx(module, length, timeout);
}

uint8_t SX1278_available(SX1278_t *module)
{
    return SX1278_LoRaRxPacket(module);
}

uint8_t SX1278_read(SX1278_t *module, uint8_t *rxBuf, uint8_t length)
{
    if (length != module->readBytes)
        length = module->readBytes;
    memcpy(rxBuf, module->rxBuffer, length);
    rxBuf[length] = '\0';
    module->readBytes = 0;
    return length;
}

uint8_t SX1278_RSSI_LoRa(SX1278_t *module)
{
    uint32_t temp = 10;
    temp = SX1278_SPIRead(module, LR_RegRssiValue); // Read RegRssiValue, Rssi value
    temp = temp + 127 - 137;                        // 127:Max RSSI, 137:RSSI offset
    return (uint8_t)temp;
}

uint8_t SX1278_RSSI(SX1278_t *module)
{
    uint8_t temp = 0xff;
    temp = SX1278_SPIRead(module, RegRssiValue);
    temp = 127 - (temp >> 1); // 127:Max RSSI
    return temp;
}

/*Added in 2022-10-25*/

/* 只接收一次 */
uint8_t SX1278_RX_Once(SX1278_t *module, uint8_t length, uint32_t timeout){
    uint8_t addr;

    module->packetLength = length;

    SX1278_SPIWrite(module, REG_LR_PADAC, 0x84);       // Normal and RX
    SX1278_SPIWrite(module, LR_RegHopPeriod, 0xFF);    // No FHSS
    SX1278_SPIWrite(module, REG_LR_DIOMAPPING1, 0x01); // DIO=00,DIO1=00,DIO2=00, DIO3=01
    SX1278_SPIWrite(module, LR_RegIrqFlagsMask, 0x3F); // Open RxDone interrupt & Timeout
    SX1278_clearLoRaIrq(module);
    SX1278_SPIWrite(module, LR_RegPayloadLength, length); // Payload Length 21byte(this register must difine when the data long of one byte in SF is 6)
    addr = SX1278_SPIRead(module, LR_RegFifoRxBaseAddr);  // Read RxBaseAddr
    SX1278_SPIWrite(module, LR_RegFifoAddrPtr, addr);     // RxBaseAddr->FiFoAddrPtr
    SX1278_SPIWrite(module, LR_RegOpMode, 0x8d);          // Mode//Low Frequency Mode
    // SX1278_SPIWrite(module, LR_RegOpMode,0x05);	//Continuous Rx Mode //High Frequency Mode
    module->readBytes = 0;

    while (1)
    {
        if ((SX1278_SPIRead(module, LR_RegModemStat) & 0x04) == 0x04)
        { // Rx-on going RegModemStat
            module->status = RX;
            // return 1;
            break;
        }
        if (--timeout == 0)
        {
            SX1278_hw_Reset(module->hw);
            SX1278_config(module);
            return 0;
        }
        //		SX1278_hw_DelayMs(1);
    }
    

    /* 处理接收 */
//    unsigned char addr;
    unsigned char packet_size;

    if (SX1278_hw_GetDIO0(module->hw))
    {
        memset(module->rxBuffer, 0x00, SX1278_MAX_PACKET);

        addr = SX1278_SPIRead(module, LR_RegFifoRxCurrentaddr); // last packet addr
        SX1278_SPIWrite(module, LR_RegFifoAddrPtr, addr);       // RxBaseAddr -> FiFoAddrPtr

        if (module->LoRa_SF == SX1278_LORA_SF_6)
        { // When SpreadFactor is six,will used Implicit Header mode(Excluding internal packet length)
            packet_size = module->packetLength;
        }
        else
        {
            packet_size = SX1278_SPIRead(module, LR_RegRxNbBytes); // Number for received bytes
        }

        SX1278_SPIBurstRead(module, 0x00, module->rxBuffer, packet_size);
        module->readBytes = packet_size;
        SX1278_clearLoRaIrq(module);

        data_link_layer_receive_callback(&module->rxBuffer[0], packet_size);
    }

    // SX1278_standby(module); // Entry standby mode
//    SX1278_sleep();//进入睡眠模式

//    return module->readBytes;
    return 1;
}

/* 只发送一次 */
uint8_t SX1278_TX_Once(SX1278_t *module, uint8_t *txBuffer, uint8_t length, uint32_t timeout){
    uint8_t addr;
    uint8_t temp;

    module->packetLength = length;

    // SX1278_config(module);                             // setting base parameter
    SX1278_SPIWrite(module, REG_LR_PADAC, 0x87);       // Tx for 20dBm
    SX1278_SPIWrite(module, LR_RegHopPeriod, 0x00);    // RegHopPeriod NO FHSS
    SX1278_SPIWrite(module, REG_LR_DIOMAPPING1, 0x41); // DIO0=01, DIO1=00,DIO2=00, DIO3=01
    SX1278_clearLoRaIrq(module);
    SX1278_SPIWrite(module, LR_RegIrqFlagsMask, 0xF7);    // Open TxDone interrupt
    SX1278_SPIWrite(module, LR_RegPayloadLength, length); // RegPayloadLength 21byte
    addr = SX1278_SPIRead(module, LR_RegFifoTxBaseAddr);  // RegFiFoTxBaseAddr
    SX1278_SPIWrite(module, LR_RegFifoAddrPtr, addr);     // RegFifoAddrPtr

    while (1)
    {
        temp = SX1278_SPIRead(module, LR_RegPayloadLength);
        if (temp == length)
        {
            module->status = TX;
            // return 1;
            break;
        }

        if (--timeout == 0)
        {
            SX1278_hw_Reset(module->hw);
            SX1278_config(module);
            return 0;
         }
    }

    SX1278_SPIBurstWrite(module, 0x00, txBuffer, length);//将数据写入FIFO
    SX1278_SPIWrite(module, LR_RegOpMode, 0x8b); //设置 Tx Mode

    while (1)
    {
        if (SX1278_hw_GetDIO0(module->hw))
        { // if(Get_NIRQ()) //Packet send over
            SX1278_SPIRead(module, LR_RegIrqFlags);
            SX1278_clearLoRaIrq(module); // Clear irq
            // SX1278_standby(module);      // Entry Standby mode
            // return 1;
            break;
        }

        if (--timeout == 0)
        {
            SX1278_hw_Reset(module->hw);
            SX1278_config(module);
            return 0;
        }
        SX1278_hw_DelayMs(1);
    }
    /* TODO:完善 */
//    SX1278_sleep();//进入睡眠模式
}

void SX1278_H_Sleep_Mode(SX1278_t *module){
    SX1278_SPIWrite(module, LR_RegOpMode, 0x08);
    module->status = SLEEP;
}

void SX1278_H_Standby_Mode(SX1278_t *module){
    SX1278_SPIWrite(module, LR_RegOpMode, 0x09);
    module->status = STANDBY;
}

void SX1278_H_Enter_Lora(SX1278_t *module){
    SX1278_SPIWrite(module, LR_RegOpMode, 0x88);;
}

void SX1278_H_CAD_Mode(SX1278_t *module){
    SX1278_SPIWrite(module, LR_RegOpMode, 0x8F);//LORA模式，CAD信道活性检测
    module->status = CAD;
}

/* CAD检测 */
void SX1278_H_CAD_Detect(SX1278_t *module){
    SX1278_H_Sleep_Mode(module);//进入睡眠模式
    /**
     * @brief 修改引脚映射为
     *   DIO5      DIO4         DIO3          DIO2           DIO1       DIO0
     * ModeReady CadDetected  CadDone FhssChangeChannel  RxTimeout    RxDone
     */
    SX1278_SPIWrite(module, REG_LR_DIOMAPPING1, 0x00);
    SX1278_SPIWrite(module, REG_LR_DIOMAPPING2, 0x00);

    SX1278_H_CAD_Mode(module);//进入CAD模式
}

void SX1278_H_Clear_Irq(SX1278_t *module){
    SX1278_SPIWrite(module, LR_RegIrqFlags, 0xFF);;
}

void SX1278_H_Config(SX1278_t *module){
    SX1278_H_Sleep_Mode(module); // Change modem mode Must in Sleep mode
    SX1278_hw_DelayMs(15);/*TODO:由sleep模式切到其他模式的时间有待测试*/
    SX1278_H_Enter_Lora(module);

    uint64_t freq = ((uint64_t)module->frequency << 19) / 32000000;
    uint8_t freq_reg[3];
    freq_reg[0] = (uint8_t)(freq >> 16);
    freq_reg[1] = (uint8_t)(freq >> 8);
    freq_reg[2] = (uint8_t)(freq >> 0);
    SX1278_SPIBurstWrite(module, LR_RegFrMsb, (uint8_t *)freq_reg, 3); // setting  frequency parameter

//    SX1278_SPIWrite(module, RegSyncWord, 0x34);//LORA模式下，这个寄存器没有意义

    // setting base parameter
    SX1278_SPIWrite(module, LR_RegPaConfig, SX1278_Power[module->power]); //PA 选择和输出功率控制

    SX1278_SPIWrite(module, LR_RegOcp, 0x0B); // 关闭过流保护控制
    SX1278_SPIWrite(module, LR_RegLna, 0x23); // High & LNA Enable

    if (SX1278_SpreadFactor[module->LoRa_SF] == 6)
    {
        /**
         * @brief 当扩频因子SF为6时，LoRa调制解调器的数据传输速率最快，因此这一扩频因子仅在特定
           情况下使用。使用时，需在SX1276/77/78寄存器上进行以下几项设置：
           1.在RegModemConfig2中，将SpreadingFactor设置为6
           2.将报头设置为隐式模式
           3.在寄存器地址（0x31）的2至0位中写入0b101一值
           4.在寄存器地址（0x37）中写入0x0C一值
         */
        uint8_t tmp;
        SX1278_SPIWrite(module,
                        LR_RegModemConfig1,
                        ((SX1278_LoRaBandwidth[module->LoRa_BW] << 4) + (SX1278_CodingRate[module->LoRa_CR] << 1) + 0x01)); // Implicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)

        SX1278_SPIWrite(module,
                        LR_RegModemConfig2,
                        ((SX1278_SpreadFactor[module->LoRa_SF] << 4) + (SX1278_CRC_Sum[module->LoRa_CRC_sum] << 2) + 0x03));

        tmp = SX1278_SPIRead(module, 0x31);
        tmp &= 0xF8;
        tmp |= 0x05;
        SX1278_SPIWrite(module, 0x31, tmp);
        SX1278_SPIWrite(module, 0x37, 0x0C);
    }
    else
    {
        SX1278_SPIWrite(module,
                        LR_RegModemConfig1,
                        ((SX1278_LoRaBandwidth[module->LoRa_BW] << 4) + (SX1278_CodingRate[module->LoRa_CR] << 1) + 0x00)); // Explicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)

        SX1278_SPIWrite(module,
                        LR_RegModemConfig2,
                        ((SX1278_SpreadFactor[module->LoRa_SF] << 4) + (SX1278_CRC_Sum[module->LoRa_CRC_sum] << 2) + 0x00)); // SFactor &  LNA gain set by the internal AGC loop
    }

    SX1278_SPIWrite(module, LR_RegModemConfig3, 0x04);//内部 AGC 环路设置的 LNA增益
    /*TODO:下面寄存器操作待研究*/
    SX1278_SPIWrite(module, LR_RegSymbTimeoutLsb, 0x08); // RegSymbTimeoutLsb Timeout = 0x3FF(Max)
    /***********************/
    SX1278_SPIWrite(module, LR_RegPreambleMsb, 0x00);    // RegPreambleMsb
    SX1278_SPIWrite(module, LR_RegPreambleLsb, 8);       // RegPreambleLsb 8+4=12byte Preamble
    /**
     * @brief 修改引脚映射为
     *   DIO5      DIO4
     * ModeReady CadDetected
     */
    SX1278_SPIWrite(module, REG_LR_DIOMAPPING2, 0x00);

    module->readBytes = 0;
    SX1278_H_Standby_Mode(module); // Entry standby mode
}

uint8_t SX1278_H_Enter_TX(SX1278_t *module,uint8_t length){
    uint8_t addr;
    uint8_t temp;

    module->packetLength = length;

    //SX1278_H_Config(module);/*TODO:不调用这个函数，看看能否正常运行*/

    SX1278_SPIWrite(module, REG_LR_PADAC, 0x87);       // Tx for 20dBm
    SX1278_SPIWrite(module, LR_RegHopPeriod, 0x00);    // RegHopPeriod NO FHSS
    /**
     * @brief 修改引脚映射为
     *   DIO3          DIO2           DIO1       DIO0
     * CadDone FhssChangeChannel  RxTimeout    TxDone
     */
    SX1278_SPIWrite(module, REG_LR_DIOMAPPING1, 0x40); // DIO0=01, DIO1=00,DIO2=00, DIO3=00
    SX1278_H_Clear_Irq(module);
    SX1278_SPIWrite(module, LR_RegIrqFlagsMask, 0xF7);    // Open TxDone interrupt
    SX1278_SPIWrite(module, LR_RegPayloadLength, length); // RegPayloadLength
    /*所有FIFO都给TX*/
    addr = SX1278_SPIRead(module, LR_RegFifoTxBaseAddr);  // RegFiFoTxBaseAddr
    SX1278_SPIWrite(module, LR_RegFifoAddrPtr, addr);     // RegFifoAddrPtr
    /**************/

    /*回读判断payload长度是否设置正确*/
    temp = SX1278_SPIRead(module, LR_RegPayloadLength);
    if(temp != length){
        return 0;
    }else{
        module->status = TX;
        /*Do nothing*/
    }

    return 1;
}

uint8_t SX1278_H_Enter_RX(SX1278_t *module,uint8_t length){
    uint8_t addr;

    module->packetLength = length;

//    SX1278_config(module);// Setting base parameter/*TODO:不调用这个函数，看看能否正常运行*/
    SX1278_SPIWrite(module, REG_LR_PADAC, 0x84);       // Normal and RX
//    SX1278_SPIWrite(module, LR_RegHopPeriod, 0xFF);
//    SX1278_SPIWrite(module, LR_RegHopPeriod, 0x00);    // No FHSS
    /**
     * @brief 修改引脚映射为
     *   DIO3          DIO2           DIO1       DIO0
     * CadDone FhssChangeChannel  RxTimeout    RxDone
     */
    SX1278_SPIWrite(module, REG_LR_DIOMAPPING1, 0x00);

//    SX1278_SPIWrite(module, LR_RegIrqFlagsMask, 0x3F); // Open RxDone interrupt & Timeout
//    SX1278_clearLoRaIrq(module);
//    SX1278_SPIWrite(module, LR_RegPayloadLength, length); // Payload Length 21byte(this register must difine when the data long of one byte in SF is 6)
//    addr = SX1278_SPIRead(module, LR_RegFifoRxBaseAddr);  // Read RxBaseAddr
//    SX1278_SPIWrite(module, LR_RegFifoAddrPtr, addr);     // RxBaseAddr->FiFoAddrPtr
//    SX1278_SPIWrite(module, LR_RegOpMode, 0x8d);          // Mode//Low Frequency Mode
//    // SX1278_SPIWrite(module, LR_RegOpMode,0x05);	//Continuous Rx Mode //High Frequency Mode
//    module->readBytes = 0;
//
//    while (1)
//    {
//        if ((SX1278_SPIRead(module, LR_RegModemStat) & 0x04) == 0x04)
//        { // Rx-on going RegModemStat
//            module->status = RX;
//            return 1;
//        }
//        if (--timeout == 0)
//        {
//            SX1278_hw_Reset(module->hw);
//            SX1278_config(module);
//            return 0;
//        }
//        //		SX1278_hw_DelayMs(1);
//    }
}

SX1278_Running_Status_t SX1278_H_Get_Running_Status(void){
    return SX1278.Running_Status;
}

uint8_t SX1278_H_TX_Once(SX1278_t *module, uint8_t *txBuffer, uint8_t length, uint32_t timeout){
    SX1278_H_Enter_TX(module,length);
    SX1278_SPIBurstWrite(module, 0x00, txBuffer, length);//将数据写入FIFO
    SX1278_SPIWrite(module, LR_RegOpMode, 0x8b); //设置 Tx Mode

    module->Running_Status = SX1278_RUNNING_TX_PENDING;//TODO:需要有pending状态的超时管理

    return 1;//上层设置pending//TODO:TIP=>tx done中断设置sx1278待机状态
}

void SX1278_H_Init(SX1278_t *module, uint64_t frequency, uint8_t power,
                   uint8_t LoRa_SF, uint8_t LoRa_BW, uint8_t LoRa_CR,
                   uint8_t LoRa_CRC_sum, uint8_t packetLength){
    SX1278_hw_init(module->hw);
    module->frequency = frequency;
    module->power = power;
    module->LoRa_SF = LoRa_SF;
    module->LoRa_BW = LoRa_BW;
    module->LoRa_CR = LoRa_CR;
    module->LoRa_CRC_sum = LoRa_CRC_sum;
    module->packetLength = packetLength;
//    SX1278_config(module);
    SX1278_H_Config(module);
}