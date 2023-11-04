#include <iostream>
#include <string>
#include <cstddef>

// 3rd party include

namespace rm_serial_port
{
/**
 * @brief 帧头结构体
 *
 */
struct FrameHeader
{
    u_int8_t SOF;
    u_int16_t data_length;
    u_int8_t seq;
    u_int8_t CRC8;
};

/**
 * @brief 帧结构体
 * 
 */
struct Frame
{
    FrameHeader header;
    u_int16_t cmd_id;
    u_int8_t *data;
    u_int16_t CRC16;
};
}
