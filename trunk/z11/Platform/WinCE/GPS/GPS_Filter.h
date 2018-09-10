#pragma once
//#include <vector>
#ifdef WINCE
#include <deque>
#include "GPS_Info.h"

//#include "SpatialMath.h"


using namespace std;


namespace z11
{
class GPS_Filter
{
public:
	enum GPS_Filter_Result
	{
		GPSFR_NORMAL,	//�ź�������info_refined = info_read
		GPSFR_STOP,	//��ֹ״̬��info_refined = info_read���ɵ����߾����Ƿ�����
		GPSFR_BAD,	//�ź��쳣��info_refined = info_read���ɵ����߾����Ƿ�����
		GPSFR_REFINED	//�źŲ��ȶ���info_refinedȡ������ѡ��ķ�������ģʽ
	};
	enum GPS_Filter_Refine_Mode
	{
		GPSFRM_SPEED_AVERAGE,	//ʹ��һ��ʱ���ƽ��ֵ����
		GPSFRM_DISTANCE_HEADING,	//ʹ��ǰ��·�̵�ָ������
		GPSFRM_MIX_REFINE,	//��������������ƽ��ֵ
		GPSFRM_NO_REFINE	//������
	};
	struct GPS_Filter_Para
	{
	public:
		//�������Щ��ֵ��Ӧ�ø����ⲿ�����źŵ�Ƶ������
		double m_distance_zero;	//�ź�֮��λ�þ���С���������ź�Ӧ�ñ���������Ϊ��ֹ
		double m_distance_impossible;	//�ź�֮��λ�þ�������������ź�Ӧ�ñ���������Ϊ���ź�
		float m_speed_zero;	//�źŵ��ٶ�С�������ź�Ӧ�ñ���������Ϊ��ֹ
		float m_speed_slow;	//�źŵ��ٶ�С��������Ϊ�ڵ�����ʻ��������������
		float m_max_ang_offset_slow; //zgr, ����ʱ�ĽǶ���������,�����ж��Ƿ�����
		float m_turnback_start_angle;	//�ź�֮�䷽��ǶȲ�������֮�䣬���ź�Ӧ�ñ���������Ϊ���ź�
		float m_turnback_end_angle;
		//ע�⣺������ν�ġ����ٶȡ���ָƽ���õ������ź�֮����ٶȲʸ��ֵ�����ٶȵĵ�λͬGPS_Info
		float m_acceleration_impossible;//�ź�֮����ٶȴ����������ź�Ӧ�ñ���������Ϊ���źţ�
		int m_history_size;	//��Ҫ��¼����ʷ��¼��������Ҳ����������ƽ��ֵ����������
	public:
		GPS_Filter_Para();
	};
private:
	GPS_Filter_Refine_Mode m_refine_mode;	//��������ģʽ
	/*vector*/deque<GPS_Info> m_history_gpsinfo;	//GPS_Info����ʷ��¼ // zgr, 2008-01-28, ʹ��deque����vector
	GPS_Info m_last_used;	//��һ����Ч�ź�
	GPS_Info m_last_info;	//by my, 09.01.08 ��һ���ź� ��һ������Ч��Ϣ
	int m_lasted_times;	//��һ����Ч�ź��Ѿ�����û�л�������ʼ����
	GPS_Filter_Para m_para;
public:
	GPS_Filter(void);
	GPS_Filter(GPS_Filter_Refine_Mode mode, GPS_Filter_Para para);
public:
	~GPS_Filter(void);
private:
	bool noLastUsed();
	double getDistance(const GPS_Info &info);
	float getDistanceHeading(const GPS_Info &info);
	float getAcceleration(const GPS_Info &info);
	float getRefinedHeadingByAverage(const GPS_Info &info);
	float getRefinedHeading(const GPS_Info &info);
	void addToHistory(const GPS_Info &info);
	// zgr, 2008-01-28,  ������ʷ��Ϣ����ƽ���Ƕ�
	float getHistoryAvgHeading();

	bool isStoping(const GPS_Info &info);
	// zgr, 2008-01-28
	bool isValidGPSPos(const GPS_Info & info);
	bool isTooFar(const GPS_Info &info);
	bool isTurnedBack(const GPS_Info &info);
	bool isHeadingWrong(const GPS_Info &info);
	//bool isAccelerationTooLarge(const GPS_Info &info);
	bool isDrivingSlow(const GPS_Info &info);
	bool isInfoBad(const GPS_Info &info);
public:
	GPS_Filter_Result filter(const GPS_Info &info_read, GPS_Info &info_refined);
	GPS_Info getLastValidInfo();
	void setRefineMode(GPS_Filter_Refine_Mode mode);
	void setPara(GPS_Filter_Para para);
	static bool force_to_good_info;	//GPSԴת��ʱ���ݿ��ܾ�������Զ ��ʱǿ������ȷ���ݴ���
};
}
#endif