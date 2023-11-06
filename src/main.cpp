/**
 * @file main.cpp
 * @brief 用于测试
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "SerialPort.hpp"

#include <thread>

int main(int argc, char const *argv[]) {
  rm_serial_port::SerialPort serial_port;
  serial_port.async_read_frame(
      [](const std::shared_ptr<rm_serial_port::Frame>& frame) {
        std::cout << "cmd_id: " << frame->cmd_id << '\n';
        std::cout << "data_length: " << frame->data_length << '\n';
        std::cout << "seq: " << frame->seq << '\n';
        std::cout << "CRC8: " << frame->CRC8 << '\n';
        std::cout << "CRC16: " << frame->CRC16 << '\n';
        std::cout << "data: ";
        for (size_t i = 0; i < frame->data_length; i++) {
          std::cout << frame->data[i] << ' ';
        }
        std::cout << '\n';
      });
  return 0;
}
