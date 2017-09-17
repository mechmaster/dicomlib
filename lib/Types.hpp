#pragma once

#include <cstdint>
#include <vector>

namespace dicom
{
  class Buffer;
  
  void SendByte(std::uint8_t byte, Buffer& dataArray);
  void RecvByte(std::uint8_t& byte, Buffer& dataArray);
}

namespace types
{
  typedef std::vector<std::uint8_t> ByteArray;
}
