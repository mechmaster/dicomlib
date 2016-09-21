#include "Buffer.hpp"
#include <iostream>
namespace dicom
{
/*
	According to Meyers, we can optimize this by replacing
	copy with member functions, but it's probably already
	a memcpy underneath, right?
*/
/*

*/
	Buffer& Buffer::operator >> (std::vector<BYTE>& data)
	{
		if(data.size()!=0)//Without this check, VC2005 build will crash here. -Sam 20070508
		{
			if(data.size()>(end()-position()))
				throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
			BYTE* pData=&data.front();
			std::copy(position(),position()+data.size(),pData);//would vector assign be faster?
			
			//data.assign(position(),position()+data.size());//can we profile this please?

			I_+=data.size();
		}
		return *this;
	}

	Buffer& Buffer::operator >>(std::vector<UINT16>& data)
	{
		UINT16* pData=&data.front();
		BYTE* pbData=reinterpret_cast<BYTE*>(pData);
		if((data.size()*2)>(end()-position()))
			throw ReadBeyondBuffer("Attempting to read beyond end of buffer");
		
		std::copy(position(),position()+data.size()*2,pbData);//would vector::assign() be faster here?

		I_+=data.size()*2;

		if(ExternalByteOrder_!=__BYTE_ORDER)
		{
			SwitchVectorEndian(data);
//#ifdef _WIN32
//			swab((char*)(pbData),(char*)(pbData),data.size()*2);
//#else
//			swab((void*)(pbData),(void*)(pbData),data.size()*2);
//#endif
//			std::cout << "swapped bytes, did it work?" << std::endl;
		}
		return *this;
	}
	Buffer& Buffer::operator >>(std::string& data)
	{
		for(std::string::iterator I=data.begin();I!=data.end();++I)
			(*this)>>*I;
		return *this;
	}

	Buffer& Buffer::operator >>(Tag& tag)
	{
		UINT16 Group;
		UINT16 Element;
		*this >> Group;
		*this >> Element;
		tag = makeTag(Group,Element);
		return *this;
	}


	Buffer& Buffer::operator <<(std::string data)
	{
		for(std::string::const_iterator I=data.begin();I!=data.end();++I)
			(*this)<<*I;
		return *this;
	}

	Buffer& Buffer::operator << (Tag tag)
	{
		*this << GroupTag(tag);
		*this << ElementTag(tag);
		return *this;
		
	}
	/*
		Really not at all happy about this.  Can we try using a deque?
	*/

	Buffer::iterator Buffer::position()
	{
		return begin()+I_;
	}

	void Buffer::Increment(size_type i)
	{
		I_+=i;
		if(I_>size())
			throw ReadBeyondBuffer("incremented beyond end of buffer");
	}

	void Buffer::clear()
	{
		std::vector<BYTE>::clear();
		I_=0;
	}

	void Buffer::AddVector(const std::vector<UINT16>& data)
	{
		if(__BYTE_ORDER==ExternalByteOrder_)
		{
			const BYTE* p_data=reinterpret_cast<const BYTE*> (&data[0]);
			insert(this->end(),p_data,p_data+data.size()*2);
		}
		else
		{
			std::vector<UINT16> swapped_data(data);
			SwitchVectorEndian(swapped_data);
			const BYTE* p_data=reinterpret_cast<const BYTE*> (&swapped_data[0]);
			insert(this->end(),p_data,p_data+data.size()*2);

		}
	}
}//namespace dicom
