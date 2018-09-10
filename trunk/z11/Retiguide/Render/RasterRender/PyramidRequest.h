#ifndef _RASTERIMAGE_PYRAMID_REQUEST_H_
#define _RASTERIMAGE_PYRAMID_REQUEST_H_

#include "./PyramidScaler.h"
#include "Retiguide/Common/crs/basic_gm_type.h"

namespace z11
{
	class MGColor;
}
namespace GIS_Embedded
{
	struct Index_Tree_Node; // here delete for its repeated defined
	class IndexTree;
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

	typedef struct tagRequest
	{
		GeoRegion_Scale		region;			// 范围  
		double			    resolution;		// 分辨率  
	}Request;

	typedef struct tagTileBlock
	{
		Index_Tree_Node *	tile;				// 对应的IndexTree上的节点  
		z11::MGColor *		color;				// 指向对应图的数据  
		MapScale 			width,height;		// 图大小  
		bool				ready;				// 是否以全部在内存  
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
		z11::uint_32		block_count;
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

} // end of namespace GIS_Embedded

#endif