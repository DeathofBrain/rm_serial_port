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
  serial_port.run_service();
  return 0;
}
