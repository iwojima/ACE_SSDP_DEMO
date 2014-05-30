
#include "message_handle.h"

boost::mutex transform_mutex;
boost::mutex peer_mutex;

int 
checkclient(int fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len;
	int32_t listen_fd, sock_fd;
	
	addr_len = sizeof(addr);
	
	memset(&addr, 0, addr_len);
	getpeername(fd, (struct sockaddr *)&addr, &addr_len);

	if (strcmp("192.168.3.93" , inet_ntoa(addr.sin_addr) ) == 0) return 1;
	
	return 0;
}

int 
handle_peer(ACE_HANDLE handle)
{
	socklen_t clientnamelen;
	struct sockaddr_in clientname;
	clientnamelen = sizeof(struct sockaddr_in);
	char buf[2048] = {0};

	int shttp = accept(handle, (struct sockaddr *)&clientname, &clientnamelen);
	if (shttp < 0)
	{
		std::cout<<GREEN<<"accept(http):  "<<strerror(errno)<<NONE<<std::endl;
		return -1;
	}
			
	if( !checkclient(shttp))  //limit ip address
	{
		close(shttp);
		return -1;
	}

	recv(shttp, buf, 2048, 0);
			
	if (std::string(buf).find("GET /rootDesc.xml HTTP/1.1") == 0)
	{
		std::cout<<GREEN<<"target hakken!\n buf = "<<buf<<NONE<<std::endl;
		SendRootContainer(shttp);
	}
	close(shttp);
	
	return 0;
}

int 
handle_alive(ACE_SOCK_Dgram_Mcast udp_t)
{

	int i = 0;
	char buf[512] = {0};
	
	while(known_service_types[i++])
	{
		snprintf(buf, sizeof(buf),
            "NOTIFY * HTTP/1.1\r\n"
            "HOST:%s:%d\r\n"
            "CACHE-CONTROL:max-age=%u\r\n"
            "LOCATION:http://%s:%d" ROOTDESC_PATH"\r\n"
            "SERVER: " MINIDLNA_SERVER_STRING "\r\n"
            "NT:%s%s\r\n"
            "USN:%s%s%s%s\r\n"
            "NTS:ssdp:alive\r\n"
            "\r\n",
            SSDP_MCAST_ADDR, SSDP_PORT,
            (895<<1)+10,
            "192.168.1.20", (unsigned int)8200,
            known_service_types[i],
            (i > 1 ? "1" : ""),
            "uuid:4d696e69-444c-164e-9d41-001ec92f0378",
            (i > 0 ? "::" : ""),
            (i > 0 ? known_service_types[i] : ""),
            (i > 1 ? "1" : ""));
		{
			boost::mutex::scoped_lock lock(transform_mutex);
			udp_t.send(buf , strlen(buf));
		}
	}

	return 0;
}


int 
handle_discover(ACE_SOCK_Dgram_Mcast udp_t)
{
	printf("handle_discover()\n");

	char buf[512] , tmstr[30];
	time_t tm = time(NULL);
	strftime(tmstr, sizeof(tmstr), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&tm));
	
	for (int i = 0 ; known_service_types[i]; i++)
	{
		snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\r\n"
                 "CACHE-CONTROL: max-age=%u\r\n"
                 "DATE: %s\r\n"
                 "ST: %s%s\r\n"
                 "USN: %s%s%s%s\r\n"
                 "EXT:\r\n"
                 "SERVER: " MINIDLNA_SERVER_STRING "\r\n"
                 "LOCATION: http://%s:%u" ROOTDESC_PATH "\r\n"
                 "Content-Length: 0\r\n"
                 "\r\n",
                 (895 << 1) + 10,
                 tmstr,
                 known_service_types[i],
                 (i > 1 ? "1":""),
                 "uuid:4d696e69-444c-164e-9d41-001ec92f0378",
                 (i > 0 ? "::" : ""),
                 (i > 0 ? known_service_types[i] : ""),
                 (i > 1 ? "1" : ""),
                 "192.168.1.20", (unsigned int)8200);
		{
			boost::mutex::scoped_lock lock(transform_mutex);
			udp_t.send(buf , strlen(buf));	
		}
	}
    return 0;				 
	
}


