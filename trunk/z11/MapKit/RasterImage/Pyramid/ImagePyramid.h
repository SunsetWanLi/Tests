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
	//draw_rect是地理坐标或投影坐标（根据配置而定）, resolution_width与resolution_height是用户在屏幕上所要绘制的大小,   
	//image_vector是符合要求的各个图片, point_vector是各个图片的左上角相对于所需图片左上角的位置, 返回值为第几层   
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
	//得到转换比例:in在src内显示;现要得到放缩后的in'在屏幕坐标dest中显示的位置.假设src和dest中width/height比例近似.
	double getTransRatio(const Rect2D& src, const Rect2D& in, const Rect2D& dest, 
							SPosition* left_top, unsigned int* width, unsigned int* height);
	//当dest框比src小:目标缩小,分辨率(放大率)<1
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
	//要画的区域 屏幕宽度 屏幕高度  返回金字塔左上角相对屏幕坐标 金字塔屏幕宽度 金字塔屏幕高度
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