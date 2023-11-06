#include "Frame.hpp"

using namespace rm_serial_port;

int main(int argc, char const *argv[]) {
  auto frame = std::make_shared<Frame>();
  frame->data_length = 0b0000000100000001;
  frame->seq = 1;
  frame->CRC8 = 0b0001;
  frame->cmd_id = 0b0011;
  frame->data = new u_int8_t[257]{0};
  frame->CRC16 = 5;
  auto buffer = frame->frame_to_buffer();
  auto frame2 = std::make_shared<Frame>();
  frame2->buffer_to_frame(buffer);
  std::cout << "data_length: " << frame2->data_length << '\n';
  std::cout << "seq: " << static_cast<int>(frame2->seq) << '\n';
  std::cout << "CRC8: " << static_cast<int>(frame2->CRC8) << '\n';
  std::cout << "cmd_id: " << frame2->cmd_id << '\n';
  std::cout << "data: ";
  for (size_t i = 0; i < frame2->data_length; i++) {
    std::cout << static_cast<int>(frame2->data[i]) << ' ';
  }
  std::cout << '\n';
  std::cout << "CRC16: " << frame2->CRC16 << '\n';
  return 0;
}
