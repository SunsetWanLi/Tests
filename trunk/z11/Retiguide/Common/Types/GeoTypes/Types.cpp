
#include "Types.h"
namespace GIS_Embedded
{
	void FileParamG::writeToFile(std::wofstream &outfile)
	{
			outfile << L"Direction: " << direction << ", filename: " << filename << endl;
	}
	void FileInfos::writeToFile(std::wofstream &outfile)
	{
			outfile << L"soundFiles: " ;
			for(int i = 0; i < soundFiles.size(); i++)
				soundFiles[i].writeToFile(outfile);

			outfile << L"videoFiles: " ;
			for(int i = 0; i < videoFiles.size(); i++)
				videoFiles[i].writeToFile(outfile);

			outfile << L"pictureFiles: " ;
			for(int i = 0; i < videoFiles.size(); i++)
				videoFiles[i].writeToFile(outfile);

			outfile << L"pictureFiles: " ;
			for(int i = 0; i < videoFiles.size(); i++)
				videoFiles[i].writeToFile(outfile);

	}
	void PolygonArea::writeToFile(std::wofstream &outfile)
	{
			outfile << L"Direction: " << direT <<endl;
			outfile << L"Outloop size: " << outerLoop.size() <<endl;
			for(int i =0; i < outerLoop.size(); i++)
			{
				outfile << outerLoop[i].d0 << L"," << outerLoop[i].d1 <<";";
			}
			outfile << endl << L"InnerLoops: " << innerLoops.size() << endl;
	}
	bool PolygonArea::isPosInPolygon(const GIS_Embedded::Pos2DEx &pos)
	{
			for(int i = innerLoops.size()-1; i>=0; i--)
			{
				if (SpatialMath::isPointInArea(pos,innerLoops[i]))
				   return false;
			}
			//pan
			bool testd = SpatialMath::isPointInArea(pos, outerLoop);
			return testd;
	}
}