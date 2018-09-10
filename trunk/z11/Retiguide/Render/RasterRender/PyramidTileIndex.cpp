#include "./PyramidTileIndex.h"
#include "./PyramidTileBuffer.h"
#include "./PyramidSettings.h"
#include "System/Thread/MGSemaphore.h"
#include <queue>
#include <algorithm>
#include <cmath>
#include <stdlib.h>
using namespace std;
using namespace z11;
namespace GIS_Embedded
{
	// struct Index_Tree_Node

	Index_Tree_Node::Index_Tree_Node()
	{
		floor_number = block_row_number = block_col_number = 0;
		resolution = 0.0;
		reference_count = 0;
		buffer = NULL;
		node_semaphore = new z11::MGSemaphore(1);
	}
	Index_Tree_Node::~Index_Tree_Node()
	{
	}
	void Index_Tree_Node::ref_inc(ImageBuffer * buffer)
	{
		node_semaphore->Wait();
		reference_count ++;
		//buffer->use_entry(this->buffer);
		buffer->dirty_inc(this->buffer);
		node_semaphore->Signal();
	} // ref_inc()
	void Index_Tree_Node::ref_dec(ImageBuffer * buffer)
	{
		node_semaphore->Wait();
		reference_count --;
		if(reference_count < 0)reference_count = 0;
		if(0 == reference_count)
		{
			buffer->release_entry(this->buffer);
		}
		node_semaphore->Signal();
	} // ref_dec()

	

	// class IndexTree
	IndexTree::IndexTree(Floor * floor_info , GeoRegion_Scale map_region,z11::uint_32 floor_amount , z11::uint_32 regular_size)
	{
		m_info_of_floor = floor_info;
		m_floor_amount = floor_amount;
		m_regular_tile_size = regular_size;
		m_tiles = NULL;
		m_floor_offsets = NULL;
		m_tile_count = 0;
		m_map_region = map_region;
		build_tree();
	} // IndexTree(Floor)

	IndexTree::~IndexTree()
	{
		delete [] m_tiles;
		delete [] m_floor_offsets;
		m_tiles = NULL;
		m_floor_offsets = NULL;

	} // ~IndexTree()

	void IndexTree::build_tree()
	{
		if(NULL != m_tiles)
			delete [] m_tiles;
		base_calc();
		m_tiles = new Index_Tree_Node[m_tile_count];

		for(z11::uint_32 floor_number = 0; floor_number < m_floor_amount ; ++floor_number)
		{
			Floor & current_floor = m_info_of_floor[floor_number];
			for(z11::uint_32 row = 0; row < current_floor.row_number ; ++row)
			{
				z11::uint_32 col = 0;
				for(col = 0; col < current_floor.regular_per_row + current_floor.non_regular_per_row; ++col)
				{
					Index_Tree_Node & tile = m_tiles[Offset(floor_number,row,col)];
					tile.floor_number = floor_number;
					tile.block_row_number = row;
					tile.block_col_number = col;
					tile.resolution = current_floor.resolution;
					//tile.node_semaphore = m_tree_semaphore;
					calcRegion(tile);
				}// for -- col
			}// for -- row
			
		}// for -- floor_number	
		return ;
	} // build_tree()

	void IndexTree::base_calc()
	{
		
		m_tile_count = 0;

		if(NULL == m_info_of_floor)
		{
			return ;
		}
		delete [] m_floor_offsets;
		m_floor_offsets = new z11::uint_32[m_floor_amount];
		for(z11::uint_32 floor_number = 0 ; floor_number < m_floor_amount ; ++ floor_number)
		{
			m_floor_offsets[floor_number] = m_tile_count;
			m_tile_count += m_info_of_floor[floor_number].row_number 
				* (m_info_of_floor[floor_number].non_regular_per_row 
				+ m_info_of_floor[floor_number].regular_per_row);
		} // for -- floor_number

		return ;
	} // base_calc()

