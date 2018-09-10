/* MIF_StructDef.h : MIF file struct define.
 * 2003-09-27
 */

#ifndef __MIF_STRUCTDEF_H__
#define __MIF_STRUCTDEF_H__

#include <map>
#include <set>
#include <vector>
#include "Retiguide/Common/Types/basic_types.h"
#include "Retiguide/Common/CRS/basic_gm_type.h"
#include "Retiguide/Common/CRS/CRS_TypeEnum.h"
#include "Retiguide/Common/CRS/DatumType.h"

//#include "../../GM_Memory30/GM_MEM_2D_if.h"
//#include "../../../Shared/Attribute/Attribute_If.h"
//#include "../../../Shared/CRS/DatumType.h"

namespace GIS_Embedded
{

//const uint_32 CRS_COORD_EARTHSYS = 2;
//const uint_32 CRS_COORD_NONEARTHSYS = 1;



enum MIF_Head_Field_enum
{ 
	MIF_VERSION, 
	MIF_CHARSET, 
	MIF_DELIMITER, 
	MIF_UNIQUE, 
	MIF_INDEX, 
	MIF_COORDSYS, 
	MIF_TRANSFORM, 
	MIF_COLUMNS, 
	NO_SUCH_HEAD_FIELD
};

enum MIF_Object_enum
{ 
	MIF_POINT, 
	MIF_LINE, 
	MIF_PLINE, 
	MIF_MULPLINE,
	MIF_REGION, 
	MIF_TEXT,
	MIF_ARC,
	NO_SUCH_OBJECT 
};

enum MIF_CoordSys_enum
{
	MIF_EARTH,
	MIF_NONEARTH,
	MIF_LAYOUT,
	MIF_TABLE,
	MIF_WINDOW,
	NONE_COORDSYS
};

enum ERR_NUM_enum
{
	ERR_MIFDATA_READ_ERR = 1,
	ERR_LAYOUT_UNITS_ERR,
	ERR_MIFHEAD_TANSFORM_ERR,
	ERR_MIFHEAD_COLUMNS_ERR,
	ERR_COLUMNS_TYPE_ERR,
};

typedef	std::vector<int> MIF_Unique;
typedef	std::vector<int> MIF_Index;

//2维空间的点序
typedef std::vector < Pos2D > PointArray2D;

//3维空间的点序
typedef std::vector < Pos3D > PointArray3D;

struct MultiDirectPosition2D
{
	int_32 pos_num;
	PointArray2D point_array;
};
//------------坐标系统的表达------------------
//地球坐标系
struct MIF_CS_Pro
{
	unsigned int type;
	unsigned int datum;
    string *unitname; 
	double *origin_longitude;
    double *origin_latitude;
    double *standard_parallel_1;
	double *standard_parallel_2;
	double *azimuth;
	double *scale_factor;
	string *false_easting;
	string *false_northing;
	double *range;

public:
	MIF_CS_Pro()
	{	
		type = 0;
		datum = 0;
		unitname = NULL;
		origin_longitude = NULL;
		origin_latitude = NULL;
		standard_parallel_1 = NULL;
		standard_parallel_2 = NULL;
		azimuth = NULL;
		scale_factor = NULL;
		false_easting = NULL;
		false_northing = NULL;
		range = NULL;
	}
	virtual ~MIF_CS_Pro()
	{
		delete unitname, unitname = NULL;
		delete origin_longitude, origin_longitude = NULL;
		delete origin_latitude, origin_latitude = NULL;
		delete standard_parallel_1, standard_parallel_2 = NULL;
		delete standard_parallel_2, standard_parallel_2 = NULL;
		delete azimuth, azimuth = NULL;
		delete scale_factor, scale_factor = NULL;
		delete false_easting, false_easting = NULL;
		delete false_northing, false_northing = NULL;
		delete range, range = NULL;
	}
};

struct MIF_CS_Affine
{
	string unitname;
	string A;
	string B;
	string C;
	string D;
	string E;
	string F;

public:
	MIF_CS_Affine(){}
	virtual ~MIF_CS_Affine(){}
};

struct MIF_CS_Bounds
{
	Pos2D p_min;
	Pos2D p_max;
	
public:
	MIF_CS_Bounds(){}
	virtual ~MIF_CS_Bounds(){}
};

struct MIF_CS_Earth
{
	MIF_CS_Pro *pro;
	MIF_CS_Affine *aff;
	MIF_CS_Bounds *bds;

public:
	MIF_CS_Earth()
	{
		pro = NULL;
		aff = NULL;
		bds = NULL;
	}
	virtual ~MIF_CS_Earth()
	{
		delete pro, pro = NULL;
		delete aff, aff = NULL;
		delete bds, bds = NULL;
	}
};
struct MIF_CS_Nonearth
{
	MIF_CS_Affine *aff;
	string unit_name;
	MIF_CS_Bounds bds;
	
public:
	MIF_CS_Nonearth()
	{
		aff = NULL;
	}
	virtual ~MIF_CS_Nonearth()
	{
		delete aff, aff = NULL;
	}
};

struct MIF_CS_Layout
{
	string papername;
	
public:
	MIF_CS_Layout(){}
	virtual ~MIF_CS_Layout(){}
};

struct MIF_CS_Table
{
	string tablename;
	
public:
	MIF_CS_Table(){}
	virtual ~MIF_CS_Table(){};
};
//
struct MIF_CS_Window
{
	int window_id;
	
public:
	MIF_CS_Window(){}
	virtual ~MIF_CS_Window(){}
};
union MIF_CoordSys
{
	MIF_CS_Earth *earth;
	MIF_CS_Nonearth *nonearth;
	MIF_CS_Layout *layout;
	MIF_CS_Table *table;
	MIF_CS_Window *window;

public:
	MIF_CoordSys()
	{
		earth = NULL;
		nonearth = NULL;
		layout = NULL;
		table = NULL;
		window = NULL;
	}
	~MIF_CoordSys(){}
};
////-----------------------------------------

struct MIF_TransForm
{
	int x_multiplier;
	int y_multiplier;
	int x_displacement;
	int y_displacement;
};
//--------------
struct MIF_ColumnUnit
{
	string name;
	string type;
};

////////构造MIFHeader///////////////
typedef vector< MIF_ColumnUnit > Column_Set;

struct MIF_Header
{
	int version;
	int column_num;
	string charset;
	string delimiter;
	MIF_Unique unique;
	MIF_Index index;
	MIF_CoordSys *coord_sys;
	MIF_TransForm *trans_form;
	Column_Set columns;
	MIF_CoordSys_enum coordsys_type;

