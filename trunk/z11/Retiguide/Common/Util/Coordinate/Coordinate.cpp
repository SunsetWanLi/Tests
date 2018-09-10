
#include "Coordinate.h"
#include <cmath>
#include <iostream>
#include <vector>
using namespace std;
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
int Coordinate::_changeCoord_(double from_x, double from_y, double *to_x, double *to_y)const
{
	Vector4 pre_trans_pos;
	
	pre_trans_pos.x = from_x;
	pre_trans_pos.y = from_y;
	pre_trans_pos.z = 0.0;
	pre_trans_pos.w = 1.0;

	pre_trans_pos = (this->m_result)*pre_trans_pos;

	float zDiv = pre_trans_pos.w == 0.0f ? 1.0f :
		(1.0f / pre_trans_pos.w );

	//if(zDiv<0)
	//	zDiv = -zDiv;

	int screen_w , screen_h ;
	screen_w = c_width/2;
	screen_h = c_height/2 ;

	*to_x = (screen_w * pre_trans_pos.x * zDiv) + screen_w;
	*to_y = screen_h - (screen_h * (pre_trans_pos.y * zDiv));

	//Vector3 pre_trans_pos;

	//pre_trans_pos.x = from_x;
	//pre_trans_pos.y = from_y;
	//pre_trans_pos.z = 0.0;

	//pre_trans_pos = this->m_result*pre_trans_pos;

	//float zDiv = pre_trans_pos.z == 0.0f ? 1.0f :
	//	(1.0f / pre_trans_pos.z );
	//int screen_w , screen_h ;
	//screen_w = c_width/2;
	//screen_h = c_height/2 ;

	//*to_x = (screen_w * pre_trans_pos.x * zDiv) + screen_w;
	//*to_y = screen_h - (screen_h * (pre_trans_pos.y * zDiv));

	return 1;
}

int Coordinate::changeCoord(int width, int height, double eye_to_car, double car_pos_x, double car_pos_y, double car_angle)
{
	double eye_radia;
	double eye_to_car_radia;
	double car_radia;
	//double PI = 3.141592654;

	c_width = width;
	c_height = height;

	m_eye_angle = 35;
	m_eye_to_car_angle = 35;

	m_eye_to_car = eye_to_car;

	eye_radia = M_PI*(m_eye_angle/180.0);
	eye_to_car_radia = M_PI*(m_eye_to_car_angle/180.0);
	car_radia = M_PI*(car_angle/180.0);

	Matrix4 mat_pro;
	Matrix4 mat_view;
	//Matrix4 mat_result;
	//this->m_result = new Matrix4();

	mat_pro = mat_pro.getGLPerspectiveMatrix(60, (float)c_width/(float)c_height, 0.1, 1000.0);
	m_znear = 0.1;
	m_zfar = 1000.0;
	m_fovy = 60.0;
	m_aspect = (float)c_width/(float)c_height;

	double eye_z = eye_to_car*sin(eye_to_car_radia);
	double eye_x = car_pos_x - sin(car_radia)*(eye_z/tan(eye_to_car_radia));
	double eye_y = car_pos_y - cos(car_radia)*(eye_to_car*cos(eye_to_car_radia));
	double up_x = sin(car_radia)*sin(eye_radia);
	double up_y = cos(car_radia)*sin(eye_radia);
	double up_z = cos(eye_radia);
	double tar_x = eye_x + sin(car_radia)*(eye_z/tan(eye_radia));
	double tar_y = eye_y + cos(car_radia)*(eye_z/tan(eye_radia));
	double tar_z = 0.0;

	m_eye_to_map = eye_z;

	mat_view = mat_view.getViewMatrix(eye_x, eye_y, eye_z, tar_x, tar_y, tar_z, up_x, up_y, up_z);
	//m_view_matrix = new Matrix4();
	m_view_matrix = mat_view;

	(this->m_result) = (mat_pro)*(mat_view);

	//pre_trans_pos = this->m_result*pre_trans_pos;

	//float zDiv = pre_trans_pos.w == 0.0f ? 1.0f :
	//	(1.0f / pre_trans_pos.w );

	//int screen_w , screen_h ;
	//screen_w = c_width/2;
	//screen_h = c_height/2 ;

	//*to_x = (screen_w * pre_trans_pos.x * zDiv) + screen_w;
	//*to_y = screen_h - (screen_h * (pre_trans_pos.y * zDiv));
	
	return 1;
}

