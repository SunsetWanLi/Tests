#ifndef _RENDER_VECTORRENDER_MAP_MAPOPRATION_H
#define _RENDER_VECTORRENDER_MAP_MAPOPRATION_H

#include "Retiguide/Common/Share/Scope.h"
#include "Retiguide/MapViews/MapView.h"
#include "Retiguide/Render/VctorRender/Map/RenderFlagAdjuster.h"
#include "Retiguide/Render/VctorRender/Map/MapTerrCtrl.h"


namespace GIS_Embedded
{
	class MapOpration
	{
	public:

	public:
		static void init(int scale_width,vector<int> &scale_ranks,MapTerrCtrl &terr_ctrl);
		//mapZoom
		static float ZoomIn(MapView *map_view);
		static float ZoomOut(MapView *map_view);
		static int Resizer(MapView* map_view,int width, int height);
		//mapLocater
		static void Locater(MapView* map_view,double x,double y);
		//mapMove
		static void moveX(MapView* map_view,double x);
		static void moveY(MapView* map_view,double y);
		static void moveXY(MapView* map_view,double x, double y);
        static void fetchData(MapView *map_view);
		//rect select
		//static void rectSelect(MapView* map_view,Scope &scope,vector<Pos2DEx>& points,vector<std::string>& names,bool flag);

	public:
		static int SCALE_WIDTH;
		static int SCALE_SIZE;
		static vector<int> SCALE;
		static MapTerrCtrl *TERR_CTRL;
		static int SCALE_CURRENT;

	};

}
#endif