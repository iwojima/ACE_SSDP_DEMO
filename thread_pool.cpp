
#include "thread_pool.h"
#include "message_handle.h"

int thread_pool::handle_data(MAPHDLSTR maphdlstr)
{
	ACE_HANDLE  sock = maphdlstr.begin()->first;
	std::string data   = maphdlstr.begin()->second;
	
	if (0 == data.find("M-SEARCH * HTTP/1.1"))  //ssdp:discover
	{
		/*
		M-SEARCH * HTTP/1.1  
		Host: 239.255.255.250:1900  #设置为协议保留多播地址和端口，必须是239.255.255.250:1900。  
		Man: "ssdp:discover"   #设置协议查询的类型，必须是"ssdp:discover"。  
		MX: 5   #设置设备响应最长等待时间，设备响应在0和这个值之间随机选择响应延迟的值。这样可以为控制点响应平衡网络负载。  
		ST: upnp:rootdevice  #设置服务查询的目标  
		*/

		SendSSDPResponse(sock);
		return 0;
	}
	
	if (0 == data.find("NOTIFY * HTTP/1.1"))   //ssdp:alive
	{
		/*
		NOTIFY * HTTP/1.1  
		HOST:239.255.255.250:1900    #协议保留多播地址和端口，必须是239.255.255.250:1900  
		CACHE-CONTROL:max-age=1810   #max-age指定通知消息存活时间，如果超过此时间间隔，控制点可以认为设备不存在  
		LOCATION:http://192.168.1.20:8200/rootDesc.xml     #包含根设备描述得URL地址  
		SERVER: 3.4.72-rt89 DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.0  
		NT:upnp:rootdevice   #在此消息中，NT头必须为服务的服务类型  
		USN:uuid:4d696e69-444c-164e-9d41-001ec92f0378::upnp:rootdevice   #表示不同服务的统一服务名，它提供了一种标识出相同类型服务的能力  
		NTS:ssdp:alive   #表示通知消息的子类型，必须为ssdp:alive 
		*/

		return 0;
	}
	
	if (0 == data.find("POST /ctl/ContentDir HTTP/1.1"))
	{
		/*
		POST /ctl/ContentDir HTTP/1.1  
		HOST: 192.168.1.20:8200  
		CONTENT-LENGTH: 488  
		CONTENT-TYPE: text/xml; charset="utf-8"  
		SOAPACTION: "urn:schemas-upnp-org:service:ContentDirectory:1#Browse"  
		USER-AGENT: 6.1.7600 2/, UPnP/1.0, Portable SDK for UPnP devices/1.6.18  
  
		<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">  
		<s:Body><u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">  
		<ObjectID>64$4</ObjectID>  
		<BrowseFlag>BrowseDirectChildren</BrowseFlag>  
		<Filter>id,dc:title,res,sec:CaptionInfo,sec:CaptionInfoEx</Filter>  
		<StartingIndex>0</StartingIndex>  
		<RequestedCount>0</RequestedCount>  
		<SortCriteria></SortCriteria>  
		</u:Browse>  
		</s:Body>  
		</s:Envelope> 
		*/

	}
	
	if (0 == data.find("HANDLE_PEER_ACCEPTOR"))
	{
		handle_peer(sock);
	}

	return 0;
}


/*void thread_pool::signal_interrupt(int signo)
{
	finish = true;
}*/


