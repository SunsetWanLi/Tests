#include "../../Retiguide/Common/CRS/GlobalCrt.h"
#include "../../Retiguide/SpatialMath/SpatialMath.h"
#include "GPS_Filter.h"
#include "GPS_Info.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
using namespace std;
using namespace GIS_Embedded;
namespace IS_GIS
{
bool GPS_Filter::force_to_good_info = false;	//GPSԴת��ʱ���ݿ��ܾ�������Զ ��ʱǿ������ȷ���ݴ���

GPS_Filter::GPS_Filter_Para::GPS_Filter_Para()
{
	//������Щ��ʼֵ��������ģ�ʵ�ʲ����ã��������Ҫ��������
	m_distance_zero = 1.0;
	m_distance_impossible = 500.0; // zgr, Լ500��
	m_speed_zero = 0.1; //grf 0.8before
	m_speed_slow = 2.0;
	m_turnback_start_angle = 150.0; 
	m_turnback_end_angle = 210.0;
	m_acceleration_impossible = 20.0;//TOSET;
	m_history_size = 3;
	m_max_ang_offset_slow = 10.0;
}

GPS_Filter::GPS_Filter(void)
{
	m_refine_mode = GPSFRM_NO_REFINE;
	m_lasted_times = 0;
}

GPS_Filter::GPS_Filter(GPS_Filter_Refine_Mode mode, GPS_Filter_Para para)
{
	m_refine_mode = mode;
	m_lasted_times = 0;
	m_para = para;
}

GPS_Filter::~GPS_Filter(void)
{
}

double GPS_Filter::getDistance(const GPS_Info &info)
{
	long prj_x, prj_y;
	long last_x, last_y;
	GlobalCrt crt;
	crt.geoToPrj(info.longitude, info.latitude, &prj_x, &prj_y);
	crt.geoToPrj(m_last_used.longitude, m_last_used.latitude, &last_x, &last_y);

	double distance = sqrt(0.0+(prj_x - last_x) * (prj_x - last_x) + (prj_y - last_y) * (prj_y - last_y));
	return distance;
}

float GPS_Filter::getDistanceHeading(const GPS_Info &info)
{
	long prj_x, prj_y;
	long last_x, last_y;
	GlobalCrt crt;
	crt.geoToPrj(info.longitude, info.latitude, &prj_x, &prj_y);
	crt.geoToPrj(m_last_used.longitude, m_last_used.latitude, &last_x, &last_y);
	Pos2DEx p_start(last_x, last_y);
	Pos2DEx p_end(prj_x, prj_y);
	double angle = SpatialMath::calLineNAngle(p_start, p_end);
	float heading = angle / M_PI * 180.0;
	return heading;
}

float GPS_Filter::getAcceleration(const GPS_Info &info)
{
	//float new_d0 = info.speed * cos(info.heading / 360.0 * SPATIAL_MATH_PI);
	//float new_d1 = info.speed * sin(info.heading / 360.0 * SPATIAL_MATH_PI);
	//Pos2D speed_new(new_d0, new_d1);
	//float old_d0 = m_last_used.speed * cos(m_last_used.heading / 360.0 * SPATIAL_MATH_PI);
	//float old_d1 = m_last_used.speed * sin(m_last_used.heading / 360.0 * SPATIAL_MATH_PI);
	//Pos2D speed_old(old_d0, old_d1);
	//float a = SpatialMath::getDistance(speed_old, speed_new);

	// zgr, 2008-01-31
	float new_north = info.speed * cos(info.heading / /*360.0*/180.0 * M_PI);
	float new_east = info.speed * sin(info.heading / /*360.0*/180.0 * M_PI);
	
	float old_north = m_last_used.speed * cos(m_last_used.heading / /*360.0*/180.0 * M_PI);
	float old_east = m_last_used.speed * sin(m_last_used.heading / /*360.0*/180.0 * M_PI);
	
	// zgr, 2008-01-31, �������¼���ʽ�� SpatialMath::getDistance(~)��ͬ�������岻ͬ
	float a = sqrt(pow(new_north - old_north,2) + pow(new_east - old_east,2));
	return a / (m_lasted_times + 1);
}

float GPS_Filter::getRefinedHeadingByAverage(const GPS_Info &info)
{
	float rh = 0.0;
	//for(int i = 0; i < m_history_gpsinfo.size(); i++)
	//{
	//	rh += m_history_gpsinfo[i].heading;
	//}
	//rh += info.heading;
	//rh += m_last_used.heading;
	//return rh / (m_history_gpsinfo.size() + 2);
	
	m_history_gpsinfo.push_back(info); // zgr, ����Ǽ�¼֮��������Ҫ������push_back
	
	rh = getHistoryAvgHeading();
	
	m_history_gpsinfo.pop_back();

	return rh;
}

float GPS_Filter::getRefinedHeading(const GPS_Info &info)
{
	float rh;
	switch(m_refine_mode)
	{
	case GPSFRM_SPEED_AVERAGE : rh = getRefinedHeadingByAverage(info); break;
	case GPSFRM_DISTANCE_HEADING : rh = getDistanceHeading(info); break;
	case GPSFRM_MIX_REFINE : rh = (getRefinedHeadingByAverage(info) + getDistanceHeading(info)) / 2.0; break;
	default: rh = info.heading;
	}
	return rh;
}

void GPS_Filter::addToHistory(const GPS_Info &info)
{
	if(m_history_gpsinfo.size() == m_para.m_history_size)
	{
		/*m_history_gpsinfo.erase(m_history_gpsinfo.begin());*/
		m_history_gpsinfo.pop_front(); // zgr, 2008-01-28, ʹ��deque����vector
		//m_history_gpsinfo.push_back(info);
	}
	//else
		m_history_gpsinfo.push_back(info);

}

bool GPS_Filter::isStoping(const GPS_Info &info)
{
	if(noLastUsed())
	{
		return info.speed < m_para.m_speed_zero;
	}
	
	double distance = getDistance(info);
	// zgr, 2008-01-28
	//double angle_off_set = fabs(info.heading - getHistoryAvgHeading() );
	//if(angle_off_set > 180)
	//	angle_off_set = 360 - angle_off_set;

	// zgr, ���� 01-14��GPS_Data���п�������һ���ڹ���ʱλ�Ʊ仯���󣬵��ǽǶ�������ת��
	//      ��ʱ�����Կ�����ת���̣�����Ƕȱ仯����m_max_ang_offset_slow����������״̬������
	//printf("distance=%f m_para.m_distance_zero=%f info.speed =%f m_para.m_speed_zero=%f\n",distance,m_para.m_distance_zero,info.speed,m_para.m_speed_zero);
	return distance < m_para.m_distance_zero || info.speed < m_para.m_speed_zero; 
		//|| info.speed < m_para.m_speed_slow && angle_off_set > m_para.m_max_ang_offset_slow;// zgr, ����ʱ����Ƕ�Ʈ�úܴ�,������ֹ����
}

bool GPS_Filter::isTooFar(const GPS_Info &info)
{
	if(noLastUsed())
	{
		return false;
	}

	long prj_x, prj_y;
	long last_x, last_y;
	GlobalCrt crt;
	crt.geoToPrj(info.longitude, info.latitude, &prj_x, &prj_y);
	crt.geoToPrj(m_last_info.longitude, m_last_info.latitude, &last_x, &last_y);

	double distance = sqrt(0.0+(prj_x - last_x) * (prj_x - last_x) + (prj_y - last_y) * (prj_y - last_y));
	double avg_speed = (info.speed + m_last_info.speed)/2;// * 1.85 / 3.6 / 2;
	double time = max((info.utc_time.mm * 60 + info.utc_time.ss - m_last_info.utc_time.mm * 60 - m_last_info.utc_time.ss), 1);
	
	return distance > avg_speed * time * 5;

}

bool GPS_Filter::noLastUsed()
{
	return m_last_used.altitude_84 == 0 && m_last_used.altitude_sea_level == 0 && m_last_used.heading == 0
		&& m_last_used.latitude == 0 && m_last_used.longitude == 0 && m_last_used.speed == 0;
}

bool GPS_Filter::isTurnedBack(const GPS_Info &info)
{
	float turned_angle = info.heading - m_last_used.heading;
	if(turned_angle < 0)
	{
		turned_angle += 360.0;
	}
	return turned_angle > m_para.m_turnback_start_angle && turned_angle < m_para.m_turnback_end_angle;
}

bool GPS_Filter::isHeadingWrong(const GPS_Info &info)
{
	if(noLastUsed()) // zgr, 2008-01-30
		return false;

	return false; // zgr, 2008-01-30, ��ʵ��
	/*float dh = getDistanceHeading(info);
	return (dh - m_last_used.heading <= 180.0 && info.heading - m_last_used.heading <= 180.0)
		|| (dh - m_last_used.heading > 180.0 && info.heading - m_last_used.heading > 180.0);*/
}

//bool GPS_Filter::isAccelerationTooLarge(const GPS_Info &info)
//{
//	if(noLastUsed()) // zgr, 2008-01-30
//		return false;
//
//	float a = getAcceleration(info);
//	return a > m_para.m_acceleration_impossible;
//}

bool GPS_Filter::isDrivingSlow(const GPS_Info &info)
{
	return info.speed < m_para.m_speed_slow;
}

bool GPS_Filter::isInfoBad(const GPS_Info &info)
{
	if(force_to_good_info)
		return false;
	if(noLastUsed())
		return !isValidGPSPos(info);

	bool ret = !isValidGPSPos(info) || isTooFar(info) || isHeadingWrong(info) /*|| isAccelerationTooLarge(info)*/;//  zgr, 2008-01-28
#ifdef DEBUG
	if(ret)
	{
		ret = !isValidGPSPos(info) || isTooFar(info) || isHeadingWrong(info) /*|| isAccelerationTooLarge(info)*/; // 
	}
#endif
	return ret;
}

GPS_Filter::GPS_Filter_Result GPS_Filter::filter(const GPS_Info &info_read, GPS_Info &info_refined)
{
	m_last_info = info_read;
	// zgr, 2008-01-28,���ж��Ƿ���ЧGPS_Info
	if(isInfoBad(info_read))
	{
		info_refined = info_read;
		m_lasted_times++;
		return GPSFR_BAD;
	}
	// zgr, ��һ����Чֵ��������������ʹֹͣҲû��ϵ
	if(noLastUsed())
	{
		addToHistory(info_read);
		m_last_used = info_read;
		m_lasted_times = 0;
		info_refined = info_read;
		return GPSFR_NORMAL;
	}

	if(isStoping(info_read))
	{
		addToHistory(info_read);
		info_refined = info_read;
		if(!noLastUsed())
		{
			m_lasted_times++;
			info_refined.heading = m_last_used.heading; // zgr, ֹͣʱʹ���ϴε���Ч����ֵ
		}
		return GPSFR_STOP;
	}

	// zgr, ���٣���������״̬��1. �ո�������2.ɲ��ֹͣ
	if(isDrivingSlow(info_read))
	{
		info_refined = info_read;
		// zgr, 2008-01-28
		double angle_off_set = fabs(info_read.heading - getHistoryAvgHeading() );
		if(angle_off_set > 180)
			angle_off_set = 360 - angle_off_set;

		if(angle_off_set > min(90.0f, m_para.m_history_size * m_para.m_max_ang_offset_slow)) // zgr,����ʱ����Ƕ�ƫ�ƹ�����ɲ��ʱͻȻ���򣬶�����Ӱ����󣬹��˵�
			info_refined.heading = m_last_used.heading;
		else if(angle_off_set > m_para.m_max_ang_offset_slow) // zgr, �Ƕ���һ��ƫ�ƣ�����
			info_refined.heading = getRefinedHeading(info_read);

		addToHistory(info_refined); 
		m_last_used = info_refined;
		m_lasted_times = 0;
		return GPSFR_REFINED;
	}
	addToHistory(info_read);
	m_last_used = info_read;
	m_lasted_times = 0;
	info_refined = info_read;
	return GPSFR_NORMAL;
}

void GPS_Filter::setRefineMode(GPS_Filter_Refine_Mode mode)
{
	m_refine_mode = mode;
}

void GPS_Filter::setPara(GPS_Filter_Para para)
{
	m_para = para;
}

// zgr, 2008-01-28
float GPS_Filter::getHistoryAvgHeading()
{
	float dir_sum = 0;

	uint_32 i = 0, size = m_history_gpsinfo.size();

	if(size == 0)
		return 0;
	
	dir_sum = m_history_gpsinfo[0].heading;
	for(i = 1; i < size; ++i)
	{
		int old_weight = i;
		float old_avs = fmod(dir_sum / old_weight, 360);
		float cur_heading = m_history_gpsinfo[i].heading;
		
		if(fabs(cur_heading - old_avs) > 180)
		{
			if(old_avs < cur_heading) 
			{
				dir_sum += 360 * old_weight + cur_heading; // zgr, �������Ͻ��ķ����Խϴ��Ȩ��
			}
			else
			{
				dir_sum += cur_heading + 360;
			}
		}
		else
		{
			dir_sum += cur_heading;
		}
		//dir_sum += v_dirs[i];
	}

	return fmod(dir_sum / size, 360);
}

bool GPS_Filter::isValidGPSPos(const GPS_Info & gps_info)
{
	return !(gps_info.longitude == 0.0 && gps_info.latitude == 0.0)
			&& gps_info.longitude >= -180.0 && gps_info.longitude <= 180.0
			&& gps_info.latitude >= -90.0 && gps_info.latitude <= 90.0;
}

GPS_Info GPS_Filter::getLastValidInfo()
{
	return m_last_used;
}

}