#include <iostream>
#include <memory>

namespace rm_serial_port {
/**
 * @brief 帧类
 *
 */
class Frame {
public:
  u_int8_t SOF = 0xA5;
  u_int16_t data_length{0};
  u_int8_t seq;
  u_int8_t CRC8;
  u_int16_t cmd_id;
  u_int8_t *data{nullptr};
  u_int16_t CRC16;

public:
  Frame() = default;

  Frame(u_int16_t data_length, u_int8_t seq, u_int16_t cmd_id, u_int8_t *data)
      : data_length(data_length), seq(seq), cmd_id(cmd_id), data(data) {
    get_crc8();
    get_crc16();
  }

  Frame(Frame &&frame) {
    SOF = frame.SOF;
    data_length = frame.data_length;
    seq = frame.seq;
    CRC8 = frame.CRC8;
    cmd_id = frame.cmd_id;
    data = frame.data;
    CRC16 = frame.CRC16;
    frame.data = nullptr;
  }

  Frame(const Frame &frame) {
    SOF = frame.SOF;
    data_length = frame.data_length;
    seq = frame.seq;
    CRC8 = frame.CRC8;
    cmd_id = frame.cmd_id;
    data = new u_int8_t[data_length];
    for (size_t i = 0; i < data_length; i++) {
      data[i] = frame.data[i];
    }
    CRC16 = frame.CRC16;
  }

  Frame &operator=(Frame &&frame) {
    SOF = frame.SOF;
    data_length = frame.data_length;
    seq = frame.seq;
    CRC8 = frame.CRC8;
    cmd_id = frame.cmd_id;
    data = frame.data;
    CRC16 = frame.CRC16;
    frame.data = nullptr;
    return *this;
  }

  Frame &operator=(const Frame &frame) {
    SOF = frame.SOF;
    data_length = frame.data_length;
    seq = frame.seq;
    CRC8 = frame.CRC8;
    cmd_id = frame.cmd_id;
    data = new u_int8_t[data_length];
    for (size_t i = 0; i < data_length; i++) {
      data[i] = frame.data[i];
    }
    CRC16 = frame.CRC16;
    return *this;
  }

  ~Frame() {
    if (data != nullptr) {
      delete[] data;
    }
  }

public:
  /**
   * @brief 将缓冲区数据转换为帧
   *
   * @param buffer 缓冲区
   */
  void buffer_to_frame(u_int8_t *buffer) {
    if (buffer[0] != SOF) {
      std::cerr << "SOF error" << '\n';
    }
    data_length = static_cast<uint16_t>(buffer[1]) << 8 | buffer[2];
    seq = buffer[3];
    CRC8 = buffer[4];
    cmd_id = static_cast<uint16_t>(buffer[5]) << 8 | buffer[6];
    data = new u_int8_t[data_length];
    for (size_t i = 0; i < data_length; i++) {
      data[i] = buffer[7 + i];
    }
    CRC16 = static_cast<uint16_t>(buffer[7 + data_length]) << 8 |
            buffer[8 + data_length];
    // 检验CRC8和CRC16
    if (!check_crc8()) {
      std::cerr << "CRC8 error" << '\n';
    }
    if (!check_crc16()) {
      std::cerr << "CRC16 error" << '\n';
    }
  }

  /**
   * @brief 将帧转换为缓冲区数据
   *
   * @return u_int8_t* 字节数组首部指针
   */
  u_int8_t *frame_to_buffer() {
    u_int8_t *buffer = new u_int8_t[9 + data_length];
    buffer[0] = SOF;
    buffer[1] = static_cast<uint8_t>(data_length >> 8);
    buffer[2] = static_cast<uint8_t>(data_length);
    buffer[3] = seq;
    buffer[4] = CRC8;
    buffer[5] = static_cast<uint8_t>(cmd_id >> 8);
    buffer[6] = static_cast<uint8_t>(cmd_id);
    for (size_t i = 0; i < data_length; i++) {
      buffer[7 + i] = data[i];
    }
    buffer[7 + data_length] = static_cast<uint8_t>(CRC16 >> 8);
    buffer[8 + data_length] = static_cast<uint8_t>(CRC16);
    return buffer;
  }

  /**
   * @brief 重置帧
   *
   */
  void reset() {
    if (data != nullptr) {
      delete[] data;
      data = nullptr;
    }
    data_length = 0;
    seq = 0;
    cmd_id = 0;
    CRC8 = 0;
    CRC16 = 0;
  }

  void get_crc8() {}

  void get_crc16() {}

  /**
   * @brief 帧头CRC8检验
   * 
   * @return true 
   * @return false 
   */
  bool check_crc8() { return true; }

  /**
   * @brief 整包CRC16检验
   * 
   * @return true 
   * @return false 
   */
  bool check_crc16() { return true; }
};

const u_int8_t CRC8_TAB[256]{
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20,
    0xa3, 0xfd, 0x1f, 0x41, 0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
    0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc, 0x23, 0x7d, 0x9f, 0xc1,
    0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e,
    0x1d, 0x43, 0xa1, 0xff, 0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5,
    0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07, 0xdb, 0x85, 0x67, 0x39,
    0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45,
    0xc6, 0x98, 0x7a, 0x24, 0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
    0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9, 0x8c, 0xd2, 0x30, 0x6e,
    0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31,
    0xb2, 0xec, 0x0e, 0x50, 0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c,
    0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, 0x32, 0x6c, 0x8e, 0xd0,
    0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea,
    0x69, 0x37, 0xd5, 0x8b, 0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
    0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16, 0xe9, 0xb7, 0x55, 0x0b,
    0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54,
    0xd7, 0x89, 0x6b, 0x35}; // CRC8表


const u_int16_t CRC16_TAB[256]{}; // CRC16表
} // namespace rm_serial_port
