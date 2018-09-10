
#include "GuiderGeoPointTrigger.h"
#include "../../../DataFetcher/DataFetcher.h"
#include <fstream>
using std::wofstream;
namespace GIS_Embedded{

	bool GuiderGeoPointTrigger::isTriggered(const Pos2DEx &pos)	// pan:判断是否在触发区域内
	{
		if(getTriggerArea()==NULL)
			return false;
		for (int i=0; i < trigger_areas->size(); i++)
		{
			if (((*trigger_areas)[i]).isPosInPolygon(pos))
			return true;
		}
		return false;
	}
	FileInfos* GuiderGeoPointTrigger::getFileInfos()
	{		
		int offset ,size = 0;
		FileInfos* fileinfos = NULL;
		int index = index_languages<<1;
		if (offset_introduce.size() > index+1)
		{
			offset = offset_introduce[index];
			size = offset_introduce[index +1];
		}
		if (size != 0)
		{
			if (introduce_files == NULL)
			{
				introduce_files = new vector<FileInfos*>(total_languages);
				for(int i = 0; i < total_languages; i++)
					(*introduce_files)[i] = NULL;
			}
			if ((*introduce_files)[index_languages] == NULL)
			{
				DataFetcher::getInstance()->getGGPointFileInfos(m_key,index_languages,offset,size,&(*introduce_files)[index_languages]);
			}

			fileinfos = (*introduce_files)[index_languages];
		}	

		return fileinfos;
	}
	vector<PolygonArea> *GuiderGeoPointTrigger::getTriggerArea()
	{
		if(trigger_areas) 
			return trigger_areas;
		if(offset_triggerarea != -1)
			DataFetcher::getInstance()->getGGPointTriggerArea(m_key,offset_triggerarea,uint_32riggerarea,&trigger_areas);
		return trigger_areas;
	}

	void GuiderGeoPointTrigger::writeItselfToFile()
	{
		static wofstream outfile("\\Storage Card\\TriggerInfo.txt");
		if(!outfile.is_open())
			assert(0);

		outfile <<L"Pos: "<< this->getMyPosition().d0 << " " << this->getMyPosition().d1 << L"\n"; 
		outfile <<L"Languages Introduce: " << this->offset_introduce.size()/2 << endl;
		for(int i=0; i < this->offset_introduce.size(); i++ )
		{
			outfile << L"Offset: "<< offset_introduce[i] ;
			outfile <<L", Size:" <<offset_introduce[++i] << endl;
		}
		FileInfos* fileinfos =getFileInfos();
		if(fileinfos) 
			fileinfos->writeToFile(outfile);
		vector<PolygonArea> * tri_area = getTriggerArea();
		if(tri_area)
		{
			outfile << L"PolygonArea Nums: " << tri_area->size() << endl;
			for(int i = 0; i<tri_area->size(); i++)
				tri_area->at(i).writeToFile(outfile);
		}

		outfile <<L"\n";
	}
	
}