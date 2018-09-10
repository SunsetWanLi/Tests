#include "Ana_Query.h"
//#include "../../Shell/Navigation/Navigation.h"
#include <set>

using std::set;
namespace GIS_Embedded
{
	Ana_Query* Ana_Query::_ana_query_ = NULL;
	

	Ana_Query* Ana_Query::getInstance()
	{
		if(!_ana_query_)
		{
			_ana_query_ = new Ana_Query;
			_ana_query_ ->init();
		}
		return _ana_query_;
	}

	void Ana_Query::releaseInstance()
	{
		if(_ana_query_ != NULL)
		{
			delete _ana_query_;
			_ana_query_ = NULL;
		}
	}

	void Ana_Query::init()
	{
		initAreaMap();
	}
	int Ana_Query::getNearestGuiderGeoPoint(z11::MGWString &type_key, std::vector<GuiderGeoPoint*> &ggps, int needNum,Pos2DEx pos )
	{
		if(pos == Pos2DEx(INVALID_POS_VALUE,INVALID_POS_VALUE))
			pos = getCurrentPos();

		ggps.clear();
		DataFetcher *data_fetcher = DataFetcher::getInstance();
		GGPointBlock *block;
		data_fetcher->getGGPoint(type_key, &block); 
		set<PointInfo,DistanceLessor> pointinfos;
		PointInfo temp;
		vector<GuiderGeoPoint* > *objs =block->ggp_objs;
		for(int i= objs->size()-1; i>=0; i--)
		{
			temp.pPoint = (*objs)[i];
			temp.distance =SpatialMath::getDistance(pos,temp.pPoint->getMyPosition());
			pointinfos.insert(temp);
		}
		set<PointInfo,DistanceLessor>::iterator it = pointinfos.begin();		
		for (int i=0; i<needNum; i++)
		{
			ggps.push_back((*it).pPoint);
			it++;
			if(it == pointinfos.end())
				break;
		}

		return 0;
	}
	int Ana_Query::getPointTriggeredGuiderGeoPoint(vector<GuiderGeoPoint*>& ggps,z11::MGWString &type_key,Pos2DEx pos)
	{
		if(pos == Pos2DEx(INVALID_POS_VALUE,INVALID_POS_VALUE))
			pos = getCurrentPos();

		ggps.clear();
		DataFetcher *data_fetcher = DataFetcher::getInstance();
		GGPointBlock *block;
		data_fetcher->getGGPoint(type_key, &block);
		set<PointInfo,DistanceLessor> pointinfos;
		PointInfo temp;
		GuiderGeoPoint *obj;
		vector<GuiderGeoPoint* > *objs =block->ggp_objs;
		for(int i=0; i<objs->size(); i++)
		{
			obj = (*objs)[i];
			if (obj->hasTriggerArea())
			{
				if(((GuiderGeoPointTrigger*)obj)->isTriggered(pos))
				{
					temp.pPoint = obj;
					temp.distance=SpatialMath::getDistance(pos,temp.pPoint->getMyPosition());
					pointinfos.insert(temp);
				}
			}
		}
		set<PointInfo,DistanceLessor>::iterator it = pointinfos.begin();		
		while (it != pointinfos.end())
		{
			ggps.push_back((*it).pPoint);
			it++;
		}
		return 0;
	}
	
	Pos2DEx Ana_Query::getCurrentPos()
	{
		return _myPos_;
	}
	int Ana_Query::getScenicAreaNameList(vector<z11::MGWString> *&pAreaList)
	{
		pAreaList =& _scenicAreaNames_;
		return 0;
	}
	int Ana_Query::getScenicSpotNameList(z11::MGWString &areaName, vector<z11::MGWString> *&pSpotList)
	{
		pSpotList = NULL;

		ScenicAreaMap::iterator it = _scenicAreaMap_.find(areaName);
		if(it != _scenicAreaMap_.end())
		{
			pSpotList = &it->second;
			return 0;
		}
		return -1;
	}
	int Ana_Query::getIndoorSpotNameList(z11::MGWString &areaName, vector<z11::MGWString> *&pSpotList)
	{
		pSpotList = NULL;

		ScenicAreaMap::iterator it = _indoorAreaMap_.find(areaName);
		if(it != _indoorAreaMap_.end())
		{
			pSpotList = &it->second;
			return 0;
		}
		return -1;
	}
	int Ana_Query::getSightButtonList(int index, vector<SightButtonConf> &sightbutton)
	{
		if (index == 1)
		{
			sightbutton = _sightbutton1_;
		}
		if (index == 0)
		{
			sightbutton = _sightbutton0_;
		}
		return -1;
	}

	int Ana_Query::GetCounterList(vector<z11::MGWString> &counterlist)
	{
		counterlist = _counterlist_;
		return -1;
	}

	int Ana_Query:: getGuiderGeoPointTypeList(vector<z11::MGWString> *&pTypeList)
	{
		pTypeList = &_objectTypes_;
		return 0;
	}
	int Ana_Query::initAreaMap()
	{
		DataFetcher *data_fetcher = DataFetcher::getInstance();
		_scenicAreaMap_.clear();
		data_fetcher->getScenicAreaMap(_scenicAreaMap_,_scenicAreaNames_);
		data_fetcher->getGGPointTypes(_objectTypes_);
		data_fetcher->getIndoorPointGroup(_indoorAreaMap_);
		data_fetcher->getPairPointGroup(_pairPointsMap_);
		data_fetcher->getSightButtonConf1(_sightbutton1_);
		data_fetcher->getSightButtonConf0(_sightbutton0_);
		data_fetcher->getCounterList(_counterlist_);
		return 0;
	}
	int Ana_Query::getGuiderGeoPoint(z11::MGWString& name, GuiderGeoPoint** p)
	{
		return DataFetcher::getInstance()->getGGPointFromName(name,p);

	}
	bool Ana_Query::isPointYBelongToGroupX(z11::MGWString &groupX, z11::MGWString &pointY)
	{
		ScenicAreaMap::iterator it = _pairPointsMap_.find(groupX);
		if(it == _pairPointsMap_.end())
			return false;

		vector<MGWString> &group = it->second;
		for(int i = 0; i < group.size(); i++)
		{
			if(group[i] == pointY)
				return true;
		}

		return false;
	}

	int Ana_Query::getNearestPointAmongGroup(vector<MGWString> &group,GuiderGeoPoint **p, Pos2DEx pos)
	{
		if(pos == Pos2DEx(INVALID_POS_VALUE,INVALID_POS_VALUE))
			pos = getCurrentPos();

		vector < GuiderGeoPoint *> group_points;
		int distance = 0x7fffffff; 
		int ret = 0;
		for(int i = 0; i < group.size(); i++)
		{
			GuiderGeoPoint *temp = 0;
			DataFetcher::getInstance()->getGGPointFromName(group[i],&temp);
			if(temp)
			{
				int dis=SpatialMath::getDistance(pos,temp->getMyPosition());
				if(dis < distance)
				{
					distance = dis;
					*p = temp;
				}

			}
			else
			{
				ret = -1;
			}		

		}

		return ret;
	}
}