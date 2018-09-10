#ifndef _CORE_SHARE_SCREENPOS2D_H
#define _CORE_SHARE_SCREENPOS2D_H

namespace GIS_Embedded{
class ScreenPos2D
{

public:
	int X;
    int Y;

	ScreenPos2D()
	{
		X = 0;
		Y = 0;

	}

    ScreenPos2D(int x, int y)
    {
        X = x;
        Y = y;
    }

    bool equal(ScreenPos2D& pos)
    {
        return pos.X == X && pos.Y == Y;
    }

    int distance(ScreenPos2D& pos);
};


}

#endif