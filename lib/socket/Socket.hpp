#ifndef SOCKET_HPP_INCLUDE_GUARD_8753431
#define SOCKET_HPP_INCLUDE_GUARD_8753431

#if (!defined _WIN32)
	#include <unistd.h>
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <arpa/inet.h>
#else//_WIN32
	#include <winsock2.h>
#endif//_WIN32

#include <string.h>

#include "SystemError.hpp"
#include <string>
#include <vector>
#include <algorithm>

#include <boost/utility.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include "EnablesWinSock.hpp"
#include "Base.hpp"
#include "SwitchEndian.hpp"

#if defined (_WIN32)
	typedef char* RECV_DATA_TYPE;

	#ifndef socklen_t //python libraries define this, grrr.
		typedef int socklen_t;
	#endif
#else
	typedef void* RECV_DATA_TYPE;
	typedef int SOCKET;
#endif
typedef  RECV_DATA_TYPE SEND_DATA_TYPE;


//!Contains all of our socket classes.
namespace Network
{
	//!Calls WSAGetLastError on windows, errno on unix.
	inline int GetLastError()
	{
#ifdef _WIN32
		return WSAGetLastError();
#else
		return errno;//Is thread safe, I think.
#endif
	}

	//!Thrown when reads (recv()) fail.
	class ConnectionLost : public std::exception
	{
		std::string Description_;
	public:
		ConnectionLost(std::string Description="Connection Lost") : Description_(Description){}
		const char* what() const throw()
		{
			return Description_.c_str();
		}
		virtual ~ConnectionLost() throw(){}
	};


	//!Wrapper for sockaddr_in structure
	/*!
		Takes host and port as constructor arguments.
	*/
	struct SocketAddress
	{
		//!A remote address.
		SocketAddress(std::string host,short port)
		{
			// get the host info
			hostent* he = gethostbyname(host.c_str());
			
			if(!he)
				throw SystemError("GetHostByName");

			address_.sin_family = AF_INET;			// host byte order
			address_.sin_port = htons(port);		// short, network byte order
			address_.sin_addr = *((in_addr*)he->h_addr);
			memset(&(address_.sin_zero), '\0', 8);  // zero the rest of the struct
		}

		//!Use local hostname and provided port
		SocketAddress(short Port)
		{
			address_.sin_family = AF_INET;
			address_.sin_port=htons(Port);
			address_.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
			memset(&(address_.sin_zero), '\0', 8); // zero the rest of the struct
		}


		//!underlying sockaddr_in structure.
		sockaddr_in address_;

		//!Conversion operator
		operator sockaddr_in()
		{
			return address_;
		}
	};


#ifdef _WIN32

	//!	Windows doesn't support MSG_WAITALL, so we need to do a bit more work manually

		/*!
		Windows doesn't MSG_WAITALL, so recv may return before all the
		data we asked for has been received.  To work around this, we 
		loop until we've got all the data.
	*/
	inline int WindowsSafeRecv(SOCKET s,RECV_DATA_TYPE data,int BytesToRead)
	{
		int BytesLeftToRead=BytesToRead;
		RECV_DATA_TYPE CurrentPointInData=data;
		while(BytesLeftToRead!=0)
		{
			int BytesRead=recv(s,CurrentPointInData,BytesLeftToRead,0);
			if(BytesRead==0)
				throw ConnectionLost();
			if(BytesRead==SOCKET_ERROR)
				throw SystemError("recv",Network::GetLastError());
			CurrentPointInData+=BytesRead;
			BytesLeftToRead-=BytesRead;
		}
		return BytesToRead;

	}


	//Force MSVC 7.0 to instantiate some templated functions
	//This is a workaround to a known bug, see http://groups.google.ca/groups?hl=en&lr=&ie=UTF-8&oe=UTF-8&threadm=4ac23acc.0301190831.34470124%40posting.google.com&rnum=20&prev=/groups%3Fq%3Dlnk1120%2Btemplate%2Bfunction%26hl%3Den%26lr%3D%26ie%3DUTF-8%26oe%3DUTF-8%26start%3D10%26sa%3DN

