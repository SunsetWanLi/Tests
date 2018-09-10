//
//  MGSocket.cpp
//  System
//
//  Created by DanYuan Hu on 12-03-31.
//  Copyright (c) 2011ƒÍ Zhejiang University. All rights reserved.
//


#include "MGSocket.h"
#include "../../System/Shared/MGLog.h"
#include <exception>
#include <iostream>
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)
#define io_mode_set ioctlsocket
#if defined(WINCE)

#else
#pragma comment(lib, "wsock32")
#endif
#else
#include <sys/ioctl.h>
#include <errno.h>
#define io_mode_set ioctl
#endif

using namespace std;
namespace z11
{
	MGSocket::MGSocket(SOCKET sock)
	{
		m_sock = sock;
#if defined(_WIN32) || defined(_WIN64)
		/*
         http://msdn.microsoft.com/zh-cn/vstudio/ms741563(en-us,VS.85).aspx
         typedef struct WSAData {
         WORD wVersion;								//winsock version
         WORD wHighVersion;							//The highest version of the Windows Sockets specification that the Ws2_32.dll can support
         char szDescription[WSADESCRIPTION_LEN+1];
         char szSystemStatus[WSASYSSTATUS_LEN+1];
         unsigned short iMaxSockets;
         unsigned short iMaxUdpDg;
         char FAR * lpVendorInfo;
         }WSADATA, *LPWSADATA;
         */
		WSADATA wsaData;
		//#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8))
		WORD version = MAKEWORD(2, 0);
		int ret = WSAStartup(version, &wsaData);//win sock start up
		if ( ret ) {
			cerr << "Initilize winsock error !" << endl;
		}
#endif
	}
	MGSocket::~MGSocket()
	{
#if defined(_WIN32) || defined(_WIN64)
        // WSACleanup()内置引用计数，只需要和WSAStartup配对即可 //
		int ret = WSACleanup();
		if ( ret ) {
			cerr << "Cleanup winsock error !" << endl;
		}
#endif
	}

