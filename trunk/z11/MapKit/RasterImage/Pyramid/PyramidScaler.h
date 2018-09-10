#ifndef __RASTERIMAGE_PYRAMID_SCALER_H__
#define __RASTERIMAGE_PYRAMID_SCALER_H__

namespace Raster_GIS
{
	namespace Pyramid_Space 
	{

		/*�ԣ�degree,minute,second����ʾ��������꣬�����ر�ʾ��ͼ���� */
		// Ŀǰϵͳ���������ַ�ʽ
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
				��γ��ֵ����ֵ��ʾ�����ͱ�γ����ֵ��ʾ��������γ
				*/
				char direction;
				/*
				direction �������𾭶Ⱥ�γ�� 
				'U'  --> δ���壬����Ϊ�Ǿ��Ȳ��γ�Ȳ�
				'O'  --> ����
				'A'  --> γ��
				���Ͼ���д
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
				Geo_Scale longitude_start,longitude_end;	//���ȷ�Χ
				Geo_Scale latitude_start,latitude_end;		//γ�ȷ�Χ
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


		// Ŀǰϵͳ���õķ�ʽ��
		/*	��ͼ��ֻ��������Ϊ����̶ȣ������Ǿ�γ������
			ͬʱ���������������꣨�羭γ�ȣ���ͼ�����������ת�����ⲿ����
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
*/ //���ඨ���� namespace Pyramid��Space ���ظ�
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