	inline bool InstantiateTemplatedFunctions()
	{
		unsigned char c(0x00);
		c = SwitchEndian<unsigned char>(c);

		return (c==0xff);
	}



#endif

	//!Base socket class.
	/*!
		Socket is an Abstract Base Class, in that it requires derived
		classes to provide an implementation of
			SOCKET GetSocketDescriptor()
		<br>
		Socket

	*/
	class Socket : public boost::noncopyable//, public CanRead, public CanSend
#ifdef _WIN32
		,public EnablesWindowsSockets
#endif
	{
	public:

		Socket(int ExternalEndian=__BIG_ENDIAN):ExternalByteOrder_(ExternalEndian){}

		const int ExternalByteOrder_;//will generally be BIG_ENDIAN, but in some dicom cases will be LITTLE_ENDIAN

		virtual const SOCKET GetSocketDescriptor() const =0;
		virtual const std::string get_remote_ip() const =0;


		//!Virtual destructor because we expect to have derived classes.
		virtual ~Socket(){}

		//!Read 'count' elements of type T onto provided array.
		/*!
			T must be a fundamental type, and non-const.
			This function will reverse endian-ness of the
			incoming data if needed.
		*/
		template<typename T>
		void Readn(T* Begin, size_t count)
		{

			BOOST_STATIC_ASSERT(::boost::is_fundamental<T>::value);
			BOOST_STATIC_ASSERT(!::boost::is_const<T>::value);

			int BytesToRead=int(count*sizeof(T));

			//Read data from socket
#ifdef __unix
			int BytesRead=recv(GetSocketDescriptor(),(RECV_DATA_TYPE)Begin,BytesToRead,MSG_WAITALL);
#else
			int BytesRead=WindowsSafeRecv(GetSocketDescriptor(),(RECV_DATA_TYPE)Begin,BytesToRead);
#endif
			//fix endian-ness - this is very slow...
			if(ExternalByteOrder_!=__BYTE_ORDER && sizeof(T)>1)
				std::transform(Begin,Begin+count,Begin,SwitchEndian<T>);

			//Check for errors.
			if(BytesRead==0)
				throw ConnectionLost();
			if(BytesRead!=BytesToRead)
				throw SystemError("Read Error",Network::GetLastError());
		}

		//!Iterator style interface.
		/*!
			With this one can write
			int i[size];
			socket.Read(i, i+size);
		*/
		template<typename T>
		void Read(T* Begin, T* End)
		{
			Readn(Begin,End-Begin);
		}

		//!Read data.size() elements from socket onto data
		/*!
			Note that 'data' must already be set to the required length.
		*/
		template <typename T>
		void Read(std::vector<T>& data)
		{
			BOOST_STATIC_ASSERT(::boost::is_fundamental<T>::value);

			//assume underlying data is contiguous - see
			//http://www.parashift.com/c++-faq-lite/containers-and-templates.html#faq-34.3
			//It is acceptable to treat the underlying data as an array.

			T* pData=&(data.front());

			Readn(pData,data.size());
		}

		//!Read string from socket
		/*!
			data must already have desired length!

		*/
		void Read(std::string& data)
		{
			std::vector<char> v(data.size());
			Read(v);
			std::copy(v.begin(),v.end(),data.begin());
		}

		//!	Is there data available to read on this socket?
		/*!
			Is there data available to read on this socket?
			Wait no longer than BlockFor seconds to find out.
			(Allows a framework to idle if nothing is to be done)
		*/

		bool MoreData(int BlockFor=0)const
		{
			fd_set rfds;
			timeval tv;

			FD_ZERO(&rfds);
			FD_SET(GetSocketDescriptor(), &rfds);

			tv.tv_sec = BlockFor;
			tv.tv_usec = 0;
			SOCKET retval = select(int(GetSocketDescriptor()+1), &rfds, NULL, NULL, &tv);
			if(retval==-1)
				throw SystemError("select.");
			return (retval!=0);
		}




		//!Read one unit of type T from socket.
		template <typename T>
		const Socket& operator >> (T& data)
		{
			Readn(&data,1);
			return *this;
		}

		//!Read
		template <typename T>
		const Socket& operator >> (std::vector<T>& data)
		{
			Read(data);
			return *this;
		}

	private:
		//!Assume endian issues already handled..
		template <typename T>
		void Sendn_AlreadySwapped(const T* Begin,size_t count) const
		{
			BOOST_STATIC_ASSERT(::boost::is_fundamental<T>::value);
			int BytesToSend=int(count*sizeof(T));

			int BytesSent = ::send(GetSocketDescriptor(),
				(SEND_DATA_TYPE)Begin,	BytesToSend,	0);

			if(BytesSent!=BytesToSend)
				throw SystemError("send",GetLastError());

		}
	public:
		template <typename T>
		void Sendn(const T* Begin, size_t count) const
		{
			BOOST_STATIC_ASSERT(::boost::is_fundamental<T>::value);

			if(ExternalByteOrder_==__BYTE_ORDER || sizeof(T)==1)
			{
				Sendn_AlreadySwapped(Begin,count);
			}
			else
			{
				/*
					This is another point for potential optimization!
					Doing heap allocations can probably be avoided in many
					cases.
				*/

				T* data_to_send=new T[count];
				std::transform(Begin,Begin+count,data_to_send,SwitchEndian<T>);
				Sendn_AlreadySwapped(data_to_send,count);
				delete[] data_to_send;
			}
		}

		//!Iterator - style interface.
		/*!
			Not sure this is a great idea, it gives the impression
			that the user can use any iterator type, which is not
			the case.
		*/
		template <typename T>
		void Send(T* Begin, T* End)
		{
			Sendn(Begin,(End-Begin));
		}
		template <typename T>
		void Send(const T& data)const
		{
			Sendn(&data,1);
		}

		template <typename T>
		const Socket& operator <<(const T& data)const
		{
			Send(data);
			return *this;
		}

		void Send(const std::string& data) const
		{
			Sendn(data.c_str(),data.size());
		}
		//!
		/*!
			Note that this assumes that we can treat the raw data
			in a vector as being a contiguous array!(See comment on CanRead::Read()).

			I think we can optimize this somewhat.
		*/
		const Socket& operator << (const std::vector<unsigned char>& data) const
		{
			Sendn_AlreadySwapped(&data[0],data.size());
			return *this;
		}

		const Socket& operator << (const std::vector<unsigned short>& data) const
		{
			if(ExternalByteOrder_==__BYTE_ORDER )
				Sendn_AlreadySwapped(&data[0],data.size());
			else
			{
				std::vector<unsigned short> swapped_data(data);	//hopefully this is fast...
				SwitchVectorEndian(swapped_data);				//optimized
				Sendn_AlreadySwapped(&swapped_data[0],data.size());
			}
		}
// 		template <typename T>
// 		const Socket& operator << (const std::vector<T>& data)const
// 		{
// 			const T* pData=&(data.front());//Get a pointer to the raw data.
// 			if(ExternalByteOrder_==__BYTE_ORDER or sizeof(T)==1)
// 				Sendn_AlreadySwapped(pData,data.size();
// 			else
// 			{
// 				std::vector<T>&
// 				//need to switch endian...
// 			}
// 			Sendn(pData,data.size());
// 			return *this;
// 		}


	};


