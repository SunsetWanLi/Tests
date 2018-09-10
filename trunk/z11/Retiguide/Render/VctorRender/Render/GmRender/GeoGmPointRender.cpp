#include <vector>
#include "GeoGmPointRender.h"
#include "Retiguide/Common/Util/Algorithmic.h"
#include "Retiguide/Common/Types/basic_types.h"
#include "MapKit/Embedded/BasicType/Point.h"

using namespace Embedded;
namespace GIS_Embedded
{
	int GeoGmPointRender::render(StyleRender& mg)
	{
		Embedded::Point* gm_point = dynamic_cast< Embedded::Point*>(m_geo);
		Embedded::ScreenPos2D pos;
		gm_point->getPoint(&pos);

		if(m_transformer->transfromMapToScr(pos.d0,pos.d1,&(m_gm_buff[0].x),&(m_gm_buff[0].y)) >=0)
			mg.drawPoint(m_gm_buff[0].x, m_gm_buff[0].y);

		return 0;
	}

}
