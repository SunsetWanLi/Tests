#include "./PyramidScaler.h"
#include "./PyramidSettings.h"
#include <stdlib.h>
#include <cmath>
namespace GIS_Embedded
{
using namespace std;
using namespace z11;
//	namespace Scale_DMS_Pixel
//	{

		Geo_Scale operator-(const Geo_Scale & minus)
		{
			Geo_Scale result;
			result.degree = - minus.degree;
			result.minute = - minus.minute;
			result.second = - minus.second;
			result.direction = minus.direction;
			return result;
		} // operator -

		/////////////  class GeoScale /////////////
		Geo_Scale::Geo_Scale(double deg,double min,double sec,char direc)
			:degree(deg),minute(min),second(sec),direction(direc)
		{
			return ;
		}

		const Geo_Scale & Geo_Scale::operator = (const Geo_Scale & source)
		{
			degree = source.degree;
			minute = source.minute;
			second = source.second;
			direction = source.direction;
			return *this;
		}
		
		void Geo_Scale::normalize(Geo_Scale & scale)const
		{
			scale.minute += long(scale.second / 60);
			scale.second = scale.second - long(scale.second / 60) * 60;
			scale.degree += long(scale.minute / 60);
			scale.minute = scale.minute - long(scale.minute / 60) * 60;

			switch(scale.direction)
			{
				case 'U':
					break;
				case 'O':
					scale.degree = scale.degree - floor((scale.degree + 180) / 360) * 360;
					break;
				case 'A':
					if(scale.degree > 90.0f || scale.degree < -90.0f 
						|| ( (fabs(scale.degree - 90.0f) < MIN_DEGREE_DELTA || fabs(scale.degree + 90.0f) < MIN_DEGREE_DELTA)
								&& ( fabs(scale.minute - 0.0f) > MIN_MINUTE_DELTA || fabs(scale.second - 0.0f) > MIN_SECOND_DELTA) )
						)throw 0;//latitude_out_of_range(L"@ GeoScale::normalize()");
					break;
				default:
					throw 0;//unknown_direction(L"@ GeoScale::normalize()");
			} // switch(scale.direction)
		} // normalize (GeoScale & )

		Geo_Scale Geo_Scale::operator+ (const Geo_Scale & add_right)const
		{
			Geo_Scale result;
			result.second = second + add_right.second;
			result.minute = minute + add_right.minute;
			result.degree = degree + add_right.degree;
			result.direction = direction;
			if(result.direction == 'U')
				result.direction = add_right.direction;
			normalize(result);
			return result;
		}// operator +

		Geo_Scale Geo_Scale::operator - (const Geo_Scale & sub_right)const
		{
			Geo_Scale result = *this + (-sub_right);
			if( (direction != 'U') && (sub_right.direction != 'U') )
				result.direction = 'U';
			return result;
		} // operator - (const GeoScale &)

		Geo_Scale Geo_Scale::operator * (double multiple )const
		{
			Geo_Scale result;
			result.degree = degree * multiple;
			result.minute = minute * multiple;
			result.second = second * multiple;
			result.direction = direction;
			normalize(result);
			return result;
		} // operator * (double) 

		Geo_Scale Geo_Scale::operator / (double multiple)const
		{
			if(0 == multiple)
				throw 0;//divide_zero(L"@ GeoScale::operator /");
			return *this * (1 / multiple);
		} // operator / (double)

		bool Geo_Scale::operator == (const Geo_Scale & cmp)const
		{
			if( (direction == 'O') && (cmp.direction == 'O')
				&& ( fabs(minute - 0.0f) < MIN_MINUTE_DELTA)  && ( fabs(cmp.minute - 0.0f) < MIN_MINUTE_DELTA)
				&& ( fabs(second - 0.0f) < MIN_SECOND_DELTA) && ( fabs(cmp.second - 0.0f) < MIN_SECOND_DELTA)
				)
				return fabs( (degree - floor((degree + 180.0f) / 360.0f) * 360.0f)
				- (cmp.degree - floor((cmp.degree + 180.0f) / 360.0f) *360.0f) )
				< MIN_DEGREE_DELTA;
			else
				return  (direction == cmp.direction)
				&&	( fabs(degree - cmp.degree) < MIN_DEGREE_DELTA)
				&&  ( fabs(minute - cmp.minute) < MIN_MINUTE_DELTA)
				&&  ( fabs(second - cmp.second) < MIN_SECOND_DELTA)
				;
		}

		bool Geo_Scale::operator > (const Geo_Scale & cmp)const
		{
			if(cmp.direction != direction)
				throw 0;//scale_can_not_compare(L"@ GeoScale::operator >");
			if( fabs(degree - cmp.degree) > MIN_DEGREE_DELTA)
				return degree > cmp.degree;
			if( fabs(minute - cmp.minute) > MIN_MINUTE_DELTA)
				return minute > cmp.minute;
			return second > cmp.second;
		} // operator > (const GeoScale &)

