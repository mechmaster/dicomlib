/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#ifndef AARQ_INCLUDE_GUARD_7UD3489IDU83
#define AARQ_INCLUDE_GUARD_7UD3489IDU83
/*
	I'd like to pull most of these classes out into individual files,
	so I can actually find the damn things when I want them.

	Either that or put them all in DicomMessages.hpp?
*/

/*
	This file should be renamed something
	more obvious, like AssociationRequest.

	Some of the other stuff in this file may not
	belong here.
*/

/*
	I don't seem to have been consistent with my naming conventions
	-sometimes I use a trailing underscore to denote membership, but
	not always.
*/

/*
	I'm not happy that message primitive classes have default constructors.
	The procedure for reading messages is often:
	Create object with default constructor, then call member function ReadDynamic
	with a socket object as an argument.
	Why not just have constructors that take socket objects as arguments?

*/

#include "UID.hpp"
#include "Types.hpp"
#include "socket/Socket.hpp"
#include <string>

/*
* PDU Service Classes:
*	A-ASSOCIATE-RQ Class.
*/
#include <vector>
namespace dicom
{


	//!Thrown if we get fed a PDU we're not expecting.
	struct BadItemType : public dicom::exception
	{
		//!What was provided.
		BYTE Item_;
		//!What should have been provided, or 0 if unknown.
		BYTE Expected_;

		BadItemType(BYTE Item,BYTE Expected):dicom::exception("Bad Item Type")
			,Item_(Item),Expected_(Expected)
		{}
	};

	//!Throws BadItemType if Given is not equal to Expected
	void EnforceItemType(BYTE Given, BYTE Expected);


	namespace primitive
	{
	/*!
		Defined in Part 8, table 9-12
	*/
		struct	ApplicationContext
		{
			static const BYTE ItemType_ = 0x10;
			static const BYTE Reserved_=0x00;
			UID UID_;
		public:

			ApplicationContext(const UID &);
			UINT32 ReadDynamic(Network::Socket& socket);
			void Write(Network::Socket& socket)const;
			UINT32 Size()const;
		};

	
	/*!
		Defined in Part 8, table 9-14
	*/
		struct	AbstractSyntax
		{
			static const BYTE ItemType_ = 0x30;
			static const BYTE Reserved_ = 0x00;

			UID UID_;
		public:

			AbstractSyntax(const UID &);

			void		Set(const UID	&);
			void		Write(Network::Socket &);
			UINT32		Read(Network::Socket &);
			UINT32		ReadDynamic(Network::Socket	&);
			UINT32	Size();
		};

		/*
		*	What is the functional overlap between this class and the one named "TS"??
		*	This is the 'message' that gets exchanged along the wire.  'TS' is our internal
		*	representation of the Transfer Syntax concept.
		*/
		/*!
			Defined in Part 8, table 9-15
		*/
		struct TransferSyntax
		{
			static const BYTE ItemType_ = 0x40;
			static const BYTE Reserved_=0x00;

			UID UID_;

		public:

			TransferSyntax(const UID &);

			void		Set(const UID	&);
			void		Write(Network::Socket &);
 			UINT32		Read(Network::Socket &);
			UINT32		ReadDynamic(Network::Socket	&);
			UINT32		Size();

		};
		//!Identifies an implementation by unique identifier
		/*!
			Defined in Part 7, table D.3-2
		*/
		struct ImplementationClass
		{
			static const BYTE ItemType_ = 0x52;
			static const BYTE Reserved_ = 0x00;

			UID UID_;



			ImplementationClass(const UID &);
			UINT32 ReadDynamic(Network::Socket& socket);
			void Write(Network::Socket& socket);
			UINT32 Size();
		};
		//!Identifies a particuler imlementation by name
		/*!
			Documented in Part 7, Annex D.3.3.2.3, especially
			table D.3-3
		*/
		struct ImplementationVersion
		{
			static const BYTE ItemType_ = 0x55;
			static const BYTE Reserved_ = 0x00;

			std::string Name;//May be 1 to 16 characters long.
		public:

			UINT32 ReadDynamic(Network::Socket& socket);
			void Write(Network::Socket& socket);
			UINT32 Size();
		};

		/*!
			Part 7, Table D.3-9

		*/
		struct	SCPSCURoleSelect
		{
			static const BYTE ItemType_ = 0x54;
			static const BYTE Reserved_ = 0x00;
			UINT16 ItemLength;//length to end of object - probably shouldn't be a member
			UID UID_;

		public:

