#ifndef _RASTERIMAGE_IMAGE_PYRAMID_H_
#define _RASTERIMAGE_IMAGE_PYRAMID_H_

#include "PyramidScaler.h"
#include "Retiguide/Common/crs/basic_gm_type.h"
#include "Retiguide/Common/crs/CRT_Affine.h"
#include<string>

namespace z11
{
	class MGColor;
}
namespace GIS_Embedded
{
	class Pyramid;
	typedef struct tagResult Result;;
	struct ImagePyramidConfig;
//******************************zhaoling***************
typedef std::vector<std::string> BMPNames;
struct LeftTop
{
	long left;
	long top;
};
typedef std::vector<LeftTop> BMP_LeftTop;
struct CRS_Range
{
	double crs_left, crs_bottom, crs_right, crs_top;
};
//****************************************************
//

class ImagePyramid
{
public:
	ImagePyramid();
	~ImagePyramid();
	z11::int_32 init(const std::string& map_name, const std::string& map_root_path);
	//draw_rect是地理坐标或投影坐标（根据配置而定）, resolution_width与resolution_height是用户在屏幕上所要绘制的大小,   
	//image_vector是符合要求的各个图片, point_vector是各个图片的左上角相对于所需图片左上角的位置  
	z11::int_32 getImage(const z11::MGRect &draw_rect, z11::int_32 resolution_width, z11::int_32 resolution_height, \
		std::vector<z11::MGColor *> &image_vector, std::vector<z11::MGPoint> &point_vector);

private:
	void init();
	void initCrt();
	//得到转换比例:in在src内显示;现要得到放缩后的in'在屏幕坐标dest中显示的位置.假设src和dest中width/height比例近似.  
	double getTransRatio(const Rect2D& src, const Rect2D& in, const Rect2D& dest, 
							SPosition* left_top, z11::uint_32* width, z11::uint_32* height);
	//当dest框比src小:目标缩小,分辨率(放大率)<1
	double getResolution(const Rect2D& src, const Rect2D& dest);
	z11::int_32 getPixelRect( const Rect2D &rect, Rect2D * pixel_rect );
	Rect2D intersectRect( const Rect2D & left, const Rect2D &right );
	//int_32 getImage(const Rect2D &draw_rect, double resolution);
	z11::int_32 getImage(const Rect2D &draw_rect, double resolution, z11::uint_32* width, z11::uint_32 * height, bool key_press);
	//要画的区域 屏幕宽度 屏幕高度  返回金字塔左上角相对屏幕坐标 金字塔屏幕宽度 金字塔屏幕高度   
	Result* getImage(const Rect2D &draw_rect, const long resolution_width, const long resolution_height,
		SPosition* left_top, z11::uint_32* width, z11::uint_32* height, bool key_press);
//********************zhaling
	void storeBMPName(std::string BMPName);
	void storeBMPLeftTop(LeftTop lt);
	void setCrsRange(ImagePyramidConfig* image_pyramid_config);
//********************end
	//********************zhaling
	void getBMPName(std::string& bmpName);
	LeftTop getBMPLeftTop();
	bool BMPNameEmpty();
	bool BMPLeftTopEmpty();

	double getProToScr_rate(){return proTOscr_rate;}
	CRS_Range* getCrsRange(){return m_crs_range;}

	//zhaoling 2011-3-21
	bool neetNotFetchTiles() const {return m_need_not_fetch_tiles;}
	void getBlitRegion(long& blit_x_start, long& blit_x_end, long& blit_y_start, long& blit_y_end) const
	{
		blit_x_start = m_blit_region.x_start;
		blit_x_end = m_blit_region.x_end;
		blit_y_start = m_blit_region.y_start;
		blit_y_end = m_blit_region.y_end;
	}

	ImagePyramidConfig* getImagePyramidConfig() {return m_crs_config;}
	//********************end
	std::string getTileName(std::string image_name,int floor, int cur_upward_row, int cur_col);

private:
	Pyramid *m_pyramid;
	//Pyramid *m_pyramid;
	CRT_Affine *m_crt_affine;
	ImagePyramidConfig *m_crs_config;
	z11::uint_32 pyramid_width;
	z11::uint_32 pyramid_height;
	std::string m_map_name;

//********************zhaling
	BMPNames m_bmp_names;
	BMP_LeftTop m_bmp_lefttop;

	double proTOscr_rate;	
	CRS_Range* m_crs_range;

	GeoRegion_Scale m_last_tiles_region;//zhaoling 2011-3-21
	GeoRegion_Scale m_blit_region;
	bool m_need_not_fetch_tiles;//zhaoling 2011-3-21
//********************end

};
}
#endif