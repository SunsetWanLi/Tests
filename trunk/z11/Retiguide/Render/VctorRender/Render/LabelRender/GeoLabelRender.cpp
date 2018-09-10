//#include "stdafx.h"
#include "GeoLabelRender.h"
using namespace GIS_Embedded;

//vector<UserScreenRect> GeoLabelRender::_rect_queue;
//vector<ScreenPos2D> GeoLabelRender::_pos_queue;
//vector<wchar_t> GeoLabelRender::_label_char_queue;
//vector<int> GeoLabelRender::_label_escape_queue;

//GeoLabelRender::GeoLabelRender()
//{
//
//}
//
//GeographyObject* GeoLabelRender::getGeographyObject()
//{
//    return _geo;
//}
//
//void GeoLabelRender::setTransformer(Transformer& transformer)
//{
//
//	_transformer=&transformer;
//
//}


//void GeoLabelRender::setGeo(GeographyObject& geo)
//{
//	
//}
//
//
//void GeoLabelRender::render(StyleRender& mg, ScreenPixMap& screen_pix_map)
//{
//	_label_char_queue.clear();
//	_pos_queue.clear();
//	_rect_queue.clear();
//	_label_escape_queue.clear();
//}

//void GeoLabelRender::drawString(StyleRender& mg, ScreenPixMap& screen_pix_map)
//{
//    for(unsigned int i = 0; i < _label_char_queue.size(); i++)
//    {
//        UserScreenRect &rect = _rect_queue[i];
//        ScreenPos2D &pos = _pos_queue[i];
//        wstring &label_char = _label_char_queue[i];
//		int escape = _label_escape_queue[i];
//
//        screen_pix_map.setDirty(rect);
//        mg.drawString(pos.X, pos.Y, escape, label_char);
//
//    }
//
//}
//
//bool GeoLabelRender::conflictTest(StyleRender& mg, ScreenPixMap& screen_pix_map, ScreenPos2D& pos, std::string& label_char)
//{
// 	
//	UserScreenRect rect;
//	mg.getStringBound(label_char,&rect);
//    rect._left = rect._left + pos.X;
//    rect._right = rect._right + pos.X;
//    rect._top = rect._top + pos.Y;
//    rect._bottom = rect._bottom + pos.Y;
// 
//    
//    
//    if(screen_pix_map.testDirty(rect))
//    {
//        return true;
//    } 
//	else
//    {
//		_rect_queue.push_back(rect);
//		_pos_queue.push_back(pos);
//		_label_char_queue.push_back(label_char);
//        return false;
//    }
//}