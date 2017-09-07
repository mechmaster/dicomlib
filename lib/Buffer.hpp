#pragma once

#include <queue>
#include <vector>

#include <boost/utility.hpp>

#include "Types.hpp"
#include "Exceptions.hpp"
#include "Tag.hpp"

namespace dicom
{
  //!Buffer for data going between the library and the network.
  /*!
  RATIONALE:
  Because data is send to us across the wire in the form of a series
  of PDVs (See Part 8, Annex E), it doesn't seem to be  practical to read data
  directly from a socket onto a dataset.  An intermediatry 'holding stage' needs
  to be introduced to pull together data from a series of PDVs and make it
  ready to be fed onto a dataset.  (An alternative solution might involve something
  clever with callbacks, but I'm not really sure how.)

  This functionality is provided by the Buffer class.  I haven't spent much time
  fine - tuning this class, I suspect that there is scope for considerable improvement
  in terms of both speed and interface.

  Note that this class also has a responsibility to perform endian corrections as data
  is fed onto and off of it.  (This means that it's duplicating functionality provided
  by class Socket.  Is there a way of abstracting this out into one place?)
  -This abstraction has now mostly been done in socket/SwitchEndian.hpp

  Lots of scope for optimization in this class - in fact, I suspect that
  this is the biggest bottleneck in the library.

  Should look at the interaction between this class and Socket - it might
  be that we CAN do in-place endian switches, because once the data
  has been written to the socket we don't care about it any more, so
  corruption isn't an issue.

  We tried using std::deque to implement this, but took a big performance hit.
  vector is the only guaranteed contiguous container, which means we can
  directly pass data to socket and file functions.

  -Trevor

  I do find that we need to support in-place endian swithches. In case of reading
  p_data_tf, there is no way to predict the endian for the data dataset, until we
  check the presentation context id in the pdv item. So, I add a function to set
  the endian in-place.
  -Sam Shen Jan29,2007
  */
  //!Thrown if Read beyond buffer.

  struct ReadBeyondBuffer : public dicom::exception
  {
    ReadBeyondBuffer(std::string what = "read beyond buffer") :
      dicom::exception(what) {}

    virtual ~ReadBeyondBuffer() throw(){}
  };

  class Buffer : public std::vector<BYTE>, boost::noncopyable
  {
    /*!
    We keep track of position using this variable rather than
    an iterator, because vector iterators can get invalidated
    by insertions. I'm not very happy about this.
    */

    size_type m_I;
    int m_externalByteOrder;

  public:

    Buffer() :
      m_I(0),
      m_externalByteOrder(__LITTLE_ENDIAN)
    {
    }

    Buffer(int ExternalByteOrder) :
      m_I(0),
      m_externalByteOrder(ExternalByteOrder)
    {
    }

    void SetEndian(int endian)
    {
      m_externalByteOrder = endian;
    }

    int GetEndian()
    {
      return m_externalByteOrder;
    }

    iterator position();
    void increment(size_type i);

    Buffer& operator << (std::string data);

    template <typename T>
    Buffer& operator << (T data)
    {
      BOOST_STATIC_ASSERT(::boost::is_fundamental<T>::value);//because we're treating it as a byte stream.

      if (m_externalByteOrder != __BYTE_ORDER && sizeof(T) != 1)
      {
        data = SwitchEndian<T>(data);
      }

      BYTE* pdata = reinterpret_cast<BYTE*>(&data);
      for(BYTE* p = pdata; p < pdata + sizeof(T); p++)
      {
        push_back(*p);
      }

      return *this;
    }

    Buffer& operator << (Tag tag);

    template <typename T>
    Buffer& operator >> (T& data)
    {
      BOOST_STATIC_ASSERT(!(::boost::is_const<T>::value));//because we're writing to it.
      BOOST_STATIC_ASSERT(::boost::is_fundamental<T>::value);//because we're treating it as a byte stream.

      BYTE* pdata = reinterpret_cast<BYTE*>(&data);

      if (int(sizeof(T)) > (end() - position()))
      {
        throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
      }

      for (BYTE* p = pdata; p < pdata + sizeof(T); p++)//this is tedious to have to check.
      {
        //can we do this with a copy?
        *p = *position();
        m_I++;
      }

      if (m_externalByteOrder != __BYTE_ORDER && sizeof(T) != 1)
      {
        data = SwitchEndian<T>(data);
      }

      return *this;
    }

    Buffer& operator >> (std::vector<UINT16>& data);
    Buffer& operator >> (std::vector<BYTE>& data);
    Buffer& operator >> (std::string& data);

    Buffer& operator >> (Tag& tag);

    //!Override this to make sure we keep I_ nice.
    void clear();

    /*!
    This has now been optimized.  We could probably combine the
    two functions into a templated one if we really wanted to.
    */

    void AddVector(const std::vector<BYTE>& data)
    {
      insert(this->end(), data.begin(), data.end());
    }

    void AddVector(const std::vector<UINT16>& data);
  };
}
