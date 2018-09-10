#ifndef _RAM_BUFFER_H_
#define _RAM_BUFFER_H_

#include "../Common/Types/DataFetcherTypes/TypeDefs.h"
#include "../Common/Types/basic_types.h"
#include "TypeDefs.h"

#include <map>
#include <vector>
//#include <comutil.h>
#include "IBuffer.h"

using namespace std;


namespace GIS_Embedded
{

	class RamBuffer : public IBuffer	//负责内存缓存管理 读取内存的数据 有内存替换策略
	{
	public:
		RamBuffer(){}
		~RamBuffer(){}
	private:
		//int block_count;	//已有缓存块数
		//map<z11::MGWString, RamBlock *> m_blocks;//各个内存块的内容
// 		map<int, RamBlock *> m_blocks;//各个内存块的内容
//		map<z11::MGWString, HBITMAP> m_bitmaps;//图片文件
// 		map<z11::MGWString, MyBITMAP> m_MyBitmaps;//图片文件
		//map<z11::MGWString, NWGridBlock *> m_nw_grid_blocks;

// 		int spaceFreeGeo(int size);
		//int spaceFreeNW(int size);

	public:
		int curr_used_space;	//已使用字节数
		int init()
		{
			//初始化m_blocks;
			curr_used_space = 0;
			return 0;
		}

		int availableSize() const
		{
			//返回剩余空间大小
			return RAM_MAX_SIZE - curr_used_space;
		}
 		int spaceFree(int size);	//内存替换策略 size表示需要的空间数
		//int saveRam(const z11::MGWString & fcls_id, int index, vector<GeographyObject> *, int size, vector<GeometryPoint> *point_buff, vector<GeometryLine> *line_buff, vector<GeometryPolygon> *polygon_buff);
		//int saveRam(int fcls_id, int index, vector<GeographyObject> *, int size, vector<GeometryPoint> *point_buff, vector<GeometryLine> *line_buff, vector<GeometryPolygon> *polygon_buff, vector<GeometryImage> *image_buff);

// 		int getBitmap(const z11::MGWString &map_name, HBITMAP &map_handler);
// 		int saveBitmapRam(const z11::MGWString map_name, HBITMAP &map_handler);

// 		int getBitmap(const z11::MGWString &map_name, MyBITMAP *bit_map);
// 		int saveBitmapRam(const z11::MGWString map_name, MyBITMAP bit_map);
// 		int deleteBitmapRam(const z11::MGWString map_name);
		//int saveRam(const z11::MGWString & fcls_id, int index, vector<p_Ref<SNW_RoadElement> > *nw_road_elements, int size);

	};

}

#endif