	MIF_Header():version(0) , charset(""), delimiter("") ,column_num(0), coordsys_type( NONE_COORDSYS ), coord_sys(NULL), trans_form(NULL){};
	virtual ~MIF_Header()
	{
		if( coordsys_type == MIF_EARTH )
			delete coord_sys->earth, coord_sys->earth = NULL;
		
		if( coordsys_type == MIF_NONEARTH )
			delete coord_sys->nonearth, coord_sys->nonearth = NULL;
		
		if( coordsys_type == MIF_LAYOUT )
			delete coord_sys->layout, coord_sys->layout = NULL;
		
		if( coordsys_type == MIF_TABLE )
			delete coord_sys->table, coord_sys->table = NULL;
		
		if( coordsys_type == MIF_WINDOW )
			delete coord_sys->window, coord_sys->window = NULL;
		
		coord_sys = NULL;
		delete trans_form, trans_form = NULL;
	};
};

//中间数据结构
//====================================
//typedef vector < GM_Object * > WSF_GM_Container;


//MIFSymbol 确定一个点的外观
struct MIF_Symbol
{
	int shape;
	int color;
	int size;
};
//MIFPen 确定一个线性物体的宽度，样式和颜色
struct MIF_Pen
{
	int wdith;
	int pattern;
	int color;
};
//MIFBrush 确定一个块物体的样式，颜色
struct MIF_Brush
{
	int pattern;
	int fore_color;
	int back_color;
};
//MIFFont 确定一个文本的样式
struct MIF_Font
{
	string font_name;
	int style;
	int size;
	int fore_color;
	int *back_color;
};

struct MIF_Object
{
	MIF_Object_enum type;

public:
	MIF_Object(){};
	virtual ~MIF_Object(){};
};

//MIFPoint 点
struct MIF_Point : public MIF_Object
{
	PointArray2D point_array;
	MIF_Symbol *symbol;	

public:
	MIF_Point() : symbol(NULL) { type = MIF_POINT; };
	virtual ~MIF_Point()
	{
		delete symbol, symbol = NULL;
	};
};

//MIFLine 线段
struct MIF_Line : public MIF_Object
{
    PointArray2D point_array;
	MIF_Pen *pen;
	
public:
	MIF_Line(): pen(NULL) { type = MIF_LINE; };
	virtual ~MIF_Line()
	{
		delete pen, pen = NULL;
	};
};

//MIFPline 多线段
struct MIF_Pline : public MIF_Object
{
	MultiDirectPosition2D multi_pos_array;
	MIF_Pen *pen;
	bool *smooth;

public:
	MIF_Pline():pen(NULL),smooth(NULL) { type = MIF_PLINE; };
	virtual ~MIF_Pline()
	{
		delete pen, pen = NULL;
		delete smooth, smooth = NULL;
	};
};

//MIFMULPLINE
struct MIF_MulPline : public MIF_Object
{
	set< MIF_Pline * > pline_set;
	int pline_num;

