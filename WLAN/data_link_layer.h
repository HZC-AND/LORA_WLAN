/* 数据分帧，流控，地址寻址，重发

L-Frame
|header|data|crc|
    2    16   2   bytes

header => |MAC address|counter-id|number|
 */

#include "data_link_layer.h"

