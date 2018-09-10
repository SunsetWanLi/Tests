//
//  MGSocket.h
//  System
//
//  Created by DanYuan Hu on 12-03-31.
//  Copyright (c) 2011Äê Zhejiang University. All rights reserved.
//

#pragma once
#ifndef _MGSOCKET_H_
#define _MGSOCKET_H_
#include <string>
#include "../../CommonBase/MGObject.h"
#include "../../CommonBase/CommonDef.h"
#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <windows.h>
typedef int				socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
typedef int				SOCKET;
//#pragma region define win32 const variable in linux
#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
//#pragma endregion
#endif

using namespace std;

namespace z11 {
	enum
	{
		MGSOCK_TYPE_RESERVED_0=0,	/* reserved */
		MGSOCK_STREAM,				/* stream socket */
		MGSOCK_DGRAM,				/* datagram socket */
		MGSOCK_RAW,					/* raw-protocol interface */
		MGSOCK_RDM,					/* reliably-delivered message */
		MGSOCK_SEQPACKET,			/* sequenced packet stream */
	};
	typedef int_32 MGSocketType;

	enum
	{
		MGSOCK_AF_UNSPEC=0,			/* unspecified */
		MGSOCK_AF_UNIX,				/* local to host (pipes, portals) */
		MGSOCK_AF_INET,				/* internetwork: UDP, TCP, etc. */
		MGSOCK_AF_IMPLINK,			/* arpanet imp addresses */
		MGSOCK_AF_PUP,				/* pup protocols: e.g. BSP */
		MGSOCK_AF_CHAOS,			/* sequenced packet stream */
		MGSOCK_AF_IPX,				/* IPX and SPX */
	};
	typedef int_32 MGSocketAddressFamily;

	enum
	{
		MGSOCK_IPPROTO_IP=0,			/* dummy for IP */
		MGSOCK_IPPROTO_ICMP,			/* control message protocol */
		MGSOCK_IPPROTO_IGMP,			/* group management protocol */
		MGSOCK_IPPROTO_GGP,				/* gateway^2 (deprecated) */
		MGSOCK_IPPROTO_TCP=6,			/* tcp */
		MGSOCK_IPPROTO_PUP=12,			/* pup */
		MGSOCK_IPPROTO_UDP=17,			/* user datagram protocol */
		MGSOCK_IPPROTO_IDP=22,			/* xns idp */
		MGSOCK_IPPROTO_ND=77,			/* UNOFFICIAL net disk proto */

		MGSOCK_IPPROTO_RAW=255,			/* raw IP packet */
		MGSOCK_IPPROTO_MAX=256,			
	};
	typedef int_32 MGSocketAddressFamily;

	class MGSocket : public MGObject{
	public:
		MGSocket(SOCKET sock = INVALID_SOCKET);
		virtual ~MGSocket();
		// Create socket object for snd/recv data
		bool create(int_32 af = MGSOCK_AF_INET, int_32 type = MGSOCK_STREAM, int_32 protocol = MGSOCK_IPPROTO_TCP);
		// Connect socket
		bool connectTo(const char* ip, unsigned short port, uint_32 overtime = 10000);

        // Set Socket Option
        int	setOption(int level, int optname, const char *optval, int optlen);
		// Bind socket
		bool bindOn(unsigned short port);
		// Listen socket
		bool listenOn(int backlog = 5); 
		// Accept socket
		bool acceptFor(MGSocket& s, string& fromip, uint_32& port);
        // Wait socket
        bool waitFor(int timeout);

		// Send socket
		int_64 sendData(const char* buf, int len, int flags = 0);
		// Recv socket
		int_64 recvData(char* buf, int len, int flags = 0);
        // Send socket
        int_64 sendDataTo(const char* buf, int len, const string& ip, const unsigned int port, int flags = 0);
        // Recv socket
        int_64 recvDataFrom(char* buf, int len, string& ip, unsigned int& port, int flags = 0);

        // Shut Down socket
        int	shutdownSocket(int how = 0x02);
		// Close socket
		int closeSocket();
		// Get errno
		int getError();
        // Local IP
        string getLocalIP();

		// Domain parse
		static bool parseDNS(const char* domain, char* ip);
		// host to network long
		static uint_32 ntohU32(uint_32 hostlong);
		// network to host long
		static uint_32 htonU32(uint_32 hostlong);
		// host to network short
		static uint_16 ntohU16(uint_16 hostshort);
		// network to host short
		static uint_16 htonU16(uint_16 hostshort);

		MGSocket& operator = (const MGSocket& s);
		MGSocket& operator = (SOCKET s);
		operator SOCKET ();
	protected:
		int_32 m_af;
		int_32 m_type;
		int_32 m_protocol;
		SOCKET m_sock;
	};
#endif
} //namespace z11
