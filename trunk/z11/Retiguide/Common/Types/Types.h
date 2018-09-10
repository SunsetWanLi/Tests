#pragma once
#include <vector>
#include <string>
#include <fstream>
#include "basic_types.h"
#include "Retiguide/SpatialMath/SpatialMath.h"
using std::vector;
using std::wstring;
using std::wofstream;
#pragma warning(disable:4018)

namespace GIS_Embedded
{
	enum DirectionTypeT
	{
		DT_NOTSENSITIVE = 0,
		DT_NORTH,
		DT_NORTHEAST,
		DT_EAST,
		DT_SOUTHEAST,
		DT_SOUTH,
		DT_SOUTHWEST,
		DT_WEST,
		DT_NORTHWEST
	};
	enum GGPTypeT{
		GGPT_NOTRIGGER,
		GGPT_TRIGGER
	};

	typedef struct FileParamG{
		DirectionTypeT direction;
		wstring filename;
		void writeToFile(wofstream &outfile);
		
	}FileParamG;

	typedef struct FileInfos{
		vector<FileParamG> soundFiles;
		vector<FileParamG> videoFiles;
		vector<FileParamG> pictureFiles;
		vector<FileParamG> textFiles;
		void writeToFile(wofstream &outfile);
		
	}FileInfos;

	typedef struct PolygonArea{
		DirectionTypeT direT;
		vector<Pos2DEx> outerLoop;
		vector<vector<Pos2DEx> > innerLoops;
		bool isPosInPolygon(const Pos2DEx &pos);		
		void writeToFile(wofstream &outfile);		

	}PolygonArea; 
}