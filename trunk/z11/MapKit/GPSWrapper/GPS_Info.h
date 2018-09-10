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

	class SAInfo  //����������Ϣ
	{
			
		// Latitude, in degrees. Positive numbers indicate north latitude
	public: double latitude;
		// Longitude, in degrees. Positive numbers indicate east longitude
	public: double longitude;

	public: double height_angle;//�߶Ƚ�
	public: double direction_angle;//��λ��
	public: int Number;//���
	public: int Strength;//ǿ�� 
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

	class  Satellites_Info//�ɼ�������Ϣ����
	{
	public:
		SAInfo Satellite[32];//��˿�ն��л����23�����ǣ�����������˵�ܹ���32��������ǣ�����Ϊ32
		int Valid_Number;//Satellite�����ǰValid_Number�������Ч��������Ϣ
		int Total_Number;//���дӴ����ж�ȡ����������Ϣ��������Ч��

		bool Gps_Valid; //���ȡ����Ч���ǿ���>=3�����ʾ��Ч������3Ϊ�ݶ�
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
	class  Satellites_Info//�ɼ�������Ϣ����
	{
	public: SAInfo Satellite[12];//�洢������Ϣ�ṹ
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