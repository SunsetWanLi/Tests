#include "MKKMLGeometry.h"
#include <stdio.h>
namespace z11
{
	MKKMLGeometry::MKKMLGeometry(void)
	{
	}

	MKKMLGeometry::~MKKMLGeometry(void)
	{
	}



	MKKMLPoint::MKKMLPoint( )
	{
		longitude=0;
		latitude=0;
		altitude=0;
	}


	void MKKMLPoint::read( const std::string &str )
	{
		sscanf(str.c_str(),"%lf,%lf,%lf",&longitude,&latitude,&altitude);
	}
}