//int Coordinate::screen2map(int screen_x, int screen_y, double *map_x, double *map_y)
//{
//	DERay ray;
//	DEPlane plane;
//	double distance;
//	Vector3 normal;
//	Vertex3 intersect_point;
//
//	normal.x = 0;
//	normal.y = 0;
//	normal.z = 1;
//	plane.d = 0/*m_eye_to_map*/;//0?不确定.
//	plane.normal = normal;
//
//	this->updateViewFrustrum();
//
//	Vector3 farLeftUp = this->m_frustrum.getFarLeftUp();
//	Vector3 lefttoright = this->m_frustrum.getFarRightUp() - farLeftUp;
//	Vector3 uptodown = this->m_frustrum.getFarLeftDown() - farLeftUp;
//
//	//int_32 height, width ;
//	//height = scene->getDeviceDriverNoRef()->getTargetNoRef()->getHeight() ;
//	//width = scene->getDeviceDriverNoRef()->getTargetNoRef()->getWidth() ;
//	float dx = screen_x / (float)c_width;
//	float dy = screen_y / (float)c_height;
//
//	ray.start = this->m_frustrum.cone_vertex;
//	ray.direction = farLeftUp + (lefttoright * dx) + (uptodown * dy) - ray.start;
//	ray.direction.normalize();
//
//	if(intersects(ray, plane, &distance))
//	{
//		intersect_point = getIntersectPoint( ray, distance);
//	}
//
//	*map_x = intersect_point.x;
//	*map_y = intersect_point.y;
//
//	return 1;
//}

int Coordinate::screen2map(int screen_x, int screen_y, int_32 *map_x, int_32 *map_y)
{
	DERay ray;
	DEPlane plane;
	double distance;
	Vector3 normal;
	Vertex3 intersect_point;

	normal.x = 0;
	normal.y = 0;
	normal.z = 1;
	plane.d = 0/*m_eye_to_map*/;//0?不确定.
	plane.normal = normal;

	this->updateViewFrustrum();

	Vector3 farLeftUp = this->m_frustrum.getFarLeftUp();
	Vector3 lefttoright = this->m_frustrum.getFarRightUp() - farLeftUp;
	Vector3 uptodown = this->m_frustrum.getFarLeftDown() - farLeftUp;

	//int_32 height, width ;
	//height = scene->getDeviceDriverNoRef()->getTargetNoRef()->getHeight() ;
	//width = scene->getDeviceDriverNoRef()->getTargetNoRef()->getWidth() ;
	float dx = screen_x / (float)c_width;
	float dy = screen_y / (float)c_height;

	ray.start = this->m_frustrum.cone_vertex;
	ray.direction = farLeftUp + (lefttoright * dx) + (uptodown * dy) - ray.start;
	ray.direction.normalize();

	if(intersects(ray, plane, &distance))
	{
		intersect_point = getIntersectPoint( ray, distance);
	}

	*map_x = intersect_point.x;
	*map_y = intersect_point.y;

	return 1;
}

DEViewFrustrum Coordinate::getLocalViewFrustrum()
{
	DEViewFrustrum frustrum;
	double half_height, half_width;
	float nearx, neary, farx, fary;
	double radian;
	
	radian = 3.141592654*(m_fovy*0.5)/180.0;
	//half_height = m_znear / tan(radian);
	half_height = m_znear * tan(radian);
	half_width  = half_height * m_aspect;

	////front, back, left, right, top, bottom.
	frustrum.planes[0].normal = Vector3::UNIT_Z;
	frustrum.planes[0].d	  = m_znear;

	frustrum.planes[1].normal = Vector3::NEGATIVE_UNIT_Z;
	frustrum.planes[1].d	  = -m_zfar;

	frustrum.planes[2].normal = Vector3(-1.0, 0.0, half_width/m_znear);
	frustrum.planes[2].d	  = 0.0;

	frustrum.planes[3].normal = Vector3(1.0, 0.0, half_width/m_znear);
	frustrum.planes[3].d      = 0.0;

	frustrum.planes[4].normal = Vector3(0.0, 1.0, half_height/m_znear);
	frustrum.planes[4].d      = 0.0;

	frustrum.planes[5].normal = Vector3(0.0, -1.0, half_height/m_znear);
	frustrum.planes[5].d      = 0.0;

	frustrum.height = m_zfar - m_znear;
	frustrum.cone_vertex = Vertex3(0.0f,0.0f,0.0f);

	nearx = (float)half_width;
	neary = (float)half_height;
	farx = m_zfar / m_znear * nearx;
	fary = m_zfar / m_znear * neary;

	//near
	frustrum.corners[0] = Vertex3(-nearx, -neary, -m_znear);
	frustrum.corners[1] = Vertex3( nearx, -neary, -m_znear);
	frustrum.corners[2] = Vertex3(-nearx,  neary, -m_znear);
	frustrum.corners[3] = Vertex3( nearx,  neary, -m_znear);
	// far
	frustrum.corners[4] = Vertex3(-farx, -fary, -m_zfar);
	frustrum.corners[5] = Vertex3( farx, -fary, -m_zfar);
	frustrum.corners[6] = Vertex3(-farx,  fary, -m_zfar);
	frustrum.corners[7] = Vertex3( farx,  fary, -m_zfar);

	return frustrum;
}

void Coordinate::updateViewFrustrum()
{
	Matrix4 eye2world;
	//vector < Vertex3 > frustrum_corners;

	//m_frustrum = new DEViewFrustrum();
	m_frustrum = this->getLocalViewFrustrum();
	eye2world = m_view_matrix.inverseMatrix();
	m_frustrum = eye2world.getDestFrustrum(m_frustrum);

	//for(int idx = 0; idx < 8; idx++)
	//	frustrum_corners.push_back(m_frustrum->corners[idx]);
	
	//this->box = getBoundBox(frustrum_corners);
}

