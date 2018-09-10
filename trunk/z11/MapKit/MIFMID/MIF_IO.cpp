/* MIF_OI.cpp : do the MIF file IO operation
 * 2003-09-26
 */

//#include "pch.h" //qzy
//#include "stdafx.h"

//#include "FDS_config.h"
#define CONFIG_HAVE_MIF_FILE_SUPPORT
#ifdef CONFIG_HAVE_MIF_FILE_SUPPORT
///////////////////////////////////////////////////////////////

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
//#include <wchar.h>
//#include <mapidefs.h>
//#include <comdef.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "MIF_IO.h"
#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <windows.h>
#endif

using namespace std;

namespace GIS_Embedded
{
#if !(defined (_WIN32)) && !(defined (_WIN64))
	int _stricmp(const char * _Str1, const char * _Str2)
	{
		assert(_Str1);
		assert(_Str2);

		int ret = 0;
		int length = 0;
		char tmp = '\0';
		for (length=0, tmp=*_Str1; tmp!='\0';)
		{
			length++;
			tmp = *(_Str1+length);
		}
		char *tmp_str1 = new char[length+1];
		strcpy(tmp_str1, _Str1);
		for(int i=0; i<length; i++)
		{
			tmp_str1[i] = toupper(tmp_str1[i]);
		}

		length = 0;
		tmp = '\0';
		for (length=0, tmp=*_Str2; tmp!='\0';)
		{
			length++;
			tmp = *(_Str2+length);
		}
		char *tmp_str2 = new char[length+1];
		strcpy(tmp_str2, _Str2);
		for(int i=0; i<length; i++)
		{
			tmp_str2[i] = toupper(tmp_str2[i]);
		}

		ret = strcmp(tmp_str1, tmp_str2);
		delete [] tmp_str1;
		delete [] tmp_str2;
		return ret;
	}
#endif
    
//----------------从文件中读取数据到中间结构----------------
int_32 MIF_IO::loadMIFFile( ifstream & mif_file, MIF_Struct & mif_struct )
{
	int_32 m_errinfo;

	m_errinfo = loadMIFHead( mif_file, mif_struct.mif_header );
	if( m_errinfo ) return m_errinfo;

	m_errinfo = loadMIFData( mif_file, mif_struct.mif_data );
    if( m_errinfo ) return m_errinfo;

	return 0;
}

//------------把中间数据结构的数据写入文件中-----------------
int_32 MIF_IO::storeMIFFile( ofstream & mif_file, const MIF_Struct & mif_struct )
{
	int_32 m_errinfo;

	m_errinfo = storeMIFHead( mif_file, mif_struct.mif_header );
	if( m_errinfo ) return m_errinfo;

	m_errinfo = storeMIFData( mif_file, mif_struct.mif_data );
    if( m_errinfo ) return m_errinfo;

	return 0;
}
//
int_32 MIF_IO::FDS_init( ifstream &mif_file, MIF_Header & mif_header, bool isExistID, long * idcount )
{
	int_32 m_errinfo;

	m_errinfo = loadMIFHead( mif_file, mif_header );
	if( m_errinfo<0 ) return m_errinfo;

	if ( !isExistID )
	{
		m_errinfo = getMifObjCount( mif_file, idcount ) ;
		if( m_errinfo< 0 ) return m_errinfo;
	}
	return 0 ;
}

int_32 MIF_IO::getMifObjCount( std::ifstream & mif_file, long *objcount )
{
	string string_temp;
	int_32 errcode ;
	long count = 0 ;
	MIF_Object * temp_obj = NULL ;
	mif_file >> string_temp;
	while ( !mif_file.eof() )
	{
		errcode = crossMifObj( mif_file, string_temp, &temp_obj ) ;
		if ( errcode < 0 )
			continue ;
		delete temp_obj ;
		count++ ;
		mif_file >> string_temp;
	}
	*objcount = count ;
	return 0 ;
}

int_32 MIF_IO::crossMifObj( std::ifstream & mif_file, string string_temp, MIF_Object ** obj )
{
	MIF_Object_enum type;
	string str_temp;

	type = searchType( string_temp.c_str() ); 
	switch ( type ){
		case MIF_POINT:
			{
				MIF_Point * mif_point = new MIF_Point;
				loadMIFPoint( mif_file, mif_point );
				*obj = mif_point ;
			}
			break;
		case MIF_LINE:
			{
				MIF_Line * mif_line = new MIF_Line;
				loadMIFLine( mif_file, mif_line );
				*obj = mif_line ;
			}
			break;
		case MIF_PLINE:
			{
				mif_file >> str_temp;
				if( !_stricmp( str_temp.c_str(), "Multiple" ) )
				{
					MIF_MulPline * mif_mulpline = new MIF_MulPline;
					loadMIFMulPline( mif_file, mif_mulpline );
					*obj = mif_mulpline ;
				}
				else
				{
					MIF_Pline * mif_pline = new MIF_Pline;
					loadMIFPline( mif_file, mif_pline, str_temp );
					*obj = mif_pline ;
				}
			}
			break;
		case MIF_REGION:
			{
				MIF_Region * mif_region = new MIF_Region;
				loadMIFRegion( mif_file, mif_region );
				*obj = mif_region ;
			}
			break;
		case MIF_TEXT:
			{
				MIF_Text * mif_text = new MIF_Text;
				loadMIFText( mif_file, mif_text );
				*obj = mif_text ;
			}
			break;
		case NO_SUCH_OBJECT:
			return ERR_MIFDATA_READ_ERR;
		case MIF_ARC:
		case MIF_MULPLINE:
		default:
//#pragma message(__FILE__ " warning: MIF_IO::crossMifObj( std::ifstream & mif_file, string string_temp, MIF_Object ** obj ) 未处理的MIF类型: MIF_ARC, MIF_MULPLINE")
			assert(0);
			break;
	}
	return 0 ;
}
//--------------从文件中读取文件头到数据结构中---------------- 
int_32 MIF_IO::loadMIFHead ( ifstream & mif_file, MIF_Header & mif_header )
{
	string textline;
	string str1;
	string str2;
	string newstr;
	string struni;
	int i_unit;
	uint_32 firstch;
	int num;
	MIF_ColumnUnit col_unit;

	getline( mif_file, textline );
	textline = stringTrim( textline );

	while ( _stricmp ( textline.c_str() , "Data" ) && !mif_file.eof() ) 
	{
		while ( textline.empty() ) 
		{	
			getline( mif_file, textline );
			textline = stringTrim( textline );
		}

		uint_32 number = textline.find_first_of( " ", 0 ); 
		if( number == -1 )
		{
			str1 = textline;
			str2 == "";
		}
		else
		{
			str1 = textline.substr( 0, number );
			str2 = textline.substr( number+1);
		}

		MIF_Head_Field_enum field = searchField( str1 );
		switch ( field ){
		case MIF_VERSION:
			{ 
				mif_header.version = atoi ( str2.c_str() );			
			}
			break;
		case MIF_CHARSET:
			{
				mif_header.charset = stringTrim( str2 );
			}
			break;
		case MIF_DELIMITER:
			{
				mif_header.delimiter = stringTrim( str2 );
			}
			break;
		case MIF_UNIQUE:
			{
				newstr = str2;
				firstch = newstr.find_first_of( "," , 0 );

				while ( firstch != -1 )
				{
					struni = newstr.substr( 0, firstch );
					newstr = newstr.substr( firstch+1 );
					i_unit = atoi( struni.c_str() );

					mif_header.unique.push_back( i_unit );
					firstch = newstr.find_first_of( ",", 0);
				}

				i_unit = atoi( newstr.c_str() );
				mif_header.unique.push_back( i_unit );				
			}
			break;
		case MIF_INDEX:
			{
				newstr = str2;
				firstch = newstr.find_first_of(",",0);

				while ( firstch != -1 ) 
				{
					struni = newstr.substr( 0, firstch );
					newstr = newstr.substr( firstch + 1 );
					i_unit = atoi( struni.c_str() );

					mif_header.index.push_back( i_unit );
					firstch = newstr.find_first_of( "," ,0);
				}
				i_unit=atoi(newstr.c_str());
				mif_header.index.push_back(i_unit);	
			}
			break;
		case MIF_COORDSYS:
			{
				string coordsys;
				newstr = str2;
				int_32 m_errinfo;
				mif_header.coord_sys = new MIF_CoordSys;

				firstch = newstr.find_first_of( " ", 0 );
				coordsys = newstr.substr( 0, firstch );
				newstr = newstr.substr( firstch + 1 );
				newstr = stringTrim( newstr );

				coordsys = stringTrim( coordsys );
				if ( !_stricmp( coordsys.c_str(),"earth") )
				{
					mif_header.coord_sys->earth = new MIF_CS_Earth;
					mif_header.coordsys_type = MIF_EARTH;

					m_errinfo = loadEarthCor( newstr, mif_header.coord_sys->earth );
					if( m_errinfo ) return m_errinfo;
				}

				if( !_stricmp( coordsys.c_str(),"nonearth") )
				{
					mif_header.coord_sys->nonearth = new MIF_CS_Nonearth;
					mif_header.coordsys_type = MIF_NONEARTH;

					m_errinfo = loadNonearthCor( newstr, mif_header.coord_sys->nonearth );
					if( m_errinfo ) return m_errinfo;
				}

				if( !_stricmp( coordsys.c_str(), "layout") )
				{
					mif_header.coord_sys->layout = new MIF_CS_Layout;
					mif_header.coordsys_type = MIF_LAYOUT;

					uint_32 m_temp = newstr.find_first_of( " ", 0 );

					string f_temp = newstr.substr( 0, m_temp );
					string l_temp =  newstr.substr( m_temp + 1 ) ;
					l_temp = newstr.substr( m_temp + 1 );
					l_temp = stringTrim( l_temp );

					if( _stricmp( f_temp.c_str(), "units" ) ) return ERR_LAYOUT_UNITS_ERR;

					if( l_temp.find_first_of('"',0) == 0 )
						l_temp = l_temp.substr(1, l_temp.size()-2 );
					mif_header.coord_sys->layout->papername = l_temp;
				}

				if( !_stricmp( coordsys.c_str(), "table") )
				{
					mif_header.coord_sys->table = new MIF_CS_Table;
					mif_header.coordsys_type = MIF_TABLE;

					mif_header.coord_sys->table->tablename = newstr;
				}

				if( !_stricmp( coordsys.c_str(), "window") )
				{
					mif_header.coord_sys->window = new MIF_CS_Window;
					mif_header.coordsys_type = MIF_WINDOW;

					mif_header.coord_sys->window->window_id = atoi( newstr.c_str() );
				}
			}
			break;
		case MIF_TRANSFORM:
			{
				mif_header.trans_form = new MIF_TransForm;
				newstr = str2;
				uint_32 deli_pos = newstr.find_first_of( ",", 0);

				if( deli_pos == -1 ) 
					return ERR_MIFHEAD_TANSFORM_ERR;
				mif_header.trans_form->x_multiplier = atoi( newstr.substr( 0, deli_pos ).c_str() );

				newstr = newstr.substr( deli_pos+1 );
				newstr = stringTrim(newstr);
				deli_pos = newstr.find_first_of( ",", 0);
				if( deli_pos == -1 ) 
					return ERR_MIFHEAD_TANSFORM_ERR;
				mif_header.trans_form->y_multiplier = atoi(newstr.substr( 0, deli_pos ).c_str() );

				newstr = newstr.substr( deli_pos+1 );
				deli_pos = newstr.find_first_of( ",", 0);
				if( deli_pos == -1 ) 
					return ERR_MIFHEAD_TANSFORM_ERR;
				mif_header.trans_form->x_displacement = atoi( newstr.substr( 0, deli_pos ).c_str() );

				newstr = newstr.substr( deli_pos+1 );
				mif_header.trans_form->y_displacement = atoi( newstr.c_str() );
			}
			break;
		case MIF_COLUMNS:
			{        
				num = atoi( str2.c_str() );
				mif_header.column_num = num;

				int i;
				for( i = 0; i < num; i++ )
				{
					getline( mif_file, textline );
					textline = stringTrim( textline );

					if( !_stricmp ( textline.c_str() , "Data" )) 
						return ERR_MIFHEAD_COLUMNS_ERR;

					uint_32 col_pos = textline.find_first_of( " ", 0 );
					col_unit.name = textline.substr( 0, col_pos );
					col_unit.type = textline.substr( col_pos + 1 );
					col_unit.type = stringTrim( col_unit.type );

					mif_header.columns.push_back( col_unit );
				}
			}
			break;
		case NO_SUCH_HEAD_FIELD:
			break;
		}	
		getline( mif_file, textline );
		textline = stringTrim( textline );
	}
	return 0;
}

//--------------把中间数据结构的文件头写入文件-------------------
int_32 MIF_IO::storeMIFHead( ofstream & mif_file, const MIF_Header & mif_header )
{
	uint_32 loop;

	mif_file << "Version " << mif_header.version <<endl;
	
	if( !mif_header.charset.empty() )
		mif_file << "Charset "<<'"'<< mif_header.charset <<'"'<<endl;
	
	if( !mif_header.delimiter.empty() ) 
		mif_file << "Delimiter "<<'"'<< mif_header.delimiter <<'"'<<endl;
	
	if( !mif_header.unique.empty() )
	{
		mif_file <<"UNIQUE ";
		for( loop = 0; loop < ( mif_header.unique.size()-1 ); loop++ )
			mif_file << mif_header.unique[loop] << ",";
		mif_file << mif_header.unique[ mif_header.unique.size()-1] << endl;
	}
	
	if( !mif_header.index.empty() )
	{
		mif_file <<"INDEX ";
		for( loop=0; loop < ( mif_header.index.size()-1); loop++ )
			mif_file <<mif_header.index[loop] << ",";
		mif_file << mif_header.index[ mif_header.index.size()-1]<<endl;
	}
	
	if( mif_header.coord_sys != NULL )
	{
		mif_file << "CoordSys ";
		if( mif_header.coordsys_type == MIF_EARTH )
		{
			mif_file << "Earth ";
			if( mif_header.coord_sys->earth->pro != NULL )
			{
				mif_file << "Projection " << mif_header.coord_sys->earth->pro->type
					<< "," << mif_header.coord_sys->earth->pro->datum;
				
				if( mif_header.coord_sys->earth->pro->unitname != NULL )
					mif_file << "," << *mif_header.coord_sys->earth->pro->unitname;
				
				if( mif_header.coord_sys->earth->pro->origin_longitude != NULL )
					mif_file << "," << *mif_header.coord_sys->earth->pro->origin_longitude;
				
				if( mif_header.coord_sys->earth->pro->origin_latitude != NULL )
					mif_file << "," << *mif_header.coord_sys->earth->pro->origin_latitude;
				
				if( mif_header.coord_sys->earth->pro->standard_parallel_1 != NULL )
					mif_file << "," << *mif_header.coord_sys->earth->pro->standard_parallel_1;
				
				if( mif_header.coord_sys->earth->pro->standard_parallel_2 != NULL )
					mif_file << "," << *mif_header.coord_sys->earth->pro->standard_parallel_2;
				
				if( mif_header.coord_sys->earth->pro->azimuth != NULL )
					mif_file << "," << *mif_header.coord_sys->earth->pro->azimuth;

				if( mif_header.coord_sys->earth->pro->scale_factor != NULL )
					mif_file << "," << *mif_header.coord_sys->earth->pro->scale_factor;

				if( mif_header.coord_sys->earth->pro->false_easting != NULL )
					mif_file << "," << *mif_header.coord_sys->earth->pro->false_easting;

				if( mif_header.coord_sys->earth->pro->false_northing != NULL )
					mif_file << "," << *mif_header.coord_sys->earth->pro->false_northing;

				if( mif_header.coord_sys->earth->pro->range != NULL )
					mif_file << "," << *mif_header.coord_sys->earth->pro->range;

			}

			if( mif_header.coord_sys->earth->aff != NULL )
			{
				mif_file << " " << "Affine Units ";
				mif_file << mif_header.coord_sys->earth->aff->unitname;
				mif_file << "," << mif_header.coord_sys->earth->aff->A;
				mif_file << "," << mif_header.coord_sys->earth->aff->B;
				mif_file << "," << mif_header.coord_sys->earth->aff->C;
				mif_file << "," << mif_header.coord_sys->earth->aff->D;
				mif_file << "," << mif_header.coord_sys->earth->aff->E;
				mif_file << "," << mif_header.coord_sys->earth->aff->F;
			}

			if( mif_header.coord_sys->earth->bds != NULL )
			{
				mif_file << " " << "Bounds (";
				mif_file << mif_header.coord_sys->earth->bds->p_min.d0;
				mif_file << "," << mif_header.coord_sys->earth->bds->p_min.d1;
				mif_file << ")(" << mif_header.coord_sys->earth->bds->p_max.d0;
				mif_file << "," << mif_header.coord_sys->earth->bds->p_max.d1;
				mif_file << ")";
			}
			mif_file << endl;
		}
		
		if( mif_header.coordsys_type == MIF_NONEARTH )
		{
			mif_file << "NonEarth ";
			
			if( mif_header.coord_sys->nonearth->aff != NULL )
			{
				mif_file << " " << "Affine Units ";
				mif_file << mif_header.coord_sys->nonearth->aff->unitname;
				mif_file << "," << mif_header.coord_sys->nonearth->aff->A;
				mif_file << "," << mif_header.coord_sys->nonearth->aff->B;
				mif_file << "," << mif_header.coord_sys->nonearth->aff->C;
				mif_file << "," << mif_header.coord_sys->nonearth->aff->D;
				mif_file << "," << mif_header.coord_sys->nonearth->aff->E;
				mif_file << "," << mif_header.coord_sys->nonearth->aff->F;
			}

			mif_file << " " << "Units " <<'"'<<mif_header.coord_sys->nonearth->unit_name<<'"';

			mif_file << " " << "Bounds (";
			mif_file << mif_header.coord_sys->nonearth->bds.p_min.d0;
			mif_file << "," << mif_header.coord_sys->nonearth->bds.p_min.d1;
			mif_file << ")(" << mif_header.coord_sys->nonearth->bds.p_max.d0;
			mif_file << "," << mif_header.coord_sys->nonearth->bds.p_max.d1;
			mif_file << ")" << endl;
		}

		if( mif_header.coordsys_type == MIF_LAYOUT )
		{
			mif_file << "Layout Units " << mif_header.coord_sys->layout->papername << endl;
		}

		if( mif_header.coordsys_type == MIF_TABLE )
		{
			mif_file << "Table " << mif_header.coord_sys->table->tablename << endl;
		}

		if( mif_header.coordsys_type == MIF_WINDOW )
		{
			mif_file << "Window " << mif_header.coord_sys->window->window_id << endl;
		}

	}
	
	if( mif_header.trans_form )
		mif_file << "Transform " << mif_header.trans_form->x_multiplier<<","<< mif_header.trans_form->y_multiplier<< ","
			 <<mif_header.trans_form->x_displacement << "," <<mif_header.trans_form->y_displacement<<endl;
	
	mif_file << "Columns " << ( mif_header.column_num - 1) <<endl;//modified by zxj 2004-12-09, column_num -1 to column_num
	for( int i = 0; i < mif_header.column_num; i++ )
	{
		if( !_stricmp( mif_header.columns[i].name.c_str(), "gn_goid" ) )
			continue;
		mif_file << '\t'<< mif_header.columns[i].name << " " << mif_header.columns[i].type << endl;
	}
	mif_file<< "Data" << endl;
	mif_file<< endl;
	
	return 0;
}

//----------------读取文件中的数据到中间数据结构------------------------
int_32 MIF_IO::loadMIFData( ifstream & mif_file, MIF_Obj_Vec & mif_data )
{
	MIF_Object_enum type;
	string string_temp;
	string str_temp;

	mif_file >> string_temp;
	while ( !mif_file.eof() )
	{		
		type = searchType( string_temp.c_str() ); 
		switch ( type ){
			case MIF_POINT:
				{
					MIF_Point * mif_point = new MIF_Point;
					
					loadMIFPoint( mif_file, mif_point );
					mif_data.push_back( mif_point );
				}				
				break;
			case MIF_LINE:
				{
					MIF_Line * mif_line = new MIF_Line;

					loadMIFLine( mif_file, mif_line );
					mif_data.push_back( mif_line );
				}
				break;
			case MIF_PLINE:
				{
					mif_file >> str_temp;
					if( !_stricmp( str_temp.c_str(), "Multiple" ) )
					{
						MIF_MulPline * mif_mulpline = new MIF_MulPline;

						loadMIFMulPline( mif_file, mif_mulpline );
						mif_data.push_back( mif_mulpline );
					}
					else
					{
						MIF_Pline * mif_pline = new MIF_Pline;

						loadMIFPline( mif_file, mif_pline, str_temp );
						mif_data.push_back( mif_pline );
					}
				}
				break;
			case MIF_REGION:
				{
					MIF_Region * mif_region = new MIF_Region;

					loadMIFRegion( mif_file, mif_region );
					mif_data.push_back( mif_region );
				}
				break;
			case MIF_TEXT:
				{
					MIF_Text * mif_text = new MIF_Text;

					loadMIFText( mif_file, mif_text );
					mif_data.push_back( mif_text );
				}
				break;
			case MIF_ARC:
				{
					MIF_Arc * mif_arc = new MIF_Arc;

					loadMIFArc( mif_file, mif_arc );
					mif_data.push_back( mif_arc );
				}
				break;
			case NO_SUCH_OBJECT:
				{
					string ignore;
					getline( mif_file, ignore ); 
				//return ERR_MIFDATA_READ_ERR;
				}
			case MIF_MULPLINE:
				assert(0);
//#pragma message(__FILE__ " warning: MIF_IO::loadMIFData( ifstream & mif_file, MIF_Obj_Vec & mif_data )未处理的类型: MIF_MULPLINE")
			default:
				assert(0);
				break;
		}
		mif_file >> string_temp;
	}

	return 0;
}

int_32 MIF_IO::loadMIFData( std::ifstream &mif_file, int max_n, MIF_Obj_Vec &mif_data )
{
	int valid_count = 0;
	MIF_Object_enum type;
	string string_temp;
	string str_temp;
	//if max_n < 0, no limitation
	while( !mif_file.eof() && (valid_count < max_n || max_n < 0 ))
	{		
		mif_file >> string_temp;		
		type = searchType( string_temp.c_str() ); 
		if(NO_SUCH_OBJECT == type)
		{
			continue;
		}
		++valid_count;
		switch ( type ){
			case MIF_POINT:
				{
					MIF_Point * mif_point = new MIF_Point;
					
					loadMIFPoint( mif_file, mif_point );
					mif_data.push_back( mif_point );
				}				
				break;
			case MIF_LINE:
				{
					MIF_Line * mif_line = new MIF_Line;

					loadMIFLine( mif_file, mif_line );
					mif_data.push_back( mif_line );
				}
				break;
			case MIF_PLINE:
				{
					mif_file >> str_temp;
					if( !_stricmp( str_temp.c_str(), "Multiple" ) )
					{
						MIF_MulPline * mif_mulpline = new MIF_MulPline;

						loadMIFMulPline( mif_file, mif_mulpline );
						mif_data.push_back( mif_mulpline );
					}
					else
					{
						MIF_Pline * mif_pline = new MIF_Pline;

						loadMIFPline( mif_file, mif_pline, str_temp );
						mif_data.push_back( mif_pline );
					}
				}
				break;
			case MIF_REGION:
				{
					MIF_Region * mif_region = new MIF_Region;

					loadMIFRegion( mif_file, mif_region );
					mif_data.push_back( mif_region );
				}
				break;
			case MIF_TEXT:
				{
					MIF_Text * mif_text = new MIF_Text;

					loadMIFText( mif_file, mif_text );
					mif_data.push_back( mif_text );
				}
				break;
			case MIF_ARC:
				{
					MIF_Arc * mif_arc = new MIF_Arc;

					loadMIFArc( mif_file, mif_arc );
					mif_data.push_back( mif_arc );
				}
				break;
			case NO_SUCH_OBJECT:
				{
					string ignore;
					getline( mif_file, ignore ); 
				//return ERR_MIFDATA_READ_ERR;
				}
			case MIF_MULPLINE:
				assert(0);
//#pragma message(__FILE__ " warning: MIF_IO::loadMIFData( ifstream & mif_file, MIF_Obj_Vec & mif_data )未处理的类型: MIF_MULPLINE")
			default:
				assert(0);
				break;
		}
		string_temp = "";
	}	
	return valid_count;
}


//-----------------把中间数据结构的数据部分写入文件------------------------
int_32 MIF_IO::storeMIFData( ofstream & mif_file, const MIF_Obj_Vec & mif_data )
{
	MIF_Point * mif_point;
	MIF_Line * mif_line;
	MIF_Pline * mif_pline;
	MIF_Region * mif_region;
	MIF_MulPline * mif_mulpline;
	MIF_Text * mif_text;

	mif_file.precision( 18 );

	MIF_Obj_Vec::const_iterator iter;
	for ( iter = mif_data.begin(); iter != mif_data.end(); iter++ )
	{
		if(( mif_point = dynamic_cast< MIF_Point *>( *iter )) != NULL )
			storeMIFPoint( mif_file, mif_point );

		else if(( mif_line = dynamic_cast< MIF_Line *>( *iter )) != NULL )
			storeMIFLine( mif_file, mif_line );
		
		else if(( mif_pline = dynamic_cast< MIF_Pline *>( *iter )) != NULL )
			storeMIFPline( mif_file,  mif_pline );

		else if(( mif_mulpline = dynamic_cast< MIF_MulPline *>( *iter )) != NULL )
			storeMIFMulPline( mif_file, mif_mulpline );
		
		else if(( mif_region = dynamic_cast< MIF_Region *>( *iter )) != NULL )
			storeMIFRegion( mif_file, mif_region );

		else if(( mif_text = dynamic_cast< MIF_Text *>( *iter )) != NULL )
			storeMIFText( mif_file, mif_text );
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------从mid文件中读取到数据结构中------------------------
int_32 MID_IO::loadMIDFile( ifstream & mid_file, const MIF_Header & mif_header, RecordSetImpl & recordsetimpl )
{
	int_32 m_errinfo;

	m_errinfo = loadMIDField( mif_header, recordsetimpl );
	if( m_errinfo ) return m_errinfo;

	m_errinfo = loadMIDRec( mid_file, mif_header, recordsetimpl );
    if( m_errinfo ) return m_errinfo;

	return 0;
}
int_32 MID_IO::loadMIDHeadFile( std::ifstream & mid_file, const MIF_Header & mif_header, RecordSetImpl & recordsetimpl )
{
	int_32 m_errinfo;

	m_errinfo = loadMIDField( mif_header, recordsetimpl );
	if( m_errinfo ) return m_errinfo;
	return 0;
    
}

////----------------------把中间数据结构中的mid数据存储到.mid文件中---------------------
//int_32 MID_IO::storeMIDFile( ofstream & mid_file, const MIF_Header & mif_header/*, const RecordSetImpl & recordsetimpl */)
//{
//	
//	string delimiter;
//	string del_temp;
//	ATB_Field_Type_enum type;
//
//	if( mif_header.delimiter != "" )
//	{
//		del_temp = mif_header.delimiter;
//		delimiter = del_temp;		
//	}
//	else delimiter = "\t";
//
//	int_32 last_index = recordsetimpl.getFieldNum() - 1;
//	wstring last_field = recordsetimpl.getFieldInfo(last_index)->getName();        
//	
//	int_32 out_loop, in_loop;
//	int_32 last_tag = 0;
//	for( out_loop = 0; out_loop < recordsetimpl.getRecordNum(); out_loop++ )
//	{
//		if( _wcsicmp( last_field.c_str(), L"gn_goid" ) == 0)
//		{
//            last_tag = 1;            
//		}
//
//        for( in_loop = 0; in_loop < ( recordsetimpl.getFieldNum() - 1 - last_tag); in_loop++ )
//		{	
//			if( !_wcsicmp( recordsetimpl.getFieldInfo( in_loop )->getName(), L"gn_goid" ) )
//				continue;
//			type = recordsetimpl.getFieldInfo( in_loop )->getType();
//			switch ( type )
//			{
//			case ATTRIBUTE_FIELD_TYPE_LONG: 			
//				mid_file << recordsetimpl.getRecord( out_loop )->getItem( in_loop )->getLongVal() 
//					<< delimiter; 
//				break;
//			
//			case ATTRIBUTE_FIELD_TYPE_DOUBLE:
//				mid_file << recordsetimpl.getRecord( out_loop )->getItem( in_loop )->getDoubleVal()
//					<< delimiter;
//				break;
//			
//			case ATTRIBUTE_FIELD_TYPE_BIGINT:
//				mid_file << recordsetimpl.getRecord( out_loop )->getItem( in_loop )->getBigintVal()
//					<< delimiter;
//				break;
//			
//			case ATTRIBUTE_FIELD_TYPE_STRING:
//				{
//					string str_temp( _bstr_t( recordsetimpl.getRecord( out_loop )->getItem( in_loop )->getStrVal()) );
//					mid_file << '"' << str_temp << '"' << delimiter;
//				}		
//				break;
//			
//			case ATTRIBUTE_FIELD_TYPE_BLOB:
//				mid_file << recordsetimpl.getRecord( out_loop )->getItem( in_loop )->getBlobVal()
//					<< delimiter;
//				break;
//			case ATTRIBUTE_FIELD_TYPE_UNKNOWN:
//			default:
//				assert(0);
//				break;
//   			}
//		}
//		if ( recordsetimpl.getFieldInfo( in_loop ) != NULL )
//		if( _wcsicmp( recordsetimpl.getFieldInfo( in_loop )->getName(), L"gn_goid" ) )
//		{
//			type = recordsetimpl.getFieldInfo( in_loop )->getType();
//			switch ( type )
//			{
//			
//			case ATTRIBUTE_FIELD_TYPE_LONG: 			
//				mid_file << recordsetimpl.getRecord( out_loop )->getItem( in_loop )->getLongVal() 
//					<< endl; 
//				break;
//			
//			case ATTRIBUTE_FIELD_TYPE_DOUBLE:
//				mid_file << recordsetimpl.getRecord( out_loop )->getItem( in_loop )->getDoubleVal()
//					<< endl;
//				break;
//			
//			case ATTRIBUTE_FIELD_TYPE_BIGINT:
//				mid_file << recordsetimpl.getRecord( out_loop )->getItem( in_loop )->getBigintVal()
//					<< endl;
//				break;
//			
//			case ATTRIBUTE_FIELD_TYPE_STRING:
//				{
//					string str_temp( _bstr_t( recordsetimpl.getRecord( out_loop )->getItem( in_loop )->getStrVal()) );
//					mid_file << '"' <<  str_temp << '"' << endl;
//				}
//				break;
//			
//			case ATTRIBUTE_FIELD_TYPE_BLOB:
//				mid_file << recordsetimpl.getRecord( out_loop )->getItem( in_loop )->getBlobVal()
//					<< endl;
//				break;
//			case ATTRIBUTE_FIELD_TYPE_UNKNOWN:
//			default:
//				assert(0);
//				break;
//			}
//		}
//		else
//			mid_file << endl;
//	}
//
//	return 0;
//}
//
#if defined(_WIN32) || defined(_WIN64)
    WCHAR *win32_mbcsToUnicode(const char *zFilename){
        int nByte;
        WCHAR *zMbcsFilename;
        int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
        
        nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, NULL,0)*sizeof(WCHAR);
        zMbcsFilename = (WCHAR *)malloc( nByte*sizeof(zMbcsFilename[0]) );
        if( zMbcsFilename==0 ){
            return 0;
        }
        nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, zMbcsFilename, nByte);
        if( nByte==0 ){
            free(zMbcsFilename);
            zMbcsFilename = 0;
        }
        return zMbcsFilename;
    }
#endif
    
