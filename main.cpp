

#include "main.h"


long GetCurrentSecond()
{
	struct timeval timeofday;
	struct timezone tz;
	
	gettimeofday(&timeofday , &tz);
	
	return timeofday.tv_sec;
} 

int
GetPeerListenSocket(unsigned short port)
{
    int s;
    int i = 1;
    struct sockaddr_in listenname;
 
    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
		std::cout<<RED<<"ocket(http): "<<strerror(errno)<<NONE<<std::endl;
        return -1;
    }

    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
		std::cout<<RED<<"setsockopt(http, SO_REUSEADDR): "<<strerror(errno)<<NONE<<std::endl;
 
    memset(&listenname, 0, sizeof(struct sockaddr_in));
    listenname.sin_family = AF_INET;
	listenname.sin_port = htons(port);
	listenname.sin_addr.s_addr = htonl(INADDR_ANY);
 
	if (bind(s, (struct sockaddr *)&listenname, sizeof(struct sockaddr_in)) < 0)
	{
		std::cout<<RED<<"bind(http): "<<strerror(errno)<<NONE<<std::endl;
		close(s);
		return -1;
		}
 
	if (listen(s, 6) < 0)
	{
		std::cout<<RED<<"listen(http): "<<strerror(errno)<<NONE<<std::endl;
		close(s);
		return -1;
	}
 
	return s;
}

int main(int argc , char *argv[])
{

	ACE_SOCK_Acceptor acceptor;
	ACE_INET_Addr local_inet_addr , remote_inet_addr;
	ACE_SOCK_Stream local_stream , remote_stream;
    struct timeval timeout , timeofday , lastnotifytime = {0 , 0};
	struct timezone tz;

	char buf[2048] = {0};
	
	ACE_SOCK_Dgram_Mcast udp;
	ACE_INET_Addr mcast_inet("239.255.255.250:1900");
	udp.join(mcast_inet);
	udp.set_option(IP_MULTICAST_TTL, MAX_MULTICAST_IP_TTL);
	
	thread_pool thrd_pool;
	thrd_pool.run();
	
	
	ACE_Handle_Set active_handle_set;
	ACE_HANDLE udp_handle = udp.get_handle();
	
	struct timeval tv = {1, 0};
	ACE_Time_Value atv(tv);

	ACE_HANDLE peer_handle = GetPeerListenSocket(8200);
	
	int res = 0 , width = 0;
	
	long time_record = GetCurrentSecond();
	while(1)
	{
		if ((GetCurrentSecond() - time_record) > 20)
		{
			handle_alive(udp);
			time_record = GetCurrentSecond();
		}
		
		active_handle_set.set_bit(udp_handle);
		active_handle_set.set_bit(peer_handle);

		width = (int)active_handle_set.max_set() + 1;
		if (ACE::select(width , &active_handle_set , 0 , 0 , &atv) < 1)  //ACE::select 返回活动的句柄数目
		{
			continue;
		}
		
		if (active_handle_set.num_set() <= 0)
		{
			continue;
		}
		

		ACE_Handle_Set_Iterator iterator(active_handle_set);
		ACE_HANDLE handle = iterator();
		for (; handle != ACE_INVALID_HANDLE; handle = iterator())
		{
			if (handle == udp_handle)
			{
				{
					boost::mutex::scoped_lock lock(transform_mutex);
					res = udp.recv(buf , 512 , remote_inet_addr);
					if(res <= 0) continue;
				}
				
				std::map<ACE_HANDLE , std::string> m;
				m.insert(std::pair<ACE_HANDLE , std::string>(udp_handle , std::string(buf)));
				thrd_pool.push_task(m);
			}
			
			if (handle == peer_handle)
			{
				std::map<ACE_HANDLE , std::string> m;
				m.insert(std::pair<ACE_HANDLE , std::string>(peer_handle , std::string("HANDLE_PEER_ACCEPTOR")));
				thrd_pool.push_task(m);
			}
		}

	}
	
	
	thrd_pool.join();
	udp.leave(mcast_inet);

	
	return 0;
}

