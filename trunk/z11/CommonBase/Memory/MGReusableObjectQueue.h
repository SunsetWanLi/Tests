/*
 * =====================================================================================
 *
 *       Filename:  MGReusableObjectQueue.h
 *
 *    Description: template class for Reusable Queue. It's thread safety. //It's a LRU  Queue.
 *
 *        Version:  1.0
 *        Created:  04/12/2012 11:39:42 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include <list>
#include <string>
#include "../../System/Thread/MGMutex.h"
namespace z11
{

	const size_t MGReusableObjectQueueNum=20; 
	template<typename T>
	class MGReusableObjectQueue
	{
		class Node
		{
		public:
			Node(T * cell,const std::string &reuseIdentifier)
				:m_reuseIdentifier(reuseIdentifier),data(cell)
			{

			}
			std::string m_reuseIdentifier;
			T *data;
		};
	public:
		static MGMutex m_reusable_queue_locker;
		static std::list<Node> m_reusable_queue; 
		static void enqueue(T * cell,const std::string &reuseIdentifier)
		{
			m_reusable_queue_locker.lock();
			m_reusable_queue.push_front(Node(cell,reuseIdentifier));
			m_reusable_queue_locker.unlock();
		}


		static T * dequeue(const std::string &reuseIdentifier)
		{
			T *ret=NULL;
			m_reusable_queue_locker.lock();
			if (!m_reusable_queue.empty())
			{
				std::list<Node>::iterator it;
				for (it=m_reusable_queue.begin();
					it!=m_reusable_queue.end();
					it++)
				{
					if ((*it).m_reuseIdentifier == reuseIdentifier)
					{
						break;
					}

				}
				if (it!=m_reusable_queue.end())
				{
					ret=(*it).data;
					m_reusable_queue.erase(it);
				}
				else
				{
					if(m_reusable_queue.size()>MGReusableObjectQueueNum)
					{
						ret=m_reusable_queue.back().data;
						m_reusable_queue.pop_back() ;
					}
				}
			}
			m_reusable_queue_locker.unlock();
			return ret;

		}
	};
}
