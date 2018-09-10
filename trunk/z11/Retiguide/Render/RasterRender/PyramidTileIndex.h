#ifndef _RASTERIMAGE_PYRAMID_TILE_INDEX_H_
#define _RASTERIMAGE_PYRAMID_TILE_INDEX_H_

#pragma once

#include "PyramidScaler.h"
#include "Retiguide/Common/crs/basic_gm_type.h"
#include <vector>
#include <queue>

namespace z11
{
	class MGSemaphore;
}
namespace GIS_Embedded
{
	struct ImageBufferEntry;
	class ImageBuffer;
	template <class T_val>
	inline bool is_in_queue(std::queue<T_val > & que , T_val  val)
	{
		//std::deque<T_val >::const_iterator re = find(que.c.begin(),que.c.end(),val);
		//return !( re >= que.c.end() );
		return false;
	};


	struct Floor
	{
		MapScale			width , height;
		unsigned int		row_number;
		unsigned int		regular_per_row;
		unsigned int		non_regular_per_row;
		double				resolution;    
	public:
		Floor()
		{
			width = height = 0;
			row_number = regular_per_row = non_regular_per_row = 0;
			resolution = 0.0;
		}
	}; 

	struct Index_Tree_Node
	{
		unsigned int			floor_number;
		unsigned int			block_row_number;
		unsigned int			block_col_number;
		double					resolution;
		GeoRegion_Scale			region;
		unsigned int			reference_count;
		ImageBufferEntry *		buffer;	
		z11::MGSemaphore *		node_semaphore;
	public:
		Index_Tree_Node();
		~Index_Tree_Node();
		void ref_inc(ImageBuffer * buffer);
		void ref_dec(ImageBuffer * buffer);
	}; 

	typedef std::vector<Index_Tree_Node *> Tiles;

	class IndexTree
	{
	protected:
		Floor *					m_info_of_floor;
		unsigned int			m_floor_amount;
		unsigned int			m_regular_tile_size;
		unsigned int			m_tile_count;
		Index_Tree_Node *		m_tiles;
		unsigned int	*		m_floor_offsets;
		GeoRegion_Scale			m_map_region;

		z11::MGSemaphore *		m_tree_semaphore;
		ImageBuffer				*m_buffer;

		void build_tree();
		void base_calc(); 
		inline unsigned int Offset(unsigned int floor_number = 0 , unsigned int row = 0 ,unsigned int col = 0); 

		virtual inline GeoRegion_Scale calcRegion(Index_Tree_Node & tile);
		virtual bool is_overlap(const Index_Tree_Node & tile , const GeoRegion_Scale & region);
		virtual bool is_right_tile(Index_Tree_Node & tile , GeoRegion_Scale & region , double resolution); 
	public:
		IndexTree(Floor * floor_info,GeoRegion_Scale map_region,unsigned int floor_amount , unsigned int regular_size);
		virtual ~IndexTree(); 
		virtual Tiles getTiles(GeoRegion_Scale region , double resolution);
		Index_Tree_Node * getTile(unsigned int floor_number,unsigned int row_number,unsigned int col_number);
		virtual inline Index_Tree_Node& parent(const Index_Tree_Node & tile); 
		virtual inline Index_Tree_Node& child_NE(const Index_Tree_Node & tile); 
		virtual inline Index_Tree_Node& child_NW(const Index_Tree_Node & tile); 
		virtual inline Index_Tree_Node& child_SE(const Index_Tree_Node & tile); 
		virtual inline Index_Tree_Node& child_SW(const Index_Tree_Node & tile); 

		virtual inline void refer(Index_Tree_Node * tile); 
		virtual inline void leave(Index_Tree_Node * tile); 
		void newBuffer(/*Buffer::*/ImageBuffer * buffer);
	}; 


} // end of namespace GIS_Embedded

#endif
