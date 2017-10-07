#include "Buffer.hpp"
#include "Types.hpp"

namespace dicom
{
  void SendByte(std::uint8_t byte, Buffer& dataArray)
  {
    dataArray << byte;
  }
  
  void RecvByte(std::uint8_t& byte, Buffer& dataArray)
  {
    dataArray >> byte;
  }
}//namespace dicom
