#include <vector>
#include "../../CommonBase/MGStructure.h"

namespace z11
{
	//��֦����ҪĿ��ȥ����Щ������㣬����˵(-4000,-6000)֮���,����FillPolygon������Ч�� //
	class CGRegionClipper
	{
	public:
		static size_t CUT_NUM; 
		static void clipRect(std::vector<MGPoint> &points,const MGRect &rect);
		static std::vector<std::vector<MGPoint> >  clipPolyline(const std::vector<MGPoint> &points,const MGRect &rect);
	};
}