	MIF_Pen *pen;
	bool *smooth;

public:
	MIF_MulPline():pen(NULL),smooth(NULL) { type = MIF_MULPLINE; };
	virtual ~MIF_MulPline() 
	{
		delete pen, pen = NULL;
		delete smooth, smooth = NULL;

		MIF_Pline *mif_pline;
		set< MIF_Pline *>::iterator iter;

		for( iter = pline_set.begin(); iter != pline_set.end(); iter++ )
		{
			mif_pline = *iter;
			delete mif_pline;
		}
		pline_set.clear();
	};
};

//MIFRegion 区域
struct MIF_Region : public MIF_Object
{
	int poly_num;
	vector< MultiDirectPosition2D > multi_pos_array;
	MIF_Pen *pen;
	MIF_Brush *brush;
	Pos2D *center;

public:
	MIF_Region() :poly_num(0),pen(NULL), brush(NULL), center(NULL) { type = MIF_REGION; };
	virtual ~MIF_Region()
	{
		delete pen, pen = NULL;
		delete brush, brush = NULL;
		delete center, center = NULL;
	};
};

struct MIF_Text : public MIF_Object
{
	Pos2D pos0, pos1;
	string text;
public:
	MIF_Text(){ type = MIF_TEXT; };
	virtual ~MIF_Text(){;};
};

struct MIF_Arc : public MIF_Object
{
	Pos2D pos1;
	Pos2D pos2;
	double a, b;
public:
	MIF_Arc(){ type = MIF_ARC; };
	virtual ~MIF_Arc(){};
};

//-------------中间数据结构----------------------------
typedef vector < MIF_Object * > MIF_Obj_Vec;

struct MIF_Struct
{
	MIF_Header mif_header;
	MIF_Obj_Vec mif_data;
};


//-------------CRS-----------------------

static uint_32 MIF_ellipse[][50] = {
	{ ETC_CLARK_1880, 1, 5, 6, 19, 21, 49, 52, 55, 57, 58, 59, 60, 78, 99, 0  },
	{ ETC_KRASSOVSKY_1940, 2, 1001, 0 },
	{ ETC_CLARK_1866, 15, 19, 34, 48, 50, 51, 62, 63, 64, 65, 66, 67, 68, 69,
	70, 71, 72, 77, 83, 0 },
	{ ETC_BESSEL_1841, 25, 54, 97, 0 },
	{ ETC_EVEREST_1830, 40, 41, 45, 96, 0 },
	{ ETC_WGS_1984, 104, 0 },
	{ ETC_IAG_1975, 1004, 0}, // by luying
	{ 0 }
};

static int MIF_crstype[][2] ={
	{(int)GCS_UNKNOWN, 0},
	{(int)GCS_WGS_1984, 104},
	{(int)GCS_BEIJING_1954, 1001}, 
	{(int)GCS_XIAN_1980, 1004}, //没有定义 by luying
	{(int)GCS_DEFAULT, 0}, //默认的投影类型	
	{ -1 , 0}
};
static uint_32 MIF_unitname[][2] = {
	{ UTC_METER, 7 },
	{ UTC_CENTIMETER, 6 },
	{ UTC_CHAIN, 31 },
	{ UTC_KILOMETER, 1 },
	{ UTC_MILE, 0 },
	{ UTC_MILlIMETER, 5 },
	{ UTC_FEET, 3 },
	{ UTC_INCHE, 2 },
	{ UTC_LINK, 30 },
	{ UTC_NAUTICALMILE, 9 },
	{ UTC_ROD, 32 },
	{ UTC_USSURVEYFEET, 8 },
	{ UTC_YARD, 4 },
	{ 0, 0 }
};

static uint_32 MIF_protype[][2] = {
	{ PRO_LONGITUDE_LATITUDE, 1 },
	{ PRJ_GAUSS, 8 },
	{ PRJ_GAUSS_INVERSE , 1},//有待进一步确定
	{ PRJ_GAUSS_ZONED , 8},
	{ PRJ_MERCATOR, 26}, 
	{ PRJ_MERCATOR_TRANVERSE ,8},
	{ PRJ_LAMBERT , 4},
	{ PRJ_LAMBERT_CONFORMAL , 19},
	{ 1 << 30, 0 }
};

} // namespace GIS_Embedded
#endif  // __MIF_STRUCTDEF_H__
