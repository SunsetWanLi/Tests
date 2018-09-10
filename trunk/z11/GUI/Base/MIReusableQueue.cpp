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
#include "MIReusableQueue.h"
#include "../../System/Shared/MGLog.h"
namespace z11
{
	
	MIReusableCell * MIReusableQueueBasedAccessTime::dequeueReusableCellWithIdentifier( const std::string &identifier )
	{
		AUTO_LOCK_THIS;
		MIReusableCell *ret=NULL;
		if (!reusable_cell_map.empty())
		{
			{
				String2CellIterator it=reusable_cell_map.find(identifier);
				if (reusable_cell_map.end()==it)
				{
					it=reusable_cell_map.begin();
					it->second->prepareForReuse();
					it->second->m_reuse_identifier="";
				}
				ret=it->second;
				reusable_cell_map.erase(it);

			}

		}
		return ret;
	}

	void MIReusableQueueBasedAccessTime::enqueueReusableCell( MIReusableCell *cell )
	{
		AUTO_LOCK_THIS;
		cell->enqueForReuse();

		//cell map
		String2CellIterator cell_map_it=cell_map.find(cell->m_reuse_identifier);
		assert (cell_map_it!=cell_map.end());
		cell_map.erase(cell_map_it);

		//life time
		Int2CellIterator reuse_life_map_it=reuse_life_map.find(cell->reuseable_life);
		assert(reuse_life_map_it!=reuse_life_map.end());
		reuse_life_map.erase(reuse_life_map_it);
		cell->reuseable_life = 0;

		//reusable view
		reusable_cell_map[cell->m_reuse_identifier]=cell;
	}

	void MIReusableQueueBasedAccessTime::collection()
	{
		AUTO_LOCK_THIS;
		assert(cell_map.size()==reuse_life_map.size());
		if (reuse_life_map.size()>max_cache_cell_size)
		{
			//always clear half
			int cell_map_size=reuse_life_map.size()/2;

			for (int i=0;i<cell_map_size;i++)
			{
				enqueueReusableCell(reuse_life_map.begin()->second);
			}
			//assert(reuse_life_map.size()==cell_map.size());
			MGLogD("============================after clear half of reuse_life_map=============================");

		}
	}

	void MIReusableQueueBasedAccessTime::clear()
	{
		AUTO_LOCK_THIS;
		assert(cell_map.size()==reuse_life_map.size());
		//always clear half
		int cell_map_size=reuse_life_map.size();

		for (int i=0;i<cell_map_size;i++)
		{
			enqueueReusableCell(reuse_life_map.begin()->second);
		}
		//assert(reuse_life_map.size()==cell_map.size());
		MGLogD("============================after clear all of reuse_life_map=============================");


	}

	void MIReusableQueueBasedAccessTime::enqueue( MIReusableCell *cell )
	{
		AUTO_LOCK_THIS;
		cell->reuseable_life = getReuseLife();
		reuse_life_map[cell->reuseable_life]=cell;
		cell_map[cell->m_reuse_identifier]=cell;
		assert(cell->reuseable_life != 0);
		assert(cell->m_reuse_identifier!="");
	}

	int_64 MIReusableQueueBasedAccessTime::getReuseLife()
	{
        return ++time_value;
	}

	MIReusableCell * MIReusableQueueBasedAccessTime::contain( const std::string &identify )
	{
		AUTO_LOCK_THIS;
		std::map<std::string,MIReusableCell *>::iterator it=cell_map.find(identify);
		if (it==cell_map.end())
		{
			return NULL;
		}
		else
		{
			assert(it->second!=NULL);
			return it->second;
		}
	}

	void MIReusableQueueBasedAccessTime::destory()
	{
		AUTO_LOCK_THIS;
		clear();
		//for(std::map<std::string,MIReusableCell *>::iterator it=reusable_tile_view.begin();it!=reusable_tile_view.end();it++)
		//{
		//	it->second->release();
		//}
		reusable_cell_map.clear();
	}

	MIReusableQueueBasedAccessTime::MIReusableQueueBasedAccessTime() :max_cache_cell_size(96)
	{
        time_value = 0;
	}
}