	MGSocket& MGSocket::operator = (const MGSocket& s)
	{
		m_sock = s.m_sock;
		m_af = s.m_af;
		m_type = s.m_type;
		m_protocol = s.m_protocol;
		return (*this);
	}
	MGSocket& MGSocket::operator = (SOCKET s)
	{
		m_sock = s;
		return (*this);
	}
	MGSocket::operator SOCKET ()
	{
		return m_sock;
	}
	//create a socket object win/lin is the same
	// af:
	bool MGSocket::create(int_32 af, int_32 type, int_32 protocol)
	{
		m_af = af;
		m_type = type;
		m_protocol = protocol;
		m_sock = socket(af, type, protocol);
		if ( m_sock == INVALID_SOCKET ) {
			return false;
		}
		return true;
	}
	bool MGSocket::connectTo(const char* ip, unsigned short port, uint_32 overtime)
	{
        bool ret = true;
        int error;
		struct sockaddr_in svraddr;
		svraddr.sin_family = m_af;
		svraddr.sin_addr.s_addr = inet_addr(ip);
		svraddr.sin_port = htons(port);
        
        unsigned long opt = 1;
        do{
            //set non-blocking
            if (io_mode_set(m_sock, FIONBIO, &opt) < 0) {
                ret = false;
                break;
            }
            
            if (connect(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR) {
                int len = sizeof(int);
                timeval tv_timeout;
                tv_timeout.tv_sec  = overtime / 1000;
                tv_timeout.tv_usec = overtime % 1000;
                fd_set set;
                FD_ZERO(&set);
                FD_SET(m_sock, &set);
                if(select(m_sock + 1, NULL, &set, NULL, &tv_timeout) > 0) {
                    getsockopt(m_sock, SOL_SOCKET, SO_ERROR, (char*)&error, (socklen_t *)&len);
                    if(error != 0) {
                        ret = false;
                        break;
                    }
                } else { //timeout or select error
                    ret = false;
                    break;
                }
            }
        }while(0);
        
        opt = 0;
        //set blocking
        if (io_mode_set(m_sock, FIONBIO, &opt) < 0) {
            ret = false;
        }
        return ret;
	}
    int	MGSocket::setOption(int level, int optname, const char *optval, int optlen)
    {
        return setsockopt(m_sock, level, optname, optval, optlen);
    }
	bool MGSocket::bindOn(unsigned short port)
	{
		struct sockaddr_in svraddr;
		svraddr.sin_family = m_af;
		svraddr.sin_addr.s_addr = INADDR_ANY;
		svraddr.sin_port = htons(port);
		int opt =  1;
		if ( setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0 ) 
			return false;
		int ret = ::bind(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));
		if ( ret == SOCKET_ERROR ) {
			return false;
		}
		return true;
	}
	//for server
	bool MGSocket::listenOn(int backlog)
	{
		int ret = listen(m_sock, backlog);
		if ( ret == SOCKET_ERROR ) {
			return false;
		}
		return true;
	}
	bool MGSocket::acceptFor(MGSocket& s, string& fromip, uint_32& port)
	{
		struct sockaddr_in cliaddr;
		socklen_t addrlen = sizeof(cliaddr);
		SOCKET sock = accept(m_sock, (struct sockaddr*)&cliaddr, &addrlen);
		if ( sock == SOCKET_ERROR ) {
			return false;
		}
		s = sock;
		fromip = inet_ntoa(cliaddr.sin_addr);
        port = cliaddr.sin_port;
		return true;
	}
    bool MGSocket::waitFor(int timeout)
    {
        fd_set fdread;
        fd_set fdexception;
        FD_ZERO(&fdread);
        FD_SET(m_sock, &fdread);
        FD_ZERO(&fdexception);
        FD_SET(m_sock, &fdexception);
        int maxsock = m_sock + 1;
        
        int ret = 0;
        if (timeout < 0) {
            ret = select(maxsock, NULL, &fdread, NULL, NULL);
        } else {
            timeval to;
            to.tv_usec = timeout;
            ret = select(maxsock, NULL, &fdread, NULL, &to);
        }
        if (FD_ISSET(m_sock, &fdexception) > 0) {
            return false;
        }
        return ret > 0;
    }
	int_64 MGSocket::sendData(const char* buf, int len, int flags)
	{
		int_64 bytes;
		int_64 count = 0;
		while ( count < len ) {
            try {
                bytes = send(m_sock, buf + count, len - count, flags);
                if (bytes == -1) {
                    throw exception();
                }
                if ( bytes == 0 )
                    return bytes;
                count += bytes;
            } catch (exception e) {
                MGLogD("sendTo exception");
                count = -1;
                return count;
            }
		} 
		return count;
	}
	int_64 MGSocket::recvData(char* buf, int len, int flags)
	{
        int_64 count = 0;
        try {
            count = recv(m_sock, buf, len, flags);
            if (count == -1) {
                throw exception();
            }
        } catch (exception e) {
            MGLogD("recvFor exception");
            count = -1;
        }
        return count;
	}
    int_64 MGSocket::sendDataTo(const char* buf, int len, const string& ip, const unsigned int port, int flags)
    {
        int_64 bytes;
        int_64 count = 0;
        while ( count < len ) {
            try {
                struct sockaddr_in cliaddr;
                cliaddr.sin_family = m_af;
                cliaddr.sin_addr.s_addr = inet_addr(ip.c_str());
                cliaddr.sin_port = htons(port);
                socklen_t addrlen = sizeof(cliaddr);
                bytes = sendto(m_sock, buf + count, len - count, flags, (struct sockaddr*)&cliaddr, addrlen);
                if (bytes == -1) {
                    throw exception();
                }
                if ( bytes == 0 )
                    return bytes;
                count += bytes;
            } catch (exception e) {
                MGLogD("sendTo exception");
                count = -1;
                return count;
            }
        }
        return count;
    }
    int_64 MGSocket::recvDataFrom(char* buf, int len, string& ip, unsigned int& port, int flags)
    {
        int_64 count = 0;
        try {
            struct sockaddr_in cliaddr;
            socklen_t addrlen = sizeof(cliaddr);
            count = recvfrom(m_sock, buf, len, flags, (struct sockaddr*)&cliaddr, &addrlen);
            ip = inet_ntoa(cliaddr.sin_addr);
            port = cliaddr.sin_port;
            if (count == -1) {
                throw exception();
            }
        } catch (exception e) {
            MGLogD("recvFor exception");
            count = -1;
        }
        return count;
    }
    int	MGSocket::shutdownSocket(int how)
    {
        return shutdown(m_sock, how);
    }
	int MGSocket::closeSocket()
	{
#if defined(_WIN32) || defined(_WIN64)
		return (closesocket(m_sock));
#else
		return (close(m_sock));
#endif
	}
	int MGSocket::getError()
	{
#if defined(_WIN32) || defined(_WIN64)
		return (WSAGetLastError());
#else
		return (errno);
#endif
	}
    string MGSocket::getLocalIP()
    {
        string local_ip = "";
        //1.获取主机名
        char hostname[256];
        int ret=gethostname(hostname,sizeof(hostname));
        if (ret!=SOCKET_ERROR)
        {
            //2.获取主机ip
            hostent* host=gethostbyname(hostname);
            if (host!=NULL)
            {
                //3.转化为char*并拷贝返回
                char ip[256];
                strcpy(ip,inet_ntoa(*(in_addr*)*host->h_addr_list));
                local_ip = ip;
            }
        }
        return local_ip;
    }
	bool MGSocket::parseDNS(const char* domain, char* ip)
	{
		struct hostent* p;
		if ( (p = gethostbyname(domain)) == NULL )
			return false;

		sprintf(ip, 
			"%u.%u.%u.%u",
			(unsigned char)p->h_addr_list[0][0], 
			(unsigned char)p->h_addr_list[0][1], 
			(unsigned char)p->h_addr_list[0][2], 
			(unsigned char)p->h_addr_list[0][3]);

		return true;
	}

	uint_32 MGSocket::ntohU32(uint_32 hostlong)
	{
		return ntohl(hostlong);
	}

	uint_32 MGSocket::htonU32(uint_32 hostlong)
	{
		return htonl(hostlong);
	}

	uint_16 MGSocket::ntohU16(uint_16 hostshort)
	{
		return ntohs(hostshort);
	}

	uint_16 MGSocket::htonU16(uint_16 hostshort)
	{
		return htons(hostshort);
	}
}
