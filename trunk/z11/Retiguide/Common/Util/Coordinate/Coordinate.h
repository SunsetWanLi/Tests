#ifndef COORDINATE_H
#define COORDINATE_H

#include <vector>
#include "Matrix.h"
#include "../../Types/basic_types.h"
using GIS_Embedded::int_32;

class Coordinate
{
private:
	int c_width;
	int c_height;

	double m_eye_angle;
	double m_eye_to_car_angle;
	double m_eye_to_car;
	double m_eye_to_map;//距离

	double m_znear, m_zfar, m_fovy, m_aspect;

	Matrix4 m_view_matrix;
	Matrix4 m_result;
	DEViewFrustrum m_frustrum;
	
public:
	vector<vector<VertexShort> > TransSearchTbl;
public:
	Coordinate()
	{
	};

	~Coordinate()
	{
	};

	int changeCoord(int width, int height, double eye_to_car, double car_pos_x, double car_pos_y, double car_angle);
	int _changeCoord_(double from_x, double from_y, double *to_x, double *to_y)const;
	int __changeCoord_(int_32 height, int_32 width, int_32 *to_x, int_32 *to_y) const;
	//int screen2map(int screen_x, int screen_y, double *map_x, double *map_y);
	int screen2map(int screen_x, int screen_y, int_32 *map_x, int_32 *map_y);
	DEViewFrustrum getLocalViewFrustrum();
	void updateViewFrustrum();
	// DERay & plane 
	static bool intersects(const DERay& ray, const DEPlane& plane, double *distance);
	static Vertex3 getIntersectPoint( const DERay& ray, double distance);

	//5.13xiajie
	void BuildSearchTable(int left_top_x, int left_top_y, 
		int right_top_x, int right_top_y, 
		int left_down_x, int left_down_y, 
		int right_down_x, int right_down_y,
		int window_width, int window_height);

	void BuildSearchTableNew(double start_x, double start_y,
		double x_pixel,double y_pixel,
		int left_top_x, int left_top_y, 
		int right_top_x, int right_top_y, 
		int left_down_x, int left_down_y, 
		int right_down_x, int right_down_y,
		int window_width, int window_height);

	int Ajust(double src);
};


#endif//