void
SendSSDPResponse(ACE_HANDLE handle)
{

	/*
	HTTP/1.1 200 OK
	CACHE-CONTROL: max-age=1810
	DATE: Wed, 21 May 2014 03:54:53 GMT
	ST: urn:schemas-upnp-org:device:MediaServer:1
	USN: uuid:4d696e69-444c-164e-9d41-001ec92f0378::urn:schemas-upnp-org:device:MediaServer:1
	EXT:
	SERVER: 3.2.0-61-generic DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.2
	LOCATION: http://192.168.1.20:8200/rootDesc.xml
	Content-Length: 0
	 */
	 
	std::cout<<"SendSSDPResponse  handle = "<<handle<<std::endl;

	char buf[512];
	char tmstr[30];
	time_t tm = time(NULL);
	
	strftime(tmstr, sizeof(tmstr), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&tm));
	snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\r\n"
                 "CACHE-CONTROL: max-age=%u\r\n"
                 "DATE: %s\r\n"
                 "ST: %s%s\r\n"
                 "USN: %s%s%s%s\r\n"
                 "EXT:\r\n"
                 "SERVER: " MINIDLNA_SERVER_STRING "\r\n"
                 "LOCATION: http://%s:%u" ROOTDESC_PATH "\r\n"
                 "Content-Length: 0\r\n"
                 "\r\n",
                 (895<<1)+20,
                 tmstr,
                 "urn:schemas-upnp-org:device:MediaServer:",
                 (3>1?"1":""),
                 "uuid:4d696e69-444c-164e-9d41-001ec92f0378",
                 (3 > 0 ? "::" : ""),
                 (3 > 0 ? "urn:schemas-upnp-org:device:MediaServer:" : ""),
                 (3 > 1 ? "1" : ""),
                 "192.168.1.20", (unsigned int)8200);
	
	{
		boost::mutex::scoped_lock lock(transform_mutex);
		ACE_SOCK_Dgram_Mcast udp_t;
		udp_t.set_handle(handle);
		udp_t.send(buf , strlen(buf));
	}
	
}

void
SendRootContainer(ACE_HANDLE handle)
{		
	const char *friendly_name = "Jane";
	const char *resp  = "HTTP/1.1 200 OK\r\n"
						"Content-Type: text/xml; charset=\"utf-8\"\r\n"
						"Connection: close\r\n"
						"Content-Length: 2189\r\n"
						"Server: 3.2.0-61-generic DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.2\r\n"
						"Date: Thu, 22 May 2014 05:29:30 GMT\r\n"
						"EXT:\r\n"
						"\r\n"
						"<?xml version=\"1.0\"?>\r\n"
						"<root xmlns=\"urn:schemas-upnp-org:device-1-0\"><specVersion><major>1</major><minor>0</minor></specVersion><device>"
						"<deviceType>urn:schemas-upnp-org:device:MediaServer:1</deviceType><friendlyName>Jane"
				        "</friendlyName><manufacturer>Justin Maggard</manufacturer><manufacturerURL>http://www.netgear.com/</manufacturerURL>"
				        "<modelDescription>MiniDLNA on Linux</modelDescription><modelName>Windows Media Connect compatible (MiniDLNA)</modelName>"
				        "<modelNumber>1</modelNumber><modelURL>http://www.netgear.com</modelURL><serialNumber>12345678</serialNumber>"
				        "<UDN>uuid:4d696e69-444c-164e-9d41-001ec92f0378</UDN><dlna:X_DLNADOC xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">DMS-1.50</dlna:X_DLNADOC>"
				        "<presentationURL>/</presentationURL><iconList><icon><mimetype>image/png</mimetype><width>48</width><height>48</height>"
				        "<depth>24</depth><url>/icons/sm.png</url></icon><icon><mimetype>image/png</mimetype><width>120</width><height>120</height>"
				        "<depth>24</depth><url>/icons/lrg.png</url></icon><icon><mimetype>image/jpeg</mimetype><width>48</width><height>48</height>"
				        "<depth>24</depth><url>/icons/sm.jpg</url></icon><icon><mimetype>image/jpeg</mimetype><width>120</width><height>120</height>"
				        "<depth>24</depth><url>/icons/lrg.jpg</url></icon></iconList><serviceList><service><serviceType>urn:schemas-upnp-org:service:ContentDirectory:1</serviceType>"
				        "<serviceId>urn:upnp-org:serviceId:ContentDirectory</serviceId><controlURL>/ctl/ContentDir</controlURL><eventSubURL>/evt/ContentDir</eventSubURL>"
				        "<SCPDURL>/ContentDir.xml</SCPDURL></service><service><serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>"
				        "<serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId><controlURL>/ctl/ConnectionMgr</controlURL><eventSubURL>/evt/ConnectionMgr</eventSubURL>"
				        "<SCPDURL>/ConnectionMgr.xml</SCPDURL></service><service><serviceType>urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1</serviceType>"
				        "<serviceId>urn:microsoft.com:serviceId:X_MS_MediaReceiverRegistrar</serviceId><controlURL>/ctl/X_MS_MediaReceiverRegistrar</controlURL>"
				        "<eventSubURL>/evt/X_MS_MediaReceiverRegistrar</eventSubURL><SCPDURL>/X_MS_MediaReceiverRegistrar.xml</SCPDURL></service></serviceList></device></root>";

	ACE_SOCK_Stream stream;
	stream.set_handle(handle);
	
	int res = 0;
	int len = strlen(resp);
	
	{
		boost::mutex::scoped_lock lock(peer_mutex);
		std::cout<<GREEN<<"response = "<<resp<<NONE<<std::endl;
		if( (res = stream.send_n(resp, len)) <= 0 )
		{
			std::cout<<RED<<"send(http): "<< strerror(errno)<<std::endl;
		}
	}
	
	stream.close();
	std::cout<<" stream close res = "<<res<<" , len = "<<strlen(resp)<<std::endl;
}















