#include <functional>
#include <iostream>

// 3rd party include
#include <boost/asio.hpp>

// own include
#include "Frame.hpp"

namespace rm_serial_port {

constexpr auto MAX_BUFFER_LENGTH =
    std::numeric_limits<u_int16_t>::max(); // 最大缓冲区长度
/**
 * @brief 串口类
 *
 */
class SerialPort {
private:
  boost::system::error_code ec_;                   // 用于存储错误码
  boost::asio::io_service io_;                     // io_service对象
  std::unique_ptr<boost::asio::serial_port> port_; // 串口对象
  std::string port_name_;                          // 串口名
  uint baud_rate_{115200};                         // 波特率
  u_int8_t *receive_buffer_;                       // 接收缓冲区
  bool async_srv_is_running{false}; // 串口异步服务是否正在运行
public:
  /**
   * @brief 初始化串口
   *
   * @param port_name 串口名
   * @param baud_rate 波特率
   */
  SerialPort(std::string port_name, u_int baud_rate)
      : port_name_(port_name), baud_rate_(baud_rate) {
    receive_buffer_ = new u_int8_t[MAX_BUFFER_LENGTH];
    try {
      if (port_ == nullptr) {
        port_ = std::make_unique<boost::asio::serial_port>(io_);
      }
      port_->open(port_name, ec_);
      if (ec_) {
        std::cerr << "Failed to open port " << port_name << " - "
                  << ec_.message() << std::endl;
      }
      port_->set_option(boost::asio::serial_port::baud_rate(
          baud_rate_)); // 波特率，默认115200
      port_->set_option(
          boost::asio::serial_port::character_size(8)); // 数据位，默认8位
      port_->set_option(boost::asio::serial_port::stop_bits(
          boost::asio::serial_port::stop_bits::one)); // 停止位，默认1位
      port_->set_option(boost::asio::serial_port::parity(
          boost::asio::serial_port::parity::none)); // 校验位，默认无
      port_->set_option(boost::asio::serial_port::flow_control(
          boost::asio::serial_port::flow_control::none)); // 流控，默认无
    } catch (const std::exception &e) {
      std::cerr << e.what() << '\n';
    }
  }

  /**
   * @brief 初始化串口，使用默认串口名（C板USB串口）, 波特率115200
   *
   */
  SerialPort() : SerialPort("/dev/ttyACM0", 115200) {}

  /**
   * @brief 析构函数
   *
   */
  ~SerialPort() {
    if (async_srv_is_running) {
      io_.stop();
    }
    delete[] receive_buffer_;
    port_->close();
  }

  /**
   * @brief 启动串口异步服务，保持异步通信
   *
   */
  void run_async_service() {
    async_srv_is_running = true;
    std::thread{[this] {
      auto work = boost::asio::io_service::work(io_);
      io_.run();
    }}.detach();
  }

  /**
   * @brief 同步方式读取数据
   *
   * @return std::shared_ptr<Frame> 帧指针
   */
  std::shared_ptr<Frame> sync_read_frame() {
    memset(receive_buffer_, 0, MAX_BUFFER_LENGTH);
    port_->read_some(boost::asio::buffer(receive_buffer_, MAX_BUFFER_LENGTH));
    auto frame = std::make_shared<Frame>();
    frame->buffer_to_frame(receive_buffer_);
    return frame;
  }

  /**
   * @brief 异步方式读取数据
   *
   * @param callback 回调函数，负责处理帧数据
   */
  void async_read_frame(
      const std::function<void(const std::shared_ptr<Frame> &)> &callback) {
    memset(receive_buffer_, 0, MAX_BUFFER_LENGTH);
    // 如果串口异步服务未启动，则启动
    if (!async_srv_is_running) {
      run_async_service();
    }
    port_->async_read_some(
        boost::asio::buffer(receive_buffer_, MAX_BUFFER_LENGTH),
        [&](const boost::system::error_code &ec, std::size_t length) {
          if (ec) {
            std::cerr << "Failed to read data from port " << port_name_ << " - "
                      << ec.message() << std::endl;
          }
          auto frame = std::make_shared<Frame>();
          frame->buffer_to_frame(receive_buffer_);
          callback(frame);
        });
  }

  /**
   * @brief 同步方式发送数据
   *
   * @param frame 帧指针
   */
  void sync_write_frame(const std::shared_ptr<Frame> &frame) {
    auto buffer = frame->frame_to_buffer();
    port_->write_some(boost::asio::buffer(buffer, 9 + frame->data_length));
    delete[] buffer;
  }

  /**
   * @brief 异步方式发送数据
   *
   * @param frame 帧指针
   */
  void async_write_frame(const std::shared_ptr<Frame> &frame,
                         const std::function<void()> &callback) {
    auto buffer = frame->frame_to_buffer();
    // 如果串口异步服务未启动，则启动
    if (!async_srv_is_running) {
      run_async_service();
    }
    port_->async_write_some(
        boost::asio::buffer(buffer, 9 + frame->data_length),
        [&](const boost::system::error_code &ec, std::size_t length) {
          if (ec) {
            std::cerr << "Failed to write data to port " << port_name_ << " - "
                      << ec.message() << std::endl;
          }
          callback();
        });
    delete[] buffer;
  }
};
} // namespace rm_serial_port
