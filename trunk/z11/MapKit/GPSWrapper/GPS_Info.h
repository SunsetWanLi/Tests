#ifndef TEST_GPS_INFO_H_
#define TEST_GPS_INFO_H_

//#include <time.h>
//#include <string>
//using namespace std;
namespace IS_GIS
{
#pragma pack(push,1)
	struct GPS_Info
	{
		// UTC Time
		struct UTC_Time
		{
			int hh;
			int mm;
			int ss;
		} utc_time;
		// UTC Date
		struct UTC_Date
		{
			int yy;
			int mm;
			int dd;
		} utc_date;
	//	tm time;
		// Latitude, in degrees. Positive numbers indicate north latitude
		double latitude;
		// Longitude, in degrees. Positive numbers indicate east longitude
		double longitude;
		// Speed, in knots (nautical miles).
		float speed;
		// Heading, in degrees. A heading of zero is true north
		float heading;		
		// Altitude, in meters, with respect to sea level.
		float altitude_sea_level;
		// Altitude, in meters, with respect to the WGS84 ellipsoid. 
		float altitude_84;

		GPS_Info() : latitude(0.0), longitude(0.0), 
			speed(0.0), heading(0.0), altitude_sea_level(0.0), altitude_84(0.0)
		{
			utc_time.hh = utc_time.mm = utc_time.ss = 0;
			utc_date.yy = 2007, utc_date.mm = 1, utc_date.dd = 1;
		}
	};

	struct Port_Info
	{
		unsigned int port;
		unsigned int baud;
	};

	class SAInfo  //单个卫星信息
	{
			
		// Latitude, in degrees. Positive numbers indicate north latitude
	public: double latitude;
		// Longitude, in degrees. Positive numbers indicate east longitude
	public: double longitude;

	public: double height_angle;//高度角
	public: double direction_angle;//方位角
	public: int Number;//序号
	public: int Strength;//强度 
	public: SAInfo()
	{
		latitude=0;
		longitude=0;
		height_angle=0;
		direction_angle=0;
		Number=0;
		Strength=0;
	}
	};

	class  Satellites_Info//可见卫星信息集合
	{
	public:
		SAInfo Satellite[32];//拉丝终端中会读到23颗卫星，据网上资料说总共有32个编号卫星，故设为32
		int Valid_Number;//Satellite数组的前Valid_Number个存放有效的卫星信息
		int Total_Number;//所有从串口中读取出的卫星信息，包括无效的

		bool Gps_Valid; //如读取的有效卫星颗数>=3，则表示有效。。。3为暂定
	public:
		Satellites_Info()
		{
			Valid_Number = 0;
			Total_Number = 0;
			Gps_Valid = false;
		}
	};
#pragma  pack(pop)
	/*
	class  Satellites_Info//可见卫星信息集合
	{
	public: SAInfo Satellite[12];//存储卫星信息结构
	public: int Total_Number;
	public: int Current_Number;
	public: Satellites_Info()
		 {
				for (int i = 0; i < 12; i++)
				{
					Satellite[i]=SAInfo();
					Total_Number = 0;
					Current_Number = 0;
				}
		 } 
	public: void Update(SAInfo info)
	   {

				for (int i = 0; i < Current_Number; i++)
				{
					if (Satellite[i].Number==info.Number)
					{
						Satellite[i] = info;
						return;
					}

				}
				if (Current_Number < Total_Number)
				{
					Satellite[Current_Number] = info;
					Current_Number++;
				}
	   }



	};*/
}
#endif //TEST_GPS_INFO_H_