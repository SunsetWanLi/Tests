
#include "ScreenPos2D.h"
#include <math.h>

int GIS_Embedded::ScreenPos2D::distance( ScreenPos2D& pos )
{
	int dis_x = pos.X - X;
	int dis_y = pos.Y - Y;
	double dis = sqrt(0.0+dis_x * dis_x + dis_y * dis_y);
	return (int)dis;
}
