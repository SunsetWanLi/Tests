#pragma once
#ifndef _ANA_QUERY_H_
#define _ANA_QUERY_H_
#include <functional>
#include <map>
#include <string>
#include <vector>
using std::binary_function;
using std::map;
//using std::wstring;
using std::vector;
#include "../../DataFetcher/DataFetcher.h"
#include "CommonBase/MGWString.h"

namespace GIS_Embedded{

	class GuiderGeoPoint;

#define INVALID_POS_VALUE -100000000

	class Ana_Query{

		struct PointInfo{
			GuiderGeoPoint * pPoint;
			double distance;
		};
		class DistanceLessor: public binary_function<const PointInfo &,const PointInfo &,bool>
		{
		public:
			virtual bool operator()(const PointInfo &Left,const PointInfo &Right)
			{
				if(Left.pPoint == Right.pPoint) return false;
				return Left.distance < Right.distance ||
					(Left.distance == Right.distance && Left.pPoint<Right.pPoint) ;
			}
		};

		//typedef hash_map<MGWString, vector<MGWString> > ScenicAreaMap;


	private:
		Ana_Query(){}
		//Ana_Query operator=(Ana_Query&);
		//Ana_Query(Ana_Query&);
		void init();
		int initAreaMap();
		static Ana_Query *_ana_query_;
	public:
		static Ana_Query* getInstance();
		static void releaseInstance();
		int getNearestGuiderGeoPoint(MGWString &type_key, vector<GuiderGeoPoint*>& ggps,int needNum = 5, Pos2DEx pos =Pos2DEx(INVALID_POS_VALUE,INVALID_POS_VALUE) );
		int getPointTriggeredGuiderGeoPoint(vector<GuiderGeoPoint*>& ggps,MGWString &type_key,Pos2DEx pos =Pos2DEx(INVALID_POS_VALUE,INVALID_POS_VALUE));
		void setCurrentPos(Pos2DEx &pos)
		{
			_myPos_ = pos;
		}
		int getScenicAreaNameList(vector<MGWString> *&pAreaList);		
		int getScenicSpotNameList(MGWString &areaName, vector<MGWString> *&pSpotList);
		int getGuiderGeoPointTypeList(vector<MGWString> *&pTypeList);
		int getGuiderGeoPoint(MGWString &name, GuiderGeoPoint** p);
		int getIndoorSpotNameList(MGWString &areaName, vector<MGWString> *&pSpotList);
		int getNearestPointAmongGroup(vector<MGWString> &group,GuiderGeoPoint **p,Pos2DEx pos =Pos2DEx(INVALID_POS_VALUE,INVALID_POS_VALUE));
		bool isPointYBelongToGroupX(MGWString &groupX,MGWString &pointY);
		int getSightButtonList(int index,vector<SightButtonConf> &sightbutton);
		int GetCounterList(vector<MGWString> &counterlist);

		Pos2DEx getCurrentPos();
	private:
		Pos2DEx _myPos_;
		vector<MGWString> _scenicAreaNames_;
		ScenicAreaMap _scenicAreaMap_;
		vector<MGWString> _objectTypes_;
		ScenicAreaMap _indoorAreaMap_;
		ScenicAreaMap _pairPointsMap_;
		vector<SightButtonConf> _sightbutton1_;
		vector<SightButtonConf> _sightbutton0_;
		vector<MGWString> _counterlist_;
	};
}
#endif