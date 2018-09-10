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
			SPECIAL			=	0,			// ʵʱ����,��ռ
			HIGHER			=	1,
			NORMAL			=	2,			// һ������Ĭ��
			MOST_POSSIBLE	=	3,			// �ڿ���ʱ����װ��
			POSSIBLE		=	4,			// ����ʱ��MOST_POSSIBLE��ɺ�װ��,Ԥ����ƬĬ��
			GUESS			=	5,			// POSSIBLE��ɺ���Buffer������װ��
			WHEN_IDLE		=	6,			// GUESS ��ɺ���Buffer��Ȼ������װ��
			PRIORITY_COUNT 	=	7
		}Priority;  

		//inline int getThreadPrio(Priority prio);

		typedef struct tagRequest
		{
			GeoRegion_Scale		region;			// ��Χ
			double			    resolution;		// �ֱ���
		}Request;

		typedef struct tagTileBlock
		{
			Index_Tree_Node *	tile;				// ��Ӧ��IndexTree�ϵĽڵ�
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