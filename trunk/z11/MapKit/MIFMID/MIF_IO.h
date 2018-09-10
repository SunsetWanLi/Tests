/* MIF_IO.h : do the MIF file IO operation
 * 2003-09-27
 */

#ifndef __MIF_IO_H__
#define __MIF_IO_H__

#include <fstream>

#include "MIF_StructDef.h"
#include "RecordSetImpl.h"

//#include "../../../Shared/Attribute/Attribute_If.h"

namespace GIS_Embedded
{
class MIF_IO
{
public:
	static int_32 loadMIFFile( std::ifstream & mif_file, MIF_Struct & mif_struct );
	static int_32 storeMIFFile( std::ofstream & mif_file, const MIF_Struct & mif_struct );

public:
	static std::string stringTrim( std::string & str_in );

public:
	static int_32 FDS_init( std::ifstream &mif_file, MIF_Header & mif_header, bool isExistID, long * idcount ) ;
	//搜索流中第pos个mif_obj 的对象（必须被中间数据结构认识的对象）的流位置,从1开始计
	//并且为了一些不能表达的obj，实时地找出第几个OBJ,也就是对应的属性数据的位置
//	static int_32 seekPos( std::ifstream & mif_file, const long pos, long * mid_pos ) ;
//	//从流的位置开始读count个对象于MIF_Obj_Vec中,
//	//并且为了一些不能表达的obj，实时地找出第几个OBJ,也就是对应的属性数据的位置
//	//static int_32 getMifObjs( std::ifstream & mif_file, const long count, MIF_Obj_Vec  & mif_data, vector< long > * mid_pos ) ;
//
private:
	static int_32 getMifObjCount( std::ifstream & mif_file, long *objcount ) ;
	static int_32 crossMifObj( std::ifstream & mif_file, string string_temp, MIF_Object ** obj ) ;
	
public:
	static int_32 loadMIFHead( std::ifstream & mif_file, MIF_Header & mif_header );
//return valid_count
	static int_32 loadMIFData( std::ifstream &mif_file, int max_n, MIF_Obj_Vec &mif_data );

	static int_32 storeMIFHead( std::ofstream & mif_file, const MIF_Header & mif_header );
	static int_32 storeMIFData( std::ofstream & mif_file, const MIF_Obj_Vec  & mif_data );

private:
	//static int_32 loadMIFHead( std::ifstream & mif_file, MIF_Header & mif_header );
	static int_32 loadMIFData( std::ifstream & mif_file, MIF_Obj_Vec  & mif_data );
	
	static void loadMIFPoint( std::ifstream & mif_file, MIF_Point * mif_point );
	static void loadMIFLine( std::ifstream & mif_file, MIF_Line * mif_line );
	static void loadMIFPline( std::ifstream & mif_file, MIF_Pline * mif_pline, string & str_temp );
	static void loadMIFMulPline( std::ifstream & mif_file, MIF_MulPline * mif_mulpline );
	static void loadMIFRegion( std::ifstream & mif_file, MIF_Region * mif_region );
	static void loadMIFText( std::ifstream & mif_file, MIF_Text * mif_text );
	static void loadMIFArc( std::ifstream & mif_file, MIF_Arc * mif_arc );

	static void storeMIFPoint( std::ofstream & mif_file, const MIF_Point * mif_point );
	static void storeMIFLine( std::ofstream & mif_file, const MIF_Line * mif_line );
	static void storeMIFPline( std::ofstream & mif_file, const MIF_Pline * mif_pline );
	static void storeMIFMulPline( std::ofstream & mif_file, const MIF_MulPline * mif_mulpline );
	static void storeMIFRegion( std::ofstream & mif_file, const MIF_Region * mif_region );
	static void storeMIFText( std::ofstream & mif_file, const MIF_Text * mif_text );
	static void storeMIFArc( std::ofstream & mif_file, const MIF_Arc * mif_arc );

private:
	static MIF_Head_Field_enum searchField( const std::string & str);
	static MIF_Object_enum searchType(const std::string & str);
	static int_32 loadEarthCor( std::string & earth_str, MIF_CS_Earth * earth );
	static int_32 loadNonearthCor( std::string & nonearth_str, MIF_CS_Nonearth * nonearth );
private:
	static void   readTextAdditionalSection(ifstream & mif_file);
};

class MID_IO
{
public:
	static int_32 loadMIDFile( std::ifstream & mid_file, const MIF_Header & mif_header, RecordSetImpl & recordsetimpl );
	//static int_32 storeMIDFile( std::ofstream & mid_file, const MIF_Header & mif_header, const RecordSetImpl & recordsetimpl );
public:	//return valid_coun
	static int_32 loadMIDHeadFile( std::ifstream & mid_file, const MIF_Header & mif_header, RecordSetImpl & recordsetimpl );
	static int_32 loadMIDRec( std::ifstream & mid_file, const MIF_Header & mif_header, int max_n, RecordSetImpl & recordsetimpl );
private:
	static int_32 loadMIDField( const MIF_Header & mif_header, RecordSetImpl & recordsetimpl );
	static int_32 loadMIDRec( std::ifstream & mid_file, const MIF_Header & mif_header, RecordSetImpl & recordsetimpl );

private:
	static ATB_Field_Type_enum searchMidType( const std::string & column_type, int_32 & str_width );
};


}  // namespace GIS_Embedded
#endif // __MIF_IO_H__