			BYTE		SCURole_;
			BYTE		SCPRole_;
			SCPSCURoleSelect();//is this a good idea?

			void		Write(Network::Socket &);
			UINT32		ReadDynamic(Network::Socket	&);
			UINT32		Size();

		};


		/*!
			defined in Part 8, table 9-13
		*/
		struct	PresentationContext
		{
		private:
			/*
				As far as I can tell, the C++ standard requires that if we ever take the _address_
				of a static const member (e.g. pass it by reference), then we actually need to
				instantiate it somewhere.  Practically, only gcc seems to enforce this - MSVC seems
				to ignore the requirement.
			*/

			static const BYTE				ItemType_=0x20;
			static const BYTE				Reserved1_=0x00;
			UINT16							Length_;
		public:
			BYTE							ID_;
		private:
			static const BYTE				Reserved2_=0x00;
			static const BYTE				Reserved3_=0x00;
			static const BYTE				Reserved4_=0x00;
		public:
			AbstractSyntax				AbsSyntax_;

			std::vector<TransferSyntax>	TransferSyntaxes_;
			PresentationContext();
			PresentationContext(const AbstractSyntax& ,const std::vector<TransferSyntax>&,BYTE id);
			PresentationContext(const AbstractSyntax&,BYTE id);

			//void		SetAbstractSyntax(AbstractSyntax	&);
			void		AddTransferSyntax(TransferSyntax	&);
			void		Write(Network::Socket &);
			UINT32		ReadDynamic(Network::Socket	&);
			UINT32	Size();

		};

		/*!
			Defined in Part 8, tables D.1-1 and D.1-2
		*/
		struct	MaximumSubLength
		{
		public	 :
			static const BYTE			ItemType_ = 0x51;
			static const BYTE			Reserved1_=0x00;;
			static const UINT16			Length_ = 0x04;
			UINT32						MaximumLength_;
		public:
			MaximumSubLength();
			MaximumSubLength(UINT32);

			void		Set(UINT32);
			UINT32		Get();
			void		Write(Network::Socket &);
	// 		void		Read(Network::Socket &);
			UINT32		ReadDynamic(Network::Socket	&);
			UINT32	Size();
		};


		/*!
			Defined in Part 8/table 9-16
		*/
		struct	UserInformation
		{

			static const BYTE			ItemType_ = 0x50;
			static const BYTE			Reserved_=0x00;
			//UINT16						Length_;// This we may or may not need...
		public:
			UINT32						UserInfoBaggage_;
			MaximumSubLength			MaxSubLength_;
			ImplementationClass			ImpClass_;
			ImplementationVersion		ImpVersion_;//this is an optional field. 

			//this is an optional field.  How do we indicate that?
			SCPSCURoleSelect			SCPSCURole_;
		public:
			UserInformation();

			void		SetMax(MaximumSubLength	&);
			//UINT32		GetMax();
			void		Write(Network::Socket &);
			//bool		Read(Network::Socket &);
			UINT32		ReadDynamic(Network::Socket	&);
			UINT16		Size();




		};

		//!A request to open a dicom association
		/*!
			Defined in Part 8, table 9-11
		*/
		struct	AAssociateRQ
		{

			static const BYTE		ItemType_=0x01;
			static const BYTE		Reserved1_ = 0x00;
			//UINT32	Length_;		//length to end of object.
			static const UINT16	ProtocolVersion_=0x01;
			static const UINT16	Reserved2_=0x00;;


			std::string CalledAppTitle_;
			std::string CallingAppTitle_;
			BYTE		Reserved3_[32];


			const ApplicationContext AppContext_;

			//! I think this should be called "ProposedPresentationContexts"
			std::vector<PresentationContext>	ProposedPresentationContexts_;//PresContexts;
			UserInformation				UserInfo_;

			AAssociateRQ();

			AAssociateRQ(const std::string& CallingAp, const std::string& CalledAp);


			void		SetUserInformation(UserInformation &);
			void		Write(Network::Socket &);
 			UINT32		Read(Network::Socket &);
			UINT32		ReadDynamic(Network::Socket	&);
			UINT32		Size();
		};
	}//namespace primitive
	//this should be replaceable with bind2nd and mem_fun?  or boost function objects?
	struct WriteToSocket
	{
		Network::Socket& socket_;
		WriteToSocket(Network::Socket& socket):socket_(socket){}
		template <typename T>
			void operator()(T& t)
		{
			t.Write(socket_);
		}
	};

}//namespace dicom
#endif //AARQ_INCLUDE_GUARD_7UD3489IDU83
