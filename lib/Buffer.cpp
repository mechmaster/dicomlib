#include <iostream>

#include "Buffer.hpp"

namespace dicom
{
  ReadBeyondBuffer::ReadBeyondBuffer(std::string what) :
    dicom::exception(what)
  {
  }
  
  ReadBeyondBuffer::~ReadBeyondBuffer() throw()
  {
  }
  
  Buffer::Buffer() :
    m_currentIndex(0),
    m_externalByteOrder(__LITTLE_ENDIAN)
  {
  }
  
  Buffer::Buffer(int ExternalByteOrder) :
    m_currentIndex(0),
    m_externalByteOrder(ExternalByteOrder)
  {
  }

  void Buffer::SetEndian(int endian)
  {
    m_externalByteOrder = endian;
  }
  
  int Buffer::GetEndian()
  {
    return m_externalByteOrder;
  }

  Buffer& Buffer::operator << (std::uint8_t data)
  {
    if (sizeof(data) != 1)
    {
      return *this;
    }
    
    this->push_back(data);
    
    return *this;
  }

  Buffer& Buffer::operator << (std::uint16_t data)
  {
    if (sizeof(data) != 2)
    {
      return *this;
    }
    
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      this->push_back(static_cast<std::uint8_t>(data >> 8));
      this->push_back(static_cast<std::uint8_t>(data));
    }
    else
    {
      this->push_back(static_cast<std::uint8_t>(data));
      this->push_back(static_cast<std::uint8_t>(data >> 8));
    }

