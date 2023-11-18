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

using namespace rm_serial_port;

int main(int argc, char const *argv[]) {
  // rm_serial_port::SerialPort serial_port;
  auto serial_port = std::make_shared<rm_serial_port::SerialPort>();
  // for (;;) {
  //   // serial_port->async_read_frame(
  //   //   [](const std::shared_ptr<rm_serial_port::Frame>& frame) {
  //   //     std::cout << "cmd_id: " << frame->cmd_id << '\n';
  //   //     std::cout << "data_length: " << frame->data_length << '\n';
  //   //     std::cout << "seq: " << (int)frame->seq << '\n';
  //   //     std::cout << "CRC8: " << (int)frame->CRC8 << '\n';
  //   //     std::cout << "CRC16: " << (int)frame->CRC16 << '\n';
  //   //     std::cout << "data: ";
  //   //     for (size_t i = 0; i < frame->data_length; i++) {
  //   //       std::cout << frame->data[i] << ' ';
  //   //     }
  //   //     std::cout << '\n';
  //   //   });
  //   auto frame = serial_port->sync_read_frame();
  //   std::cout << "cmd_id: " << frame->cmd_id << '\n';
  //   std::cout << "data_length: " << frame->data_length << '\n';
  //   std::cout << "seq: " << (int)frame->seq << '\n';
  //   std::cout << "CRC8: " << (int)frame->CRC8 << '\n';
  //   std::cout << "CRC16: " << (int)frame->CRC16 << '\n';
  //   std::cout << "data: ";
  //   for (size_t i = 0; i < frame->data_length; i++) {
  //     std::cout << frame->data[i] << ' ';
  //   }
  //   std::cout << '\n';
  // }

  auto frame = std::make_shared<Frame>();
  u_int8_t* data = new u_int8_t[2];
  data[0] = 0x01;
  data[1] = 0x02;
  frame->data = data;
  frame->data_length = 2;
  frame->cmd_id = 0x0001;
  frame->seq = 0x01;
  frame->CRC8 = 0x01;
  frame->CRC16 = 0x01;
  serial_port->sync_write_frame(frame);
  return 0;
}
