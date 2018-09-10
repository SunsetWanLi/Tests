#pragma  once

#include "GuiderGeoPointNoTrigger.h"
#include "Types.h"
//
#include <vector>
#include <string>
using std::vector;
using std::wstring;
namespace GIS_Embedded{
	class GuiderGeoPointTrigger: public GuiderGeoPointNoTrigger
	{
		
	private:
		vector<int> offset_introduce;//每一种语言一个offset
		int offset_triggerarea,uint_32riggerarea;
		vector<FileInfos*> *introduce_files;
		vector<PolygonArea> *trigger_areas; 
		//bool bTriggerArea;		
	public:
		virtual bool hasTriggerArea() const { return offset_triggerarea !=-1; }
		virtual ~GuiderGeoPointTrigger() { delete introduce_files; delete trigger_areas;}
		void setTriggerAreaOffsetAndSize(int offset_trig,int uint_32rig)
		{			
			offset_triggerarea = offset_trig;
			uint_32riggerarea = uint_32rig;
		}
		void addIntroduceFileOffsetAndSize(int offset,int size)
		{
			offset_introduce.push_back(offset);
			offset_introduce.push_back(size);
		}
		GuiderGeoPointTrigger()
		{
			//offset_introduce = -1;
			offset_triggerarea = -1;
			introduce_files = NULL;
			trigger_areas = NULL;
			offset_introduce.clear();
			//bTriggerArea = false;
		}
		FileInfos* getFileInfos();
		vector<PolygonArea> * getTriggerArea();		
		bool isTriggered(const Pos2DEx &pos);
		void writeItselfToFile();
	};
}
