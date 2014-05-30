
#ifndef MESSAGE_HANDLE_H
#define MESSAGE_HANDLE_H



#include <iostream>
#include <ace/OS_NS_sys_socket.h>
#include <ace/Thread_Manager.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include "ace/os_include/os_netdb.h"
#include <ace/SOCK_Stream.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp> 
#include <boost/function.hpp>

#define NONE         "\033[m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"

static const char * const known_service_types[] =
{
	"uuid:00000000-0000-0000-0000-000000000000",
	"upnp:rootdevice",
	"urn:schemas-upnp-org:device:MediaServer:",
	"urn:schemas-upnp-org:service:ContentDirectory:",
	"urn:schemas-upnp-org:service:ConnectionManager:",
	"urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:",
	0
};



#define MAX_MULTICAST_IP_TTL  5
#define MULTICAST "239.192.2.3:12345"
#define SSDP_MCAST_ADDR ("239.255.255.250")
#define SSDP_PORT (1900)

#define OS_VERSION "3.4.72-rt89"
#define SERVER_NAME "MiniDLNA"
#define MINIDLNA_VERSION "1.1.0"
#define MINIDLNA_SERVER_STRING  OS_VERSION " DLNADOC/1.50 UPnP/1.0 " SERVER_NAME "/" MINIDLNA_VERSION
#define ROOTDESC_PATH "/rootDesc.xml"


int  handle_peer(ACE_HANDLE handle);
int  handle_alive(ACE_SOCK_Dgram_Mcast udp_t);
int  handle_discover();
void SendSSDPResponse(ACE_HANDLE handle);
void SendRootContainer(ACE_HANDLE handle);

#endif