	inline z11::uint_32 IndexTree::Offset(z11::uint_32 floor_number,z11::uint_32 row,z11::uint_32 col)
	{
		if(floor_number >= m_floor_amount)floor_number = m_floor_amount - 1;
		if(row >= m_info_of_floor[floor_number].row_number)row = m_info_of_floor[floor_number].row_number - 1;
		if(col >= (m_info_of_floor[floor_number].regular_per_row + m_info_of_floor[floor_number].non_regular_per_row) )
			col = m_info_of_floor[floor_number].regular_per_row + m_info_of_floor[floor_number].non_regular_per_row - 1;
		if (floor_number<0 || row<0 || col<0)
			return -1;
		return m_floor_offsets[floor_number] 
			+ row * (m_info_of_floor[floor_number].regular_per_row + m_info_of_floor[floor_number].non_regular_per_row)
			+ col;
	} // Offset(z11::uint_32 , z11::uint_32 , z11::uint_32)

	inline GeoRegion_Scale IndexTree::calcRegion(Index_Tree_Node & tile)
	{
		Floor & current_floor = m_info_of_floor[tile.floor_number];
		GeoRegion_Scale & region = tile.region;
		/*GeoScale longitude_region = m_map_region.longitude_end - m_map_region.longitude_start;
		GeoScale latitude_region = m_map_region.latitude_end - m_map_region.latitude_start;

		region.longitude_start = m_map_region.longitude_start
			+ longitude_region / current_floor.width * tile.block_col_number * m_regular_tile_size;
		if((tile.block_col_number + 1) * m_regular_tile_size >= current_floor.width)
			region.longitude_end = m_map_region.longitude_end;
		else
			region.longitude_end = region.longitude_start + longitude_region / current_floor.width * m_regular_tile_size;

		region.latitude_start = m_map_region.latitude_start 
			+ latitude_region / current_floor.height * tile.block_row_number * m_regular_tile_size;
		if((tile.block_row_number + 1) * m_regular_tile_size >= current_floor.height)
			region.latitude_end = m_map_region.latitude_end;
		else
			region.latitude_end = region.latitude_start + latitude_region / current_floor.height * m_regular_tile_size;
		return region;
		*/
		GeoScale x_region = m_map_region.x_end - m_map_region.x_start + 1;
		GeoScale y_region = m_map_region.y_end - m_map_region.y_start + 1;

		region.x_start = m_map_region.x_start + (GeoScale)(x_region * tile.block_col_number * m_regular_tile_size / current_floor.width);
		if( (tile.block_col_number + 1) * m_regular_tile_size >= current_floor.width )
			region.x_end = m_map_region.x_end;
		else
			region.x_end = region.x_start + (GeoScale)(x_region  * m_regular_tile_size / current_floor.width);

		region.y_start = m_map_region.y_start + (GeoScale)(y_region * tile.block_row_number * m_regular_tile_size / current_floor.height);
		if( (tile.block_row_number + 1) * m_regular_tile_size >= current_floor.height )
			region.y_end = m_map_region.y_end;
		else
			region.y_end = region.y_start + (GeoScale)(y_region * m_regular_tile_size / current_floor.height);
		
		return region;
	} // calcRegion(Index_Tree_Node &) 

	bool IndexTree::is_overlap(const Index_Tree_Node & tile , const GeoRegion_Scale & region)
	{
		return tile.region.is_overlap(region);
	} // is_overlap(const Index_Tree_Node & , const GeoRegion &)

	bool IndexTree::is_right_tile(Index_Tree_Node & tile , GeoRegion_Scale & region , double resolution)
	{
		// Here , we assume that (true == is_overlap(tile , region)) already
		return ( (tile.resolution - resolution) / resolution < ACCEPTABLE_RESOLUTION_LOSS_RATE ) //resolution最接近  
			|| (tile.floor_number <= 0 );//最后一层  
	} // is_right_tile(Index_Tree_Node & , GoeRegion & , double)

	inline Index_Tree_Node & IndexTree::parent(const Index_Tree_Node & tile)
	{
		if(tile.floor_number + 1 >= m_floor_amount)
			return static_cast<Index_Tree_Node>(tile);
		else
			return m_tiles[Offset(tile.floor_number + 1,tile.block_row_number /2 , tile.block_col_number /2)];
	} // parent(Index_Tree_Node & )

