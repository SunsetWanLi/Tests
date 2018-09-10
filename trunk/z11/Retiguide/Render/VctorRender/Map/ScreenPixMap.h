#ifndef _CORE_SHARE_SCREENPIXMAP_H
#define _CORE_SHARE_SCREENPIXMAP_H

#include "Retiguide/Common/Share/UserScreenRect.h"
#include "Retiguide/Common/Share/ScreenPos2D.h"
#include "CommonBase/MGStructure.h"
#include <vector>
using namespace std;
using namespace z11;

namespace GIS_Embedded{
	class ScreenPixMap
{
private:
	vector<unsigned char> _map;
    int _length;
    int _unit_width;
    int _unit_height;
    int _rows;
    int _cols;
    unsigned char _dirty;

public:
	z11::MGRect2 m_screen_rect;

public:
	ScreenPixMap();

	void setScreenSize(int scr_width,int scr_height, int unit_width, int unit_height);
	void setScreenSizeUpToDown(int scr_width,int scr_height,int unit_width,int unit_height);

    void clear();

	bool testDirty(UserScreenRect& screen_rect);
	bool testDirty(/*vector<pair<int,int> >* test,*/z11::MGPoint& from,z11::MGPoint& to,z11::MGSize& labelSize);
	bool testDirty(z11::MGPoint& left_top,z11::MGSize& size);

	void setDirty(UserScreenRect& screen_rect);
	void setDirty(z11::MGPoint& left_top,z11::MGSize& size);
	void setDirty(z11::MGPoint& from,z11::MGPoint& to,z11::MGSize& labelSize);


};
}
#endif