    return *this;
  }

  Buffer& Buffer::operator << (std::uint32_t data)
  {
    if (sizeof(data) != 4)
    {
      return *this;
    }

    if (m_externalByteOrder == __BYTE_ORDER)
    {
      this->push_back(static_cast<std::uint8_t>(data >> 24));
      this->push_back(static_cast<std::uint8_t>(data >> 16));
      this->push_back(static_cast<std::uint8_t>(data >> 8));
      this->push_back(static_cast<std::uint8_t>(data));
    }
    else
    {
      this->push_back(static_cast<std::uint8_t>(data));
      this->push_back(static_cast<std::uint8_t>(data >> 8));
      this->push_back(static_cast<std::uint8_t>(data >> 16));
      this->push_back(static_cast<std::uint8_t>(data >> 24));
    }

    return *this;
  }

  Buffer& Buffer::operator << (std::uint64_t data)
  {
    if (sizeof(data) != 8)
    {
      return *this;
    }
    
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      this->push_back(static_cast<std::uint8_t>(data >> 56));
      this->push_back(static_cast<std::uint8_t>(data >> 48));
      this->push_back(static_cast<std::uint8_t>(data >> 40));
      this->push_back(static_cast<std::uint8_t>(data >> 32));
      this->push_back(static_cast<std::uint8_t>(data >> 24));
      this->push_back(static_cast<std::uint8_t>(data >> 16));
      this->push_back(static_cast<std::uint8_t>(data >> 8));
      this->push_back(static_cast<std::uint8_t>(data));
    }
    else
    {
      this->push_back(static_cast<std::uint8_t>(data));
      this->push_back(static_cast<std::uint8_t>(data >> 8));
      this->push_back(static_cast<std::uint8_t>(data >> 16));
      this->push_back(static_cast<std::uint8_t>(data >> 24));
      this->push_back(static_cast<std::uint8_t>(data >> 32));
      this->push_back(static_cast<std::uint8_t>(data >> 40));
      this->push_back(static_cast<std::uint8_t>(data >> 48));
      this->push_back(static_cast<std::uint8_t>(data >> 56));
    }
    
    return *this;
  }

  Buffer& Buffer::operator << (std::int8_t data)
  {
    if (sizeof(data) != 1)
    {
      return *this;
    }
    
    this->push_back(data);
    
    return *this;
  }

  Buffer& Buffer::operator << (std::int16_t data)
  {
    if (sizeof(data) != 2)
    {
      return *this;
    }

    if (m_externalByteOrder == __BYTE_ORDER)
    {
      this->push_back(static_cast<std::uint8_t>(data >> 8));
      this->push_back(static_cast<std::uint8_t>(data));
    }
    else
    {
      this->push_back(static_cast<std::uint8_t>(data));
      this->push_back(static_cast<std::uint8_t>(data >> 8));
    }

    return *this;
  }

  Buffer& Buffer::operator << (std::int32_t data)
  {
    if (sizeof(data) != 4)
    {
      return *this;
    }
    
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      this->push_back(static_cast<std::uint8_t>(data >> 24));
      this->push_back(static_cast<std::uint8_t>(data >> 16));
      this->push_back(static_cast<std::uint8_t>(data >> 8));
      this->push_back(static_cast<std::uint8_t>(data));
    }
    else
    {
      this->push_back(static_cast<std::uint8_t>(data));
      this->push_back(static_cast<std::uint8_t>(data >> 8));
      this->push_back(static_cast<std::uint8_t>(data >> 16));
      this->push_back(static_cast<std::uint8_t>(data >> 24));
    }
    
    return *this;
  }

  Buffer& Buffer::operator << (std::int64_t data)
  {
    if (sizeof(data) != 8)
    {
      return *this;
    }
    
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      this->push_back(static_cast<std::uint8_t>(data >> 56));
      this->push_back(static_cast<std::uint8_t>(data >> 48));
      this->push_back(static_cast<std::uint8_t>(data >> 40));
      this->push_back(static_cast<std::uint8_t>(data >> 32));
      this->push_back(static_cast<std::uint8_t>(data >> 24));
      this->push_back(static_cast<std::uint8_t>(data >> 16));
      this->push_back(static_cast<std::uint8_t>(data >> 8));
      this->push_back(static_cast<std::uint8_t>(data));
    }
    else
    {
      this->push_back(static_cast<std::uint8_t>(data));
      this->push_back(static_cast<std::uint8_t>(data >> 8));
      this->push_back(static_cast<std::uint8_t>(data >> 16));
      this->push_back(static_cast<std::uint8_t>(data >> 24));
      this->push_back(static_cast<std::uint8_t>(data >> 32));
      this->push_back(static_cast<std::uint8_t>(data >> 40));
      this->push_back(static_cast<std::uint8_t>(data >> 48));
      this->push_back(static_cast<std::uint8_t>(data >> 56));
    }
    
    return *this;
  }

  Buffer& Buffer::operator << (float data)
  {
    if (sizeof(data) != 4)
    {
      return *this;
    }
    
    std::uint8_t* pData = reinterpret_cast<std::uint8_t*>(&data);
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      this->push_back(pData[0]);
      this->push_back(pData[1]);
      this->push_back(pData[2]);
      this->push_back(pData[3]);
    }
    else
    {
      this->push_back(pData[3]);
      this->push_back(pData[3]);
      this->push_back(pData[1]);
      this->push_back(pData[0]);
    }
    
    return *this;
  }

  Buffer& Buffer::operator << (double data)
  {
    if (sizeof(data) != 8)
    {
      return *this;
    }
    
    std::uint8_t* pData = reinterpret_cast<std::uint8_t*>(&data);
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      this->push_back(pData[0]);
      this->push_back(pData[1]);
      this->push_back(pData[2]);
      this->push_back(pData[3]);
      this->push_back(pData[4]);
      this->push_back(pData[5]);
      this->push_back(pData[6]);
      this->push_back(pData[7]);
    }
    else
    {
      this->push_back(pData[7]);
      this->push_back(pData[6]);
      this->push_back(pData[5]);
      this->push_back(pData[4]);
      this->push_back(pData[3]);
      this->push_back(pData[3]);
      this->push_back(pData[1]);
      this->push_back(pData[0]);
    }
    
    return *this;
  }

  Buffer& Buffer::operator >> (std::uint8_t& data)
  {
    if (sizeof(data) != 1)
    {
      return *this;
    }
    
    if (this->size() - m_currentIndex < 1)
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }

    data = this->operator[](m_currentIndex);

    m_currentIndex += 1;

    return *this;
  }

  Buffer& Buffer::operator >> (std::uint16_t& data)
  {
    if (sizeof(data) != 2)
    {
      return *this;
    }
    
    if (this->size() - m_currentIndex < 2)
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }

    data = 0;
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      data |= (this->operator[](m_currentIndex + 0) << 8);
      data |= this->operator[](m_currentIndex + 1);
    }
    else
    {
      data |= (this->operator[](m_currentIndex + 1) << 8);
      data |= this->operator[](m_currentIndex + 0);
    }
    
    m_currentIndex += 2;

    return *this;
  }

  Buffer& Buffer::operator >> (std::uint32_t& data)
  {
    if (sizeof(data) != 4)
    {
      return *this;
    }
    
    if (this->size() - m_currentIndex < 4)
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }
    
    data = 0;
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      data |= (this->operator[](m_currentIndex + 0) << 24);
      data |= (this->operator[](m_currentIndex + 1) << 16);
      data |= (this->operator[](m_currentIndex + 2) << 8);
      data |= this->operator[](m_currentIndex + 3);
    }
    else
    {
      data |= (this->operator[](m_currentIndex + 3) << 24);
      data |= (this->operator[](m_currentIndex + 2) << 16);
      data |= (this->operator[](m_currentIndex + 1) << 8);
      data |= this->operator[](m_currentIndex + 0);
    }
    
    m_currentIndex += 4;
    
    return *this;
  }

  Buffer& Buffer::operator >> (std::uint64_t& data)
  {
    if (sizeof(data) != 8)
    {
      return *this;
    }
    
    if (this->size() - m_currentIndex < 8)
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }
    
    data = 0;
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 0)) << 56);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 1)) << 48);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 2)) << 40);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 3)) << 32);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 4)) << 24);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 5)) << 16);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 6)) << 8);
      data |= static_cast<uint64_t>(this->operator[](m_currentIndex + 7));
    }
    else
    {
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 7)) << 56);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 6)) << 48);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 5)) << 40);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 4)) << 32);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 3)) << 24);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 2)) << 16);
      data |= (static_cast<uint64_t>(this->operator[](m_currentIndex + 1)) << 8);
      data |= static_cast<uint64_t>(this->operator[](m_currentIndex + 0));
    }
    
    m_currentIndex += 8;
    
    return *this;
  }

  Buffer& Buffer::operator >> (std::int8_t& data)
  {
    if (sizeof(data) != 1)
    {
      return *this;
    }
    
    if (this->size() - m_currentIndex < 1)
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }
    
    data = this->operator[](m_currentIndex);
    
    m_currentIndex += 1;
    
    return *this;
  }

  Buffer& Buffer::operator >> (std::int16_t& data)
  {
    if (sizeof(data) != 2)
    {
      return *this;
    }
    
    if (this->size() - m_currentIndex < 2)
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }
    
    data = 0;
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      data |= (this->operator[](m_currentIndex + 0) << 8);
      data |= this->operator[](m_currentIndex + 1);
    }
    else
    {
      data |= (this->operator[](m_currentIndex + 1) << 8);
      data |= this->operator[](m_currentIndex + 0);
    }
    
    m_currentIndex += 2;
    
    return *this;
  }

  Buffer& Buffer::operator >> (std::int32_t& data)
  {
    if (sizeof(data) != 4)
    {
      return *this;
    }
    
    if (this->size() - m_currentIndex < 4)
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }
    
    data = 0;
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      data |= (this->operator[](m_currentIndex + 0) << 24);
      data |= (this->operator[](m_currentIndex + 1) << 16);
      data |= (this->operator[](m_currentIndex + 2) << 8);
      data |= this->operator[](m_currentIndex + 3);
    }
    else
    {
      data |= (this->operator[](m_currentIndex + 3) << 24);
      data |= (this->operator[](m_currentIndex + 2) << 16);
      data |= (this->operator[](m_currentIndex + 1) << 8);
      data |= this->operator[](m_currentIndex + 0);
    }
    
    m_currentIndex += 4;
    
    return *this;
  }

  Buffer& Buffer::operator >> (std::int64_t& data)
  {
    if (sizeof(data) != 8)
    {
      return *this;
    }
    
    if (this->size() - m_currentIndex < 8)
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }
    
    data = 0;
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 0)) << 56);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 1)) << 48);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 2)) << 40);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 3)) << 32);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 4)) << 24);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 5)) << 16);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 6)) << 8);
      data |= static_cast<int64_t>(this->operator[](m_currentIndex + 7));
    }
    else
    {
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 7)) << 56);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 6)) << 48);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 5)) << 40);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 4)) << 32);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 3)) << 24);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 2)) << 16);
      data |= (static_cast<int64_t>(this->operator[](m_currentIndex + 1)) << 8);
      data |= static_cast<int64_t>(this->operator[](m_currentIndex + 0));
    }
    
    m_currentIndex += 8;
    
    return *this;
  }

  Buffer& Buffer::operator >> (float& data)
  {
    if (sizeof(data) != 4)
    {
      return *this;
    }
    
    if (this->size() - m_currentIndex < 4)
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }
    
    std::uint8_t* pData = reinterpret_cast<std::uint8_t*>(&data);
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      pData[0] = this->operator[](m_currentIndex + 0);
      pData[1] = this->operator[](m_currentIndex + 1);
      pData[2] = this->operator[](m_currentIndex + 2);
      pData[3] = this->operator[](m_currentIndex + 3);
    }
    else
    {
      pData[3] = this->operator[](m_currentIndex + 3);
      pData[2] = this->operator[](m_currentIndex + 2);
      pData[1] = this->operator[](m_currentIndex + 1);
      pData[0] = this->operator[](m_currentIndex + 0);
    }
    
    m_currentIndex += 4;
    
    return *this;
  }

  Buffer& Buffer::operator >> (double& data)
  {
    if (sizeof(data) != 8)
    {
      return *this;
    }
    
    if (this->size() - m_currentIndex < 8)
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }
    
    std::uint8_t* pData = reinterpret_cast<std::uint8_t*>(&data);
    if (m_externalByteOrder == __BYTE_ORDER)
    {
      pData[0] = this->operator[](m_currentIndex + 0);
      pData[1] = this->operator[](m_currentIndex + 1);
      pData[2] = this->operator[](m_currentIndex + 2);
      pData[3] = this->operator[](m_currentIndex + 3);
      pData[0] = this->operator[](m_currentIndex + 4);
      pData[1] = this->operator[](m_currentIndex + 5);
      pData[2] = this->operator[](m_currentIndex + 6);
      pData[3] = this->operator[](m_currentIndex + 7);
    }
    else
    {
      pData[3] = this->operator[](m_currentIndex + 7);
      pData[2] = this->operator[](m_currentIndex + 6);
      pData[1] = this->operator[](m_currentIndex + 5);
      pData[0] = this->operator[](m_currentIndex + 4);
      pData[3] = this->operator[](m_currentIndex + 3);
      pData[2] = this->operator[](m_currentIndex + 2);
      pData[1] = this->operator[](m_currentIndex + 1);
      pData[0] = this->operator[](m_currentIndex + 0);
    }
    
    m_currentIndex += 8;
    
    return *this;
  }
  
  /*
  According to Meyers, we can optimize this by replacing
  copy with member functions, but it's probably already
  a memcpy underneath, right?
  */
  /*

  */
  Buffer& Buffer::operator >> (std::vector<std::uint8_t>& data)
  {
    if (data.size() == 0) //Without this check, VC2005 build will crash here. -Sam 20070508
    {
      return *this;
    }

    if (data.size() > static_cast<size_t>(std::distance(position(), end())))
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }

    data.assign(position(), position() + data.size());

    m_currentIndex += data.size();
    
    if (m_externalByteOrder != __BYTE_ORDER)
    {
      std::reverse(std::begin(data), std::end(data));
    }

    return *this;
  }

  Buffer& Buffer::operator >> (std::vector<std::uint16_t>& data)
  {
    if (data.size() == 0)
    {
      return *this;
    }

    if((data.size() * 2) > static_cast<size_t>(std::distance(position(), end())))
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }
    
    data.assign(position(), position() + data.size() * 2);

    m_currentIndex += data.size() * 2;

    if (m_externalByteOrder != __BYTE_ORDER)
    {
      std::reverse(std::begin(data), std::end(data));
    }
    return *this;
  }

  Buffer& Buffer::operator >> (std::string& data)
  {
    if(data.size() > static_cast<size_t>(std::distance(position(), end())))
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }
    
    for (unsigned int i = 0; i < data.size(); ++i)
    {
      data[i] = this->operator[](m_currentIndex + i);
    }
    
    m_currentIndex += data.size();

    return *this;
  }

  Buffer& Buffer::operator >> (Tag& tag)
  {
    if(this->size() - m_currentIndex < 4)
    {
      throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
    }
    
    std::uint16_t Group = this->operator[](m_currentIndex);
    std::uint16_t Element = this->operator[](m_currentIndex + 1);

    tag = makeTag(Group, Element);
    
    m_currentIndex += 4;

    return *this;
  }


  Buffer& Buffer::operator << (std::string data)
  {
    std::size_t count = this->size();
    this->resize(count + data.size());
  
    for (unsigned int i = 0; i < data.size(); ++i)
    {
      std::size_t currentPos = count + i;
      this->operator[](currentPos) = data[i];
    }

    return *this;
  }

  Buffer& Buffer::operator << (Tag tag)
  {
    push_back(GroupTag(tag));
    push_back(ElementTag(tag));
    
    return *this;
  }

  /*
  Really not at all happy about this.  Can we try using a deque?
  */

  Buffer::iterator Buffer::position()
  {
    return begin() + m_currentIndex;
  }

  void Buffer::increment(size_type i)
  {
    m_currentIndex += i;
    if (m_currentIndex > size())
    {
      throw ReadBeyondBuffer("incremented beyond end of buffer");
    }
  }

  void Buffer::clear()
  {
    std::vector<std::uint8_t>::clear();
    m_currentIndex = 0;
  }

  void Buffer::AddVector(const std::vector<std::uint8_t>& data)
  {
    insert(this->end(), data.begin(), data.end());
  }
  
  void Buffer::AddVector(const std::vector<std::uint16_t>& data)
  {
    if (__BYTE_ORDER == m_externalByteOrder)
    {
      const std::uint8_t* p_data = reinterpret_cast<const std::uint8_t*>(&data[0]);
      insert(this->end(), p_data, p_data + data.size() * 2);
    }
    else
    {
      const std::uint8_t* p_data = reinterpret_cast<const std::uint8_t*>(&data[0]);
      insert(this->end(), p_data, p_data + data.size() * 2);
      
      std::reverse(this->begin(), this->end());
    }
  }
}//namespace dicom
