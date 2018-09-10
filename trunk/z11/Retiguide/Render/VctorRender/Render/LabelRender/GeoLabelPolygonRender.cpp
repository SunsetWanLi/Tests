//#include "stdafx.h"
//#include "GeoLabelPolygonRender.h"
//#include "GeometryPolygon.h"
//#include "Algorithmic.h"
//using namespace GIS_Embedded;
//
//GeoLabelPolygonRender::GeoLabelPolygonRender()
//{
//
//}
//
//void GeoLabelPolygonRender::setGeo(GeographyObject& geo)
//{
//	_geo=&geo;
//}
//
//void GeoLabelPolygonRender::render(StyleRender& mg, ScreenPixMap& screen_pix_map)
//{
//	GeoLabelRender::render(mg, screen_pix_map);
//    wstring label = _geo->getName();
//    if(label == WS(L""))
//        return;
//  
//    
//    getLabelCtrlPoints();
//    
//    
//    
//    
//    if(_label_ctrl_points.size() == 0)
//        return;
//    int length = label.length();
//    if(_right_most - _left_most > _bottom_most - _top_most)
//    {
//        int step = (_right_most - _left_most) / (length + 1);
//        if(step < 16)
//            return;
//        for(int i = 1; i <= length; i++)
//        {
//            int yline_x = _left_most + i * step;
//            ScreenPos2D* pos = getMidPointOnYLineInsPolygon(yline_x);
//            if(conflictTest(mg, screen_pix_map, *pos, label.substr(i - 1, 1)))
//                return;
//        }
//
//    } else
//    {
//        int step = (_bottom_most - _top_most) / (length + 1);
//        if(step < 16)
//            return;
//        for(int i = 1; i <= length; i++)
//        {
//            int xline_y = _top_most + i * step;
//            ScreenPos2D* pos = getMidPointOnXLineInsPolygon(xline_y);
//            if(conflictTest(mg, screen_pix_map, *pos, label.substr(i - 1, 1)))
//                return;
//        }
//
//    }
//    drawString(mg, screen_pix_map);
//}
//
///**
// * 多边形与窗口进行裁减
// * @author WL
// * 
// *
// */
//void GeoLabelPolygonRender::getLabelCtrlPoints()
//{
//    GeometryPolygon* gm_polygon = (GeometryPolygon*)_geo->getGeometry();
//    
//   
//   
//   
//    vector<GeometryPos2D*> pos2Ds;
//	gm_polygon->getExteriorRing()->getPos2Ds(pos2Ds);
//    int sum = pos2Ds.size();
//	_label_ctrl_points.clear();
//   
//    initialMainPolygon(pos2Ds);
//   
//    Vertex* main_polygon_list = createMainPolygonList(pos2Ds);
//    Vertex* cut_polygon_list = createCutPolygonList();
//   
//    Vertex* cut_result_list = NULL;
//    Vertex* p1 = main_polygon_list;
//    Vertex* p2 = main_polygon_list->_next_to_main_polygon;
//   
//    do
//    {
//        if(!_transformer->getScreenScope()->isContain(p1->_p->_x, p1->_p->_y))
//        {
//            cut_result_list = lineCutByPolygon(*p1, *p2, *cut_polygon_list);
//            if(cut_result_list != NULL)
//                break;
//        }
//        p1 = p2;
//        p2 = p1->_next_to_main_polygon;
//    } while(p1 != main_polygon_list);
//    if(cut_result_list == NULL)
//    {
//        if(_transformer->getScreenScope()->isContain(p1->_p->_x, p1->_p->_y))
//        {
//            initialMostPoint();
//            for(unsigned int i = 0; i < pos2Ds.size(); i++)
//            {
//                GeometryPos2D* map_pos = (GeometryPos2D*)pos2Ds[i];
//                int src_x = _transformer->transformXaccordingView(map_pos->_x);
//                int src_y = _transformer->transformYaccordingView(map_pos->_y);
//                computeMostPoint(src_x, src_y, i);
//                _label_ctrl_points.push_back(new ScreenPos2D(src_x, src_y));
//            }
//
//        } else
//        {
//            _geo->getGeometry()->getBound()->isContain(*_transformer->getScreenScope());
//        }
//        return;
//    }
//    main_polygon_list = p1;
//    p1 = p2;
//    p2 = p1->_next_to_main_polygon;
//    do
//    {
//        lineCutByPolygon(*p1, *p2, *cut_polygon_list);
//        p1 = p2;
//        p2 = p1->_next_to_main_polygon;
//    } while(p1 != main_polygon_list);
//    Vertex* p = cut_result_list;
//    initialMostPoint();
//    bool ON_MAIN_POLYGON = false;
//    do
//    {
//        GeometryPos2D* map_pos = p->_p;
//        int src_x = _transformer->transformXaccordingView(map_pos->_x);
//        int src_y = _transformer->transformYaccordingView(map_pos->_y);
//        computeMostPoint(src_x, src_y, _label_ctrl_points.size());
//		_label_ctrl_points.push_back(new ScreenPos2D(src_x, src_y));
//        if(p->IS_INTERSECTION)
//        {
//            if(ON_MAIN_POLYGON)
//            {
//                ON_MAIN_POLYGON = false;
//                p = p->_next_to_cut_polygon;
//            } 
//            else
//            {
//                ON_MAIN_POLYGON = true;
//                p = p->_next_to_main_polygon;
//            }
//        } 
//        else
//        if(ON_MAIN_POLYGON)
//            p = p->_next_to_main_polygon;
//        else
//            p = p->_next_to_cut_polygon;
//    } while(p != cut_result_list);
//}
//
//Vertex* GeoLabelPolygonRender::lineCutByPolygon(Vertex& p1, Vertex& p2, Vertex& cut_polygon_head)
//{
//    Vertex* ret = NULL;
//    bool is_begin = false;
//    Vertex* p3 = &cut_polygon_head;
//    Vertex* p4 = p3->_next_to_cut_polygon;
//    do
//    {
//        if(p3 == &cut_polygon_head)
//        {
//            if(is_begin)
//                break;
//            is_begin = true;
//        }
//		GeometryPos2D* pos = GIS_Embedded::Algorithmic::computeLineIntersect(*(p1._p), *(p2._p), *(p3->_p), *(p4->_p));
//        if(pos != NULL)
//        {
//            Vertex* p = new Vertex(*pos);
//            p->IS_INTERSECTION = true;
//            p3->insertVertexToCutPolygon(*p);
//            insertToMainPolygonByOrder(p1, p2, *p);
//        }
//        p3 = p4;
//        p4 = p3->_next_to_cut_polygon;
//    } while(true);
//    if(&p2 != (p1._next_to_main_polygon))
//        return p1._next_to_main_polygon;
//    else
//        return NULL;
//}
//
//
///**
// * 对多边形进行微调,保证其上的点不与屏幕边重合,达到简化计算的目的同时确定多边形方向
// * @param main_polygon
// */
//
//void GeoLabelPolygonRender::initialMainPolygon(vector<GeometryPos2D*>& main_polygon)
//{
//    int sum = main_polygon.size();
//    if(sum == 0)
//        return;
//
//    double map_left_most = _transformer->getScreenScope()->getRight();
//	double map_right_most = _transformer->getScreenScope()->getLeft();
//	double map_top_most = _transformer->getScreenScope()->getBottom();
//	double map_bottom_most = _transformer->getScreenScope()->getTop();
//    int left = 0;
//    int top = 0;
//    int right = 0;
//    int bottom = 0;
//    double map_x_adjust = _transformer->getScreenScope()->getWidth() / (double)_transformer->getScreenPixWidth();
//    double map_y_adjust = _transformer->getScreenScope()->getHeight() / (double)_transformer->getScreenPixHeight();
//  
//    for(int i = 0; i < sum; i++)
//    {
//        GeometryPos2D* pos = (GeometryPos2D*)main_polygon[i];
//		if(pos->_x == _transformer->getScreenScope()->getLeft())
//            pos->_x += map_x_adjust;
//		if(pos->_x == _transformer->getScreenScope()->getRight())
//            pos->_x -= map_x_adjust;
//		if(pos->_y == _transformer->getScreenScope()->getTop())
//            pos->_y -= map_y_adjust;
//		if(pos->_y == _transformer->getScreenScope()->getBottom())
//            pos->_y += map_y_adjust;
//        if(pos->_x < map_left_most)
//        {
//            map_left_most = pos->_x;
//            left = i;
//        }
//        if(pos->_x > map_right_most)
//        {
//            map_right_most = pos->_x;
//            right = i;
//        }
//        if(pos->_y > map_top_most)
//        {
//            map_top_most = pos->_y;
//            top = i;
//        }
//        if(pos->_y < map_bottom_most)
//        {
//            map_bottom_most = pos->_y;
//            bottom = i;
//        }
//    }
//  
//    int index = left;
//    int cmp_order = 0;
//    _cut_polygon_reverse = true;
//    for(int i = 0; i < sum; i++)
//    {
//        int k = (index + i) % sum;
//        if(cmp_order == 0 && k == top)
//            cmp_order = 1;
//        if(cmp_order != 1 || k != right)
//            continue;
//        _cut_polygon_reverse = false;
//        break;
//    }
//
//}
//
//Vertex* GeoLabelPolygonRender::createMainPolygonList(vector<GeometryPos2D*>& polygon)
//{
//    if(polygon.size() == 0)
//        return NULL;
//    GeometryPos2D* pos = (GeometryPos2D*)polygon[0];
//    Vertex* head = new Vertex(*pos);
//    Vertex* tail = head;
//    int sum = polygon.size();
//    for(int i = 1; i < sum; i++)
//    {
//        Vertex* p = new Vertex(*(GeometryPos2D*)polygon[i]);
//        tail->_next_to_main_polygon = p;
//        tail = p;
//    }
//
//    tail->_next_to_main_polygon = head;
//    return head;
//}
//
//Vertex* GeoLabelPolygonRender::createCutPolygonList()
//{
//    double x[] = {
//		_transformer->getScreenScope()->getLeft(), 
//		_transformer->getScreenScope()->getRight(), 
//		_transformer->getScreenScope()->getRight(), 
//		_transformer->getScreenScope()->getLeft()
//    };
//    double y[] = {
//		_transformer->getScreenScope()->getTop(), 
//		_transformer->getScreenScope()->getTop(), 
//		_transformer->getScreenScope()->getBottom(),
//		_transformer->getScreenScope()->getBottom()
//    };
//    Vertex* head = NULL;
//    Vertex* tail;
//    if(!_cut_polygon_reverse)
//    {
//        head = new Vertex(x[0], y[0]);
//        tail = head;
//        for(int i = 1; i < 4; i++)
//        {
//            Vertex* p = new Vertex(x[i], y[i]);
//            tail->_next_to_cut_polygon = p;
//            tail = p;
//        }
//
//    } else
//    {
//        head = new Vertex(x[3], y[3]);
//        tail = head;
//        for(int i = 2; i >= 0; i--)
//        {
//            Vertex* p = new Vertex(x[i], y[i]);
//            tail->_next_to_cut_polygon = p;
//            tail = p;
//        }
//
//    }
//    tail->_next_to_cut_polygon = head;
//    return head;
//}
//
//void GeoLabelPolygonRender::insertToMainPolygonByOrder(Vertex& p1, Vertex& p2, Vertex& p)
//{
//    Vertex* head = &p1;
//    Vertex* tail = &p2;
//    Vertex* up = head;
//    Vertex* now = head;
//    if(p1._p->_x < p2._p->_x)
//    {
//        now = head;
//        up = now;
//        for(; now != tail; now = now->_next_to_main_polygon)
//        {
//            if(now->_p->_x >= p._p->_x)
//                break;
//            up = now;
//        }
//
//    } else
//    if(p1._p->_x > p2._p->_x)
//    {
//        now = head;
//        up = now;
//        for(; now != tail; now = now->_next_to_main_polygon)
//        {
//            if(now->_p->_x <= p._p->_x)
//                break;
//            up = now;
//        }
//
//    } else
//    if(p1._p->_y > p2._p->_y)
//    {
//        now = head;
//        up = now;
//        for(; now != tail; now = now->_next_to_main_polygon)
//        {
//            if(now->_p->_y <= p._p->_y)
//                break;
//            up = now;
//        }
//
//    } else
//    if(p1._p->_y < p2._p->_y)
//    {
//        now = head;
//        up = now;
//        for(; now != tail; now = now->_next_to_main_polygon)
//        {
//            if(now->_p->_y >= p._p->_y)
//                break;
//            up = now;
//        }
//
//    }
//    up->insertVertexToMainPolygon(p);
//}
//
//void GeoLabelPolygonRender::initialMostPoint()
//{
//    _left_most = _transformer->getScreenPixWidth();
//    _right_most = 0;
//    _top_most = _transformer->getScreenPixHeight();
//    _bottom_most = 0;
//}
//
//void GeoLabelPolygonRender::computeMostPoint(int x, int y, int i)
//{
//    if(x < _left_most)
//    {
//        _left_most_index = i;
//        _left_most = x;
//    } else
//    if(x > _right_most)
//    {
//        _right_most_index = i;
//        _right_most = x;
//    }
//    if(y < _top_most)
//    {
//        _top_most_index = i;
//        _top_most = y;
//    } else
//    if(y > _bottom_most)
//    {
//        _bottom_most_index = i;
//        _bottom_most = y;
//    }
//}
//
//ScreenPos2D* GeoLabelPolygonRender::getMidPointOnYLineInsPolygon(int x)
//{
//    int low = _right_most_index;
//    int high = _left_most_index;
//    if(_left_most_index < _right_most_index)
//    {
//        low = _left_most_index;
//        high = _right_most_index;
//    }
//    ScreenPos2D* pos1 = findIntersectPoint(_label_ctrl_points, low, high, x, 1);
//    vector<ScreenPos2D*> temp;
//    for(int i = low; i >= 0; i--)
//		temp.push_back(_label_ctrl_points[i]);
//
//    for(int i = _label_ctrl_points.size() - 1; i >= high; i--)
//		temp.push_back(_label_ctrl_points[i]);
//
//    ScreenPos2D* pos2 = findIntersectPoint(temp, 0, temp.size() - 1, x, 1);
//    return new ScreenPos2D((pos1->X + pos2->X) / 2, (pos2->Y + pos1->Y) / 2);
//}
//
//ScreenPos2D* GeoLabelPolygonRender::getMidPointOnXLineInsPolygon(int y)
//{
//    int low = _bottom_most_index;
//    int high = _top_most_index;
//    if(_top_most_index < _bottom_most_index)
//    {
//        low = _top_most_index;
//        high = _bottom_most_index;
//    }
//    ScreenPos2D* pos1 = findIntersectPoint(_label_ctrl_points, low, high, y, 0);
//    vector<ScreenPos2D*> temp;
//    for(int i = low; i >= 0; i--)
//		temp.push_back(_label_ctrl_points[i]);
//
//    for(int i = _label_ctrl_points.size() - 1; i >= high; i--)
//		temp.push_back(_label_ctrl_points[i]);
//
//    ScreenPos2D* pos2 = findIntersectPoint(temp, 0, temp.size() - 1, y, 0);
//    return new ScreenPos2D((pos1->X + pos2->X) / 2, (pos2->Y + pos1->Y) / 2);
//}
//
//ScreenPos2D* GeoLabelPolygonRender::computeIntersectWithXline(ScreenPos2D& pos1, ScreenPos2D& pos2, int y)
//{
//    if(y == pos1.Y)
//        return new ScreenPos2D(pos1.X, y);
//    if(y == pos2.Y)
//    {
//        return new ScreenPos2D(pos2.X, y);
//    } else
//    {
//        int x = ((y - pos1.Y) * (pos2.X - pos1.X)) / (pos2.Y - pos1.Y) + pos1.X;
//        return new ScreenPos2D(x, y);
//    }
//}
//
//ScreenPos2D* GeoLabelPolygonRender::computeIntersectWithYline(ScreenPos2D& pos1, ScreenPos2D& pos2, int x)
//{
//    if(x == pos1.X)
//        return new ScreenPos2D(x, pos1.Y);
//    if(x == pos2.X)
//    {
//        return new ScreenPos2D(x, pos2.Y);
//    } else
//    {
//        int y = ((x - pos1.X) * (pos2.Y - pos1.Y)) / (pos2.X - pos1.X) + pos1.Y;
//        return new ScreenPos2D(x, y);
//    }
//}
//
//ScreenPos2D* GeoLabelPolygonRender::findIntersectPoint(vector<ScreenPos2D*>& label_ctrl_points, int low, int high, int value, int direction)
//{
//    ScreenPos2D* pos1 = (ScreenPos2D*)label_ctrl_points[low];
//    ScreenPos2D* pos2 = (ScreenPos2D*)label_ctrl_points[high];
//    bool reverse;
//    if(direction == 0)
//    {
//        if(pos1->Y < pos2->Y)
//            reverse = false;
//        else
//            reverse = true;
//    } else
//    if(pos1->X < pos2->X)
//        reverse = false;
//    else
//        reverse = true;
//    int max = high;
//    while(low <= high) 
//    {
//        int mid = (low + high) / 2;
//        pos1 = (ScreenPos2D*)label_ctrl_points[mid];
//        if(mid < max)
//            pos2 = (ScreenPos2D*)label_ctrl_points[mid + 1];
//        else
//            pos2 = (ScreenPos2D*)label_ctrl_points[mid - 1];
//        if(direction == 0)
//        {
//            if((pos1->Y - value) * (pos2->Y - value) <= 0)
//                return computeIntersectWithXline(*pos1, *pos2, value);
//            if(pos1->Y < value)
//            {
//                if(!reverse)
//                    low = mid + 1;
//                else
//                    high = mid - 1;
//            } else
//            if(!reverse)
//                high = mid - 1;
//            else
//                low = mid + 1;
//        }
//        if(direction == 1)
//        {
//            if((pos1->X - value) * (pos2->X - value) <= 0)
//                return computeIntersectWithYline(*pos1, *pos2, value);
//            if(pos1->X < value)
//            {
//                if(!reverse)
//                    low = mid + 1;
//                else
//                    high = mid - 1;
//            } else
//            if(!reverse)
//                high = mid - 1;
//            else
//                low = mid + 1;
//        }
//    }
//    return NULL;
//}