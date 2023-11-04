#include <iostream>

// 3rd party include
#include <boost/asio.hpp>

// own include
#include "Frame.hpp"

class SerialPort
{
private:
    boost::system::error_code ec;
    boost::asio::io_service io;
    boost::asio::serial_port port;
    Frame frame;
public:
    SerialPort(/* args */);
    ~SerialPort();
};