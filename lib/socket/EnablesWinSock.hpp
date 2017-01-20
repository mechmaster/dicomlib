#ifndef ENABLESWINSOCK_HPP_INCLUDE_GUARD_1196542978689
#define ENABLESWINSOCK_HPP_INCLUDE_GUARD_1196542978689
#include "Base.hpp"
#include "SystemError.hpp"
#ifdef _WIN32

namespace Network
{
	//!Automatically calls WSAStartup and WSACleanup as needed.
	/*!
		(Windows Specific)
			A class that inherits from this class can rely on the fact
			WSAStartup and WSACleanup will be called at the appropriate
			time.  (WSAStartup is called on the first instantiation of
			the class, WSACleanup called on the last destructor.)

		Note that this currently IS NOT thread-safe, and needs to be made
		so.  

		A better version would probably maintain a static member object that
		called WSAStartup at construction and WSACleanup at destruction.
	*/
	class EnablesWindowsSockets
	{
		//!Keeps tally of how many times this class has been instantiated.
		/*!
			I don't think this function is thread safe, unless
			we know the creation of an integer is an atomic operation.
		*/
		int& UsageCount()
		{
			static int UsageCount(0);
			return UsageCount;
		}

		//!Wraps call to WSAStartup
		void StartWinSock() throw(SystemError)		
		{
			WORD VersionNeeded = 0x0101;
			static	WSADATA	wsaData;
			if(0!=WSAStartup( VersionNeeded, &wsaData))
				throw SystemError("Couldn't start WinSock");
		}
	public:
		
		EnablesWindowsSockets()
		{
			if(UsageCount()==0)
				StartWinSock();
			UsageCount()++;
		}

		~EnablesWindowsSockets()
		{
			UsageCount()--;
			if(UsageCount()==0)
				WSACleanup();
		}
	
	};
}//namespace Network
#endif//_WIN32
#endif//ENABLESWINSOCK_HPP_INCLUDE_GUARD_1196542978689
