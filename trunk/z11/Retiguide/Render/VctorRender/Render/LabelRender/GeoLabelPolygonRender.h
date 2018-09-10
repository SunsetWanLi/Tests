//#ifndef _CORE_RENDERENGINE_GEOLABELPOLYGONRENDER_H
//#define _CORE_RENDERENGINE_GEOLABELPOLYGONRENDER_H
//
//#include <vector>
//#include "GeographyObject.h"
//#include "StyleRender.h"
//#include "ScreenPixMap.h"
//#include "Vertex.h"
//#include "GeoLabelRender.h"
//#include "ScreenPos2D.h"
//#include "GeometryPos2D.h"
//using namespace std;
//namespace GIS_Embedded{
//	class GeoLabelPolygonRender:public GeoLabelRender
//{
//
//private:
//	vector<ScreenPos2D*> _label_ctrl_points;
//    int _left_most_index;
//    int _right_most_index;
//    int _top_most_index;
//    int _bottom_most_index;
//    int _left_most;
//    int _right_most;
//    int _top_most;
//    int _bottom_most;
//    bool _cut_polygon_reverse;
//
//public:
//	GeoLabelPolygonRender();
//
//    void setGeo(GeographyObject& geo);
//    
//    void render(StyleRender& mg, ScreenPixMap& screen_pix_map);
//
//    /**
//     * 多边形与窗口进行裁减
//     * @author WL
//     * 
//     *
//     */
//private:
//	void getLabelCtrlPoints();
//
//    Vertex* lineCutByPolygon(Vertex& p1, Vertex& p2, Vertex& cut_polygon_head);
//
//    
//    /**
//     * 对多边形进行微调,保证其上的点不与屏幕边重合,达到简化计算的目的同时确定多边形方向
//     * @param main_polygon
//     */
//    
//    void initialMainPolygon(vector<GeometryPos2D*>& main_polygon);
//
//    Vertex* createMainPolygonList(vector<GeometryPos2D*>& polygon);
//
//    Vertex* createCutPolygonList();
//
//    void insertToMainPolygonByOrder(Vertex& p1, Vertex& p2, Vertex& p);
//
//    void initialMostPoint();
//
//    void computeMostPoint(int x, int y, int i);
//
//    ScreenPos2D* getMidPointOnYLineInsPolygon(int x);
//
//    ScreenPos2D* getMidPointOnXLineInsPolygon(int y);
//
//    ScreenPos2D* computeIntersectWithXline(ScreenPos2D& pos1, ScreenPos2D& pos2, int y);
//
//    ScreenPos2D* computeIntersectWithYline(ScreenPos2D& pos1, ScreenPos2D& pos2, int x);
//
//    ScreenPos2D* findIntersectPoint(vector<ScreenPos2D*>& label_ctrl_points, int low, int high, int value, int direction);
//};
//}
//#endif