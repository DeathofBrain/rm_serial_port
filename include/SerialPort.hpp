#include <functional>
#include <iostream>

// 3rd party include
#include <boost/asio.hpp>

// own include
#include "Frame.hpp"

namespace rm_serial_port {

constexpr auto MAX_DATA_LENGTH = std::numeric_limits<short>::max(); // 取short最大值
/**
 * @brief 串口类
 *
 */
class SerialPort {
private:
  boost::system::error_code ec;                   // 用于存储错误码
  boost::asio::io_service io;                     // io_service对象
  std::unique_ptr<boost::asio::serial_port> port; // 串口对象
  std::string port_name{"/dev/ttyACM0"};          // 串口名
  uint baud_rate{115200};                         // 波特率
  u_int8_t *receive_buffer;                       // 接收缓冲区

public:
  /**
   * @brief 初始化串口与接收缓冲区
   *
   */
  SerialPort() {
    receive_buffer = new u_int8_t[MAX_DATA_LENGTH];
    try {
      if (port == nullptr) {
        port = std::make_unique<boost::asio::serial_port>(io);
      }
      port->open(port_name, ec);
      if (ec) {
        std::cerr << "Failed to open port " << port_name << " - "
                  << ec.message() << std::endl;
      }
      port->set_option(
          boost::asio::serial_port::baud_rate(baud_rate)); // 波特率，默认115200
      port->set_option(
          boost::asio::serial_port::character_size(8)); // 数据位，默认8位
      port->set_option(boost::asio::serial_port::stop_bits(
          boost::asio::serial_port::stop_bits::one)); // 停止位，默认1位
      port->set_option(boost::asio::serial_port::parity(
          boost::asio::serial_port::parity::none)); // 校验位，默认无
      port->set_option(boost::asio::serial_port::flow_control(
          boost::asio::serial_port::flow_control::none)); // 流控，默认无
    } catch (const std::exception &e) {
      std::cerr << e.what() << '\n';
    }
  }

  /**
   * @brief 析构函数
   *
   */
  ~SerialPort() {
    delete[] receive_buffer;
    port->close();
  }

  void async_read() {
    memset(receive_buffer, 0, MAX_DATA_LENGTH);
    port->async_read_some(
        boost::asio::buffer(receive_buffer, MAX_DATA_LENGTH),
        std::bind(read_handler, std::placeholders::_1, std::placeholders::_2));
  }

  void read_handler(const boost::system::error_code &ec, std::size_t length) {
    if (ec) {
      std::cerr << "Failed to read data from port " << port_name << " - "
                << ec.message() << std::endl;
    }
    Frame frame;
    frame.buffer_to_frame(receive_buffer);
    
  }
};
} // namespace rm_serial_port