	//!Represents a valid, OS-assigned socket.
	/*!
	Encapsulates a call to ::socket(), i.e. is guaranteed to represent
	a valid socket descriptor.
	*/
	class AssignedSocket:public Socket
	{
		const SOCKET socket_descriptor_;
	public:
		virtual const SOCKET GetSocketDescriptor()const
		{
			return socket_descriptor_;
		}

		AssignedSocket():socket_descriptor_ ( socket ( AF_INET, SOCK_STREAM,0 ))
		{
			if(socket_descriptor_<0)
				throw SystemError("Couldn't allocate socket",GetLastError());
		}
		virtual const std::string get_remote_ip()const
		{
			return std::string("");
		}

		virtual ~AssignedSocket()
		{
#if defined(_WIN32)
			closesocket(socket_descriptor_);
#else
			close(socket_descriptor_);
#endif
		}



	};

	//!A socket that has succesfully connected to a server.
	/*!
	client socket uses connect, server socket uses bind.
	Further extends socket concept to provide a CONNECTED socket,
	for use in client applications.
	*/
	class ClientSocket : public AssignedSocket
	{
	public:
		ClientSocket(std::string host, short port)
			:AssignedSocket()
		{
			SocketAddress address(host,port);
			int Error=connect(GetSocketDescriptor(),(sockaddr *)&(address.address_),sizeof(sockaddr));
			if (Error)
				throw SystemError("Connect error",GetLastError());
		}



	};

