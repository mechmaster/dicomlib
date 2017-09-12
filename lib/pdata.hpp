#ifndef P_DATA_HPP_INCLUDE_GUARD_4520084566
#define P_DATA_HPP_INCLUDE_GUARD_4520084566
/****************************************************************************
 *
 * Abstract Base Class for P-DATA Service
 *
 *
 ****************************************************************************/

#include "Types.hpp"
#include "DataSet.hpp"

/*

	We could redesign the following two classes to mirror the standard better
	by having PDataTF HAVE A list<> of PDVs.  I think we can avoid copying-related
	performance issues if this is THE place where the associated DataSets reside, and
	they are passed to other classes by reference.  After all, I don't see why a
	Command or Data message would ever be changed after it had been received.
*/
namespace dicom
{


	namespace MessageControlHeader
	{
		typedef BYTE Code;
		const Code	DATASET			= 0x00,
					COMMAND 		= 0x01,
					LAST_FRAGMENT	= 0x02;
	}



	/*!
		defined in Part8, table 9-22
		Further documentation in Part 8, Annex E

		I think this class needs to be seriously re-thought, or even removed altogether,
		as it's implementation is very misleading.  This class does _not_ corresponding
		exactly to the P_DATA_TF request primitive, rather it serves to manage a _stream_
		of them!
		
		We need to know the maximum length specified during association negotiation.

		
	*/
	class	PDataTF
	{
	public:

		Buffer buffer_;
		const static BYTE			ItemType = 0x04;
		const static BYTE			Reserved1 = 0x00;


		UINT32			Length;

		int		MsgStatus;
		BYTE	PresentationContextID;
		BYTE	MessageHeader;

		bool	ReadDynamic(Network::Socket& socket);

				PDataTF(int ByteOrder);
	};
}//namespace dicom

#endif //P_DATA_HPP_INCLUDE_GUARD_4520084566