    wchar_t * asci_to_unicode(const char* src)
    {
        wchar_t* dest = NULL;
#if defined(_WIN32) || defined(_WIN64)
        dest =  win32_mbcsToUnicode(src);
#else //ios
        const char * curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
        
        size_t size = mbstowcs(NULL, src, 0);
        size_t _Dsize = (size + 1);
        wchar_t *_Dest = (wchar_t*)malloc(_Dsize*sizeof(wchar_t));
        wmemset(_Dest, 0, _Dsize);
        mbstowcs(_Dest,src,strlen(src));
        
        setlocale(LC_ALL, curLocale);
        dest = _Dest;
#endif
        return dest ;
    }
    
//---------------得到RecordSetImpl的fieldInfo部分-----------------------
int_32 MID_IO::loadMIDField( const MIF_Header & mif_header, RecordSetImpl & recordsetimpl )
{
	FieldInfoImpl *fieldinfo=new FieldInfoImpl;
    wstring name;
	int_32 flag; 
	int_32 len;
	int_32 str_width;

	vector< MIF_ColumnUnit >::const_iterator col_unit;

	for( col_unit = mif_header.columns.begin(); col_unit !=mif_header.columns.end(); ++col_unit )
	{
		ATB_Field_Type_enum type = searchMidType( col_unit->type, str_width );
		
		if( type == ATTRIBUTE_FIELD_TYPE_UNKNOWN )
			return ERR_COLUMNS_TYPE_ERR;

        if( type == ATTRIBUTE_FIELD_TYPE_STRING )
			len = str_width * 2;
		else len = 0;		
		
		name = wstring(asci_to_unicode(col_unit->name.c_str()));
		//MultiByteToWideChar( 0, 0, col_unit->name.c_str(), -1, name, 128 * 2 );
		flag = 0;
		
		fieldinfo->setdata(name.c_str(), type, flag, len );
		recordsetimpl.newFieldInfo( *fieldinfo );
	}

	cout << recordsetimpl.getFieldNum() << endl;

	return 0;
}

//--------------------从文件中读取mid的数据部分----------------------------
int_32 MID_IO::loadMIDRec( ifstream & mid_file, const MIF_Header & mif_header, RecordSetImpl & recordsetimpl )
{
	string delimiter;
	string del_temp;
	string str_temp;
	string str_buf;

	ATB_Record *rd;

	if( mif_header.delimiter != "" ){
		del_temp = mif_header.delimiter;
		delimiter = del_temp.substr( 1, del_temp.size() - 2 );
	}
	else
		delimiter = "\t" ;

	//if(mid_file.eof())
	//	return 0;

	//getline( mid_file, str_temp );
	//int kk = 0;
	while( !mid_file.eof() )
	{
		//kk++;
		//if( kk == 62165 )
		//	kk = kk-1;
		
		/*if( ( MIF_IO::stringTrim( str_temp ) ).empty() )
		{
			recordsetimpl.newRecord();
			getline( mid_file, str_temp );
			continue;
		}*/

		getline( mid_file, str_temp );
        rd = recordsetimpl.newRecord();
		for ( int_32 i = 0; i< recordsetimpl.getFieldNum(); i++ )
		{
			str_temp = MIF_IO::stringTrim( str_temp );
			if( i == ( recordsetimpl.getFieldNum() - 1 ) ) 
				str_buf = str_temp;
			else
			{	
				if( str_temp.find_first_of('"',0) == 0 )
				{
					str_temp = str_temp.substr( 1 );
					str_buf = str_temp.substr( 0, str_temp.find_first_of( '"', 0 ) );
					str_temp = str_temp.substr( str_temp.find_first_of( '"', 0 )+1 );
					str_temp = str_temp.substr( str_temp.find_first_of( delimiter.c_str(), 0 )+1 );
				}
				else
				{
					str_buf = str_temp.substr( 0, str_temp.find_first_of( delimiter.c_str(), 0 ) );
					str_temp = str_temp.substr( str_temp.find_first_of( delimiter.c_str(), 0 )+1 );
				}
			}

			ATB_Field_Type_enum type = recordsetimpl.getFieldInfo(i)->getType();
			switch ( type )
			{
			case ATTRIBUTE_FIELD_TYPE_LONG:
				{ 
					long itemvalue;
					itemvalue = atol( str_buf.c_str() );
					rd->getItem(i)->setLongVal( itemvalue );
				}
				break;
			case ATTRIBUTE_FIELD_TYPE_DOUBLE:
				{
					double itemvalue;
					itemvalue = atof( str_buf.c_str() );
					rd->getItem(i)->setDoubleVal( itemvalue );
				}
				break;
			case ATTRIBUTE_FIELD_TYPE_BIGINT:
				{
					int_64 itemvalue;
					//itemvalue = _atoi64( str_buf.c_str() );
					sscanf(str_buf.c_str(), "%lld", &itemvalue);
					rd->getItem(i)->setBigintVal( itemvalue );
				}
				break;
			case ATTRIBUTE_FIELD_TYPE_STRING:
				{
					if( str_buf.find_first_of('"',0) == 0 )
						str_buf=str_buf.substr(1,str_buf.size()-2);
					str_buf = MIF_IO::stringTrim( str_buf );
                    wstring strtemp = wstring(asci_to_unicode(str_buf.c_str()));
					rd->getItem(i)->setStrVal(strtemp.c_str());
				}
				break;
			case ATTRIBUTE_FIELD_TYPE_BLOB:
				{
				}
				break;
			case ATTRIBUTE_FIELD_TYPE_UNKNOWN:
				{
				}
				break;
			}
		}
	//	getline(mid_file,str_temp);
	}
	return 0;
}

int_32 MID_IO::loadMIDRec( ifstream & mid_file, const MIF_Header & mif_header,int max_n, RecordSetImpl & recordsetimpl )
{
	string delimiter;
	string del_temp;
	string str_temp;
	string str_buf;

	ATB_Record *rd;

	if( mif_header.delimiter != "" ){
		del_temp = mif_header.delimiter;
		delimiter = del_temp.substr( 1, del_temp.size() - 2 );
	}
	else
		delimiter = "\t" ;

	int valid_count = 0;

	while( !mid_file.eof() && (valid_count < max_n || max_n < 0 ))
	{
		//kk++;
		//if( kk == 62165 )
		//	kk = kk-1;
		
		/*if( ( MIF_IO::stringTrim( str_temp ) ).empty() )
		{
			recordsetimpl.newRecord();
			getline( mid_file, str_temp );
			continue;
		}*/
		
		getline( mid_file, str_temp );
		if("" == str_temp)
			return valid_count;
		++valid_count;
        rd = recordsetimpl.newRecord();
		for ( int_32 i = 0; i< recordsetimpl.getFieldNum(); i++ )
		{
			str_temp = MIF_IO::stringTrim( str_temp );
			if( i == ( recordsetimpl.getFieldNum() - 1 ) ) 
				str_buf = str_temp;
			else
			{	
				if( str_temp.find_first_of('"',0) == 0 )
				{
					str_temp = str_temp.substr( 1 );
					str_buf = str_temp.substr( 0, str_temp.find_first_of( '"', 0 ) );
					str_temp = str_temp.substr( str_temp.find_first_of( '"', 0 )+1 );
					str_temp = str_temp.substr( str_temp.find_first_of( delimiter.c_str(), 0 )+1 );
				}
				else
				{
					str_buf = str_temp.substr( 0, str_temp.find_first_of( delimiter.c_str(), 0 ) );
					str_temp = str_temp.substr( str_temp.find_first_of( delimiter.c_str(), 0 )+1 );
				}
			}

			ATB_Field_Type_enum type = recordsetimpl.getFieldInfo(i)->getType();
			switch ( type )
			{
			case ATTRIBUTE_FIELD_TYPE_LONG:
				{ 
					long itemvalue;
					itemvalue = atol( str_buf.c_str() );
					rd->getItem(i)->setLongVal( itemvalue );
				}
				break;
			case ATTRIBUTE_FIELD_TYPE_DOUBLE:
				{
					double itemvalue;
					itemvalue = atof( str_buf.c_str() );
					rd->getItem(i)->setDoubleVal( itemvalue );
				}
				break;
			case ATTRIBUTE_FIELD_TYPE_BIGINT:
				{
					int_64 itemvalue;
					//itemvalue = _atoi64( str_buf.c_str() );
					sscanf(str_buf.c_str(), "%lld", &itemvalue);
					rd->getItem(i)->setBigintVal( itemvalue );
				}
				break;
			case ATTRIBUTE_FIELD_TYPE_STRING:
				{
					if( str_buf.find_first_of('"',0) == 0 )
						str_buf=str_buf.substr(1,str_buf.size()-2);
					str_buf = MIF_IO::stringTrim( str_buf );
                    wstring strtemp = wstring(asci_to_unicode(str_buf.c_str()));
					rd->getItem(i)->setStrVal(strtemp.c_str());
				}
				break;
			case ATTRIBUTE_FIELD_TYPE_BLOB:
				{
				}
				break;
			case ATTRIBUTE_FIELD_TYPE_UNKNOWN:
				{
				}
				break;
			}
		}
	//	getline(mid_file,str_temp);
	}
	return valid_count;

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


MIF_Head_Field_enum MIF_IO::searchField( const string & str)
{
	if (!_stricmp(str.c_str(),"Version")) 
		return MIF_VERSION;

	else if (!_stricmp(str.c_str(),"Charset"))
		return MIF_CHARSET;
	
	else if (!_stricmp(str.c_str(),"Delimiter"))
		return MIF_DELIMITER;
	
	else if (!_stricmp(str.c_str(),"UNIQUE"))
		return MIF_UNIQUE;
	
	else if (!_stricmp(str.c_str(),"INDEX"))
		return MIF_INDEX;
    
	else if (!_stricmp(str.c_str(),"CoordSys"))
		return MIF_COORDSYS;
	
	else if (!_stricmp(str.c_str(),"TransForm"))
		return MIF_TRANSFORM;
	
	else if (!_stricmp(str.c_str(),"Columns"))
		return MIF_COLUMNS;
	
	else
		return NO_SUCH_HEAD_FIELD;
}

//------------------把字符串两端的空格和tab去掉-------------------
string MIF_IO::stringTrim( string &str_in )
{
	int i = 0;
	uint_32 size;
	string str_out;
	
	size = str_in.size();
	if ( i == size ) return string("");
	while( str_in[i] ==' '|| str_in[i] == '\t' || str_in[i] == 13){
		i++;
	}
	if ( i == size ) return string("");
	else
	{
		str_out = str_in.substr( i );
		size = str_out.size();
		
		while( str_out[size-1] == ' '|| str_out[size-1] == '\t' || str_out[size-1] == 13 ){
			size--;
		}
		str_out = str_out.substr(0,size);
		return str_out;
	}
}

//---------------从文件数据中构造出地球坐标系-----------------------
int_32 MIF_IO::loadEarthCor( string & earth_str, MIF_CS_Earth * earth )
{
	string str_pro;
	string str_aff;
	string str_bds;
	string str_temp;
	uint_32 pos_temp;

	pos_temp = earth_str.find( "Bounds", 0 );
	if( pos_temp != -1 )
	{
		str_bds = earth_str.substr( pos_temp + 6 );
		str_bds = stringTrim( str_bds );
		earth_str = earth_str.substr( 0, pos_temp );
	}

	pos_temp = earth_str.find( "Affine", 0 );
	if( pos_temp != -1 )
	{
		str_aff = earth_str.substr( pos_temp + 6 );
		str_aff = stringTrim( str_aff );
		earth_str = earth_str.substr( 0, pos_temp );
	}

	pos_temp = earth_str.find( "Projection", 0 );
	if( pos_temp != -1 )
	{
		str_pro = earth_str.substr( 10 );
		str_pro = stringTrim( str_pro );
	}

	if( !str_bds.empty() )
	{
		earth->bds = new MIF_CS_Bounds;

		str_bds = str_bds.substr( 1, str_bds.size()-2 );
		str_bds = stringTrim( str_bds );
		earth->bds->p_min.d0 = atof( str_bds.substr( 0, str_bds.find_first_of( ",", 0 ) ).c_str() );

		earth->bds->p_min.d1 = atof( str_bds.substr( (str_bds.find_first_of( ",", 0 )+ 1),( str_bds.find_first_of( ")", 0 ) ) ).c_str() );

		earth->bds->p_max.d0 = atof( str_bds.substr( (str_bds.find_first_of( "(", 0 )+ 1),( str_bds.find_last_of( "," ) ) ).c_str() );

		earth->bds->p_max.d1 = atof( str_bds.substr( str_bds.find_last_of( "," ) + 1 ).c_str() );
	}

	if( !str_aff.empty() )
	{
		earth->aff = new MIF_CS_Affine;

		str_aff = str_aff.substr( str_aff.find_first_of( " ", 0 ) + 1 );
		str_aff = stringTrim( str_aff );

		str_temp = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		str_temp = stringTrim( str_temp );
		if( str_temp.find_first_of('"',0) == 0 )
			str_temp = str_temp.substr(1,str_temp.size()-2 );

		earth->aff->unitname = str_temp;
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		earth->aff->A = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		earth->aff->A = stringTrim( earth->aff->A );
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		earth->aff->B = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		earth->aff->B = stringTrim( earth->aff->B );
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		earth->aff->C = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		earth->aff->C = stringTrim( earth->aff->C );
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		earth->aff->D = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		earth->aff->D = stringTrim( earth->aff->D );
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		earth->aff->E = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		earth->aff->E = stringTrim( earth->aff->E );
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		earth->aff->F = stringTrim( str_aff );
	}

	if( !str_pro.empty() )
	{
		earth->pro = new MIF_CS_Pro;

		uint_32 delimi = str_pro.find_first_of( ",", 0 );
		earth->pro->type = atoi( str_pro.substr( 0, delimi ).c_str() );
		str_pro = str_pro.substr( delimi + 1 );
		str_pro = stringTrim( str_pro );

		delimi = str_pro.find_first_of( ",", 0 );
		if( delimi == -1 )
		{
			earth->pro->datum = atoi( str_pro.c_str() );
			goto RETURN;		
		}
		else
		{
			earth->pro->datum = atoi( str_pro.substr( 0, delimi ).c_str() );
			str_pro = str_pro.substr( delimi + 1 );
			str_pro = stringTrim( str_pro );
		}

		delimi = str_pro.find_first_of( ",", 0 );
		if( delimi == -1 )
		{
			earth->pro->unitname = new string;
			str_temp = str_pro;
			if( str_temp.find_first_of('"',0) == 0 )
				str_temp = str_temp.substr(1, str_temp.size()-2 );

			*earth->pro->unitname = str_temp;
			goto RETURN;
		}
		else
		{
			earth->pro->unitname = new string;
			str_temp = str_pro.substr( 0, delimi );
			str_temp = stringTrim( str_temp );
			if( str_temp.find_first_of('"',0) == 0 )
				str_temp = str_temp.substr(1, str_temp.size()-2 );

			*earth->pro->unitname = str_temp;
			str_pro = str_pro.substr( delimi + 1 );
			str_pro = stringTrim( str_pro );
		}

		delimi = str_pro.find_first_of( ",", 0 );
		if( delimi == -1 )
		{
			earth->pro->origin_longitude = new double;
			*earth->pro->origin_longitude = atof( str_pro.c_str() );
			goto RETURN;
		}
		else
		{
			earth->pro->origin_longitude = new double;
			*earth->pro->origin_longitude = atof( str_pro.substr( 0, delimi ).c_str() );
			str_pro = str_pro.substr( delimi + 1 );
			str_pro = stringTrim( str_pro );
		}

		delimi = str_pro.find_first_of( ",", 0 );
		if( delimi == -1 )
		{
			earth->pro->origin_latitude = new double;
			*earth->pro->origin_latitude = atof( str_pro.c_str() );
			goto RETURN;
		}
		else
		{
			earth->pro->origin_latitude = new double;
			*earth->pro->origin_latitude = atof( str_pro.substr( 0, delimi ).c_str() );
			str_pro = str_pro.substr( delimi + 1 );
			str_pro = stringTrim( str_pro );
		}

		delimi = str_pro.find_first_of( ",", 0 );
		if( delimi == -1 )
		{
			earth->pro->standard_parallel_1 = new double;
			*earth->pro->standard_parallel_1 = atof( str_pro.c_str() );
			goto RETURN;
		}
		else
		{
			earth->pro->standard_parallel_1 = new double;
			*earth->pro->standard_parallel_1 = atof( str_pro.substr( 0, delimi ).c_str() );
			str_pro = str_pro.substr( delimi + 1 );
			str_pro = stringTrim( str_pro );
		}	

		delimi = str_pro.find_first_of( ",", 0 );
		if( delimi == -1 )
		{
			earth->pro->standard_parallel_2 = new double;
			*earth->pro->standard_parallel_2 = atof( str_pro.c_str() );
			goto RETURN;
		}
		else
		{
			earth->pro->standard_parallel_2 = new double;
			*earth->pro->standard_parallel_2 = atof( str_pro.substr( 0, delimi ).c_str() );
			str_pro = str_pro.substr( delimi + 1 );
			str_pro = stringTrim( str_pro );
		}

		delimi = str_pro.find_first_of( ",", 0 );
		if( delimi == -1 )
		{
			earth->pro->azimuth = new double;
			*earth->pro->azimuth = atof( str_pro.c_str() );
			goto RETURN;
		}
		else
		{
			earth->pro->azimuth = new double;
			*earth->pro->azimuth = atof( str_pro.substr( 0, delimi ).c_str() );
			str_pro = str_pro.substr( delimi + 1 );
			str_pro = stringTrim( str_pro );
		}

		delimi = str_pro.find_first_of( ",", 0 );
		if( delimi == -1 )
		{
			earth->pro->scale_factor = new double;
			*earth->pro->scale_factor = atof( str_pro.c_str() );
			goto RETURN;
		}
		else
		{
			earth->pro->scale_factor = new double;
			*earth->pro->scale_factor = atof( str_pro.substr( 0, delimi ).c_str() );
			str_pro = str_pro.substr( delimi + 1 );
			str_pro = stringTrim( str_pro );
		}

		delimi = str_pro.find_first_of( ",", 0 );
		if( delimi == -1 )
		{
			earth->pro->false_easting = new string;
			*earth->pro->false_easting = str_pro;
			goto RETURN;
		}
		else
		{
			earth->pro->false_easting = new string;
			*earth->pro->false_easting = str_pro.substr( 0, delimi );
			str_pro = str_pro.substr( delimi + 1 );
			str_pro = stringTrim( str_pro );
		}

		delimi = str_pro.find_first_of( ",", 0 );
		if( delimi == -1 )
		{
			earth->pro->false_northing = new string;
			*earth->pro->false_northing = str_pro;
			goto RETURN;
		}
		else
		{
			earth->pro->false_northing = new string;
			*earth->pro->false_northing = str_pro.substr( 0, delimi );
			str_pro = str_pro.substr( delimi + 1 );
			str_pro = stringTrim( str_pro );
		}

		delimi = str_pro.find_first_of( ",", 0 );
		if( delimi == -1 )
		{
			earth->pro->range = new double;
			*earth->pro->range = atof( str_pro.c_str() );
			goto RETURN;
		}
		else
		{
			earth->pro->range = new double;
			*earth->pro->range = atof( str_pro.substr( 0, delimi ).c_str() );
			str_pro = str_pro.substr( delimi + 1 );
			str_pro = stringTrim( str_pro );
		}
	}
RETURN:	
	return 0;
}

//------------------构造出nonearth坐标系------------------------
int_32 MIF_IO::loadNonearthCor( string & nonearth_str, MIF_CS_Nonearth * nonearth )
{
	string str_uni;
	string str_aff;
	string str_bds;
	string str_temp;
	uint_32 pos_temp;
	
	pos_temp = nonearth_str.find( "Bounds", 0 );
	if( pos_temp != -1 )
	{
		str_bds = nonearth_str.substr( pos_temp + 6 );
		str_bds = stringTrim( str_bds );
		nonearth_str = nonearth_str.substr( 0, pos_temp );
	}

	pos_temp = nonearth_str.find( "Units", 0 );
	if( pos_temp != -1 )
	{
		str_uni = nonearth_str.substr( pos_temp + 5 );
		str_uni = stringTrim( str_uni );
		nonearth_str = nonearth_str.substr( 0, pos_temp );
   	}

	pos_temp = nonearth_str.find( "Affine", 0 );
	if( pos_temp != -1 )
	{
		str_aff = nonearth_str.substr( 6 );
		str_aff = stringTrim( str_aff );
	}
	
	if( !str_bds.empty() )
	{
		str_bds = str_bds.substr( 1, str_bds.size()-2 );
		str_bds = stringTrim( str_bds );
		nonearth->bds.p_min.d0 = atof( str_bds.substr( 0, str_bds.find_first_of( ",", 0 ) ).c_str() );
		
		nonearth->bds.p_min.d1 = atof( str_bds.substr( (str_bds.find_first_of( ",", 0 )+ 1),( str_bds.find_first_of( ")", 0 ) ) ).c_str() );

		nonearth->bds.p_max.d0 = atof( str_bds.substr( (str_bds.find_first_of( "(", 0 )+ 1),( str_bds.find_last_of( "," ) ) ).c_str() );

		nonearth->bds.p_max.d1 = atof( str_bds.substr( str_bds.find_last_of( "," ) + 1 ).c_str() );
	}

	if ( !str_uni.empty() )
	{		
		str_temp = str_uni;
		if( str_temp.find_first_of('"',0) == 0 )
			str_temp = str_temp.substr(1,str_temp.size()-2 );
				
		nonearth->unit_name = str_temp;
	}

	if( !str_aff.empty() )
	{
		nonearth->aff = new MIF_CS_Affine;

		str_aff = str_aff.substr( str_aff.find_first_of( " ", 0 ) + 1 );
		str_aff = stringTrim( str_aff );

		str_temp = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		str_temp = stringTrim( str_temp );
		if( str_temp.find_first_of('"',0) == 0 )
			str_temp = str_temp.substr(1, str_temp.size()-2 );
		
		nonearth->aff->unitname = str_temp;
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		nonearth->aff->A = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		nonearth->aff->A = stringTrim( nonearth->aff->A );
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		nonearth->aff->B = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		nonearth->aff->B = stringTrim( nonearth->aff->B );
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		nonearth->aff->C = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		nonearth->aff->C = stringTrim( nonearth->aff->C );
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		nonearth->aff->D = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		nonearth->aff->D = stringTrim( nonearth->aff->D );
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		nonearth->aff->E = str_aff.substr( 0, str_aff.find_first_of( ",", 0 ) );
		nonearth->aff->E = stringTrim( nonearth->aff->E );
		str_aff = str_aff.substr( str_aff.find_first_of( ",", 0 ) + 1 );

		nonearth->aff->F = stringTrim( str_aff );
	}

	return 0;
}

MIF_Object_enum MIF_IO::searchType(const string & str)
{
	if (!_stricmp(str.c_str(),"Point")) 
		return MIF_POINT;
	else if (!_stricmp(str.c_str(),"Line"))
		return MIF_LINE;
	else if (!_stricmp(str.c_str(),"Pline"))
		return MIF_PLINE;
	else if (!_stricmp(str.c_str(),"Region"))
		return MIF_REGION;
	else if (!_stricmp(str.c_str(), "Text") )
		return MIF_TEXT;
	else if (!_stricmp(str.c_str(), "Arc"))
		return MIF_ARC;
	else 
		return NO_SUCH_OBJECT;
}

ATB_Field_Type_enum MID_IO::searchMidType( const string & column_type, int_32 & str_width )
{
	string str_temp;
	string col_type;

	col_type = column_type;
	uint_32 position = col_type.find_first_of( "(", 0);
	
	if( position !=-1 )
	{
        str_temp = col_type.substr( position + 1 );
		col_type = col_type.substr( 0, position);
	}
	
	col_type = MIF_IO::stringTrim( col_type );
	if( !_stricmp( col_type.c_str(), "char" ) )
	{
		str_temp = str_temp.substr( 0,str_temp.find_last_of( ")", 0));
		str_temp = MIF_IO::stringTrim( str_temp );
		str_width = atoi(str_temp.c_str());
		
		return ATTRIBUTE_FIELD_TYPE_STRING;
	}
	
	if( !_stricmp( col_type.c_str(), "decimal" ) ||  !_stricmp(col_type.c_str(),"float") )
		return ATTRIBUTE_FIELD_TYPE_DOUBLE;
	
	if( !_stricmp( col_type.c_str(), "date" ) )
	{
		str_width = 32;
		return ATTRIBUTE_FIELD_TYPE_STRING;
	}
	
	if( !_stricmp( col_type.c_str(),"integer") || !_stricmp( col_type.c_str(),"smallint"))
		return ATTRIBUTE_FIELD_TYPE_LONG;
	
	else
		return ATTRIBUTE_FIELD_TYPE_UNKNOWN;
}

//--------------把mif文件中各种类型的数据读入到相应的中间数据结构中----------------------
//------读入点-------
void MIF_IO::loadMIFPoint( ifstream & mif_file, MIF_Point * mif_point )
{
	Pos2D point;
	string str_temp;
	string str_def;
	
	mif_file >> point.d0;
	mif_file >> point.d1;
	mif_point->point_array.push_back( point );	
	
	getline( mif_file, str_temp );
	while ( stringTrim( str_temp ).empty() && !mif_file.eof() ) 
	{
		getline( mif_file, str_temp );
	}
	
	uint_32 postion = str_temp.find_first_of( "(", 0 );
	if( postion !=-1 ){
		mif_point->symbol = new MIF_Symbol;
		
		str_def = str_temp.substr( postion + 1 );
		mif_point->symbol->shape = atoi( str_def.substr( 0, str_def.find_first_of( ",", 0 ) ).c_str() );
		
		str_def = str_def.substr( str_def.find_first_of( ",", 0 )+1 );
		mif_point->symbol->color = atoi( str_def.substr( 0, str_def.find_first_of( ",", 0 ) ).c_str() );
		
		str_def = str_def.substr( str_def.find_first_of( ",", 0 )+1 );
		mif_point->symbol->size = atoi( str_def.substr( 0, str_def.find_first_of( ")", 0 ) ).c_str() );
        
		getline( mif_file, str_temp );
		while ( stringTrim( str_temp ).empty() && !mif_file.eof() ) 
		{	
			getline( mif_file, str_temp );
    	}
	}
	mif_file.seekg( -int(str_temp.length())-2, ios::cur );
}

//------读入线-------
void MIF_IO::loadMIFLine( ifstream & mif_file, MIF_Line * mif_line )
{
	Pos2D point;
	string str_temp;
	string str_def;
	
	mif_file >> point.d0;
	mif_file >> point.d1;
	mif_line->point_array.push_back( point );
	
	mif_file >> point.d0;
	mif_file >> point.d1;
	mif_line->point_array.push_back( point );
	
	getline( mif_file, str_temp );
	while ( stringTrim( str_temp ).empty() && !mif_file.eof() ) 
	{
		getline( mif_file, str_temp );
	}

	uint_32 postion = str_temp.find_first_of( "(", 0 );
	if( postion != -1 )
	{
		mif_line->pen = new MIF_Pen;
		
		str_def = str_temp.substr( postion + 1 );
		mif_line->pen->wdith = atoi( str_def.substr( 0, str_def.find_first_of( ",", 0 ) ).c_str() );
		
		str_def = str_def.substr( str_def.find_first_of( ",", 0 )+1 );
		mif_line->pen->pattern = atoi( str_def.substr( 0, str_def.find_first_of( ",", 0 ) ).c_str() );
		
		str_def = str_def.substr( str_def.find_first_of( ",", 0)+1 );
		mif_line->pen->color = atoi(str_def.substr( 0,str_def.find_first_of( ")", 0 ) ).c_str() );
        
		getline( mif_file,str_temp );
		while ( stringTrim( str_temp ).empty() && !mif_file.eof() ) 
		{	
			getline( mif_file, str_temp );
    	}
	}
	mif_file.seekg( -int(str_temp.length())-2, ios::cur);
}

//------读入多线段-------
void MIF_IO::loadMIFPline( ifstream & mif_file, MIF_Pline * mif_pline, string &str_temp )
{
	string temp;
	string str_trim;
	string str_def;
	unsigned int loop;
	Pos2D point;
	
	mif_pline->multi_pos_array.pos_num = atoi( str_temp.c_str() );
	for ( loop = mif_pline->multi_pos_array.pos_num; loop >0 ; loop-- )
	{
        mif_file >>point.d0;
		mif_file >>point.d1;
		mif_pline->multi_pos_array.point_array.push_back( point );
	    }

	getline( mif_file, temp );
	while ( stringTrim( temp ).empty() && !mif_file.eof() ) 
	{
		getline( mif_file,temp );
	}

	uint_32 postion = temp.find_first_of( "(", 0 );
	if( postion != -1 )
	{
		mif_pline->pen = new MIF_Pen;

		str_def = temp.substr( postion+1 );
		mif_pline->pen->wdith = atoi( str_def.substr( 0, str_def.find_first_of( ",", 0 ) ).c_str() );
		
		str_def = str_def.substr( str_def.find_first_of( ",", 0 )+1 );
		mif_pline->pen->pattern = atoi( str_def.substr( 0, str_def.find_first_of( ",", 0 ) ).c_str() );
		
		str_def = str_def.substr( str_def.find_first_of( ",", 0 )+1 );
		mif_pline->pen->color = atoi( str_def.substr( 0, str_def.find_first_of( ")", 0 ) ).c_str() );
		
		getline( mif_file, temp );
		while ( stringTrim(temp).empty() && !mif_file.eof())
		{
			getline( mif_file, temp );
    	}
	}

	str_trim = stringTrim( temp );
	if( !_stricmp( str_trim.c_str(), "Smooth" ))
	{	
		mif_pline->smooth = new bool;
		*mif_pline->smooth = true;
	}
	else
	{
		mif_file.seekg( -int(temp.length())-2, ios::cur);
	}
}

//------读入多线段----
void MIF_IO::loadMIFMulPline( ifstream & mif_file, MIF_MulPline * mif_mulpline )
{
	int loop;
	MIF_Pline *mif_pline;
	string str_temp;

	mif_file >> mif_mulpline->pline_num;
	for( loop = 0; loop < mif_mulpline->pline_num; loop++ )
	{
		mif_file >> str_temp;
		mif_pline = new MIF_Pline;
		loadMIFPline( mif_file, mif_pline, str_temp );

		if( mif_pline->pen )
		{
			mif_mulpline->pen = new MIF_Pen;

			mif_mulpline->pen->color = mif_pline->pen->color;
			mif_mulpline->pen->pattern = mif_pline->pen->pattern;
			mif_mulpline->pen->wdith = mif_pline->pen->wdith;
		}
		if( mif_pline->smooth )
		{
			mif_mulpline->smooth = new bool;
			*mif_mulpline->smooth = true;
		}
		mif_mulpline->pline_set.insert( mif_pline );
	}
}

//------读入面-------
void MIF_IO::loadMIFRegion( ifstream & mif_file, MIF_Region * mif_region )
{
	string str_temp;
	string str_def;
	string str_head;
	string str_trim;
	string c_str;
	unsigned int out_loop ,in_loop;
	Pos2D point;
	MultiDirectPosition2D multi_dir_pos;

	mif_file >> mif_region->poly_num;
	for ( out_loop = mif_region->poly_num; out_loop > 0; out_loop-- )
	{
		mif_file >> multi_dir_pos.pos_num;
		for ( in_loop = multi_dir_pos.pos_num; in_loop > 0; in_loop-- )//
		{
			mif_file >> point.d0;
			mif_file >> point.d1;
			multi_dir_pos.point_array.push_back( point );
		}

		mif_region->multi_pos_array.push_back( multi_dir_pos );
		multi_dir_pos.point_array.clear();
	}

	getline( mif_file, str_temp );
	while (stringTrim(str_temp).empty()&&!mif_file.eof()) 
	{
		getline(mif_file,str_temp);
	}

	uint_32 postion = str_temp.find_first_of( "(", 0 );
	if( postion != -1 )
	{
		str_head = str_temp.substr( 0, postion );
		str_head = stringTrim( str_head );

		if(!_stricmp( str_head.c_str(), "Pen" ))
		{
			mif_region->pen = new MIF_Pen;

		    str_def = str_temp.substr( postion + 1 );
		    mif_region->pen->wdith = atoi(str_def.substr( 0, str_def.find_first_of(",", 0)).c_str());
		    
			str_def = str_def.substr( str_def.find_first_of( ",", 0 )+1);
		    mif_region->pen->pattern = atoi( str_def.substr( 0,str_def.find_first_of(",", 0 )).c_str());
		    
			str_def = str_def.substr(str_def.find_first_of(",",0)+1);
		    mif_region->pen->color = atoi(str_def.substr( 0,str_def.find_first_of(")", 0 ) ).c_str());
            
			getline( mif_file, str_temp );
			while ( stringTrim(str_temp).empty() && !mif_file.eof())
			{
				getline(mif_file,str_temp);
	        }

			postion = str_temp.find_first_of("(",0);
		}

		if( postion != -1 )
		{
			mif_region->brush = new MIF_Brush;

		    str_def = str_temp.substr(postion+1);
		    mif_region->brush->pattern = atoi(str_def.substr(0,str_def.find_first_of(",",0)).c_str());
		    
			str_def = str_def.substr(str_def.find_first_of(",",0)+1);
		    mif_region->brush->fore_color = atoi(str_def.substr(0,str_def.find_first_of(",",0)).c_str());
		    
			str_def = str_def.substr(str_def.find_first_of(",",0)+1);
		    mif_region->brush->back_color = atoi(str_def.substr(0,str_def.find_first_of(")",0)).c_str());
            
			getline( mif_file, str_temp );
			while (stringTrim( str_temp ).empty() && !mif_file.eof())
			{
				getline( mif_file, str_temp );
	        }
		}
	}
	str_trim = stringTrim( str_temp );
	str_trim = str_trim.substr( 0, str_trim.find_first_of(" ",0) );

	if( !_stricmp( str_trim.c_str(), "Center" ))
	{	
		mif_file.seekg( -int(str_temp.length())-2, ios::cur );
	    
		mif_region->center =new Pos2D;
		mif_file >> c_str;
		mif_file >> mif_region->center->d0;
		mif_file >> mif_region->center->d1;
	}
	else
	{
		mif_file.seekg( -int(str_temp.length())-2, ios::cur);
	}
}

//---------导入文本----------
void MIF_IO::loadMIFText( ifstream & mif_file, MIF_Text * mif_text )
{
	Pos2D point1, point2;
	string str_temp;

	getline( mif_file, str_temp );
	while ( stringTrim( str_temp ).empty() && !mif_file.eof() ) 
	{
		getline( mif_file, str_temp );
	}
	str_temp = stringTrim(str_temp);
	if( str_temp.find_first_of('"',0) == 0 )
		str_temp = str_temp.substr(1, str_temp.size()-2);
	mif_text->text = str_temp;

	mif_file >> mif_text->pos0.d0;
	mif_file >> mif_text->pos0.d1;
	mif_file >> mif_text->pos1.d0;
	mif_file >> mif_text->pos1.d1;

	//mif_text->pos.d0 = ( point1.d0 + point2.d0 ) / 2;
	//mif_text->pos.d1 = ( point1.d1 + point2.d1 ) / 2;

	//patch by lmt  2006.05.18
	readTextAdditionalSection(mif_file);

}

void MIF_IO::readTextAdditionalSection(ifstream & mif_file)
{//add by lmt 2006.05.18
	string str_temp;
	string str_trim;
	do{
		getline( mif_file, str_temp );
		str_trim = stringTrim( str_temp );
	}while (  !mif_file.eof()
			  &&
			  (
					str_trim.empty() ||
					str_trim.find( "Font" ) != -1  ||
					str_trim.find( "Spacing" ) != -1  ||
					str_trim.find( "Angle" ) != -1  ||
					str_trim.find( "Label" ) != -1  ||
					str_trim.find( "Justify" ) != -1  
			   )
			 );
	mif_file.seekg( -int(str_temp.length())-2, ios::cur );
}
void MIF_IO::loadMIFArc( std::ifstream & mif_file, MIF_Arc * mif_arc )
{
	string str_temp;
	string str_trim;

	mif_file >> mif_arc->pos1.d0;
	mif_file >> mif_arc->pos1.d1;
	mif_file >> mif_arc->pos2.d0;
	mif_file >> mif_arc->pos2.d1;

	mif_file >> mif_arc->a;
	mif_file >> mif_arc->b;

	getline( mif_file, str_temp );
	str_trim = stringTrim( str_temp );
	str_trim = str_trim.substr( 0, str_trim.find_first_of(" ",0) );

	while ( stringTrim( str_temp ).empty() 
		| !_stricmp( str_trim.c_str(), "FONT" ) ) 
	{
		getline( mif_file, str_temp );
		if( mif_file.eof() )
			return;
		str_trim = stringTrim( str_temp );
		str_trim = str_trim.substr( 0, str_trim.find_first_of(" ",0) );
	}

	uint_32 postion = str_temp.find_first_of( "(", 0 );
	if( postion != -1 )
	{
		getline( mif_file,str_temp );
		while ( stringTrim( str_temp ).empty() && !mif_file.eof() ) 
		{	
			getline( mif_file, str_temp );
		}
	}

	mif_file.seekg( -int(str_temp.length())-2, ios::cur );
}

//--------------把中间数据结构中的数据按不同类型写入到文件中---------------------
//------写入点-------
void MIF_IO::storeMIFPoint( ofstream & mif_file, const MIF_Point * mif_point )
{
	Pos2D point;

	point = mif_point->point_array[0];
	mif_file << "Point " << point.d0 << " " << point.d1 <<endl;
	
	if( mif_point->symbol )
	mif_file << "Symbol (" << mif_point->symbol->shape << "," << mif_point->symbol->color 
	<< "," << mif_point->symbol->size << ")" << endl;
}

//------写入线-------
void MIF_IO::storeMIFLine( ofstream & mif_file, const MIF_Line * mif_line )
{
	Pos2D point;
	uint_32 loop;
	
	mif_file <<"Line ";
	for( loop=0; loop < mif_line->point_array.size(); loop++ )
	{
		point = mif_line->point_array[loop];
		mif_file << point.d0 <<" "<< point.d1 << " ";
	}
	mif_file << endl;
	
	if( mif_line->pen )
		mif_file << "Pen (" << mif_line->pen->wdith << ","
		<< mif_line->pen->pattern << "," << mif_line->pen->color<<")" << endl;
}

//------写入曲线-------
void MIF_IO::storeMIFPline( ofstream & mif_file, const MIF_Pline * mif_pline )
{
	Pos2D point;
	int loop;
	
	mif_file << "Pline " << mif_pline->multi_pos_array.pos_num << endl;
	for( loop = 0; loop < mif_pline->multi_pos_array.pos_num; loop++ )
	{	
		point = mif_pline->multi_pos_array.point_array[loop];
		mif_file << point.d0 << " " << point.d1 <<endl;
	}

	if( mif_pline->pen )
		mif_file << "Pen (" << mif_pline->pen->wdith << "," << 
		mif_pline->pen->pattern <<","<< mif_pline->pen->color << ")" <<endl;
	
	if( mif_pline->smooth )
		mif_file << "Smooth" <<endl;
}

//-----写入多条曲线集合-----
void MIF_IO::storeMIFMulPline( ofstream & mif_file, const MIF_MulPline * mif_mulpline )
{
	Pos2D point;
	int loop;
	set< MIF_Pline *>::const_iterator pln_iter;
	MIF_Pline * mif_pline;

	mif_file << "Pline Multiple " << mif_mulpline->pline_num << endl;
	
	for( pln_iter = mif_mulpline->pline_set.begin(); pln_iter
		!= mif_mulpline->pline_set.end(); pln_iter++ )
	{
		mif_pline = *pln_iter;
		mif_file << "  " << mif_pline->multi_pos_array.pos_num << endl;

		for( loop = 0; loop < mif_pline->multi_pos_array.pos_num; loop++ )
		{
			point = mif_pline->multi_pos_array.point_array[loop];
			mif_file << point.d0 << " " << point.d1 << endl;
		}

	}
	if( mif_mulpline->pen )
		mif_file << "Pen (" << mif_mulpline->pen->wdith << "," <<
		mif_mulpline->pen->pattern << "," << mif_mulpline->pen->color << "," << endl;

	if( mif_mulpline->smooth )
		mif_file << "Smooth" << endl;

}

//------写入面-------
void MIF_IO::storeMIFRegion( ofstream & mif_file, const MIF_Region * mif_region )
{
	Pos2D point;
	int out_loop,in_loop;
	
	mif_file << "Region "<< mif_region->poly_num << endl;
	for( out_loop = 0; out_loop < mif_region->poly_num; out_loop++ )
	{
		mif_file << mif_region->multi_pos_array[out_loop].pos_num << endl;

		for( in_loop = 0; in_loop < mif_region->multi_pos_array[out_loop].pos_num; in_loop++ )
		{
			point = mif_region->multi_pos_array[out_loop].point_array[in_loop];
			mif_file << point.d0 << " " << point.d1 <<endl;
		}
	}

	if( mif_region->pen )
		mif_file << "Pen (" << mif_region->pen->wdith <<","<< mif_region->pen->pattern 
		<< "," << mif_region->pen->color << ")" <<endl;

	if( mif_region->brush )
		mif_file << "Brush ("<< mif_region->brush->pattern << ","<< mif_region->brush->fore_color 
		<< "," << mif_region->brush->back_color <<")" << endl;
	
	if( mif_region->center )
		mif_file << "Center "<< mif_region->center->d0 <<" "<< mif_region->center->d1 <<endl;
}

//------写入文本-------
void MIF_IO::storeMIFText( std::ofstream & mif_file, const MIF_Text * mif_text )
{
	//TODO : pw
}

void MIF_IO::storeMIFArc( std::ofstream & mif_file, const MIF_Arc * mif_arc )
{
	//TODO : pw
}

} // namespace GIS_Embedded

////////////////////////////////////////////////
#endif // #ifdef CONFIG_HAVE_MIF_FILE_SUPPORT
