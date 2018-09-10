/*
 * =====================================================================================
 *
 *       Filename:  MKMultiPoint.cpp
 *
 *    Description:  The MKMultiPoint class is an abstract superclass used 
 *    to define shapes composed of multiple points
 *
 *        Version:  1.0
 *        Created:  04/12/2012 12:28:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MKMultiPoint.h"
using namespace  std;

namespace z11
{
	uint_32 MKMultiPoint::pointCount()
	{
		return m_coords.size();
	}

	 MKMultiPoint::~MKMultiPoint()
	{

	}
	const std::vector<MGPoint> & MKMultiPoint::points()
	{
		return m_points;
	}


	void MKMultiPoint::getCoordinates( vector<MKLocationCoordinate2D > &coords)
	{
        for(auto it : m_coords){
            MKLocationCoordinate2D tmp=it;
            if((map_shift_type&MKMapShiftGoogle)!=0)
                tmp=MKLocationShiftProvider::applyChinaLocationShift(it);
            else
                ;
            if((map_shift_type&MKMapShiftBaidu)!=0)
                ;
            else
                ;
            if((map_shift_type&MKMapShiftBing)!=0)
                ;
            else
                ;
            if((map_shift_type&MKMapShiftOSM)!=0)
                ;
            else;
            
            coords.push_back(tmp);
        }
	}

	MKMultiPoint::MKMultiPoint( const std::vector<MKMapPoint>& pts ) 
	{

	}

	MKMultiPoint::MKMultiPoint( const std::vector<MKLocationCoordinate2D>& coords ) :m_coords(coords)
	{

	}

	void MKMultiPoint::setPoints( const std::vector<MGPoint>&points )
	{
		m_points=points;
	}

	z11::MKLocationCoordinate2D MKMultiPoint::getCenterCoordinate( const std::vector<MKLocationCoordinate2D > &coords )
	{
		if (coords.empty())
		{
			return MKLocationCoordinate2D(0,0);
		}
		
		double max_longitude=coords[0].longitude;
		double min_longitude=coords[0].longitude;
		double max_latitude=coords[0].latitude;
		double min_latitude=coords[0].latitude;
		for (std::vector<MKLocationCoordinate2D>::const_iterator it=coords.begin()+1;it!=coords.end();it++)
		{
			if (it->longitude>max_longitude)
			{
				max_longitude=it->longitude;
			}
			else if(it->longitude<min_longitude)
			{
				min_longitude=it->longitude;
			}

			if (it->latitude>max_latitude)
			{
				max_latitude=it->latitude;
			}
			else if(it->latitude<min_latitude)
			{
				min_latitude=it->latitude;
			}

		}
		return MKLocationCoordinate2D((max_latitude+min_latitude)/2,(min_longitude+max_longitude)/2);
	}

}