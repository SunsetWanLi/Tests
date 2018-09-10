#ifndef _GIS_NAVI_BIND_H_
#define _GIS_NAVI_BIND_H_

#include "../../Common/Types/basic_types.h"
#include "../../Common/Types/Fraction.h"
#include "../../../MapKit/GPSWrapper/GPS_Info.h"
#include "../../../MapKit/Embedded/Application/PathResult.h"
#include "../AnaEnumCommon.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
using namespace IS_GIS;
using namespace Embedded;

namespace GIS_Embedded
{
class CurvePoint;
class NaviPathContext;
struct BindWeight;
class GlobalCrt;

struct Navi_GPS_Info : public GPS_Info
{
public:
	explicit Navi_GPS_Info(const GPS_Info & gps_info);
private:
	static GlobalCrt * CRT;
};


struct BindWeight
{
	//BindWeight():m_dis(DBL_MAX), m_dir(M_PI / 2) GRF,DBL_MAX no definition
	BindWeight():m_dis(INT_MAX), m_dir(M_PI / 2)
	{}
	double m_dis;
	double m_dir;
	static bool lessor(const BindWeight & l_value,const BindWeight & r_value)
	{
		return l_value.m_dis < r_value.m_dis;
	}
	bool operator!=(const BindWeight & l_value)const
	{
		return m_dis != l_value.m_dis || m_dir != l_value.m_dir;
	}
	static BindWeight NULL_BIND_WEIGHT;
};

class NaviBind
{
public:
	NaviBind(){}
	~NaviBind(){}
public:
	static double DIR_TOL_BACK; // �����󻡶�ƫ��
	static double DIR_TOL_FORTH; //�����ǰ����ƫ��
	static double MAX_OFF_SET; // the max 0ff set pos distance from the road
	static unsigned int MAX_DIR_BACK_COUNT;

	// zgr,2008-01-15,��·�󶨾���
	static double BOUND_MAX_OFF_SET;
	static double BOUND_MAX_OFF_SET_FAR; // ����,far��nearֻ����Զ���
	static double BOUND_MAX_OFF_SET_NEAR;

	static double PRIORITY_FOR_BIND_ROAD; // when bind, the path_result is given the higher priority, ·�������ȼ�
public:
	// ���������ݵ�ǰGPS�Լ���ǰ·�����а󶨣�
	// ����� bound_cp�󶨵㣬bind_state��״̬��PathContext--����󶨵�·����ʱ����·�������ģ�������ʾ��
	// Navi_GPS_Info��ͬ��ԭʼ��GPS_Info�� ����ΪͶӰ���Ƕ�ΪΪ����
	void naviBind(const Navi_GPS_Info & gps_info, PathResult * path_result, Pos2DEx * bound_pos,AnaPosVerdict * bind_state, NaviPathContext * path_navi_context);
	static void resetLastPath(); // ·������ʱ��������
private:
	//
	struct PathBindParam
	{
		uint_32 m_ps_index;// the subscript of cur_bound_path_segment, ��ǰ�󶨵�·��λ��·�����Ķ�
		Fraction m_pos_param;// the bound param of cur_bound_path_segment, ��ǰ��λ��λ�ڰ�·�εľ���λ��
		double m_off_set;// the off_set of pos and bound_ps

		double m_bound_angle;// the current angle of cur_bound_pos(according to the bound_path_segment)
		Pos2DEx m_bound_pos;
	};

	void bindToPath(const Pos2DEx & pos, const PathResult & path_result, PathBindParam * pbp);
private:
	double getAngle(const vector<Pos2DEx> &ctrl_points, bool from_start);

	// calculator of pointBindPLine
	static bool bindCalculator(const BindWeight & l_value,const BindWeight & r_value);
private:
	static unsigned int dir_back_count;

private:
	static const PathResult * last_path; // shallow copy, the addres  of the path result, to compare if it is the same with last time
	static uint_32 last_ps_index;
	// last_bind_param: ��¼�ϴΰ󶨵�λ��param������getNextCrossʱ������
	static Fraction last_bind_param;
	// ��¼�ϴ�ƫ���������ڼ��� ���¼���·����ƫ��������,
	static double last_offset;

};

}

#endif
