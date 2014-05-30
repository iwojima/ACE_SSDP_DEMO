
#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include "message_handle.h"

#include <iostream>
#include <queue>
#include <map>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp> 
#include <boost/function.hpp>

#include <ace/OS_NS_sys_socket.h>
#include <ace/Thread_Manager.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include "ace/os_include/os_netdb.h"
#include <ace/SOCK_Stream.h>

extern boost::mutex transform_mutex;
extern boost::mutex peer_mutex;

class thread_pool : public boost::noncopyable
{
	private:
		
		boost::thread_group thrds;
		boost::mutex io_mutex;

		std::map<std::string , std::string>::iterator iter;
		
		bool finish;
		int thread_num;
		
		
		typedef std::map<ACE_HANDLE , std::string> MAPHDLSTR;
		typedef std::queue<MAPHDLSTR> QUEMAPHDLSTR;
		QUEMAPHDLSTR vecque;
	public:
		thread_pool(){ thread_num = 8;finish = false;} 		
		thread_pool(int num):thread_num(num){}
		
		~thread_pool()
		{
			//处理队列中剩余的任务
		}
		
		void proxy()
		{
			while(!finish)
			{
				MAPHDLSTR maphdlstr;
				{
					boost::mutex::scoped_lock lock(io_mutex);
					if (!vecque.empty())
					{
						maphdlstr = vecque.front();
						vecque.pop();
					}
				}

				if (!maphdlstr.empty())
				{
					handle_data(maphdlstr);
				}
			}
			
		}
		
		void push_task(MAPHDLSTR m)
		{
			vecque.push(m);
		}
		//static void signal_interrupt(int signo);

		void run()
		{
			for(size_t i = 0; i < thread_num ; ++i)
			{
				thrds.create_thread(boost::bind(&thread_pool::proxy,this));
			}
		}
		
		
		void join(){thrds.join_all();}
		int get_static(){return finish;}
		int handle_data(MAPHDLSTR maphdlstr);
};

//bool thread_pool::finish = false;

#endif