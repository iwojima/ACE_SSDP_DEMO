
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
		Host: 239.255.255.250:1900  #����ΪЭ�鱣���ಥ��ַ�Ͷ˿ڣ�������239.255.255.250:1900��  
		Man: "ssdp:discover"   #����Э���ѯ�����ͣ�������"ssdp:discover"��  
		MX: 5   #�����豸��Ӧ��ȴ�ʱ�䣬�豸��Ӧ��0�����ֵ֮�����ѡ����Ӧ�ӳٵ�ֵ����������Ϊ���Ƶ���Ӧƽ�����縺�ء�  
		ST: upnp:rootdevice  #���÷����ѯ��Ŀ��  
		*/

		SendSSDPResponse(sock);
		return 0;
	}
	
	if (0 == data.find("NOTIFY * HTTP/1.1"))   //ssdp:alive
	{
		/*
		NOTIFY * HTTP/1.1  
		HOST:239.255.255.250:1900    #Э�鱣���ಥ��ַ�Ͷ˿ڣ�������239.255.255.250:1900  
		CACHE-CONTROL:max-age=1810   #max-ageָ��֪ͨ��Ϣ���ʱ�䣬���������ʱ���������Ƶ������Ϊ�豸������  
		LOCATION:http://192.168.1.20:8200/rootDesc.xml     #�������豸������URL��ַ  
		SERVER: 3.4.72-rt89 DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.0  
		NT:upnp:rootdevice   #�ڴ���Ϣ�У�NTͷ����Ϊ����ķ�������  
		USN:uuid:4d696e69-444c-164e-9d41-001ec92f0378::upnp:rootdevice   #��ʾ��ͬ�����ͳһ�����������ṩ��һ�ֱ�ʶ����ͬ���ͷ��������  
		NTS:ssdp:alive   #��ʾ֪ͨ��Ϣ�������ͣ�����Ϊssdp:alive 
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


