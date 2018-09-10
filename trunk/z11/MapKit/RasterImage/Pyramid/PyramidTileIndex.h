
#ifndef __RASTERIMAGE_PYRAMID_TILE_INDEX_H__
#define __RASTERIMAGE_PYRAMID_TILE_INDEX_H__

#include "PyramidScaler.h"
#include <vector>
#include <queue>
#include <set>

typedef void * HANDLE;
namespace Raster_GIS
{

class Semaphore;
namespace Pyramid_Space
{

//	namespace Buffer
//	{
		struct ImageBufferEntry;
		class ImageBuffer;
//	} // end of namespace Buffer

//	namespace Index
//	{
		
//		using namespace /*Pyramid_Space::*/Scale_Pixel_Pixel;
//		using Buffer::ImageBufferEntry;
//		using Buffer::ImageBuffer;
		using namespace std;

		template <class T_val>
		inline bool is_in_queue(std::vector<T_val > & que , T_val  val)
		{
			typename std::vector<T_val >::const_iterator re = find(que.begin(),que.end(),val);
			return !( re >= que.end() );
		};
		template <class T_val>
		inline void detach_front_from_queue(std::vector<T_val > & que)
		{
			typename std::vector<T_val >::iterator re;
			for (re=que.begin(); re!=que.end(); re++)
			{
				que.erase(re);
				break;
			}
		}


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
		}; // Floor
		
		struct Index_Tree_Node
		{
			unsigned int			floor_number;
			unsigned int			block_row_number;
			unsigned int			block_col_number;
			double					resolution;
			GeoRegion_Scale			region;
			unsigned int			reference_count;
			bool					is_regular_tile;
		public:
			Index_Tree_Node();
			~Index_Tree_Node();
		}; // Index_Tree_Node
		
		typedef vector<Index_Tree_Node *> Tiles;

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
			
			void build_tree();
			void base_calc(); 
			inline unsigned int Offset(unsigned int floor_number = 0 , unsigned int row = 0 ,unsigned int col = 0)
			{
				if(floor_number >= m_floor_amount)floor_number = m_floor_amount - 1;
				if(row >= m_info_of_floor[floor_number].row_number)row = m_info_of_floor[floor_number].row_number - 1;
				if(col >= (m_info_of_floor[floor_number].regular_per_row + m_info_of_floor[floor_number].non_regular_per_row) )
					col = m_info_of_floor[floor_number].regular_per_row + m_info_of_floor[floor_number].non_regular_per_row - 1;
				return m_floor_offsets[floor_number] 
				+ row * (m_info_of_floor[floor_number].regular_per_row + m_info_of_floor[floor_number].non_regular_per_row)
					+ col;
			}
			
			virtual inline GeoRegion_Scale calcRegion(Index_Tree_Node & tile)
			{
				Floor & current_floor = m_info_of_floor[tile.floor_number];
				GeoRegion_Scale & region = tile.region;

				GeoScale x_region = m_map_region.x_end - m_map_region.x_start + 1;
				GeoScale y_region = m_map_region.y_end - m_map_region.y_start + 1;

				region.x_start = m_map_region.x_start + (unsigned long long)x_region * tile.block_col_number * m_regular_tile_size / current_floor.width; 
				if( (tile.block_col_number + 1) * m_regular_tile_size >= current_floor.width )
				{
					region.x_end = m_map_region.x_end;
					tile.is_regular_tile = false;
				}
				else
					region.x_end = region.x_start + (unsigned long long)x_region  * m_regular_tile_size / current_floor.width;

				region.y_start = m_map_region.y_start + (unsigned long long)y_region * tile.block_row_number * m_regular_tile_size / current_floor.height;
				if( (tile.block_row_number + 1) * m_regular_tile_size >= current_floor.height )
				{
					region.y_end = m_map_region.y_end;
					tile.is_regular_tile = false;
				}
				else
					region.y_end = region.y_start + (unsigned long long)y_region * m_regular_tile_size / current_floor.height;

				return region;
			}
			virtual bool is_overlap(const Index_Tree_Node & tile , const GeoRegion_Scale & region);
			virtual bool is_right_tile(Index_Tree_Node & tile , GeoRegion_Scale & region , double resolution); 
		public:
			IndexTree(Floor * floor_info,GeoRegion_Scale map_region,unsigned int floor_amount , unsigned int regular_size);
			virtual ~IndexTree(); 
			virtual Tiles getTiles(GeoRegion_Scale region , double resolution);
			Index_Tree_Node * getTile(unsigned int floor_number,unsigned int row_number,unsigned int col_number);

			virtual  Index_Tree_Node& parent(  Index_Tree_Node & tile)
			{
				if(tile.floor_number + 1 >= m_floor_amount)
					return /*static_cast<Index_Tree_Node>*/(tile);
				else
					return m_tiles[Offset(tile.floor_number + 1,tile.block_row_number /2 , tile.block_col_number /2)];
			}
			virtual  Index_Tree_Node & child_NE(  Index_Tree_Node & tile)
			{
				if(tile.floor_number <= 0)
					return /*static_cast<Index_Tree_Node>*/(tile);
				else
					return m_tiles[Offset(tile.floor_number - 1 , tile.block_row_number * 2 + 1, tile.block_col_number * 2 + 1)];
			} // child_NE(const Index_Tree_Node &)

			virtual  Index_Tree_Node & child_NW(  Index_Tree_Node & tile)
			{
				if(tile.floor_number <= 0)
					return /*static_cast<Index_Tree_Node>*/(tile);
				else
					return m_tiles[Offset(tile.floor_number - 1,tile.block_row_number * 2 + 1, tile.block_col_number * 2)];
			} // child_NW(const Index_Tree_Node &)

			virtual  Index_Tree_Node & child_SE(  Index_Tree_Node & tile)
			{
				if(tile.floor_number <= 0)
					return /*static_cast<Index_Tree_Node>*/(tile);
				else
					return m_tiles[Offset(tile.floor_number - 1 , tile.block_row_number * 2 , tile.block_col_number * 2 +1)];
			} // child_SE(const Index_Tree_Node &)

			virtual  Index_Tree_Node & child_SW(  Index_Tree_Node & tile)
			{
				if(tile.floor_number <= 0)
					return /*static_cast<Index_Tree_Node>*/(tile);
				else
					return m_tiles[Offset(tile.floor_number - 1 , tile.block_row_number * 2 , tile.block_col_number * 2)];
			} // child_SW(const Index_Tree_Node & )

			//virtual inline Index_Tree_Node& child_NE( Index_Tree_Node & tile); 
			//virtual inline Index_Tree_Node& child_NW( Index_Tree_Node & tile); 
			//virtual inline Index_Tree_Node& child_SE( Index_Tree_Node & tile); 
			//virtual inline Index_Tree_Node& child_SW( Index_Tree_Node & tile); 

		}; // IndexTree


//	} // end of namespace index

} // end of namespace Pyramid_Space
} // end of namespace Raster_GIS

#endif