	inline Index_Tree_Node & IndexTree::child_NE(const Index_Tree_Node & tile)
	{
		if(tile.floor_number <= 0)
			return static_cast<Index_Tree_Node>(tile);
		else
			return m_tiles[Offset(tile.floor_number - 1 , tile.block_row_number * 2 + 1, tile.block_col_number * 2 + 1)];
	} // child_NE(const Index_Tree_Node &)

	inline Index_Tree_Node & IndexTree::child_NW(const Index_Tree_Node & tile)
	{
		if(tile.floor_number <= 0)
			return static_cast<Index_Tree_Node>(tile);
		else
			return m_tiles[Offset(tile.floor_number - 1,tile.block_row_number * 2 + 1, tile.block_col_number * 2)];
	} // child_NW(const Index_Tree_Node &)

	inline Index_Tree_Node & IndexTree::child_SE(const Index_Tree_Node & tile)
	{
		if(tile.floor_number <= 0)
			return static_cast<Index_Tree_Node>(tile);
		else
			return m_tiles[Offset(tile.floor_number - 1 , tile.block_row_number * 2 , tile.block_col_number * 2 +1)];
	} // child_SE(const Index_Tree_Node &)

	inline Index_Tree_Node & IndexTree::child_SW(const Index_Tree_Node & tile)
	{
		if(tile.floor_number <= 0)
			return static_cast<Index_Tree_Node>(tile);
		else
			return m_tiles[Offset(tile.floor_number - 1 , tile.block_row_number * 2 , tile.block_col_number * 2)];
	} // child_SW(const Index_Tree_Node & )

	Tiles IndexTree::getTiles(GeoRegion_Scale region , double resolution)
	{
		queue<Index_Tree_Node *> waiting_analysis;
		Tiles result;
		if(is_overlap(m_tiles[Offset(m_floor_amount - 1)],region))
			waiting_analysis.push(& m_tiles[Offset(m_floor_amount - 1)]);
		while( !waiting_analysis.empty() )
		{
			Index_Tree_Node & current_tile = *waiting_analysis.front();
			if(is_right_tile(current_tile,region,resolution))
			{
				result.push_back(&current_tile);
			}
			else
			{
				Index_Tree_Node & child_ne = child_NE(current_tile);
				Index_Tree_Node & child_nw = child_NW(current_tile);
				Index_Tree_Node & child_se = child_SE(current_tile);
				Index_Tree_Node & child_sw = child_SW(current_tile);
				if( is_overlap(child_ne,region) && ! is_in_queue(waiting_analysis,&child_ne) )
					waiting_analysis.push(&child_ne);
				if( is_overlap(child_nw,region) && ! is_in_queue(waiting_analysis,&child_nw) )
					waiting_analysis.push(&child_nw);
				if( is_overlap(child_se,region) && ! is_in_queue(waiting_analysis,&child_se) )
					waiting_analysis.push(&child_se);
				if( ! is_in_queue(waiting_analysis,&child_sw) && is_overlap(child_sw,region) )
					waiting_analysis.push(&child_sw);
				//std::unique(waiting_analysis.c.begin(),waiting_analysis.c.end());
			}
			waiting_analysis.pop();
		} // while
		return result;
	} // getTiles(GeoRegion , double)
//attention 2008-08-13
	Index_Tree_Node * IndexTree::getTile(z11::uint_32 floor_number,z11::uint_32 row_number,z11::uint_32 col_number)
	{
		return &m_tiles[Offset(floor_number,row_number,col_number)];
	} // getTile(z11::uint_32 , z11::uint_32 , z11::uint_32 )

	inline void IndexTree::refer(Index_Tree_Node * tile)
	{
		tile->ref_inc(m_buffer);
	} 

	inline void IndexTree::leave(Index_Tree_Node * tile)
	{
		tile->ref_dec(m_buffer);
	} 
	
	void IndexTree::newBuffer(ImageBuffer * buffer)
	{
		m_buffer = buffer;
		return ;
	} 

} // end of namespace GIS_Embedded
