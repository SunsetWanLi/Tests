#pragma once

#if defined(_WIN32) || defined(_WIN64)
	#include <winsock2.h>
	#include <ws2tcpip.h> 
	#include <windows.h>
	#ifdef _MSC_VER
#ifdef WINCE
#pragma  comment(lib,"ws2.lib")
#else
#pragma comment(lib, "Ws2_32.lib" )
#pragma warning(disable : 4996)
#endif
	#endif // If you are using GCC(MinGW/Cygwin) or a compiler that doesnot support
// auto-linking, you need to fix the linker command. For example, in case
// of GCC, you have to add "-lws2_32" to the linker command. 

#else// __LINUX__
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <unistd.h>
	#include <fcntl.h>
#endif // #if defined(_WIN32) || defined(_WIN64)

#if defined(_WIN32) || defined(_WIN64)	
	typedef SOCKET socket_t ;
	#define getLastErrorWL WSAGetLastError
	#define CLOSE_SOCKET(s) ::closesocket(s)
	typedef int socklen_t;
#else // linux
	typedef int socket_t;
	int getle(); //
	#define getLastErrorWL getle
	#define SOCKET_ERROR (-1)
	#define INVALID_SOCKET (-1)
	#define CLOSE_SOCKET(s) ::close(s)	
#endif //#if defined(_WIN32) || defined(_WIN64)
