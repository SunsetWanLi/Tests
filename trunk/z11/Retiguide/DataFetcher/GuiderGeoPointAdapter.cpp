#include "GuiderGeoPointAdapter.h"

namespace GIS_Embedded{

	int GuiderGeoPointAdapter::languages = 0;

	int GuiderGeoPointAdapter::translate(GIS_Embedded::GGPTypeT type, byte_8  *data, int size, std::vector<GuiderGeoPoint*> **gg_points)
	{
		DataReader reader;
		reader.setStartPointer(data);
		vector<GuiderGeoPoint*> *temp_points = new vector<GuiderGeoPoint*>;
		GuiderGeoPoint * ggp = NULL;
		languages = reader.getByte();
		GuiderGeoPointNoTrigger::setTotalLanguages(languages);// pan ?
		switch(type)
		{
		case GGPT_NOTRIGGER:
			while ((reader.getStartPointer() - data) < size)
			{
				getGuiderGeoPointNoTrigger(reader,&ggp);
				temp_points->push_back(ggp);
			}
			break;
		case GGPT_TRIGGER:	
			while ((reader.getStartPointer() - data) < size)
			{
				getGuiderGeoPointTrigger(reader,&ggp);
				temp_points->push_back(ggp);
			}
             break;		
		}
		*gg_points = temp_points;
		return 0;
	}
	int GuiderGeoPointAdapter::translateTrigerArea(byte_8 *data,int size,vector<PolygonArea> **ploygon_areas)
	{
		DataReader reader;
		reader.setStartPointer(data);
		int num = reader.getByte();
		vector<PolygonArea> *temp = new vector<PolygonArea>(num);
		for (int i=0; i<num; i++)
		{
			(*temp)[i].direT = (DirectionTypeT)reader.getByte();
			int outerPoints = reader.getByte();
			Pos2DEx tempPos;
			for(int j = 0; j < outerPoints; j++)
			{
				tempPos.d0 = reader.getInt();
				tempPos.d1 = reader.getInt();
				(*temp)[i].outerLoop.push_back(tempPos);
			}
			
			int inerloops = reader.getByte();
			for(int j = 0; j < inerloops; j++)
			{
				(*temp)[i].innerLoops.push_back( vector<Pos2DEx>() );
				int innerPoints = reader.getByte();
				for(int k = 0; k < innerPoints; k++ )
				{
					tempPos.d0 = reader.getInt();
					tempPos.d1 = reader.getInt();
					(*temp)[i].innerLoops[j].push_back(tempPos);
				}
			}
		}
		if (reader.getStartPointer() - data > size)
		{
			delete temp;
			return -1;
		}
		*ploygon_areas = temp;

		return 0;
	}
	int GuiderGeoPointAdapter::translateFileInfos(byte_8 *data, int size,FileInfos** file_infos)
	{
		DataReader reader;
		reader.setStartPointer(data);
		FileInfos * tempfileinfos = new FileInfos;
		FileParamG tempParamG;

		int soundFileNum = reader.getByte();
		for (int i = 0; i < soundFileNum; i++)
		{
			tempParamG.direction = (DirectionTypeT) reader.getByte();
			tempParamG.filename = reader.getString(); // sum ä¸?Byte
			tempfileinfos->soundFiles.push_back(tempParamG);
		}

		int videoFileNum = reader.getByte();
		for (int i = 0; i < videoFileNum; i++)
		{
			tempParamG.direction = (DirectionTypeT) reader.getByte();
			tempParamG.filename = reader.getString(); // sum ä¸?Byte
			tempfileinfos->videoFiles.push_back(tempParamG);
		}

		int pictureFileNum = reader.getByte();
		for (int i = 0; i < pictureFileNum; i++)
		{
			tempParamG.direction = (DirectionTypeT) reader.getByte();
			tempParamG.filename = reader.getString(); // sum ä¸?Byte
			tempfileinfos->pictureFiles.push_back(tempParamG);
		}

		int textFileNum = reader.getByte();
		for (int i = 0; i < textFileNum; i++)
		{
			tempParamG.direction = (DirectionTypeT) reader.getByte();
			tempParamG.filename = reader.getString(); // sum ä¸?Byte
			tempfileinfos->textFiles.push_back(tempParamG);
		}
		if (reader.getStartPointer()-data > size)
		{
			delete tempfileinfos;
			return -1;
		}

		*file_infos = tempfileinfos;
		return 0;
	}
	int GuiderGeoPointAdapter::getGuiderGeoPointNoTrigger(DataReader &reader,  GuiderGeoPoint **ggp)
	{
		   GuiderGeoPointNoTrigger *ggpn = new GuiderGeoPointNoTrigger();
		   int posx = reader.getInt();
		   int posy = reader.getInt();
			ggpn->setMyPosition(posx,posy);
            int lang = languages;
			while(lang--)
			{
				ggpn->addName(reader.getString_1());
				ggpn->addDescription(reader.getString_1());
			}	
			*ggp = ggpn;
			return 0;
	}
	int GuiderGeoPointAdapter::getGuiderGeoPointTrigger(DataReader &reader,  GuiderGeoPoint **ggp)
	{
		GuiderGeoPointTrigger *ggpt = new GuiderGeoPointTrigger();
		int posx = reader.getInt();
		int posy = reader.getInt();
		ggpt->setMyPosition(posx,posy);
		int lang = languages;
		int offset = reader.getInt();
		int size = reader.getInt();
		ggpt->setTriggerAreaOffsetAndSize(offset,size);		
		while(lang--)
		{
			ggpt->addName(reader.getString_1());
			ggpt->addDescription(reader.getString_1());
			offset = reader.getInt();
			size = reader.getInt();
			ggpt->addIntroduceFileOffsetAndSize(offset,size);			
		}	
		
		*ggp = ggpt;
		return 0;
	}
}