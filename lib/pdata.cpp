#include "pdata.hpp"
#include <iostream>
#include <assert.h>
#include "Decoder.hpp"
#include "iso646.h"
//#include "Profiling.hpp"




/*
	This is seriously ugly, and quite misleading.  We definitely need some clean up work here!

*/
namespace dicom
{

	/*
		I think the object model presented in this file
		is sub-optimal.
	*/

	/*!
		defined in Part8, table 9-23
	*/
	class	PDV//presentation data value item
	{
	public:
		UINT32	Length;
		BYTE	PresentationContextID;
		BYTE	MessageHeader;
		//defined in Part 8, Annex E.2, see figure E.2-1

		bool IsCmdMessage(){return (MessageHeader bitand MessageControlHeader::COMMAND);}
		bool IsDataMessage(){return not(MessageHeader bitand MessageControlHeader::COMMAND);}
		bool IsLastFragment(){return ((MessageHeader bitand MessageControlHeader::LAST_FRAGMENT)!=0);}
		//UINT32 ReadDynamic(Network::Socket& socket);
	};

	/*!
		documented in Part 8, section 7.6,  figure  9-2 and tables 9-22 and 9-23
	*/

 	bool	PDataTF::ReadDynamic(Network::Socket& socket)
 	{
//		scoped_profiler prof("PDataTF::ReadDynamic") ;
 		UINT32		Count;

 		if(!Length)//why would this ever not be the case?
 		{
			BYTE Reserved_dummy;
 			socket >> Reserved_dummy;//shouldn't this fail?
 			socket >> Length;
 		}

 		Count = Length;
 		MsgStatus = 0;	// continue
		PDV pdv;//should be inside following loop, i think
 		while ( Count > 0)
 		{

 			//I think that this should happen in a member of PDV-.
 			socket >> pdv.Length;
 			socket >> pdv.PresentationContextID;
 			socket >> pdv.MessageHeader;


			//now read actual data from socket onto buffer.
			//std::vector<BYTE> data(pdv.Length-2);
			//socket >> data;
			//std::copy(data.begin(),data.end(),std::back_inserter(buffer_));
			buffer_.insert(buffer_.end(),pdv.Length-2,0x00);
			socket.Readn(&*(buffer_.end()-(pdv.Length-2)),(pdv.Length-2));//slightly more complicated.


			/*
				The previous 3 lines could be dramatically speeded up if
				we _first_ allocate the space on buffer, then pass _pointers_
				to the begin and end of the new space to Socket::Readn();

				I'm not going to change this till I have a good unit test for the change

				Meyers says in 'Efficient STL' that member functions are preferable
				over algorithms for efficiency?

				So possible optimizations would be:
				1)

				buffer_.insert(buffer_.end(),data.begin(),data.end());//this is simple.

				2)

				buffer_.insert(buffer_.end(),pdv.Length-2,0x00);
				socket.Readn(buffer_.end()-(pdv.Length-2),(pdv.Length-2));//slightly more complicated.

				We need profiling tests for both, such as dicomtest::SubmitLotsOfImages()
			*/

 			Count = Count - pdv.Length - sizeof(UINT32);
 			Length = Length - pdv.Length - sizeof(UINT32);

			if(pdv.IsLastFragment())
 			{
 				MsgStatus = 1;
 				PresentationContextID = pdv.PresentationContextID;
 				return ( true );
 			}
 		}

		if(pdv.IsLastFragment())

		{
			assert(0);//how can this ever happen?
			MsgStatus = 1;
		}

 		PresentationContextID = pdv.PresentationContextID;

 		return ( true );
 	}

	PDataTF::PDataTF(int ByteOrder)
	:buffer_(ByteOrder)
	,Length(0)

	{
	}

}//namespace dicom
