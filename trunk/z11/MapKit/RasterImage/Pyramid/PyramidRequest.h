#ifndef __RASTERIMAGE_PYRAMID_REQUEST_H__
#define __RASTERIMAGE_PYRAMID_REQUEST_H__

#include "PyramidScaler.h"
#undef NULL
#define NULL 0
namespace Raster_GIS
{
namespace Pyramid_Space
{
//	namespace Index
//	{
		struct Index_Tree_Node; // here delete for its repeated defined
		class IndexTree;
//	} // end of namespace Index

//	namespace MapRequest
//	{
		//using namespace Scale_DMS_Pixel;
//		using namespace Scale_Pixel_Pixel;
//		using Index::Index_Tree_Node;   here delete by me
//		using Index::IndexTree;
		typedef enum tagPriority
		{
			SPECIAL			=	0,			// 实时读入,抢占
			HIGHER			=	1,
			NORMAL			=	2,			// 一般请求默认
			MOST_POSSIBLE	=	3,			// 在空闲时优先装入
			POSSIBLE		=	4,			// 空闲时，MOST_POSSIBLE完成后装入,预测瓦片默认
			GUESS			=	5,			// POSSIBLE完成后，若Buffer空闲则装入
			WHEN_IDLE		=	6,			// GUESS 完成后若Buffer任然空闲则装入
			PRIORITY_COUNT 	=	7
		}Priority;  

		//inline int getThreadPrio(Priority prio);

		typedef struct tagRequest
		{
			GeoRegion_Scale		region;			// 范围
			double			    resolution;		// 分辨率
		}Request;

		typedef struct tagTileBlock
		{
			Index_Tree_Node *	tile;				// 对应的IndexTree上的节点
		public:
			tagTileBlock()
			{
				tile = NULL;
			}
			~tagTileBlock()
			{
				tile = NULL;
			}
		}TileBlock;

		typedef  struct tagResult
		{
			IndexTree *			from;
			unsigned int		block_count;
			TileBlock * 		blocks;
		public:
			tagResult()
			{
				from = NULL;
				block_count = 0;
				blocks = NULL;
			}
			~tagResult();
		}Result;


//	}// namespace MapRequest
} // end of namespace Pyramid_Space
} // end of namespace IS_GIS

#endif