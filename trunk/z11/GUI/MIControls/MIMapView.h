//
//  MIMapView.h
//  Gui
//
//  Created by Liu Wenwen on 11-7-22
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "MIControl.h"
#include "../../GraphicCore/MGImage.h"
#include "../../Retiguide/Shell/Navigation/Navigation.h" 


using namespace GIS_Embedded;

namespace z11
{
	enum {
		MIMapEventZoomIn				= 1 <<  0,
		MIMapEventZoomOut				= 1 <<  1,
		MIMapEventMove					= 1 <<  2,
	};
	class MIMapView : public MIControl
	{
	public:
		MIMapView();
		virtual ~MIMapView();
	public:
		MIRESPONDER_DECLARE;
		MIVIEW_DECLARE;
		MIRESPONDER_TOUCH_DECLARE;
		void centraPoint(const std::set<MITouch*>& touches, MGPoint &point);
		void endPoint(const std::set<MITouch*>& touches, MGPoint &point);
		//void setBackgroundColor(MGColor *_color);
	public:
		float pre_radius;					//触摸开始的等效半径
		MGPoint pre_centra_point;			//触摸开始的等效几何中心
		float scaling;						//缩放比例
		MGPoint last_point;					//上次触摸的几何中心点
	private:
		MGColor *m_native_color;

	private:
		void initialData(uint_32 screenWidth,uint_32 screenHeight);//lw
	protected:
		Navigation*         g_navi;

	public:
		void mapZoomIn();
		void mapZoomOut();
		void locater();
		void roadSearch();
		void roadReturn();
		void startNavi();
		void startSimuNavi();
		void stopNavi();
		void setHost();
		void setDest(float dest_x, float dest_y);
	};
}