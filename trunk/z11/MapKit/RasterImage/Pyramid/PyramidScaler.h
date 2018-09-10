#ifndef __RASTERIMAGE_PYRAMID_SCALER_H__
#define __RASTERIMAGE_PYRAMID_SCALER_H__

namespace Raster_GIS
{
	namespace Pyramid_Space 
	{

		/*以（degree,minute,second）表示点地理坐标，以像素表示地图坐标 */
		// 目前系统不采用这种方式
//		namespace Scale_DMS_Pixel
//		{
//			typedef unsigned long MapScale;
			struct Pixel
			{
				unsigned char Blue;
				unsigned char Green;
				unsigned char Red;
			public:
				Pixel()
				{
					Blue = Green = Red = 0;
				}
			};

			class Geo_Scale
			{
			public:
				double degree;
				double minute;
				double second;
				/*
				经纬度值：正值表示东经和北纬，负值表示西经和南纬
				*/
				char direction;
				/*
				direction 用于区别经度和纬度 
				'U'  --> 未定义，可认为是经度差或纬度差
				'O'  --> 经度
				'A'  --> 纬度
				以上均大写
				*/
			public:
				Geo_Scale(double deg = 0.0,double min = 0.0,double sec = 0.0 , char direction = 'U');
				const Geo_Scale& operator = (const Geo_Scale & source);


				void normalize(Geo_Scale &scale)const;

				friend Geo_Scale operator- (const Geo_Scale & minus);
				Geo_Scale operator+ (const Geo_Scale & add_right)const;
				Geo_Scale operator- (const Geo_Scale & sub_right)const;
				Geo_Scale operator* (double multiple)const;
				Geo_Scale operator/ (double multiple)const;
				bool operator == (const Geo_Scale & cmp)const;
				bool operator > (const Geo_Scale & cmp)const;
				bool operator < (const Geo_Scale & cmp)const;
				bool operator >= (const Geo_Scale & cmp)const;
				bool operator <= (const Geo_Scale & cmp)const;
			};

			Geo_Scale operator-(const Geo_Scale & minus);

			typedef struct tag_GeoRegion
			{
				Geo_Scale longitude_start,longitude_end;	//经度范围
				Geo_Scale latitude_start,latitude_end;		//纬度范围
			public:
				tag_GeoRegion();
				tag_GeoRegion(const Geo_Scale &lo_st,const Geo_Scale &lo_nd,const Geo_Scale &la_st,const Geo_Scale &la_nd);
				const tag_GeoRegion & operator = (const tag_GeoRegion & source_region);
				bool operator == (const tag_GeoRegion & cmp_region);
				bool includes(const Geo_Scale & pot_longigude , const Geo_Scale & pot_latitude);
				bool is_overlap(const tag_GeoRegion & region)const;
			}GeoRegion;

			typedef unsigned char MapImage;

//		} //end of namespace/struct Scale_DMS_Pixel


		// 目前系统采用的方式：
		/*	即图上只用像素作为坐标刻度，而不是经纬度坐标
			同时，假设由其它坐标（如经纬度）到图上区域的坐标转换在外部进行
		*/
//		namespace Scale_Pixel_Pixel
//		{
			typedef unsigned long MapScale;
	/*		struct Pixel
			{
				unsigned char Blue;
				unsigned char Green;
				unsigned char Red;
			public:
				Pixel()
				{
					Blue = Green = Red = 0;
				}
			};
*/ //多余定义与 namespace Pyramid—Space 中重复
			typedef long GeoScale;

			typedef struct tagGeoRegion
			{
				GeoScale x_start , x_end;
				GeoScale y_start , y_end;
			public:
				tagGeoRegion(const GeoScale x_st = 0L,const GeoScale x_nd = 0L,const GeoScale y_st = 0L,const GeoScale y_nd = 0L);
				const tagGeoRegion & operator = (const tagGeoRegion & source_region);
				bool operator == (const tagGeoRegion & cmp_region);
				bool includes(const GeoScale x , const GeoScale y);
				bool is_overlap(const tagGeoRegion & region)const;
			}GeoRegion_Scale;

	//		typedef unsigned char MapImage;

//		} // end of namespace Scale_Pixel_Pixel

	} // end of namespace Pyramid_Space
} // end of namespace Raster_GIS
#endif