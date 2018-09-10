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
		GPSFR_NORMAL,	//信号正常，info_refined = info_read
		GPSFR_STOP,	//静止状态，info_refined = info_read，由调用者决定是否舍弃
		GPSFR_BAD,	//信号异常，info_refined = info_read，由调用者决定是否舍弃
		GPSFR_REFINED	//信号不稳定，info_refined取决于所选择的方向修正模式
	};
	enum GPS_Filter_Refine_Mode
	{
		GPSFRM_SPEED_AVERAGE,	//使用一段时间的平均值修正
		GPSFRM_DISTANCE_HEADING,	//使用前段路程的指向修正
		GPSFRM_MIX_REFINE,	//上面两种修正的平均值
		GPSFRM_NO_REFINE	//不修正
	};
	struct GPS_Filter_Para
	{
	public:
		//下面的这些界值都应该根据外部输入信号的频率设置
		double m_distance_zero;	//信号之间位置距离小于它，后信号应该被抛弃，认为静止
		double m_distance_impossible;	//信号之间位置距离大于它，后信号应该被抛弃，认为坏信号
		float m_speed_zero;	//信号的速度小于它，信号应该被抛弃，认为静止
		float m_speed_slow;	//信号的速度小于它，认为在低速行驶，开启方向修正
		float m_max_ang_offset_slow; //zgr, 低速时的角度最大便移量,用于判断是否修正
		float m_turnback_start_angle;	//信号之间方向角度差在它们之间，后信号应该被抛弃，认为坏信号
		float m_turnback_end_angle;
		//注意：这里所谓的“加速度”是指平均拿到两次信号之间的速度差（矢量值），速度的单位同GPS_Info
		float m_acceleration_impossible;//信号之间加速度大于它，后信号应该被抛弃，认为坏信号，
		int m_history_size;	//需要记录的历史记录的数量，也就是用来求平均值修正的数量
	public:
		GPS_Filter_Para();
	};
private:
	GPS_Filter_Refine_Mode m_refine_mode;	//方向修正模式
	/*vector*/deque<GPS_Info> m_history_gpsinfo;	//GPS_Info的历史纪录 // zgr, 2008-01-28, 使用deque代替vector
	GPS_Info m_last_used;	//上一个有效信号
	GPS_Info m_last_info;	//by my, 09.01.08 上一个信号 不一定是有效信息
	int m_lasted_times;	//上一个有效信号已经几次没有换过，初始是零
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
	// zgr, 2008-01-28,  根据历史信息计算平均角度
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
	static bool force_to_good_info;	//GPS源转换时数据可能距离相差很远 此时强制做正确数据处理
};
}
#endif