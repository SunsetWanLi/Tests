#pragma once
class SkCanvas;
class SkBitmap;
class SkPath;
#include <string>
#include <vector>
#include "../../CommonBase/MGStructure.h"
#include "CGBitmap.h"
using namespace std;

class SkAutoGraphics;
namespace z11
{
	class MGCanvas;
	class CGCanvas:public MGRefObject
	{
	public:
		//Construct an empty raster canvas.
		CGCanvas(const CGBitmap &bitmap);
		~CGCanvas();
        MGImage* toImage(bool reverse = false);
		void writeToFile(const std::string &path, MGImage* buffer, bool reverse = false);
        
		static void createSkAutoGraphics();
		static void releaseSkAutoGraphics();
	private:
		static SkAutoGraphics* auto_graphics;
        SkCanvas *impl;
		CGBitmap *m_bitmap;
		CGCanvas();
		void makePolyline(SkPath &path,const vector<MGPoint> &points);
		void makePolygon(SkPath &path,const vector<MGPoint> &points);
		friend class MGCanvas;
	};
}