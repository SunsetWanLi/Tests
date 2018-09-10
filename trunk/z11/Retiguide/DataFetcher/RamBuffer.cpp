#include "RamBuffer.h"
#include "PathDef.h"

namespace GIS_Embedded
{
// 	int RamBuffer::getBitmap(const z11::MGWString &map_name, HBITMAP &map_handler)
// 	{
// 		if(m_bitmaps.empty())
// 			return -1;
// 		if(m_bitmaps.find(map_name) == m_bitmaps.end())
// 			return -1;
// 		map_handler = m_bitmaps[map_name];
// 		return 0;
// 	}

// 	int RamBuffer::getBitmap(const z11::MGWString &map_name, MyBITMAP *bit_map)
// 	{
// 		if(m_MyBitmaps.empty())
// 			return -1;
// 		if(m_MyBitmaps.find(map_name) == m_MyBitmaps.end())
// 			return -1;
// 		*bit_map = m_MyBitmaps[map_name];
// 		return 0;
// 	}
// 	int RamBuffer::deleteBitmapRam(const z11::MGWString map_name)
// 	{
// 		if(m_MyBitmaps.empty())
// 			return -1;
// 		map<z11::MGWString, MyBITMAP>::iterator iter = m_MyBitmaps.find(map_name);
// 		if(m_MyBitmaps.end() == iter)
// 			return -1;
// 		m_MyBitmaps.erase(iter);
// 		return 0;
// 	}
// 	int RamBuffer::saveBitmapRam(const z11::MGWString map_name, HBITMAP &map_handler)
// 	{
// 		m_bitmaps[map_name] = map_handler;
// 		return 0;
// 	}
// 	int RamBuffer::saveBitmapRam(const z11::MGWString map_name, MyBITMAP bit_map)
// 	{
// 		m_MyBitmaps[map_name] = bit_map;
// 		return 0;
// 	}

 	int RamBuffer::spaceFree(int size)
 	{
 		//return spaceFreeGeo(size);
		return 0;//temp lw
 	}
}