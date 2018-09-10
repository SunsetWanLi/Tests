
#ifndef _RASTERIMAGE_PYRAMID_H_
#define _RASTERIMAGE_PYRAMID_H_

#include "./PyramidScaler.h"
#include "./PyramidRequest.h"
#include "Retiguide/Common/crs/basic_gm_type.h"
#include "System/Thread/MGThread.h"
#include <string>
#include <queue>
#include <vector>
#include <map>

namespace z11
{
	class MGSemaphore;
}
namespace GIS_Embedded
{
	enum ThreadPriorities
	{
		THREAD_TIME_CRITICAL,
		THREAD_HIGHEST,
		THREAD_ABOVE_NORMAL,
		THREAD_NORMAL,
		THREAD_BELOW_NORMAL,
		THREAD_LOWEST,
		THREAD_IDLE
	};

	enum PYRAMID_IMAGE_TYPE;
	class IndexTree;
	struct Index_Tree_Node;
	struct Floor;
	typedef struct tagResult Result;
	class ImageBuffer;
	typedef std::vector<Index_Tree_Node *>  Tiles;

	typedef	struct tagTILE_PYRAMID
	{
		MapScale		orig_width,orig_height;	//第0层像素矩阵大小  
		GeoRegion_Scale region;					//坐标范围  
		double 			orig_resolution;		//初始分辨率 [单位像素表示的实际距离 meter / pixel]  
		z11::uint_32 	tile_size;				//瓦片大小  
		z11::uint_32	samp_multi;				//采样倍率  
		int 			samp_method;			//采样方法     // 暂未使用  
		z11::uint_32	level;					//金字塔层数  
		//PYRAMID_IMAGE_TYPE image_type;
		RasterType		raster_type;			//栅格类型  
		std::string		raster_name;			//栅格名称  
		RepositoryType	repo_type;				//存储类型  
		std::string		repo_name;				//存储位置（db：表名；file：根目录）  
	}TILE_PYRAMID;

	// 处理Application对金字塔的请求, 管理请求队列  
	class Pyramid : public z11::MGThread
	{		
	protected:

		struct Tile_Request
		{
			std::string			raster_name;
			int					floor, row, col;
			Index_Tree_Node		*tile;
			int					prio;
		};

		TILE_PYRAMID 		m_pyramid_info;		
		//std::string 		m_map_name;		//image_name;
		//std::string		m_pyramid_root_path;

		IndexTree			*m_index;
		ImageBuffer			*m_buffer;
		bool				m_outter_buffer;
		Floor				*m_floor_info;
		bool				m_exiting;
		bool				m_can_exit;
		z11::uint_32		m_ref_count;
		static z11::int_32		m_thread_priority;

		std::queue<Tile_Request *> m_requests[/*MapRequest::*/PRIORITY_COUNT];
		z11::MGSemaphore * semaphore_of_queue;
		z11::MGSemaphore * semaphore_of_ref_cnt;
		static unsigned int __cdecl deal_with_request(Pyramid * this_ptr);
		int initialize(const TILE_PYRAMID& pyramid_info);
		//
		static void setThreadPriority(z11::int_32 priority);
		static z11::int_32 getThreadPriority();
		virtual bool threadLoop();

	public:
		Pyramid(const TILE_PYRAMID& pyramid_info, ImageBuffer * buffer = NULL);
		Tiles indexTiles(const Request & request);
		Result * getMap(const Request & request);
		Result * getTile(z11::uint_32 floor_number ,z11::uint_32 row_number, z11::uint_32 col_number);
		void reserveMap(const Request & request , /*MapRequest::*/Priority priority = /*MapRequest::*/POSSIBLE);
		void reserveTile(z11::uint_32 floor_number , z11::uint_32 row_number ,
			z11::uint_32 col_number , /*MapRequest::*/Priority priority = /*MapRequest::*/POSSIBLE);
		~Pyramid();
		
		// Reference count ------transplant

		void refer();
		int leave();

		int getImageWidth( )const;
		int getImageHeight()const;
	public://by hqs,为了支持金字塔变换 2008-05-26
		void getTilePyramidInfo(TILE_PYRAMID* info){*info = m_pyramid_info;}
	}; // class Pyramid	

	

} // end of namespace GIS_Embedded
#endif
