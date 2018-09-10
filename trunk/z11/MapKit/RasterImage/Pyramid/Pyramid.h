#ifndef __RASTERIMAGE_PYRAMID_H__
#define __RASTERIMAGE_PYRAMID_H__

#include "PyramidSettings.h"
#include "PyramidScaler.h"
#include "PyramidRequest.h"
#include <string>
#include <queue>
#include <vector>
#include <map>
//typedef void * HANDLE;
using namespace std;
namespace Raster_GIS
{

class Semaphore;


namespace Pyramid_Space
{

	//enum PYRAMID_IMAGE_TYPE;
//	namespace Index
//	{
		class IndexTree;
		struct Index_Tree_Node;
		struct Floor;
		typedef std::vector<Index_Tree_Node *>  Tiles;


//	} // end of namespace Index

//	namespace Buffer
//	{
//	} // end of namespace Buffer

//	using Buffer::ImageBuffer;
	//using namespace Scale_DMS_Pixel;
//	using namespace Scale_Pixel_Pixel;
//	using Index::IndexTree;                //delete from here
//	using Index::Index_Tree_Node;
//	using Index::Floor;                    //delete Index end of here
//	using namespace MapRequest;

	typedef	struct tagTILE_PYRAMID
	{
		MapScale		orig_width,orig_height;	//第0层像素矩阵大小
		GeoRegion_Scale region;					//坐标范围
		double 			orig_resolution;		//初始分辨率 [单位像素表示的实际距离 meter / pixel]
		unsigned int 	tile_size;				//瓦片大小
		unsigned int	sampMulti;				//采样倍率
		int 			sampMethod;				//采样方法     // 暂未使用
		unsigned int	level;					//金字塔层数
		PYRAMID_IMAGE_TYPE image_type;
	}TILE_PYRAMID;




	// 处理Application对金字塔的请求, 管理请求队列
	class Pyramid
	{		
	protected:

		struct Tile_Request
		{
			std::string		file_name;
			int floor, row, col;
			Index_Tree_Node	*	tile;
			int					prio;
		};


		TILE_PYRAMID 		m_pyramid_info;		
		std::string 		m_map_name;		//image_name;
		//std::std::string		m_pyramid_root_path;

		IndexTree *			m_index;
		bool				m_outter_buffer;
		Floor		*		m_floor_info;
		bool				m_exiting;
		bool				m_can_exit;
		

		// Reference count ------  transplant
		unsigned int m_ref_count;
		
//		bool  m_is_single_file;
//		bool  m_is_load_at_first;
		//struct ImageFileInfo
		//{
		//	int position;
		//	int length;
		//};
		//map<string, ImageFileInfo> m_file_pos_info;
		//std::string m_extent_name[4];
		//char * m_image_file_buf;	
		//FILE * m_image_file_ptr;	

		std::queue<Tile_Request *> m_requests[/*MapRequest::*/PRIORITY_COUNT];
		int initialize();
	//	std::std::string getConfFile(); 		
	//	std::std::string getPositionFile(); 
	//private:
	//	string getImageName()const;
	public:
		Pyramid(/*const std::std::string root_path, */const std::string map_name);
		/*Index::*/Tiles indexTiles(const Request & request);
		Result * getMap(const Request & request , bool single_thread = true);
		Result * getTile(unsigned int floor_number ,unsigned int row_number, unsigned int col_number);
		void reserveMap(const Request & request , /*MapRequest::*/Priority priority = /*MapRequest::*/POSSIBLE);
		void reserveTile(unsigned int floor_number , unsigned int row_number ,
			unsigned int col_number , /*MapRequest::*/Priority priority = /*MapRequest::*/POSSIBLE);
		virtual ~Pyramid();
		
		// Reference count ------transplant

		void refer();
		int leave();

		int getImageWidth( )const;
		int getImageHeight()const;
	public://by hqs,为了支持金字塔变换 2008-05-26
		void getTilePyramidInfo(TILE_PYRAMID* info){*info = m_pyramid_info;}
	}; // class Pyramid	
} // end of namespace Pyramid_Space
} // end of namespace Raster_GIS
#endif