		bool Geo_Scale::operator < (const Geo_Scale & cmp)const
		{
			if(cmp.direction != direction)
				throw 0;//scale_can_not_compare(L"@ Geo_Scale::operator < ");
			if( fabs(degree - cmp.degree) > MIN_DEGREE_DELTA)
				return degree < cmp.degree;
			if( fabs(minute - cmp.minute) > MIN_MINUTE_DELTA)
				return minute < cmp.minute;
			return second < cmp.second;
		} // operator < (const GeoScale &)

		bool Geo_Scale::operator >= (const Geo_Scale & cmp)const
		{
			return ! (*this < cmp);
		} // operator >= (const GeoScale &)

		bool Geo_Scale::operator <= (const Geo_Scale & cmp)const
		{
			return ! (*this > cmp);
		} // operator <= (const GeoScale &)

		////////////  struct tagGeoRegion ////////////
		tag_GeoRegion::tag_GeoRegion()
		{
			longitude_start.direction = 'O';
			longitude_end.direction = 'O';
			latitude_start.direction = 'A';
			latitude_end.direction = 'A';
			return ;
		}
		
		tag_GeoRegion::tag_GeoRegion(const Geo_Scale & lo_st,const Geo_Scale & lo_nd,const Geo_Scale & la_st,const Geo_Scale & la_nd)
			:longitude_start(lo_st),longitude_end(lo_nd),latitude_start(la_st),latitude_end(la_nd)
		{
			return ;
		}

		const tag_GeoRegion & tag_GeoRegion::operator = (const tag_GeoRegion & source_region)
		{
			longitude_start = source_region.longitude_start;
			longitude_end = source_region.longitude_end;
			latitude_start = source_region.latitude_start;
			latitude_end = source_region.latitude_end;
			return * this;
		} // operator = 

		bool tag_GeoRegion::operator == (const tag_GeoRegion & cmp_region)
		{
			return (longitude_start == cmp_region.longitude_start)
				&& (longitude_end == cmp_region.longitude_end)
				&& (latitude_start == cmp_region.latitude_start)
				&& (latitude_end == cmp_region.latitude_end);
		} // ooperator ==

		bool tag_GeoRegion::includes(const Geo_Scale & pot_longitude , const Geo_Scale & pot_latitude)
		{
			return (pot_longitude >= longitude_start) && (pot_longitude <= longitude_end)
				&& (pot_latitude >= latitude_start) && (pot_longitude <= latitude_end);
		} // includes(const GeoScale &)

		bool tag_GeoRegion::is_overlap(const tag_GeoRegion & region)const
		{
			Geo_Scale delta_longitude = ((longitude_end > region.longitude_end)?longitude_end:region.longitude_end)
				- ((longitude_start < region.longitude_start)?longitude_start:region.longitude_start);
			Geo_Scale delta_latitude = ((latitude_end > region.latitude_end)?latitude_end:region.latitude_end)
				- ((latitude_start < region.latitude_start)?latitude_start:region.latitude_start);
			Geo_Scale longitude_length = (longitude_end - longitude_start) + (region.longitude_end - region.longitude_start);
			Geo_Scale latitude_length = (latitude_end - latitude_start) + (region.latitude_end - region.latitude_start);

			return (delta_longitude < longitude_length) && (delta_latitude < latitude_length);

		} // is_overlap(const tagGeoRegion &)
//	} // end of namespace/struct Scale_DMS_Pixel

//	namespace Scale_Pixel_Pixel
//	{
		tagGeoRegion::tagGeoRegion(const GeoScale x_st,const GeoScale x_nd,const GeoScale y_st,const GeoScale y_nd)
		{
			x_start = x_st;
			x_end	= x_nd;
			y_start	= y_st;
			y_end	= y_nd;
		}

		const tagGeoRegion & tagGeoRegion::operator =(const tagGeoRegion & source_region)
		{
			x_start = source_region.x_start;
			x_end	= source_region.x_end;
			y_start = source_region.y_start;
			y_end	= source_region.y_end;
			return * this;
		}
		
		bool tagGeoRegion::operator ==(const tagGeoRegion & cmp_region)
		{
			return (x_start == cmp_region.x_start) 
				&& (x_end	== cmp_region.x_end)
				&& (y_start	== cmp_region.y_start)
				&& (y_end	== cmp_region.y_end);
		}

		bool tagGeoRegion::includes(const GeoScale x, const GeoScale y)
		{
			return (x_start <= x) 
				&& (x_end	>= x) 
				&& (y_start <= y) 
				&& (y_end	>= y);
		}
		
		bool tagGeoRegion::is_overlap(const tagGeoRegion & region)const
		{
			unsigned long x_delta = abs(  ( (x_end > region.x_end) ? x_end : region.x_end) 
										- ( (x_start < region.x_start) ? x_start : region.x_start) );
			unsigned long y_delta = abs(  ( (y_end > region.y_end) ? y_end : region.y_end)
										- ( (y_start < region.y_start) ? y_start : region.y_start) );
			unsigned long x_length = abs(x_end - x_start) + abs(region.x_end - region.x_start);
			unsigned long y_length = abs(y_end - y_start) + abs(region.y_end - region.y_start);
			return (x_delta < x_length) && (y_delta < y_length);
		}

//	} // end of namespace Scale_Pixel_Pixel


} // end of namespace GIS_Embedded

