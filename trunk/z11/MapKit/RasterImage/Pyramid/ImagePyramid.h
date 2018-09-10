#ifndef IS_GIS_DATASRC_FEATURE_IMAGE_PYRAMID_H_
#define IS_GIS_DATASRC_FEATURE_IMAGE_PYRAMID_H_
#include<string>
#include "../Common/basic_gm_type.h"
//#include "../../Common/basic_gm_type.h"

namespace Raster_GIS
{
	class CRT_Affine;
	struct ImagePyramidConfig;
	namespace Pyramid_Space
	{
		class Pyramid;
	}
typedef std::vector<std::string> BMPNames;
struct LeftTop
{
	long left;
	long top;
};
typedef std::vector<LeftTop> BMP_LeftTop;

class ImagePyramid
{
public:	
	int init(const std::string& map_name, const std::string& map_root_path);
	//draw_rect�ǵ��������ͶӰ���꣨�������ö�����, resolution_width��resolution_height���û�����Ļ����Ҫ���ƵĴ�С,   
	//image_vector�Ƿ���Ҫ��ĸ���ͼƬ, point_vector�Ǹ���ͼƬ�����Ͻ����������ͼƬ���Ͻǵ�λ��, ����ֵΪ�ڼ���   
	int getImage(const Rect2D &draw_rect, const long resolution_width, const long resolution_height, \
		std::vector<std::string> &image_path_vector, std::vector<LeftTop> &point_vector, long &get_col);
	Rect2D getPyramidRegion();
	double getWidthScalePixelGeo(unsigned int floor_num);
	double getHeightScalePixelGeo(unsigned int floor_num);
	int getTotalFloorNum();
	int getLevelFactor();
	int getRegularTileSize();
	int tryToGetImage(const Rect2D &draw_rect, const long resolution_width, const long resolution_height);

	ImagePyramidConfig* getImagePyramidConfig() {return m_crs_config;}
private:
	void init();
	void initCrt();
	//�õ�ת������:in��src����ʾ;��Ҫ�õ��������in'����Ļ����dest����ʾ��λ��.����src��dest��width/height��������.
	double getTransRatio(const Rect2D& src, const Rect2D& in, const Rect2D& dest, 
							SPosition* left_top, unsigned int* width, unsigned int* height);
	//��dest���srcС:Ŀ����С,�ֱ���(�Ŵ���)<1
	double getResolution(const Rect2D& src, const Rect2D& dest);
	int_32 getPixelRect( const Rect2D &rect, Rect2D * pixel_rect );
	Rect2D intersectRect( const Rect2D & left, const Rect2D &right );
	int getImage(const Rect2D &draw_rect, double resolution, long &get_col);
	void storeBMPName(std::string BMPName);
	void storeBMPLeftTop(LeftTop lt);
	std::string getTileName(std::string image_name,int floor, int cur_upward_row, int cur_col);
	std::string getBMPName();
	LeftTop* getBMPLeftTop();
	bool BMPNameEmpty();
	bool BMPLeftTopEmpty();
	static bool isEqualZero(double value);
	//Ҫ�������� ��Ļ��� ��Ļ�߶�  ���ؽ��������Ͻ������Ļ���� ��������Ļ��� ��������Ļ�߶�
	int getImage(const Rect2D &draw_rect, const long resolution_width, const long resolution_height, 
		SPosition* left_top, unsigned int* width, unsigned int* height, long &get_col);
	
	
private:
	Pyramid_Space::Pyramid* m_pyramid;
	CRT_Affine* m_crt_affine;
	ImagePyramidConfig* m_crs_config;
	unsigned int pyramid_width;
	unsigned int pyramid_height;
	std::string m_map_name;
	std::string m_map_root_path;
	BMPNames m_bmp_names;
	BMP_LeftTop m_bmp_lefttop;

};
}
#endif