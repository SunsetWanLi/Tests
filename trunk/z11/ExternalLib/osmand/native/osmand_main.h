#pragma once
#include <string>
class RenderingInfo {
public:
	int left, right, top, bottom;
	double lattop, lonleft;
	int tileWX, tileHY;
	std::string tileFileName;
	std::string renderingFileName;
	std::string imagesFileName;
	int zoom;
	int width ;
	int height;
	RenderingInfo()
	{

	}

	RenderingInfo(int argc, char params[][256]);
};

extern void runSimpleRendering( string renderingFileName, string resourceDir, RenderingInfo* info);