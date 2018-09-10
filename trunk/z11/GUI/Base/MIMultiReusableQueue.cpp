/*
 * =====================================================================================
 *
 *       Filename:  MGReusableQueue.inl
 *
 *    Description:  for memory performance
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
#include "MIMultiReusableQueue.h"
#include "../../System/Shared/MGLog.h"
namespace z11
{
	
	MIReusableCell * MIMultiReusableQueueBasedAccessTime::dequeueReusableCellWithIdentifier( const std::string &identifier )
	{
		AUTO_LOCK_THIS;
		MIReusableCell *ret=NULL;
		if (!reusable_cell_multimap.empty())
		{
			String2CellIterator it=reusable_cell_multimap.find(identifier);
			if (reusable_cell_multimap.end()!=it)
			{
				if (!it->second.empty())
				{
					ret=it->second.back();
					ret->prepareForReuse();
					it->second.pop_back();
					if (it->second.empty())
					{
						reusable_cell_multimap.erase(it);
					}	
				}
				
			}

		}
		return ret;
	}

	void MIMultiReusableQueueBasedAccessTime::enqueueReusableCell( MIReusableCell *cell )
	{
		AUTO_LOCK_THIS;
		cell->enqueForReuse();

		//cell map
		CellSetIterator cell_map_it=cell_map.find(cell);
		assert (cell_map_it!=cell_map.end());
		cell_map.erase(cell_map_it);

		//life time
		Int2CellIterator reuse_life_map_it=reuse_life_map.find(cell->reuseable_life);
		assert(reuse_life_map_it!=reuse_life_map.end());
		reuse_life_map.erase(reuse_life_map_it);
		cell->reuseable_life = 0;

		//reusable view
		reusable_cell_multimap[cell->m_reuse_identifier].push_back(cell);
	}

	void MIMultiReusableQueueBasedAccessTime::collection()
	{
		AUTO_LOCK_THIS;
		//assert(cell_multimap.size()==reuse_life_map.size());
		if (reuse_life_map.size()>max_cache_cell_size)
		{
			//always clear half
			int cell_map_size=reuse_life_map.size()/2;

			for (int i=0;i<cell_map_size;i++)
			{
				enqueueReusableCell(reuse_life_map.begin()->second);
			}
			//assert(reuse_life_map.size()==cell_multimap.size());
			MGLogD("============================after clear half of reuse_life_map=============================");

		}
	}

	void MIMultiReusableQueueBasedAccessTime::clear()
	{
		AUTO_LOCK_THIS;
		//assert(cell_multimap.size()==reuse_life_map.size());
		//always clear all
		int cell_map_size=reuse_life_map.size();

		for (int i=0;i<cell_map_size;i++)
		{
			enqueueReusableCell(reuse_life_map.begin()->second);
		}
		//assert(reuse_life_map.size()==cell_multimap.size());
		MGLogD("============================after clear all of reuse_life_map=============================");


	}

	void MIMultiReusableQueueBasedAccessTime::enqueue( MIReusableCell *cell )
	{
		AUTO_LOCK_THIS;
		cell->reuseable_life=getReuseLife();
		reuse_life_map[cell->reuseable_life]=cell;
		cell_map.insert(cell);
		assert(cell->reuseable_life != 0);
		assert(cell->m_reuse_identifier!="");
	}

	int_64 MIMultiReusableQueueBasedAccessTime::getReuseLife()
	{
		return ++time_value;
	}

	bool MIMultiReusableQueueBasedAccessTime::contain( MIReusableCell *cell)
	{
		AUTO_LOCK_THIS;
		return cell_map.find(cell)!=cell_map.end();
	}

	void MIMultiReusableQueueBasedAccessTime::destory()
	{
		AUTO_LOCK_THIS;
		clear();
		//for(std::map<std::string,MIReusableCell *>::iterator it=reusable_tile_view.begin();it!=reusable_tile_view.end();it++)
		//{
		//	it->second->release();
		//}
		reusable_cell_multimap.clear();
	}

	MIMultiReusableQueueBasedAccessTime::MIMultiReusableQueueBasedAccessTime() :max_cache_cell_size(96)
	{
        time_value = 0;
	}
}