	const int ListenBacklog(10);


	//!	a bound, listening socket.

	class ServerSocket : public AssignedSocket
	{

	public:
		ServerSocket(short Port)
			:AssignedSocket()
		{
			SocketAddress address(Port);

			/*
			Unix Network Programming (Richards) section 7.5 says we should ALWAYS set the SO_REUSEADDR
			option.  I'm not sure we need to if we're using threads rather than fork(), but let's play
			it safe.  This post http://mail.python.org/pipermail/patches/2004-June/015140.html implies
            that we should use SO_EXCLUSIVEADDRUSE in the same situation on windows.  If anyone has
            a better idea, please let me know!
			*/
			int on = 1;

#ifdef _WIN32
			if (setsockopt(GetSocketDescriptor(), SOL_SOCKET, SO_EXCLUSIVEADDRUSE, ( const char* ) &on, sizeof ( on ) ) == -1 )
				throw SystemError("Couldn't set socket option");
#else
			if (setsockopt(GetSocketDescriptor(), SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
				throw SystemError("Couldn't set socket option");
#endif 

			int status=bind(GetSocketDescriptor(),(sockaddr*)&(address.address_),sizeof(sockaddr));
			if(status)
				throw SystemError("Bind error.");

			status=listen(GetSocketDescriptor(),ListenBacklog);//backlog should be changable at runtime for tuning?
			if(status)
				throw SystemError("Listen error.");
		}
	};


	//!An accepted connection.
	/*!
		Constructor calls accept() on a valid
		ServerSocket object.

		I'm not entirely happy about this yet.
	*/
	class AcceptedSocket : public Socket
	{
		SOCKET socket_descriptor_;
		sockaddr_in remote_addr_;
	public:
		virtual const SOCKET GetSocketDescriptor()const 
		{
			return socket_descriptor_;
		}


		/*!
		This constructor contains a blocking call to accept()
		*/

		AcceptedSocket(ServerSocket& server)
		{
			//socket_descriptor_ = accept(server.GetSocketDescriptor(),(sockaddr*)NULL,0);
			socklen_t len=sizeof(remote_addr_);
			socket_descriptor_ = accept(server.GetSocketDescriptor(),(sockaddr*)&remote_addr_,&len);
			if(socket_descriptor_<=0)
				throw SystemError("Accept error.");
		}
		virtual const std::string get_remote_ip()const
		{
			return std::string(inet_ntoa(remote_addr_.sin_addr));
		}
		virtual ~AcceptedSocket()
		{
#if defined(_WIN32)
			closesocket(socket_descriptor_);
#else
			close(socket_descriptor_);
#endif
		}

	};




}//namespace Network

#endif //SOCKET_HPP_INCLUDE_GUARD_8753431
