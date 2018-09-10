#ifndef _CORE_SHARE_USERSCREENRECT_H
#define _CORE_SHARE_USERSCREENRECT_H

namespace GIS_Embedded{
class UserScreenRect
{

public:
	int _left;
    int _top;
    int _right;
    int _bottom;
	int _width() const
	{
		return _right - _left;
	}
	int _height() const
	{
		return _bottom - _top;
	}

	UserScreenRect()
	{
		_left = 0;
		_top = 0;
		_right = 0;
		_bottom = 0;
		//_width = 0;
		//_height = 0;
	}

    UserScreenRect(int left, int top, int right, int bottom)
    {
        _left = left;
        _top = top;
        _right = right;
        _bottom = bottom;

		//_width = _right - _left;
		//_height = _bottom - _top;
    }

    bool isContain(int x, int y)
    {
        return x > _left && x < _right && y > _top && y < _bottom;
    }

};

}

#endif