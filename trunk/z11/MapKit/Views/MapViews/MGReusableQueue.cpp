/*
 * =====================================================================================
 *
 *       Filename:  MGReusableQueue.cpp
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

#include "MGReusableQueue.h"
#include "MKTileLayerOverlay.h"
#include "System/Shared/MGLog.h"
#include "MKTileMapView.h"

namespace z11
{
	void MGReusableQueueBasedAccessTime::enqueue( MKSateliteMapViewCell *cell )
	{
		AUTO_LOCK_THIS;
		cell->reuseable_life=getReuseLife();
        
        {
            ReuseCellIter it = reuse_life_map.find(cell->m_tms_code);
            assert(it == reuse_life_map.end());
        }
        
		reuse_life_map[cell->m_tms_code] = cell;
        time2index_map[cell->reuseable_life] = cell->m_tms_code;
		assert(cell->reuseable_life != 0);
	}

	void MGReusableQueueBasedAccessTime::update( MKSateliteMapViewCell *cell )
	{
		AUTO_LOCK_THIS;
		ReuseCellIter it=reuse_life_map.find(cell->m_tms_code);
        assert(it != reuse_life_map.end());
        assert(it->second == cell);
        
        Time2IndexIter t_it = time2index_map.find(cell->reuseable_life);
        assert(t_it != time2index_map.end());
        
        time2index_map.erase(t_it);
		reuse_life_map.erase(it);
		cell->reuseable_life=getReuseLife();
		reuse_life_map[cell->m_tms_code]=cell;
        time2index_map[cell->reuseable_life] = cell->m_tms_code;
	}
	int_64 MGReusableQueueBasedAccessTime::getReuseLife()
	{
		return ++time_value;
	}

	MKSateliteMapViewCell * MGReusableQueueBasedAccessTime::contain( const MKGlobalTileIndex &identify )
	{
		AUTO_LOCK_THIS;
		ReuseCellIter it = reuse_life_map.find(identify);
		if (it == reuse_life_map.end())
		{
			return NULL;
		}
		else
		{
			assert(it->second!=NULL);
			return it->second;
		}
	}

	std::set<MKTileLayerOverlay*> MGReusableQueueBasedAccessTime::collectNTiles( uint_32 n )
	{
		AUTO_LOCK_THIS;
		std::set<MKTileLayerOverlay*> ret;
		{
			//always clear half
			uint_32 cell_map_size=n;

			while(cell_map_size&&!time2index_map.empty())
			{
                MKGlobalTileIndex index = time2index_map.begin()->second;
                ReuseCellIter it = reuse_life_map.find(index);
                assert(it != reuse_life_map.end());
				MKSateliteMapViewCell *cell = it->second;
				if (cell->fetchState()==MKFetchStateProcessing)
				{
					index = time2index_map.rbegin()->second;
                    it = reuse_life_map.find(index);
                    assert(it != reuse_life_map.end());
                    cell = it->second;
				}
				
				if (cell->fetchState()!=MKFetchStateProcessing)
				{
					cell_map_size--;
				}else
                    break;
				
				cell->detachFromSuperview();

                //time map
				Time2IndexIter time2index_it = time2index_map.find(cell->reuseable_life);
				assert (time2index_it != time2index_map.end());
				time2index_map.erase(time2index_it);
                
				//life time
				ReuseCellIter reuse_life_map_it = reuse_life_map.find(cell->m_tms_code);
				assert(reuse_life_map_it!=reuse_life_map.end());
				reuse_life_map.erase(reuse_life_map_it);
				cell->reuseable_life = 0;

				ret.insert(cell);
                //MGLogD("Colloect Cell %p", cell);
			}
		}
		return ret;
	}

    uint_32 MGReusableQueueBasedAccessTime::size()
    {
        AUTO_LOCK_THIS;
        assert(reuse_life_map.size() == time2index_map.size());
        return reuse_life_map.size();
    }
    
    MGReusableQueueBasedAccessTime::MGReusableQueueBasedAccessTime()
    {
        time_value = 0;
    }
    
}