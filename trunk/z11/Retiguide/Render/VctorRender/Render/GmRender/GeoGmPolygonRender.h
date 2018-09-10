#ifndef	_CORE_ENDERENGINE_GEOGMPOLYGONRENDER_H
#define _CORE_ENDERENGINE_GEOGMPOLYGONRENDER_H

#include "GeoGmRender.h"
#include "../StyleRender.h"
#include <vector>
using namespace std;

namespace GIS_Embedded{

class GeoGmPolygonRender:public GeoGmRender
{


public:
	GeoGmPolygonRender(){}
	~GeoGmPolygonRender(){}
   
    int render(StyleRender& mg);

private:
	void resizeClipBuff(int orig_sum)
	{
		int ret_size = orig_sum * 2 + 6;
		if(ret_size > m_clip_buff_size)
		{
			m_clip_buff.resize(ret_size);
			m_clip_buff_size = ret_size;
		}
	}

private:
	static vector<Pos2DEx> m_clip_buff;
	static int m_clip_buff_size;

};

}

#endif