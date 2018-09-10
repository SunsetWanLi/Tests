/*
 * =====================================================================================
 *
 *       Filename:  MGReusableQueue.h
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
#include <string>
#include <map>
#include <set>
#include "System/Shared/Syncable.h"
#include "System/Shared/MGTime.h"
#include "../../DataTypes/Base/MKGeometry.h"
namespace z11
{
	class MKSateliteMapViewCell;
	class MKTileLayerOverlay;
	class MGReusableQueueBasedAccessTime:public Syncable
	{
    public:
        MGReusableQueueBasedAccessTime();
	public:
		void enqueue(MKSateliteMapViewCell *cell);
		void update(MKSateliteMapViewCell *cell);
		MKSateliteMapViewCell *contain(const MKGlobalTileIndex &identify);
		std::set<MKTileLayerOverlay*> collectNTiles(uint_32 n);
        uint_32 size();
	private:
		std::map<MKGlobalTileIndex ,MKSateliteMapViewCell*> reuse_life_map;
		std::map<int_64, MKGlobalTileIndex>  time2index_map;
		int_64 getReuseLife();
        int_64 time_value;
	};
    typedef std::map<MKGlobalTileIndex ,MKSateliteMapViewCell *>::iterator ReuseCellIter;
    typedef std::map<int_64, MKGlobalTileIndex>::iterator Time2IndexIter;
};