//检测射线与平面相交
bool Coordinate::intersects(const DERay& ray, const DEPlane& plane, double *distance)
{
	double denom;
	double nom;
	Vector3 ray_start;

	//double len;
	//len = plane.normal.length();
	//const_cast < DEPlane&>(plane).normal.normalize();
	//const_cast < DEPlane&>(plane).d /= len;

	//单位化...
	const_cast < DERay &>(ray).direction.normalize();

	denom = plane.normal.dotProduct(ray.direction);
	ray_start = ray.start;

	if(fabs(denom) < 1e-06)		//平行
		return false;
	else
	{
		nom = plane.normal.dotProduct(ray_start) + plane.d;
		*distance = -(nom/denom);

		return (*distance >= 0);
	}
}

//return intersect point
Vertex3 Coordinate::getIntersectPoint( const DERay& ray, double distance)
{
	const_cast < DERay &>(ray).direction.normalize();

	return ray.start + ray.direction * distance;
}

int Coordinate::Ajust(double src)
{
	int ret;

	ret = floor(src);

	if(src - ret >= 0.5)
		ret++;

	return ret;
}

//5.13xiajie
void Coordinate::BuildSearchTable(int left_top_x, int left_top_y, 
		int right_top_x, int right_top_y, 
		int left_down_x, int left_down_y, 
		int right_down_x, int right_down_y,
		int window_width, int window_height)
{
	double original_height = left_down_y - left_top_y + 1;
	double original_width = right_top_x - left_top_x + 1;
	double original_down_width = right_down_x - left_down_x + 1;
	double original_diff_width = (original_width - original_down_width)/2.0;
	double diff_width = (original_width - window_width)/2.0;
	double diff_height = original_height - window_height;

	vector<VertexShort> row;
	VertexShort point;

	int i, j;
	double tmp;

	//建空表
	TransSearchTbl.clear();

	point.x = 0;
	point.y = 0;

	//for(j = 0; j < original_width; j++)
	//{
	//	row.push_back(point);
	//}

	//for(i = 0; i < original_height; i++)
	//{
	//	TransSearchTbl.push_back(row);
	//}

	TransSearchTbl.resize((int)original_height);
	for(i = 0; i < original_height; i++)
	{
		TransSearchTbl[i].resize((int)original_width);
	}

	//变换宽度
	double diff;
	double mid_x;
	for(i = 0; i < original_height; i++)
	{
		diff = diff_width - i/original_height*original_diff_width;

		mid_x = left_top_x + floor(original_width/2);

		for(j = 0; j < mid_x; j++)
		{
			tmp = (mid_x - j)/(mid_x - i/original_height*original_diff_width)*diff + j - diff_width;
			TransSearchTbl[i][j].x = Ajust(tmp);
		}

		for(j = mid_x; j < original_width; j++)
		{
			tmp = window_width - ((j - mid_x)/(mid_x - i/original_height*original_diff_width)*diff + (original_width - j) - diff_width);
			TransSearchTbl[i][j].x = Ajust(tmp);
		}
	}

	//变换高度
	double y;
	for(i = 0; i < original_height; i++)
	{
		if(diff_height >= 0)
		{
			y = i + (original_height - i)/original_height*diff_height - diff_height;
		}
		else
		{
			y = i - i/original_height*diff_height;
		}

		tmp = Ajust(y);

		for(j = 0; j < original_width; j++)
		{
			TransSearchTbl[i][j].y = tmp;
		}
	}
}

void Coordinate::BuildSearchTableNew(double start_x, double start_y,
		double x_pixel,double y_pixel,
		int left_top_x, int left_top_y, 
		int right_top_x, int right_top_y, 
		int left_down_x, int left_down_y, 
		int right_down_x, int right_down_y,
		int window_width, int window_height)
{
	int original_width = right_top_x - left_top_x + 1;
	int original_height = left_down_y - left_top_y + 1;
	int i, j;
	double s_x, s_y;
	double d_x, d_y;

	TransSearchTbl.clear();

	TransSearchTbl.resize(original_height);
	for(i = 0; i < original_height; i++)
	{
		TransSearchTbl[i].resize(original_width);
	}

	for(i = 0; i < original_height; i++)
	{
		for(j = 0; j < original_width; j++)
		{
			s_x = start_x + x_pixel*j;
			s_y = start_y - y_pixel*i;
			_changeCoord_(s_x, s_y, &d_x, &d_y);
			TransSearchTbl[i][j].x = Ajust(d_x);
			TransSearchTbl[i][j].y = Ajust(d_y);
		}
	}
}

int Coordinate::__changeCoord_(int_32 height, int_32 width, int_32 *to_x, int_32 *to_y) const
{
	if(height >=0 && height < TransSearchTbl.size()
		&& width >=0 && width < TransSearchTbl[height].size())
	{
		*to_x = TransSearchTbl[height][width].x;

		*to_y = TransSearchTbl[height][width].y/*+0*/;

		return 0;
	}
	else
	{
		int debug = 0;
	}

	return -1;
}