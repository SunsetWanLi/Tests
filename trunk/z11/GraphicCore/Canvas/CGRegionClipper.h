#include <vector>
#include "../../CommonBase/MGStructure.h"

namespace z11
{
	//剪枝的主要目的去除那些不合理点，比如说(-4000,-6000)之类的,对于FillPolygon有明显效果 //
	class CGRegionClipper
	{
	public:
		static size_t CUT_NUM; 
		static void clipRect(std::vector<MGPoint> &points,const MGRect &rect);
		static std::vector<std::vector<MGPoint> >  clipPolyline(const std::vector<MGPoint> &points,const MGRect &rect);
	};
}