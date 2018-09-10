#include "ImagePyramid.h"
#include "CommonBase/MGWString.h"

using namespace z11;
using namespace std;

namespace GIS_Embedded
{
	class RasterData /*: public Basic_render*/
	{
	private:
		ImagePyramid* m_ip; 

		long m_resolution_width;
		long m_resolution_height;

		int m_current_floor_num;
		int m_total_floor;
		Rect2D m_rect;//屏幕对应的投影坐�?
		Pos2D m_center_prj;//屏幕中心对应的投影坐�?

 		int m_delta_x, m_delta_y;
 
 		double m_host_prj_x, m_host_prj_y;
 		string m_host_bmp_file_name;
//  		MyBITMAP m_host_bmp;

		double m_dest_prj_x,m_dest_prj_y;//lw
		string m_dest_bmp_file_name;
	//	MyBITMAP m_dest_bmp;


		ImagePyramidConfig* m_crs_config;

 		bool m_zoom;

// 		GXDC m_gx_bmp_DC;
// 		GXDC m_gx_mem_DC;

		CRT_Affine* m_crt_affine;

	public:
		RasterData(int current_floor_num);
		~RasterData();

 		void zoomIn();//缩小 
 		void zoomOut();//放大
 		void move(int delta_x, int delta_y);
		void resizer(int cur_floor_num);//快速放大缩小地�? lw
		void setRect(long left,long top,long right,long bottom);


		int render();

		int scrPosToPrjPos(int scr_x, int scr_y, double& prj_x, double& prj_y) const;
		int prjPosToScrPos(double prj_x, double prj_y, int& scr_x, int& scr_y) const;

		int geoPosToPrjPos(double longitude, double latitude, long& prj_x, long& prj_y) const;
		int prjPosToGeoPos(long prj_x, long prj_y, double& longitude, double& latitude) const;

		void setHostPosition(int host_x, int host_y);
		void renderHostPosition(int host_x, int host_y);
// 		void changeHostPosition(int new_host_scr_x, int new_host_scr_y);

		void setDestPosition(int dest_x,int dest_y);//lw 
		void getDestPosition(double *dest_x, double *dest_y)
		{
			*dest_x = m_dest_prj_x;
			*dest_y = m_dest_prj_y;
		}

		void renderDestPostion(int dest_x, int dest_y);

		void changeHostPosition(long new_host_prj_x, long new_host_prj_y);
		void locater(long center_x, long center_y);

		void getCurrentPrjScope(long* prj_x_start, long* prj_y_start, long* prj_x_end, long* prj_y_end) const;

		void rendBMPFromPrjPosition(string bmp_name, long prj_x, long prj_y) const;
		void rendBMPFromScrPosition(string bmp_name, int scr_x, int scr_y) const;

		int getCurrentFloorNumber();
		int getTotalFloorNumber();//lw

		void setRasterMapRootPath(const string& file_root);
		void setRasterMapName(const string& map_name);
		int rasterDataInit(const string& raster_map_name, const string& raster_map_root_path);

	private:
		void setRect(int floor_num);
		void adjustRect(double prj_move_x, double prj_move_y);

		bool isLegalPrjPostion(const long& prj_x, const long& prj_y) const;
		bool isLegalScrPostion(const int& scr_x, const int& scr_y) const;
	};

}
