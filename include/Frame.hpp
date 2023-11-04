#include <iostream>
#include <memory>

// 3rd party include

namespace rm_serial_port {
/**
 * @brief 帧结构体
 *
 */
struct Frame {
  u_int8_t SOF = 0xA5; // 裁判系统中，SOF为0xA5
  u_int16_t data_length{0};
  u_int8_t seq;
  u_int8_t CRC8;
  u_int16_t cmd_id;
  u_int8_t *data{nullptr};
  u_int16_t CRC16;

  /**
   * @brief 将缓冲区数据转换为帧
   * 
   * @param buffer 缓冲区
   */
  void buffer_to_frame(u_int8_t *buffer) {
    if (buffer[0] != SOF)
    {
      std::cerr << "SOF error" << '\n';
    }
    data_length = static_cast<uint16_t>(buffer[1]) << 8 | buffer[2];
    seq = buffer[3];
    CRC8 = buffer[4];
    cmd_id = static_cast<uint16_t>(buffer[5]) << 8 | buffer[6];
    data = new u_int8_t[data_length];
    for (size_t i = 0; i < data_length; i++)
    {
      data[i] = buffer[7 + i];
    }
    CRC16 = static_cast<uint16_t>(buffer[7 + data_length]) << 8 | buffer[8 + data_length];
  }

  /**
   * @brief 将帧转换为缓冲区数据
   * 
   * @return std::shared_ptr<u_int8_t[]> 字节数组智能指针
   */
  std::shared_ptr<u_int8_t[]> frame_to_buffer()
  {
    auto buffer = std::make_shared<u_int8_t[]>(data_length + 9);
    buffer[0] = SOF;
    buffer[1] = static_cast<uint8_t>(data_length >> 8);
    buffer[2] = static_cast<uint8_t>(data_length);
    buffer[3] = seq;
    buffer[4] = CRC8;
    buffer[5] = static_cast<uint8_t>(cmd_id >> 8);
    buffer[6] = static_cast<uint8_t>(cmd_id);
    for (size_t i = 0; i < data_length; i++)
    {
      buffer[7 + i] = data[i];
    }
    buffer[7 + data_length] = static_cast<uint8_t>(CRC16 >> 8);
    buffer[8 + data_length] = static_cast<uint8_t>(CRC16);
    return buffer;
  }

  ~Frame() {
    if (data != nullptr) {
      delete[] data;
    }
    std::cout << "Frame destructed" << std::endl;
  }
};
} // namespace rm_serial_port
