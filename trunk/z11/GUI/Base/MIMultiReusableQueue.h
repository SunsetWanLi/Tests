/*
 * =====================================================================================
 *
 *       Filename:  MIReusableQueue.h
 *
 *    Description: maintain a queue for reuse
 *
 *        Version:  1.0
 *        Created:  06/09/2012 02:57:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once
#include <vector>
#include <set>
#include "MIReusableQueue.h"
#include "System/Shared/MGTime.h"
namespace z11
{
	class MIMultiReusableQueueBasedAccessTime:public Syncable
	{
	public:
		MIMultiReusableQueueBasedAccessTime();
		MIReusableCell *dequeueReusableCellWithIdentifier(const std::string &identifier);
		void collection();
		void enqueue(MIReusableCell *cell);
		void enqueueReusableCell(MIReusableCell *cell);
		bool contain(MIReusableCell *);
		void clear();
		void destory();
	private:
		std::map<std::string,std::vector<MIReusableCell *> >  reusable_cell_multimap;
		std::map<int_64 ,MIReusableCell *> reuse_life_map;
		std::set<MIReusableCell *>  cell_map;
		typedef std::map<std::string,std::vector<MIReusableCell *> >::iterator String2CellIterator ;
		typedef std::map<int_64,MIReusableCell * >::iterator Int2CellIterator ;
		typedef std::set<MIReusableCell *>::iterator CellSetIterator;
		int_64 getReuseLife();
		size_t max_cache_cell_size;
        int_64 time_value;
	};
};