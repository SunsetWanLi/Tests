///* MIF_Adaptor.cpp : FileDataSrc -- MIF adaptor
// * 2003-09-27
// */
//
//#include "pch.h" //qzy
////#include "stdafx.h"
//#include "FDS_config.h"
//#include <algorithm>
//#define CONFIG_HAVE_MIF_FILE_SUPPORT
//#ifdef CONFIG_HAVE_MIF_FILE_SUPPORT
////////////////////////////////////////////////////
//
//#include <set>
//#include <map>
//#include <vector>
//#include <algorithm>
//#include <fstream>
//#include <string>
//
//#include "comutil.h"
//#include "MIF_IO.h"
//#include "MIF_Adaptor.h"
//#include "../../../Shared/CRS/CRS_Factory.h"
////#include "../../../Common/err_out.h"
////#include "../../../Shared/CRS/CRS.h"
//#include "ConnectCVT.h"
//#include "../TextPosition.h"
//#include "../BufferedText.h"
//
//
//using namespace std;
//
//namespace GIS_Embedded
//{
//MIF_Adaptor::MIF_Adaptor()
//{
//	mid_file = false;
//	m_text_layer = false;
//
//	//for cursor
//	m_gm_cursor = 0;
//	m_attr_cursor = 0;
//}
//
//MIF_Adaptor::~MIF_Adaptor()
//{
//	uint_32 i;
//
//	//delete mif_struct.mif_header.coord_sys, mif_struct.mif_header.coord_sys = NULL;
//	//delete mif_struct.mif_header.trans_form, mif_struct.mif_header.trans_form = NULL;
//
//	for ( i = 0; i <  mif_struct.mif_data.size(); i++ )
//		delete mif_struct.mif_data[i];
//	mif_struct.mif_data.clear();
//
//	for ( i = 0; i <  mif_struct.mif_data.size(); i++ )
//		delete mif_struct.mif_data[i];
//	mif_struct.mif_data.clear();
//
//	//for cursor
//	delete m_gm_cursor, m_gm_cursor = 0;
//	delete m_attr_cursor, m_attr_cursor = 0;
//}
////return valid_count
//int_32 GMEntityCursor_MIF::getGMs(int max_n, vector<GM_Object*>* gms,vector<int>* gm_type) 
//{
//	MIF_Struct mif_struct;
//	if(0 == m_gm_file)
//		return 0;
//	int valid_count = MIF_IO::loadMIFData(*m_gm_file, max_n, mif_struct.mif_data);
//	dynamic_cast<MIF_Adaptor*>(m_adaptor)->getAllEntityGM(mif_struct, gms);
//	for(int i = 0; i < mif_struct.mif_data.size(); ++i)
//		delete mif_struct.mif_data[i];
//	mif_struct.mif_data.clear();
//			
//	return valid_count;
//}
//
//int_32 AttrEntityCursor_MIF::getAttrs(int max_n, ATB_RecordSet* attrs__,const vector<int> &gm_type) 
//{
//	RecordSetImpl* attrs = dynamic_cast<RecordSetImpl*>(attrs__);
//	if(0 == attrs || 0 == m_attr_file || 0 == m_mif_header)
//		return 0;
//	int valid_count = MID_IO::loadMIDRec(*m_attr_file, *m_mif_header, max_n, *attrs);
//	return valid_count;
//}
//void AttrEntityCursor_MIF::setFclsField(const ATB_RecordSet& attr)
//{
//	m_rs = new RecordSetImpl;
//	m_rs->copyAllFieldInfo(attr);
//	return;
//}
//ATB_RecordSet* AttrEntityCursor_MIF::getFclsField()const
//{
//	RecordSetImpl* tmp_rs = new RecordSetImpl;
//	tmp_rs->copyAllFieldInfo(*m_rs);
//	return tmp_rs;
//}
//void AttrEntityCursor_MIF::setMIFHeader(const MIF_Header& header)
//{
//	m_mif_header = new MIF_Header;
//	*m_mif_header = header;	//just pointer-shallow-copy here
//	return;
//}
//
////-------------把文件中的数据装入到中间数据结构中--------------
//int_32 MIF_Adaptor::open( const string & filename )
//{
//	int_32 m_errinfo;
//    ifstream MID_file;
//	ifstream MIF_file;
//	string extName;
//
//	mif_filename = filename;
//	
//	fileOpen(MIF_file,filename.c_str(), ios::in);
//    //MIF_file.open( filename.c_str() , ios::in );
//	if( !MIF_file.is_open()) return ERR_MIFFILE_OPEN_ERR;
//	
//	extName = filename.substr( filename.rfind('.') + 1 );
//	transform(extName.begin(),extName.end(),extName.begin(),toupper);
//	if( extName != "MIF" ) return ERR_MIFFILE_OPEN_ERR; 
//
//	m_errinfo = MIF_IO::loadMIFFile( MIF_file, mif_struct );
//	MIF_file.close();
//
//	if( m_errinfo ) return m_errinfo;
//	
//	string m_midfile = filename.substr( 0, filename.rfind('.'));
//	m_midfile = m_midfile + ".mid";
//
//	fileOpen(MID_file,m_midfile.c_str(), ios::in);
//	//MID_file.open( m_midfile.c_str(), ios::in );
//	if( MID_file.is_open() )
//	{
//		mid_file = true;
//		m_errinfo = MID_IO::loadMIDFile( MID_file, mif_struct.mif_header, mid_struct );
//		MID_file.close();
//
//		if( m_errinfo ) return m_errinfo;
//	}
//	else
//	{
//		return ERR_MIFFILE_OPEN_ERR;
//	}
//	if( isTextLayer( mif_struct.mif_data ) )
//	{
//		m_text_layer = true;
//		addTextField( mif_struct.mif_data, &mid_struct );
//	}
//
//	return 0;
//}
//int_32 MIF_Adaptor::export( const string &filename )
//{
//	int_32 errinfo;
//	ofstream mif_file;
//	ofstream mid_file;
//	string extName;
//
//	mif_filename = filename;
//
//	if( filename.find('.') != -1 )
//	{
//		extName = filename.substr( filename.rfind('.') + 1 );
//		transform( extName.begin(), extName.end(), extName.begin(), toupper );
//		if( extName != "MIF" ) return ERR_MIFFILE_OPEN_ERR; 
//	}
//	else
//	{
//		mif_filename += ".mif";
//	}
//	fileOpen(mif_file,filename.c_str(), ios_base::in | ios_base::out | ios_base::trunc);
//	//mif_file.open( filename.c_str(), ios_base::in | ios_base::out | ios_base::trunc);
//	if( !mif_file.is_open() ) return ERR_MIFFILE_OPEN_ERR;
//
//
//	errinfo = MIF_IO::storeMIFFile( mif_file, mif_struct);
//	mif_file.close();
//	
//	string m_midfile = filename.substr( 0, filename.rfind('.'));
//	m_midfile = m_midfile + ".mid";
//
//	fileOpen(mid_file,m_midfile.c_str(), ios_base::in | ios_base::out | ios_base::trunc);
//	//mid_file.open( m_midfile.c_str(), ios_base::in | ios_base::out | ios_base::trunc);
//	if( !mid_file.is_open() ) return ERR_MIFFILE_OPEN_ERR;
//
//
//	errinfo = MID_IO::storeMIDFile( mid_file,mif_struct.mif_header, mid_struct);
//	mid_file.close();
//	return errinfo;
//
//}
//int_32 MIF_Adaptor::exportHead( const string & filename )
//{
//	int_32 errinfo;
//	string extName;
//
//	mif_filename = filename;
//
//	if( filename.find('.') != -1 )
//	{
//		extName = filename.substr( filename.rfind('.') + 1 );
//		transform( extName.begin(), extName.end(), extName.begin(), toupper );
//		if( extName != "MIF" ) return ERR_MIFFILE_OPEN_ERR; 
//	}
//	else
//	{
//		mif_filename += ".mif";
//	}
//	fileOpen(m_mif_file,filename.c_str(), ios_base::in | ios_base::out | ios_base::trunc);
//	//m_mif_file.open( filename.c_str(), ios_base::in | ios_base::out | ios_base::trunc);
//	if( !m_mif_file.is_open() ) return ERR_MIFFILE_OPEN_ERR;
//
//
//	errinfo = MIF_IO::storeMIFHead( m_mif_file, mif_struct.mif_header);
//
//	string m_midfile = filename.substr( 0, filename.rfind('.'));
//	m_midfile = m_midfile + ".mid";
//
//	fileOpen(m_mid_file,m_midfile.c_str(),ios_base::in | ios_base::out | ios_base::trunc);
//	//m_mid_file.open( m_midfile.c_str(), ios_base::in | ios_base::out | ios_base::trunc);
//	if( !m_mid_file.is_open() ) return ERR_MIFFILE_OPEN_ERR;
//
//	return errinfo;
//
//
//}
//int_32 MIF_Adaptor::exportData(bool is_end)
//{
//	int_32 errinfo;
//	errinfo = MIF_IO::storeMIFData( m_mif_file, mif_struct.mif_data);
//
//	errinfo = MID_IO::storeMIDFile( m_mid_file,mif_struct.mif_header, mid_struct);
//	
//	if(is_end)
//	{
//		m_mif_file.close();
//		m_mid_file.close();
//	}
//	return errinfo;
//
//}
////int_32 MIF_Adaptor::openPartAndInit( const string &filename, bool *isExistID, long * idcount )
////{
////	int_32 m_errinfo;
////	ifstream ID_file;
////	ifstream MIF_file;
////	string extName;
////	//long idcount ;
////	//bool isExistID = true ;
////
////	mif_filename = filename;
////
////	MIF_file.open( filename.c_str() , ios::in );
////	if( !MIF_file.is_open()) return ERR_MIFFILE_OPEN_ERR;
////
////	string id_file = filename.substr( 0, filename.rfind('.'));
////	id_file = id_file + ".ID";
////	ID_file.open( id_file.c_str(), ios::in );
////	if ( !ID_file.is_open() )
////		*isExistID = false ;
////	else
////	{
////		*isExistID = true ;
////		ID_file.close() ;
////	}
////
////	extName = filename.substr( filename.rfind('.') + 1 );
////	transform(extName.begin(),extName.end(),extName.begin(),toupper);
////	if( extName != "MIF" ) return ERR_MIFFILE_OPEN_ERR; 
////
////	m_errinfo = MIF_IO::FDS_init( MIF_file, mif_struct.mif_header, *isExistID, idcount );
////	MIF_file.close();
////
////	if( m_errinfo ) return m_errinfo;
////
////	//if ( !isExistID )
////	//{
////	//	retids->resize( idcount ) ;
////	//	for ( long i = 1; i<=idcount; i++ )
////	//		retids[i-1] = i ;
////	//}
////
////	return 0;
////}
////---------------使文件和中间数据结构同步------------------
//int_32 MIF_Adaptor::flush( bool file_updata )
//{
//	int_32 m_errinfo;
//	
//	if( file_updata )
//	{
//		ofstream MIF_file;
//		ofstream MID_file;
//
//        fileOpen(MIF_file,mif_filename.c_str(), ios_base::out|ios_base::trunc);
//		//MIF_file.open( mif_filename.c_str(), ios_base::out|ios_base::trunc );
//		if( !MIF_file.is_open() ) return ERR_OPEN_OUTPUT_MIFFILE_ERR;
//
//		MIF_file.clear();
//		m_errinfo = MIF_IO::storeMIFFile( MIF_file, mif_struct );
//		MIF_file.close();
//
//		if( m_errinfo ) return m_errinfo;
//		
//		if( mid_struct.getRecordNum() )
//		{
//			string m_midfile = mif_filename.substr( 0, mif_filename.rfind('.') );
//			m_midfile = m_midfile + ".mid";
//
//			fileOpen(MID_file,m_midfile.c_str(), ios_base::out|ios_base::trunc);
//		//	MID_file.open( m_midfile.c_str(), ios_base::out|ios_base::trunc );
//			if( !MID_file.is_open() ) return ERR_OPEN_OUTPUT_MIDFILE_ERR;
//
//			MID_file.clear();
//			m_errinfo = MID_IO::storeMIDFile( MID_file, mif_struct.mif_header, mid_struct );
//			MID_file.close();
//
//			if( m_errinfo ) return m_errinfo;
//		}
//	}
//	else
//	{
//		clear();
//		m_errinfo = open( mif_filename );
//		if( m_errinfo ) return m_errinfo;
//	}
//	
//	return 0;
//}
//
//int_32 MIF_Adaptor::addToFileEnd( bool file_updata )
//{//it is like flush(),but the mif_struct is the part (just to be added)
//	//and not to clear the original content
//	//int_32 m_errinfo;
//
//	//if( file_updata )
//	//{
//	//	ofstream MIF_file;
//	//	ofstream MID_file;
//
//	//	MIF_file.open( mif_filename.c_str(), ios_base::out|ios_base::ate );
//	//	if( !MIF_file.is_open() ) return ERR_OPEN_OUTPUT_MIFFILE_ERR;
//
//	//	MIF_file.clear();
//	//	//m_errinfo = MIF_IO::storeMIFFile( MIF_file, mif_struct );
//	//	m_errinfo = MIF_IO::storeMIFData( MIF_file, mif_struct.mif_data ) ;
//	//	MIF_file.close();
//
//	//	if( m_errinfo ) return m_errinfo;
//
//	//	if( mid_struct.getRecordNum() )
//	//	{
//	//		string m_midfile = mif_filename.substr( 0, mif_filename.rfind('.') );
//	//		m_midfile = m_midfile + ".mid";
//
//	//		MID_file.open( m_midfile.c_str(), ios_base::out|ios_base::ate );
//	//		if( !MID_file.is_open() ) return ERR_OPEN_OUTPUT_MIDFILE_ERR;
//
//	//		MID_file.clear();
//	//		m_errinfo = MID_IO::storeMIDFile( MID_file, mif_struct.mif_header, mid_struct );
//	//		MID_file.close();
//
//	//		if( m_errinfo ) return m_errinfo;
//	//	}
//	//}
//	////else
//	////{
//	////	clear();
//	////	m_errinfo = open( mif_filename );
//	////	if( m_errinfo ) return m_errinfo;
//	////}
//
//	//return 0;
//	return -1 ;
//}
////--------------得到文件名-------------------
//int_32 MIF_Adaptor::getFileName( string & filename ) const
//{
//	filename = mif_filename;
//	return 0;
//}
//int_32 MIF_Adaptor::setFileName( const string & filename ) 
//{
//	mif_filename = filename ;
//	return 0 ;
//}
//bool MIF_Adaptor::textLayer()
//{
//	return m_text_layer;
//}
//
////----------------把中间数据结构清空---------------------
//int_32 MIF_Adaptor::clear()
//{
//	mif_struct.mif_header.version = 0;
//	mif_struct.mif_header.charset = "";
//	mif_struct.mif_header.delimiter = "";
//	mif_struct.mif_header.column_num = 0;
//
//	mif_struct.mif_header.unique.clear();
//	mif_struct.mif_header.index.clear();
//	mif_struct.mif_header.columns.clear();
//
//	delete mif_struct.mif_header.coord_sys, mif_struct.mif_header.coord_sys = NULL;
//	delete mif_struct.mif_header.trans_form, mif_struct.mif_header.trans_form = NULL;
//
//	mif_struct.mif_header.coordsys_type = NONE_COORDSYS;
//	
//	uint_32 i;
//	for ( i = 0; i <  mif_struct.mif_data.size(); i++ )
//		delete mif_struct.mif_data[i];
//	mif_struct.mif_data.clear();
//
//	mid_struct.clearAllRecord();
//    return 0;
//}
//
//int_32 MIF_Adaptor::getMeta() const
//{
//	return 0;
//}
//
//int_32 MIF_Adaptor::setMeta()
//{
//	return 0;
//}
//
////-------------转化坐标系，待完成-----------------
//int_32 MIF_Adaptor::getCRS( CRS ** crs ) const
//{
//	//CRS_RealWorld * rw_crs = NULL ;
//	CRS *ret = NULL;
//
//	if( mif_struct.mif_header.coordsys_type == MIF_EARTH )
//	{
//		if( mif_struct.mif_header.coord_sys->earth->pro != NULL )
//		{
//			//single_crs =  new CRS_Single ;
//
//			MIF_CS_Pro * project = mif_struct.mif_header.coord_sys->earth->pro ;
//			
//			uint_32 projectType ;
//			//UnitName unit_type;
//			
//			convertType( project->type, projectType ) ;
//			
//			//unit_type = UTC_STD ;
//			
//			//CoordinateSystemType csType = CS_CARTESIAN ;
//			
//			//CD_GeodesicDatum datum ;
//			//datum._ellipsoid = CRS_Factory::createEllipse( (EllipsoidType) datumType ) ;
//			//datum._primeMeridian = CRS_Factory::createMeridian( PMC_GREENWICH ) ;
//
//			if(projectType == PRO_LONGITUDE_LATITUDE)
//			{
//				uint_32 ellipse_type;
//				convertDatum( project->datum, ellipse_type ) ;
//				//CRS_Geography *crs_g = new CRS_Geography;
//				//rw_crs = crs_g;
//				if((EllipsoidType)ellipse_type == ETC_UNKNOWN || ellipse_type >= ETC_LAST)
//					ellipse_type = ETC_DEFAULT;
//				ret = CRS_Factory::createCRS_Geography((EllipsoidType)ellipse_type);
//			}
//			else if (projectType == PRJ_GAUSS || projectType == PRJ_MERCATOR)//todo: may some others
//			{
//				uint_32 crs_type;
//				crs_type = getGeoCrsType(project->datum) ;
//
//				if ((GCSType)crs_type == GCS_UNKNOWN || crs_type >= GCS_CUSTOM)
//				{
//					crs_type = GCS_DEFAULT;
//				}
//
//				CRS_Geography *crsgpy = new CRS_Geography();
//				crsgpy->m_datum = CRS_Factory::createDatum((GCSType)crs_type);			
//
//				ret = CRS_Factory::createCRS_Project(*crsgpy,(PrjType)projectType);
//				delete crsgpy;
//			} 			
//			else
//			{
//				//CRS_Project *crs_p = new CRS_Project;
//				//rw_crs = crs_p;
//				ret = CRS_Factory::createGenericCRS();				
//			}
//
//			//rw_crs->setCoordinateSystem( CRS_Factory::createCoordinateSystem( unit_type, 2, csType ) ) ;
//			//rw_crs->setDatum( datum ) ;
//
//			//CRS_SingleType crss_type ; 
//			//
//			//crss_type = (projectType == PRO_LONGITUDE_LATITUDE) ? 
//			//				CRSS_GEOGRAPHIC : CRSS_GEOCENTRIC ;
//			//single_crs->setSingleCrsType( crss_type );  //类型设为空间坐标
//			//single_crs->setSingleCrsType( CRSS_GEOGRAPHIC );
//			//rw_crs->setGeoCrsType((GCSType)getGeoCrsType(project->datum) );
////			* crs = rw_crs ;
//		}
//
//	}
//	else if( mif_struct.mif_header.coordsys_type == MIF_NONEARTH )
//	{
//		ret = CRS_Factory::createGenericCRS();
//		//single_crs = dynamic_cast<CRS_Single*>(CRS_Factory::createDefaultSingleCrs());
//		//rw_crs = dynamic_cast<CRS_RealWorld*>(CRS_Factory::createSingleCrs(CRSS_PROJECTED));
//		//single_crs->setSingleCrsType( CRSS_PROJECTED );// ? setcrstype or setSingleCrsType  add note by lmt
//
//	}
//
//	if(ret == NULL )
//	{
//		ret = CRS_Factory::createGenericCRS();
//		//rw_crs = dynamic_cast<CRS_RealWorld*>(CRS_Factory::createDefaultSingleCrs());
//		//single_crs->setSingleCrsType( CRSS_GEOGRAPHIC );// ? setcrstype or setSingleCrsType ,add node by lmt
//
//	}
//
//	*crs = ret;
//	return 0;
//}
//
//int_32 MIF_Adaptor::setCRS( CRS * crs , Rect2D &extent1)
//{
//	/*if( crs->getCrsType() == CRS_COORD_NONEARTHSYS )
//	{
//		mif_struct.mif_header.coordsys_type = MIF_NONEARTH;
//		mif_struct.mif_header.version = 300;
//		mif_struct.mif_header.coord_sys = new MIF_CoordSys;
//		mif_struct.mif_header.coord_sys->nonearth = new MIF_CS_Nonearth;
//
//		mif_struct.mif_header.coord_sys->nonearth->unit_name = "m";
//		mif_struct.mif_header.coord_sys->nonearth->bds.p_min.d0 = extent.left_low.d0;
//		mif_struct.mif_header.coord_sys->nonearth->bds.p_min.d1 = extent.left_low.d1;
//		mif_struct.mif_header.coord_sys->nonearth->bds.p_max.d0 = extent.right_top.d0;
//		mif_struct.mif_header.coord_sys->nonearth->bds.p_max.d1 = extent.right_top.d1;
//	}*/
//	CRS_Generic*	generic = NULL; //普通
//	CRS_Geocentric*	geocentric = NULL; //地心
//	CRS_Geography*  geography = NULL; //地理
//	CRS_Project*    project = NULL; // 投影
//
//	if( (generic= dynamic_cast<CRS_Generic*>(crs)) != NULL)
//	{
//		
//		GCSType gcs_type = GCS_UNKNOWN;
//		CRS* temp_crs = generic->getRefCRS();
//		if( temp_crs != NULL )
//		{
//			CRS_Type crs_type = temp_crs->getCrsType();
//			switch(crs_type)
//			{
//			case CRS_GEOGRAPHY:
//				gcs_type = (dynamic_cast<CRS_Geography*>(temp_crs))->m_datum.m_geo_crs_type; // Gcs_Type
//				break;
//			case CRS_GEOCENTRIC:
//				if( temp_crs != NULL )
//					gcs_type = (dynamic_cast<CRS_Geocentric*>(temp_crs))->m_datum.m_geo_crs_type; // Gcs_type
//				break;
//			default:
//				gcs_type = GCS_UNKNOWN;
//				break;
//			}
//		}
//
//		if( gcs_type == GCS_UNKNOWN )
//		{
//			mif_struct.mif_header.coordsys_type = MIF_NONEARTH;
//			mif_struct.mif_header.version = 300;
//			mif_struct.mif_header.coord_sys = new MIF_CoordSys;
//			mif_struct.mif_header.coord_sys->nonearth = new MIF_CS_Nonearth;
//			mif_struct.mif_header.coord_sys->nonearth->unit_name = "m";
//			mif_struct.mif_header.coord_sys->nonearth->bds.p_min.d0 = extent.left_low.d0;
//			mif_struct.mif_header.coord_sys->nonearth->bds.p_min.d1 = extent.left_low.d1;
//			mif_struct.mif_header.coord_sys->nonearth->bds.p_max.d0 = extent.right_top.d0;
//			mif_struct.mif_header.coord_sys->nonearth->bds.p_max.d1 = extent.right_top.d1;
//		}
//		else
//		{
//			mif_struct.mif_header.coordsys_type = MIF_EARTH;
//			mif_struct.mif_header.version = 300;
//			mif_struct.mif_header.charset = "WindowsSimpChinese";
//			mif_struct.mif_header.delimiter = ",";
//			mif_struct.mif_header.coord_sys = new MIF_CoordSys;
//			mif_struct.mif_header.coord_sys->earth = new MIF_CS_Earth;
//			mif_struct.mif_header.coord_sys->earth->pro = new MIF_CS_Pro;
//			mif_struct.mif_header.coord_sys->earth->bds = new MIF_CS_Bounds;
//
//			CRT_Projection* temp_prj = dynamic_cast<CRT_Projection*>(generic->getRefCRT());
//			if(temp_prj == NULL)
//				return -1;
//			int prjtype =(int)temp_prj->prjType();
//			prjtype++;
//			mif_struct.mif_header.coord_sys->earth->pro->type = MIF_protype[prjtype][1];
//
//			mif_struct.mif_header.coord_sys->earth->pro->datum = MIF_crstype[gcs_type][1];
//			mif_struct.mif_header.coord_sys->earth->bds->p_min.d0 = extent.left_low.d0;
//			mif_struct.mif_header.coord_sys->earth->bds->p_min.d1 = extent.left_low.d1;
//			mif_struct.mif_header.coord_sys->earth->bds->p_max.d0 = extent.right_top.d0;
//			mif_struct.mif_header.coord_sys->earth->bds->p_max.d1 = extent.right_top.d1;
//
//
//		}
//
//	}
//	else if ( (geocentric = dynamic_cast<CRS_Geocentric*>(crs)) != NULL )
//	{
//		mif_struct.mif_header.coordsys_type = MIF_EARTH;
//		mif_struct.mif_header.version = 300;
//		mif_struct.mif_header.charset = "WindowsSimpChinese";
//		mif_struct.mif_header.delimiter = ",";
//
//        mif_struct.mif_header.coord_sys = new MIF_CoordSys;
//		mif_struct.mif_header.coord_sys->earth = new MIF_CS_Earth;
//		mif_struct.mif_header.coord_sys->earth->pro = new MIF_CS_Pro;
//		mif_struct.mif_header.coord_sys->earth->bds = new MIF_CS_Bounds;
//
//        mif_struct.mif_header.coord_sys->earth->pro->type = 1;
//
//		int crs_type = (int)geocentric->m_datum.m_geo_crs_type;
//		
//
//        mif_struct.mif_header.coord_sys->earth->pro->datum = MIF_crstype[crs_type][1];
//		mif_struct.mif_header.coord_sys->earth->bds->p_min.d0 = extent.left_low.d0;
//		mif_struct.mif_header.coord_sys->earth->bds->p_min.d1 = extent.left_low.d1;
//		mif_struct.mif_header.coord_sys->earth->bds->p_max.d0 = extent.right_top.d0;
//		mif_struct.mif_header.coord_sys->earth->bds->p_max.d1 = extent.right_top.d1; 
//
//		/*mif_struct.mif_header.coordsys_type = MIF_NONEARTH;
//		mif_struct.mif_header.version = 300;
//		mif_struct.mif_header.coord_sys = new MIF_CoordSys;
//		mif_struct.mif_header.coord_sys->nonearth = new MIF_CS_Nonearth;
//
//		mif_struct.mif_header.coord_sys->nonearth->unit_name = "m";
//		mif_struct.mif_header.coord_sys->nonearth->bds.p_min.d0 = extent.left_low.d0;
//		mif_struct.mif_header.coord_sys->nonearth->bds.p_min.d1 = extent.left_low.d1;
//		mif_struct.mif_header.coord_sys->nonearth->bds.p_max.d0 = extent.right_top.d0;
//		mif_struct.mif_header.coord_sys->nonearth->bds.p_max.d1 = extent.right_top.d1;*/
//	}
//	else if( (project = dynamic_cast<CRS_Project*>(crs)) != NULL)
//	{
//		CRT_Projection* temp_prj = NULL;
//		CRS_Geography* temp_crs = NULL;
//
//		temp_crs =dynamic_cast<CRS_Geography*>(project->getRefCRS());		
//		temp_prj = dynamic_cast<CRT_Projection*>(project->getRefCRT());
//		if(temp_prj == NULL || temp_crs == NULL)
//		{
//			return -1;
//		}
//		CRT_ProjectionMercatorTransverse* temp_pmt = NULL;
//		temp_pmt = dynamic_cast<CRT_ProjectionMercatorTransverse*>(temp_prj);
//		if(temp_pmt != NULL)
//		{
//			mif_struct.mif_header.coordsys_type = MIF_NONEARTH;
//			mif_struct.mif_header.version = 300;
//			mif_struct.mif_header.coord_sys = new MIF_CoordSys;
//			mif_struct.mif_header.coord_sys->nonearth = new MIF_CS_Nonearth;
//
//			mif_struct.mif_header.coord_sys->nonearth->unit_name = "m";
//			mif_struct.mif_header.coord_sys->nonearth->bds.p_min.d0 = extent.left_low.d0;
//			mif_struct.mif_header.coord_sys->nonearth->bds.p_min.d1 = extent.left_low.d1;
//			mif_struct.mif_header.coord_sys->nonearth->bds.p_max.d0 = extent.right_top.d0;
//			mif_struct.mif_header.coord_sys->nonearth->bds.p_max.d1 = extent.right_top.d1;
//			return 0;
//		}
//
//		mif_struct.mif_header.coordsys_type = MIF_EARTH;
//		mif_struct.mif_header.version = 300;
//		mif_struct.mif_header.charset = "WindowsSimpChinese";
//		mif_struct.mif_header.delimiter = ",";
//		mif_struct.mif_header.coord_sys = new MIF_CoordSys;
//		mif_struct.mif_header.coord_sys->earth = new MIF_CS_Earth;
//		mif_struct.mif_header.coord_sys->earth->pro = new MIF_CS_Pro;
//		mif_struct.mif_header.coord_sys->earth->bds = new MIF_CS_Bounds;
//		int prjtype =(int)temp_prj->prjType();
//		prjtype++;
//        
//		mif_struct.mif_header.coord_sys->earth->pro->type = MIF_protype[prjtype][1];
//
//		int crs_type = (int)temp_crs->m_datum.m_geo_crs_type;
//		
//
//		mif_struct.mif_header.coord_sys->earth->pro->datum = MIF_crstype[crs_type][1];
//		mif_struct.mif_header.coord_sys->earth->bds->p_min.d0 = extent.left_low.d0;
//		mif_struct.mif_header.coord_sys->earth->bds->p_min.d1 = extent.left_low.d1;
//		mif_struct.mif_header.coord_sys->earth->bds->p_max.d0 = extent.right_top.d0;
//		mif_struct.mif_header.coord_sys->earth->bds->p_max.d1 = extent.right_top.d1;
//	}
//	else if( (geography = dynamic_cast<CRS_Geography*>(crs)) != NULL)
//	{
//		mif_struct.mif_header.coordsys_type = MIF_EARTH;
//		mif_struct.mif_header.version = 300;
//		mif_struct.mif_header.charset = "WindowsSimpChinese";
//		mif_struct.mif_header.delimiter = ",";
//
//        mif_struct.mif_header.coord_sys = new MIF_CoordSys;
//		mif_struct.mif_header.coord_sys->earth = new MIF_CS_Earth;
//		mif_struct.mif_header.coord_sys->earth->pro = new MIF_CS_Pro;
//		mif_struct.mif_header.coord_sys->earth->bds = new MIF_CS_Bounds;
//
//        mif_struct.mif_header.coord_sys->earth->pro->type = 1;
//
//		int crs_type = (int)geography->m_datum.m_geo_crs_type;
//
//        mif_struct.mif_header.coord_sys->earth->pro->datum = MIF_crstype[crs_type][1];
//		mif_struct.mif_header.coord_sys->earth->bds->p_min.d0 = extent.left_low.d0;
//		mif_struct.mif_header.coord_sys->earth->bds->p_min.d1 = extent.left_low.d1;
//		mif_struct.mif_header.coord_sys->earth->bds->p_max.d0 = extent.right_top.d0;
//		mif_struct.mif_header.coord_sys->earth->bds->p_max.d1 = extent.right_top.d1; 	
//	}
//	return 0;
//}
//
////----------------得到RecordSetImpl中的表头---------------------
//int_32 MIF_Adaptor::getTableHead( ATB_RecordSet *tablehead ) const
//{
//	tablehead->clearAllFieldInfo();
//	tablehead->copyAllFieldInfo( mid_struct );	
////	for( int i = 0; i < mid_struct.getFieldNum(); i++ )
////		tablehead->newFieldInfo( mid_struct.getFieldInfo( i ) );
//
//	return 0;
//}
//
////---------------改变RecordSetImpl中的表头---------------------
//int_32 MIF_Adaptor::setTableHead( const ATB_RecordSet *tablehead )
//{
//	int i;
//	MIF_ColumnUnit mif_column;
//	ATB_Field_Type_enum type;
//	string str;
//	char buffer[128];
//
//	mid_struct.clearAllFieldInfo();
//	mid_struct.copyAllFieldInfo( *tablehead );
//
//	mif_struct.mif_header.columns.clear();
//	mif_struct.mif_header.column_num = mid_struct.getFieldNum();
//
//	for( i = 0; i < mid_struct.getFieldNum(); i++ )
//	{
//		string str_temp( _bstr_t( mid_struct.getFieldInfo(i)->getName()) );
//		mif_column.name = str_temp;
//
//		type = mid_struct.getFieldInfo(i)->getType();
//		switch ( type )
//		{
//		case ATTRIBUTE_FIELD_TYPE_LONG:
//			mif_column.type = "Integer"; 
//			break;
//		
//		case ATTRIBUTE_FIELD_TYPE_DOUBLE:
//			mif_column.type = "Decimal(46,15)"; 
//			break;
//		
//		case ATTRIBUTE_FIELD_TYPE_BIGINT:
//			mif_column.type = "Integer";
//			break;
//		
//		case ATTRIBUTE_FIELD_TYPE_STRING:
//			{
//				itoa( mid_struct.getFieldInfo(i)->getLen(), buffer, 10 );
//				string str( buffer );
//				mif_column.type = "Char(" + str + ")";
//			}
//			break;
//		
//		case ATTRIBUTE_FIELD_TYPE_BLOB:
//#pragma CMPL_MESSAGE(MIF_Adaptor::setTableHead( const ATB_RecordSet *tablehead )未处理的数据类型: BLOB)
//			assert(0);
//			break;
//		case ATTRIBUTE_FIELD_TYPE_UNKNOWN:
//		default:
//			assert(0);
//			break;
//		}
//		mif_struct.mif_header.columns.push_back( mif_column );
//	}
//	return 0;
//}
//
////----------------在中间数据结构中增加一条记录--------------------------
//int_32 MIF_Adaptor::addEntity( GM_Object *gm_obj, const ATB_RecordSet *record )
//{
//	int_32 m_errinfo;
//	MIF_Object * mif_obj;
//
//	if( (int)( mif_struct.mif_data.size() ) == 0 )
//		setTableHead( record );
//
//	m_errinfo = mid_struct.copyRecord( *record, 0 );
//	if ( m_errinfo ) return ERR_ADD_RECORD_ERR;
//
//	m_errinfo = convertMIFObj( gm_obj, &mif_obj );
//	if ( m_errinfo ) return m_errinfo;
//	mif_struct.mif_data.push_back( mif_obj );
//
//	if( mid_struct.getRecordNum() != 0 )
//		mid_file = true;
//	return 0;
//}
//
////-----------------在中间结构中增加多条记录---------------------------
//int_32 MIF_Adaptor::addEntitySet( const vector< GM_Object *> *gm_obj_vec, const ATB_RecordSet *recordset )
//{
//	int_32 m_errinfo;
//	GM_Object *gm_obj;
//	MIF_Object *mif_obj;
//	vector< GM_Object *>::const_iterator gm_iter;
//
//	if( (int)( mif_struct.mif_data.size() ) == 0 )
//		setTableHead( recordset );
//
//	for( gm_iter = gm_obj_vec->begin(); gm_iter != gm_obj_vec->end(); gm_iter++ )
//	{
//		gm_obj = *gm_iter;
//
//		m_errinfo = convertMIFObj( gm_obj, &mif_obj );
//		if( m_errinfo ) return m_errinfo;
//	
//		mif_struct.mif_data.push_back( mif_obj );
//	}
//
//	m_errinfo = mid_struct.copyAllRecord( *recordset );
//	if( m_errinfo == -1 ) return ERR_ADD_RECORD_ERR;
//    return 0;
//}
//
////---------------把中间数据结构中的一条记录改变成传入的值------------------------
//int_32 MIF_Adaptor::modEntity( int index,  GM_Object *gm_obj, const ATB_RecordSet *record )
//{
//	int_32 m_errinfo;
//	MIF_Object * mif_obj = NULL;
//
//	if( index < 0 || index >= (int)mif_struct.mif_data.size() )
//		return ERR_MOD_ENTITY_ERR;
//
//	if( record != NULL )
//	{
//		m_errinfo = mid_struct.updateRecord( index, *record, 0 );
//		if( m_errinfo == -1 ) 
//			return ERR_UPDATA_RECORD_ERR;
//	}
//
//	if ( gm_obj != NULL )
//	{
//		m_errinfo = convertMIFObj( gm_obj, &mif_obj );
//		if( m_errinfo ) 
//			return m_errinfo;
//		mif_struct.mif_data.erase( mif_struct.mif_data.begin() + index );
//		mif_struct.mif_data.insert( mif_struct.mif_data.begin() + index, mif_obj );
//	}
//	return 0;
//}
//
////------------------删除一条中间数据结果的记录------------------------
//int_32 MIF_Adaptor::delEntity( int index )
//{
//	if( index < 0 || index >= (int)mif_struct.mif_data.size() )
//		return ERR_DELETE_ENTITY_ERR;
//
//	int_32 m_errinfo = mid_struct.delRecord( index );
//	//if( m_errinfo == -1 ) 
//	//	return 0;/*return ERR_DELETE_RECORD_ERR;*/ 
//	//edit by zxj 2004-12-03, no record means success then do nothing
//		
//
//	mif_struct.mif_data.erase( mif_struct.mif_data.begin() + index );
//	return 0;
//}
//
////----------------得到数据记录的数量------------------
//int_32 MIF_Adaptor::getEntityNum( long & entity_num ) const 
//{
//	entity_num = (long)mif_struct.mif_data.size();
//	return 0;
//}
//
////-------------转化一个特定中间对象为gm_object，并返回------------------
//int_32 MIF_Adaptor::getEntityGM( int index, GM_Object **gm_obj ) const
//{
//	MIF_Object * mif_obj;
//	int_32 m_errinfo;
//	
//	mif_obj = mif_struct.mif_data[index];
//	m_errinfo = convertGMObj( mif_obj, gm_obj );
//	if ( m_errinfo ) return m_errinfo;
//
//	return 0;
//}
//
////---------------得到某个特定中间对象的mid数据---------------------
//int_32 MIF_Adaptor::getEntityAttr( const int index, ATB_RecordSet *record ) const 
//{
//	int errinfo;
//	
//	getTableHead( record );
//	errinfo = record->clearAllRecord();
//	if ( mid_struct.getRecordNum() == 0 )
//	{
//		return 0 ;	
//	}
//	errinfo = record->copyRecord( mid_struct, index );
//	if( errinfo == -1 ) 
//		return ERR_GET_ENTITY_ATTR_ERR;
//	return 0;
//}
//
////-----------------把所有的mif对象转化为gm_object对象，并返回-----------------
//int_32 MIF_Adaptor::getAllEntityGM( vector< GM_Object *> *gm_obj_vec ) const
//{
//	MIF_Object * mif_obj;
//	
//	uint_32 i;
//	int_32 m_errinfo;
//
//	for ( i = 0; i < mif_struct.mif_data.size(); i++ )
//	{
//		mif_obj = mif_struct.mif_data[i];
//		if(mif_obj->type == MIF_TEXT)
//		{
//			continue;
//		}
//		GM_Object * gm_obj = NULL;
//		m_errinfo = convertGMObj( mif_obj, &gm_obj );
//		if ( m_errinfo ) return m_errinfo;
//
//		if( gm_obj != NULL )
//			gm_obj_vec->push_back( gm_obj );
//		// patch by zzj for multiple pline ---------->>>> called by sack-util's import
//		else if(m_is_last_multiple)
//		{
//			m_multi_part_num[i] = m_current_multi_vector.size();
//			for(vector<GM_Object*>::iterator it = m_current_multi_vector.begin();
//				it != m_current_multi_vector.end(); ++it)
//			{
//				gm_obj_vec->push_back(*it);
//			}
//		}
//		// <<<<<<<------------------------------------------
//	}
//
//	return 0;
//}
//int_32 MIF_Adaptor::getAllEntityGM( const vector< GO_ID > &ids,vector< GM_Object *> *gm_obj_vec ) const
//{
//	MIF_Object * mif_obj;
//	GM_Object * gm_obj;
//	uint_32 i;
//	int_32 m_errinfo;
//
//	for ( i = 0; i < mif_struct.mif_data.size(); i++ )
//	{
//		mif_obj = mif_struct.mif_data[i];
//		m_errinfo = convertGMObj( mif_obj,ids[i], &gm_obj );
//		if ( m_errinfo ) return m_errinfo;
//
//		if( gm_obj != NULL )
//			gm_obj_vec->push_back( gm_obj );
//	}
//	return 0;
//}
//int_32 MIF_Adaptor::getAllEntityGM(const MIF_Struct& in_mif_struct, vector< GM_Object *> *gm_obj_vec) const
//{
//	MIF_Object * mif_obj;
//	
//	uint_32 i;
//	int_32 m_errinfo;
//
//	for ( i = 0; i < in_mif_struct.mif_data.size(); i++ )
//	{
//		mif_obj = in_mif_struct.mif_data[i];
//		//if(mif_obj->type == MIF_TEXT)
//		//{
//		//	continue;
//		//}
//		GM_Object * gm_obj = NULL;
//		m_errinfo = convertGMObj( mif_obj, &gm_obj );
//		if ( m_errinfo ) return m_errinfo;
//
//		if( gm_obj != NULL )
//			gm_obj_vec->push_back( gm_obj );
//		// patch by zzj for multiple pline ---------->>>> called by sack-util's import
//		else if(m_is_last_multiple)
//		{
//			m_multi_part_num[i] = m_current_multi_vector.size();
//			for(vector<GM_Object*>::iterator it = m_current_multi_vector.begin();
//				it != m_current_multi_vector.end(); ++it)
//			{
//				gm_obj_vec->push_back(*it);
//				break;//为了和属性对齐
//			}
//		}
//		// <<<<<<<------------------------------------------
//	}
//	return 0;
//}
//int_32 MIF_Adaptor::getAllEntityText(vector<GeoText*> *text_vec) const
//{
//	GeoText *text = NULL;
//	MIF_Object *mif_obj = NULL;
//	text_vec->clear();
//
//	if(! m_text_layer)
//	{
//		return 0;
//	}
//
//	for (uint_32 i = 0; i < mif_struct.mif_data.size(); i++ )
//	{
//		mif_obj = mif_struct.mif_data[i];
//		if(mif_obj->type == MIF_TEXT)
//		{
//			convertTextObj(mif_obj, &text);
//			if(text)
//			{
//				text_vec->push_back(text);
//			}
//		}
//	}
//
//	return 0;
//}
//
//int_32 MIF_Adaptor::getRangeGM(const GO_ID &start_id, 
//		const GO_ID &end_id, const int_32 &max_pack_size, const vector< GO_ID > &ids,
//		vector < GM_Object * > *retgm, GO_ID *next_id)
//{
//	MIF_Object * mif_obj;
//	GM_Object * gm_obj;
//	uint_32 i, length;
//	int_32 m_errinfo = 0;
//	int_32 count = 0;
//
//	*next_id = MAX_GO_ID;
//
//	vector< GO_ID >::const_iterator start_itr, end_itr;
//	start_itr = lower_bound( ids.begin(), ids.end(), start_id );
//	end_itr = lower_bound( ids.begin(), ids.end(), end_id );
//
//	if( start_itr == ids.end() )
//		return 0;
//	i = start_itr - ids.begin();
//	length = end_itr - ids.begin();
//
//	for ( ; i < mif_struct.mif_data.size() && i < length; i++ )
//	{
//		mif_obj = mif_struct.mif_data[i];
//		m_errinfo = convertGMObj( mif_obj,ids[i], &gm_obj );
//		if ( m_errinfo ) return m_errinfo;
//
//		if( gm_obj != NULL )
//			retgm->push_back( gm_obj );
//		count += dynamic_cast<BufferedObject *>(gm_obj)->size();
//		if( count > max_pack_size )
//		{
//			i++;
//			break;
//		}
//	}
//	if( i<length )
//		*next_id = ids[i];
//
//	return m_errinfo;
//}
//
//uint_32 MIF_Adaptor::getIndexByID(const GO_ID &id, const vector< GO_ID > &ids )
//{
//	//uint_32 i;
//	//i = id - ids[0];
//	////这段代码有错----->
//	//if( i > ids.size() )
//	//	i = ids.size()-1;
//	////<----------------
//	//for( ; i>=0; i--)
//	//{
//	//	if( id == ids[i] )
//	//		break;
//	//}
//	//assert(0);
//#pragma CMPL_MESSAGE(getIndexByID：整段代码实现有错，重写)
//	return 0;
//	//return i;
//}
//
////------------------得到所有的RecordSetImpl中的记录-----------------------
//int_32 MIF_Adaptor::getAllEntityAttr( ATB_RecordSet *recordset ) const
//{
//	int_32 err_info = 0;
//
//	getTableHead( recordset );
//	// patch by zzj: to skip texts
//	//int_32 recordnum = mid_struct.getRecordNum();
//	int_32 entity_sum = mif_struct.mif_data.size();
//	int_32 record_sum = mid_struct.getRecordNum();
//	for(int_32 i = 0; i < entity_sum; i ++)
//	{
//		if(mif_struct.mif_data[i]->type != MIF_TEXT)
//		{
//			if(i < record_sum)
//			{
//				err_info = recordset->copyRecord(mid_struct, i);
//				if(err_info < 0)
//					break;
//			}
//			else
//			{
//				err_info = recordset->copyRecord(mid_struct, 0);	// how to ???
//				if(err_info < 0)
//					break;
//			}
//		}
//	}
//	//m_errinfo =	recordset->copyAllRecord( mid_struct );
//	if ( err_info < 0 ) return ERR_GET_ALLRECORD_ERR;
//
//	return 0;
//}
//
////------------------得到网络数据结构-----------------------
//int_32 MIF_Adaptor::getAllEntityNW( vector< NW_Object * > *obj_vec, ATB_RecordSet *recordset ) const
//{
//
//	int_32 errinfo = 0 ;
//
//	//ATB_RecordSet * recordtemp;0
//	vector< GM_Object * > data_cont ;
//
//	getAllEntityGM( & data_cont );
//	getAllEntityAttr( recordset );
//
//	ConnectCVT cvt ;
//	cvt.init( data_cont ) ;
//	
//	for( vector< GM_Object * >::iterator iter = data_cont.begin() ; 
//		iter != data_cont.end() ; ++ iter )
//		delete *iter ;
//	data_cont.clear() ;
//	
//	errinfo = cvt.getNWObjs( obj_vec ) ;
//
//	return errinfo;
//}
//
//int_32 MIF_Adaptor::getAllEntityTP( vector< TP_Object * > *obj_vec, ATB_RecordSet *recordset ) const
//{
//	int_32 errinfo = 0 ;
//
//	//ATB_RecordSet * recordtemp;0
//	vector< GM_Object * > data_cont ;
//
//	getAllEntityGM( & data_cont );
//	getAllEntityAttr( recordset );
//
//	ConnectCVT cvt ;
//	cvt.init( data_cont ) ;
//
//	for( vector< GM_Object * >::iterator iter = data_cont.begin() ; 
//		iter != data_cont.end() ; ++ iter )
//		delete *iter ;
//	data_cont.clear() ;
//
//	errinfo = cvt.getTPObjs( obj_vec ) ;
//
//	return errinfo;
//}
//
////-------------------把一个gm_object转化为相应的mif_object--------------------
//int_32 MIF_Adaptor::convertMIFObj( const GM_Object * gm_obj, MIF_Object **mif_obj ) 
//{
//	const MEM_LineString_2D * gm_linestr;
//	const MEM_Polygon_2D * gm_polygon;
//	const MEM_Point_2D * gm_point;
//	const MEM_NW_Point_2D * gm_nw_point;
//	const MEM_NW_LineString_2D * gm_nw_linestr;
//	//const MEM_Aggregate_2D * gm_aggre;
//
//	int loop;
//	Pos2D pos;
//	vector< Pos2D > gm_pos_array;
//
//	if(( gm_point = dynamic_cast< const MEM_Point_2D *>( gm_obj ))!= NULL )
//	{
//		MIF_Point *mif_point = new MIF_Point;
//		
//		pos = gm_point->getPos();		
//		point_array.push_back(pos);
//		mif_point->point_array.push_back( pos );
//		*mif_obj = mif_point;
//	}
//
//	else if(( gm_linestr = dynamic_cast< const MEM_LineString_2D * >( gm_obj )) != NULL )
//	{
//		if( gm_linestr->getPointNum() == 2 )
//		{	
//			MIF_Line *mif_line = new MIF_Line;
//
//			gm_linestr->controlPoint( &gm_pos_array);
//			for( loop=0; loop< 2; loop++)
//			{
//				pos =gm_pos_array[loop];
//				point_array.push_back(pos);
//				mif_line->point_array.push_back( pos );
//			}
//
//			*mif_obj = mif_line;
//		}
//
//		else
//		{
//			MIF_Pline * mif_pline = new MIF_Pline;
//			
//			gm_linestr->controlPoint( &gm_pos_array );
//			mif_pline->multi_pos_array.pos_num = (int)gm_pos_array.size();
//			
//			for( loop=0; loop< mif_pline->multi_pos_array.pos_num; loop++ )
//			{
//				pos = gm_pos_array[loop];
//				point_array.push_back(pos);
//				mif_pline->multi_pos_array.point_array.push_back( pos );
//			}
//			
//			*mif_obj = mif_pline;		
//		}
//	}
//
//	//else if(( gm_aggre = dynamic_cast< const MEM_Aggregate_2D *>( gm_obj )) != NULL )
//	//{
//	//	MIF_MulPline * mif_mulpline = new MIF_MulPline;
//
//	//	set< GM_Object *>::iterator pln_iter;
//	//	set< GM_Object *> gm_set;
//	//	MEM_LineString_2D * gm_line;
//	//	MIF_Pline * mif_pline;
//	//	
//	//	gm_aggre->element( &gm_set );
//	//	mif_mulpline->pline_num = (int)gm_set.size();
//
//	//	for( pln_iter = gm_set.begin(); pln_iter != gm_set.end(); pln_iter++ )
//	//	{
//	//		if( ( gm_line = dynamic_cast< MEM_LineString_2D *>( *pln_iter ) ) == NULL )
//	//			return ERR_AGGREGATE_TYPE_ERR;
//
//	//		mif_pline = new MIF_Pline;
//	//			
//	//		gm_line->controlPoint( &gm_pos_array );
//	//		mif_pline->multi_pos_array.pos_num = (int)gm_pos_array.size();
//	//		
//	//		for( loop=0; loop< mif_pline->multi_pos_array.pos_num; loop++ )
//	//		{
//	//			pos = gm_pos_array[loop];
//	//			mif_pline->multi_pos_array.point_array.push_back( pos );
//	//		}
//	//		
//	//		mif_mulpline->pline_set.insert( mif_pline );
//	//	}
//
//	//	*mif_obj = mif_mulpline;
//	//}
//
//	else if(( gm_polygon = dynamic_cast< const MEM_Polygon_2D * >( gm_obj )) != NULL)
//	{
//		MIF_Region * mif_region = NULL ;
//
//		GM_Ring_2D * ex_ring = NULL ;
//		vector< GM_Ring_2D * > in_ring_vec ;
//		vector< GM_Ring_2D * >::iterator it ;
//
//		GM_Boundary_2D *p_boundary = NULL ;
//		MEM_SurfaceBoundary_2D *mem_boundary = NULL ;
//		
//		vector< Pos2D > gm_pos_array;
//		MultiDirectPosition2D multipos;
//		
//		p_boundary = gm_polygon->boundary() ;
//		
//		if ( p_boundary )
//		{
//			mif_region = new MIF_Region ;
//
//			mem_boundary = dynamic_cast < MEM_SurfaceBoundary_2D * > (p_boundary) ;
//
//			mem_boundary->exterior( &ex_ring ) ;
//			ex_ring->controlPoint( &gm_pos_array ) ;
//			multipos.pos_num = (int)gm_pos_array.size();
//
//			point_array.insert(point_array.end(),gm_pos_array.begin(), gm_pos_array.end());
//
//			multipos.point_array.insert( multipos.point_array.end(), gm_pos_array.begin(), gm_pos_array.end() );
//			
//			mif_region->multi_pos_array.push_back( multipos ) ;
//			
//			mem_boundary->interior( &in_ring_vec ) ;
//			mif_region->poly_num = (int)in_ring_vec.size() + 1;
//			for ( it = in_ring_vec.begin(); it != in_ring_vec.end(); it++ )
//			{
//				gm_pos_array.clear() ;
//				multipos.point_array.clear() ;
//
//				(*it)->controlPoint( &gm_pos_array ) ;
//				multipos.pos_num = (int)gm_pos_array.size();
//
//                point_array.insert(point_array.end(),gm_pos_array.begin(), gm_pos_array.end());
//				multipos.point_array.insert( multipos.point_array.end(), gm_pos_array.begin(), gm_pos_array.end() ) ;
//				mif_region->multi_pos_array.push_back( multipos ) ;
//			}
//			*mif_obj = mif_region;
//		}
//		//MIF_Region * mif_region = new MIF_Region;
//		//int in_loop;
//		//Pos2D pos;
//		//MultiDirectPosition2D multipos;
//		//
//		//GM_Ring_2D *ring;
//		//vector< GM_Ring_2D * > *in_ring_vec;
//		//vector< Pos2D > gm_pos_array;
//		//vector< GM_Ring_2D * >::iterator iter;
//
//		//gm_polygon->get_ex_ring()->controlPoint( &gm_pos_array );
//		//multipos.pos_num = (int)gm_pos_array.size();
//		//
//		//for ( in_loop = 0; in_loop < multipos.pos_num; in_loop++ )
//		//{
//		//	pos = gm_pos_array[in_loop];
//		//	multipos.point_array.push_back( pos );
//		//}
//		//mif_region->multi_pos_array.push_back( multipos );
//
//		//in_ring_vec = gm_polygon->get_in_ring_vector();
//		//mif_region->poly_num = (int)in_ring_vec->size() + 1;
//
//		//for( iter = in_ring_vec->begin(); iter != in_ring_vec->end(); iter++ )
//		//{
//		//	ring = *iter;
//		//	gm_pos_array.clear();
//		//	multipos.point_array.clear();
//
//		//	ring->controlPoint( &gm_pos_array );
//		//	multipos.pos_num = (int)gm_pos_array.size();
//
//		//	for( in_loop = 0; in_loop < multipos.pos_num; in_loop++ )
//		//	{
//		//		pos = gm_pos_array[in_loop];
//		//		multipos.point_array.push_back( pos );
//		//	}
//		//	mif_region->multi_pos_array.push_back( multipos );
//
//		//}
//		//*mif_obj = mif_region;
//	}
//	else if(( gm_nw_point = dynamic_cast< const MEM_NW_Point_2D * >( gm_obj )) != NULL)
//	{
//		MIF_Point *mif_point = new MIF_Point;  
//		pos = gm_nw_point->getPos();		
//		point_array.push_back(pos);
//		mif_point->point_array.push_back( pos );		
//		*mif_obj = mif_point;
//
//	}
//	else if(( gm_nw_linestr = dynamic_cast< const MEM_NW_LineString_2D * >( gm_obj )) != NULL)
//	{
//		gm_nw_linestr->controlPoint( &gm_pos_array);
//		if( gm_pos_array.size() == 2 )
//		{	
//			MIF_Line *mif_line = new MIF_Line;
//			
//			for( loop=0; loop< 2; loop++)
//			{
//				pos =gm_pos_array[loop];
//				point_array.push_back(pos);
//				mif_line->point_array.push_back( pos );
//			}
//
//			*mif_obj = mif_line;
//		}
//
//		else
//		{
//			MIF_Pline * mif_pline = new MIF_Pline;
//			mif_pline->multi_pos_array.pos_num = (int)gm_pos_array.size();
//			
//			for( loop=0; loop< mif_pline->multi_pos_array.pos_num; loop++ )
//			{
//				pos = gm_pos_array[loop];
//				point_array.push_back(pos);
//				mif_pline->multi_pos_array.point_array.push_back( pos );
//			}
//			
//			*mif_obj = mif_pline;		
//		}
//
//	}
//
//	else
//		return ERR_ANTICONVERT_OBJECT_ERR;
//	
//	return 0;
//}
//
////-------------------把一个mif_object转化为相应的gm_object------------------------
//int_32 MIF_Adaptor::convertGMObj( MIF_Object * mif_obj, GM_Object **gm_obj ) const
//{
//	Pos2D pos;
//	int loop;
//	MultiDirectPosition2D pos_array;
//	vector< Pos2D > gmpos_v;
//	m_is_last_multiple = false;	// reset before reading next obj
//
//	switch( mif_obj->type ) {
//		case MIF_POINT:
//			{
//				MIF_Point *m_point = ( MIF_Point *)( mif_obj );
//				MEM_Point_2D *gm_point =new MEM_Point_2D;
//	            
//				pos = m_point->point_array[0];
//				gm_point->setPos( pos );
//				
//				*gm_obj = gm_point;
//			}   
//			break;
//		case MIF_LINE:
//			{
//				MIF_Line *m_line = ( MIF_Line * )( mif_obj );
//				MEM_LineString_2D *gm_linestr = new MEM_LineString_2D;
//					
//				pos = m_line->point_array[0];
//				gmpos_v.push_back( pos );
//				
//				pos = m_line->point_array[1];
//				gmpos_v.push_back( pos );
//            
//				if( gm_linestr->create( gmpos_v ) )
//					*gm_obj = gm_linestr;
//				else
//				{
//					*gm_obj = NULL;
//					delete gm_linestr;
//				}
//			}
//			break;
//		case MIF_PLINE:
//			{
//				MIF_Pline *m_pline = ( MIF_Pline * )( mif_obj );
//				
//				MEM_LineString_2D *gm_linestr =new MEM_LineString_2D;
//
//				pos_array = m_pline->multi_pos_array;
//				if( pos_array.pos_num == 1 )
//				{
//					gmpos_v.resize( 2 );
//					gmpos_v[0].d0 = gmpos_v[1].d0 = pos_array.point_array[0].d0;
//					gmpos_v[1].d1 = gmpos_v[1].d1 = pos_array.point_array[1].d1;
//				}
//				else
//				{
//					gmpos_v.resize( pos_array.pos_num );
//					
//					for( loop = 0; loop < pos_array.pos_num; loop++ )
//					{
//						gmpos_v[loop].d0 = pos_array.point_array[loop].d0;
//						gmpos_v[loop].d1 = pos_array.point_array[loop].d1;
//					}
//				}
//
//				if( gm_linestr->create( gmpos_v ) )
//					*gm_obj = gm_linestr;
//				else
//				{
//					*gm_obj = NULL;
//					delete gm_linestr;
//				}
//			}
//			break;
//		case MIF_MULPLINE:
//			{
//				//MIF_MulPline *m_mulpline = ( MIF_MulPline * )( mif_obj );
//				//MEM_Aggregate_2D *gm_aggre = new MEM_Aggregate_2D;
//				//
//				//set< MIF_Pline *>::iterator pln_iter;
//				//set< GM_Object *> gm_set;
//				//MIF_Pline *pln_temp;
//				//MEM_LineString_2D *gm_linestr;
//
//				//for( pln_iter = m_mulpline->pline_set.begin(); pln_iter != 
//				//	m_mulpline->pline_set.end(); pln_iter++ )
//				//{
//				//	pln_temp = *pln_iter;
//				//	pos_array = pln_temp->multi_pos_array;
//				//	gmpos_v.resize( pos_array.pos_num );
//				//	
//				//	for( loop = 0; loop < pos_array.pos_num; loop++ )
//				//	{
//				//		gmpos_v[loop].d0 = pos_array.point_array[loop].d0;
//				//		gmpos_v[loop].d1 = pos_array.point_array[loop].d1;
//				//	}
//
//				//	gm_linestr = new MEM_LineString_2D;
//				//	if( gm_linestr->create( gmpos_v ) )
//				//		gm_set.insert( gm_linestr );
//				//	else
//				//	{
//				//		delete gm_linestr;
//				//	}
//				//}
//				//if( !gm_set.empty() )
//				//{
//				//	gm_aggre->fromSet( gm_set );
//				//	*gm_obj = gm_aggre;
//				//}
//				//else
//				//{
//				//	*gm_obj = NULL;
//				//	delete gm_aggre;
//				//}
//// -------------zzj --------------------------------->>>>>>
//// temp solve for multiple-pline
//				MIF_MulPline *m_mulpline = ( MIF_MulPline * )( mif_obj );
//				m_is_last_multiple = true;
//				m_current_multi_vector.resize(m_mulpline->pline_num);
//				uint_32 i = 0;
//				for( set<MIF_Pline*>::iterator it = m_mulpline->pline_set.begin();
//					it != m_mulpline->pline_set.end(); ++it, ++i)
//				{
//					vector<Pos2D> v_pos;
//					v_pos.resize((*it)->multi_pos_array.pos_num);
//					copy((*it)->multi_pos_array.point_array.begin(), (*it)->multi_pos_array.point_array.end(), v_pos.begin());
//
//					MEM_LineString_2D *ls;
//					ls = new MEM_LineString_2D;
//					ls->create(v_pos);
//					m_current_multi_vector[i] = ls;
//				}
//				*gm_obj = NULL;
//// <<<<<<<<<<-----------------------------------------
//			}
//			break;
//		case MIF_REGION:
//			{
//				MIF_Region *m_region = ( MIF_Region * )( mif_obj );
//
//				int in_loop,out_loop;
//
//				vector< GM_OrientableCurve_2D * > gm_orcurve_v;
//				vector< GM_Ring_2D *> gm_ring_v;
//
//				MEM_Ring_2D *ex_ring;
//				MEM_Ring_2D *gm_ring;
//				MEM_LineString_2D *gm_linestr;
//
//				//外环
//				ex_ring = new MEM_Ring_2D;
//				gm_linestr = new MEM_LineString_2D;
//				MEM_Polygon_2D *gm_polygon = new MEM_Polygon_2D;
//
//				pos_array = m_region->multi_pos_array[0];
//				gmpos_v.resize( pos_array.pos_num );
//
//				for( in_loop = 0; in_loop < pos_array.pos_num; in_loop++ )
//				{
//					gmpos_v[in_loop].d0 = pos_array.point_array[in_loop].d0;
//					gmpos_v[in_loop].d1 = pos_array.point_array[in_loop].d1;
//				}
//				if( gmpos_v[0] != gmpos_v[pos_array.pos_num - 1] )
//					gmpos_v.push_back( gmpos_v[0] );
//				
//				if( !gm_linestr->create( gmpos_v ) )
//				{
//					*gm_obj = NULL;
//					delete ex_ring, ex_ring = NULL;
//					delete gm_linestr, gm_linestr = NULL;
//					delete gm_polygon, gm_polygon = NULL;
//					return 0;
//				}
//				gm_orcurve_v.push_back( gm_linestr );
//				ex_ring->create( gm_orcurve_v);
//
//				//内环
//				for( out_loop= 1; out_loop< m_region->poly_num; out_loop++ )
//				{
//					gm_ring = new MEM_Ring_2D;
//					gm_linestr = new MEM_LineString_2D;
//					
//					gm_orcurve_v.clear();
//
//					pos_array = m_region->multi_pos_array[out_loop];
//					gmpos_v.resize( pos_array.pos_num );
//
//					for( in_loop=0; in_loop< pos_array.pos_num; in_loop++)
//					{
//						gmpos_v[in_loop].d0 = pos_array.point_array[in_loop].d0;
//						gmpos_v[in_loop].d1 = pos_array.point_array[in_loop].d1;
//					}
//					if( gmpos_v[0] != gmpos_v[pos_array.pos_num - 1] )
//						gmpos_v.push_back( gmpos_v[0] );
//
//					if( !gm_linestr->create( gmpos_v ) )
//					{
//						delete gm_ring, gm_ring = NULL;
//						delete gm_linestr, gm_linestr = NULL;
//						continue;
//					}
//					gm_orcurve_v.push_back(gm_linestr );
//					gm_ring->create( gm_orcurve_v );
//					gm_ring_v.push_back( gm_ring );
//				}
//				
//				gm_polygon->create( ex_ring, gm_ring_v);
//				*gm_obj = gm_polygon;
//			}
//			break;
//		case MIF_TEXT:
//			{
//				MIF_Text *m_text = ( MIF_Text *)( mif_obj );
//				MEM_Point_2D *gm_point = new MEM_Point_2D;
//
//				pos = m_text->pos0;
//				gm_point->setPos( pos );
//
//				*gm_obj = gm_point;
//			}
//			break;
//		case MIF_ARC:
//#pragma CMPL_MESSAGE(MIF_Adaptor::convertGMObj()不支持的类型: MIF_ARC)
//		case NO_SUCH_OBJECT:
//		default:
//			assert(0);
//			break;
//	}
//	return 0;
//}
//
//int_32 MIF_Adaptor::convertTextObj(MIF_Object *mif_obj, GeoText **text) const
//{
//	if(mif_obj->type != MIF_TEXT)
//	{
//		assert(mif_obj->type == MIF_TEXT);
//		*text = NULL;
//		return 0;
//	}
//
//	MIF_Text *mif_text = ( MIF_Text *)( mif_obj );
//	BufferedText *b_text = new BufferedText();
//	TextPosBox *t_b = new TextPosBox();
//	t_b->x0 = t_b->x3 = mif_text->pos0.d0;
//	t_b->x1 = t_b->x2 = mif_text->pos1.d0;
//	t_b->y0 = t_b->y1 = mif_text->pos0.d1;
//	t_b->y2 = t_b->y3 = mif_text->pos1.d1;
//	b_text->setPosition(t_b);
//	b_text->setAngle(0);
//	//b_text->setFont( -----> how to get font??
//	wchar_t * value = new wchar_t[128];
//	MultiByteToWideChar( 0, 0, mif_text->text.c_str(), -1, value, 128 * 2 );
//	b_text->setText(value);
//	delete value;
//
//	*text = b_text;
//
//	return 0;
//}
//
//int_32 MIF_Adaptor::convertGMObj( MIF_Object * mif_obj, GO_ID id, GM_Object **gm_obj ) const
//{
//	Pos2D pos;
//	int loop;
//	MultiDirectPosition2D pos_array;
//	vector< Pos2D > gmpos_v;
//
//	switch( mif_obj->type ) {
//		case MIF_POINT:
//			{
//				MIF_Point *m_point = ( MIF_Point *)( mif_obj );
//				MEM_Point_2D *gm_point = new MEM_Point_2D(id);
//
//				pos = m_point->point_array[0];
//				gm_point->setPos( pos );
//
//				*gm_obj = gm_point;
//			}   
//			break;
//		case MIF_LINE:
//			{
//				MIF_Line *m_line = ( MIF_Line * )( mif_obj );
//				MEM_LineString_2D *gm_linestr = new MEM_LineString_2D(id);
//
//				pos = m_line->point_array[0];
//				gmpos_v.push_back( pos );
//
//				pos = m_line->point_array[1];
//				gmpos_v.push_back( pos );
//
//				if( gm_linestr->create( gmpos_v ) )
//					*gm_obj = gm_linestr;
//				else
//				{
//					*gm_obj = NULL;
//					delete gm_linestr;
//				}
//			}
//			break;
//		case MIF_PLINE:
//			{
//				MIF_Pline *m_pline = ( MIF_Pline * )( mif_obj );
//
//				MEM_LineString_2D *gm_linestr = new MEM_LineString_2D(id);
//
//				pos_array = m_pline->multi_pos_array;
//				gmpos_v.resize( pos_array.pos_num );
//
//				for( loop = 0; loop < pos_array.pos_num; loop++ )
//				{
//					gmpos_v[loop].d0 = pos_array.point_array[loop].d0;
//					gmpos_v[loop].d1 = pos_array.point_array[loop].d1;
//				}
//
//				if( gm_linestr->create( gmpos_v ) )
//					*gm_obj = gm_linestr;
//				else
//				{
//					*gm_obj = NULL;
//					delete gm_linestr;
//				}
//			}
//			break;
//		case MIF_MULPLINE:
//			{
//				//MIF_MulPline *m_mulpline = ( MIF_MulPline * )( mif_obj );
//				//MEM_Aggregate_2D *gm_aggre = new MEM_Aggregate_2D(id);
//
//				//set< MIF_Pline *>::iterator pln_iter;
//				//set< GM_Object *> gm_set;
//				//MIF_Pline *pln_temp;
//				//MEM_LineString_2D *gm_linestr;
//
//				//for( pln_iter = m_mulpline->pline_set.begin(); pln_iter != 
//				//	m_mulpline->pline_set.end(); pln_iter++ )
//				//{
//				//	pln_temp = *pln_iter;
//				//	pos_array = pln_temp->multi_pos_array;
//				//	gmpos_v.resize( pos_array.pos_num );
//
//				//	for( loop = 0; loop < pos_array.pos_num; loop++ )
//				//	{
//				//		gmpos_v[loop].d0 = pos_array.point_array[loop].d0;
//				//		gmpos_v[loop].d1 = pos_array.point_array[loop].d1;
//				//	}
//
//				//	gm_linestr = new MEM_LineString_2D;
//				//	if( gm_linestr->create( gmpos_v ) )
//				//		gm_set.insert( gm_linestr );
//				//	else
//				//	{
//				//		delete gm_linestr;
//				//	}
//				//}
//				//if( !gm_set.empty() )
//				//{
//				//	gm_aggre->fromSet( gm_set );
//				//	*gm_obj = gm_aggre;
//				//}
//				//else
//				//{
//				//	*gm_obj = NULL;
//				//	delete gm_aggre;
//				//}
//			}
//			break;
//		case MIF_REGION:
//			{
//				MIF_Region *m_region = ( MIF_Region * )( mif_obj );
//
//				int in_loop,out_loop;
//
//				vector< GM_OrientableCurve_2D * > gm_orcurve_v;
//				vector< GM_Ring_2D *> gm_ring_v;
//
//				MEM_Ring_2D *ex_ring;
//				MEM_Ring_2D *gm_ring;
//				MEM_LineString_2D *gm_linestr;
//
//				//外环
//				ex_ring = new MEM_Ring_2D;
//				gm_linestr = new MEM_LineString_2D;
//				MEM_Polygon_2D *gm_polygon = new MEM_Polygon_2D(id);
//
//				pos_array = m_region->multi_pos_array[0];
//				gmpos_v.resize( pos_array.pos_num );
//
//				for( in_loop = 0; in_loop < pos_array.pos_num; in_loop++ )
//				{
//					gmpos_v[in_loop].d0 = pos_array.point_array[in_loop].d0;
//					gmpos_v[in_loop].d1 = pos_array.point_array[in_loop].d1;
//				}
//				if( gmpos_v[0] != gmpos_v[pos_array.pos_num - 1] )
//					gmpos_v.push_back( gmpos_v[0] );
//
//				if( !gm_linestr->create( gmpos_v ) )
//				{
//					*gm_obj = NULL;
//					delete ex_ring, ex_ring = NULL;
//					delete gm_linestr, gm_linestr = NULL;
//					delete gm_polygon, gm_polygon = NULL;
//					return 0;
//				}
//				gm_orcurve_v.push_back( gm_linestr );
//				ex_ring->create( gm_orcurve_v);
//
//				//内环
//				for( out_loop= 1; out_loop< m_region->poly_num; out_loop++ )
//				{
//					gm_ring = new MEM_Ring_2D;
//					gm_linestr = new MEM_LineString_2D;
//
//					gm_orcurve_v.clear();
//
//					pos_array = m_region->multi_pos_array[out_loop];
//					gmpos_v.resize( pos_array.pos_num );
//
//					for( in_loop=0; in_loop< pos_array.pos_num; in_loop++)
//					{
//						gmpos_v[in_loop].d0 = pos_array.point_array[in_loop].d0;
//						gmpos_v[in_loop].d1 = pos_array.point_array[in_loop].d1;
//					}
//					if( gmpos_v[0] != gmpos_v[pos_array.pos_num - 1] )
//						gmpos_v.push_back( gmpos_v[0] );
//
//					if( !gm_linestr->create( gmpos_v ) )
//					{
//						delete gm_ring, gm_ring = NULL;
//						delete gm_linestr, gm_linestr = NULL;
//						continue;
//					}
//					gm_orcurve_v.push_back(gm_linestr );
//					gm_ring->create( gm_orcurve_v );
//					gm_ring_v.push_back( gm_ring );
//				}
//
//				gm_polygon->create( ex_ring, gm_ring_v);
//				*gm_obj = gm_polygon;
//			}
//			break;
//		case MIF_TEXT:
//			{
//				MIF_Text *m_text = ( MIF_Text *)( mif_obj );
//				MEM_Point_2D *gm_point =new MEM_Point_2D(id);
//
//				pos = m_text->pos0;
//				gm_point->setPos( pos );
//
//				*gm_obj = gm_point;
//			}
//			break;
//		case MIF_ARC:
//#pragma CMPL_MESSAGE(MIF_Adaptor::convertGMObj()不支持的类型: MIF_ARC)
//		case NO_SUCH_OBJECT:
//			assert(0);
//			break;
//	}
//	return 0;
//}
//int_32 MIF_Adaptor::convertDatum( uint_32 mif_datum, uint_32 &datum ) const
//{
//	int i = 0;
//
//	while( MIF_ellipse[i][0] != 0 )
//	{
//		int j = 1;
//
//		while( MIF_ellipse[i][j] != 0 )
//		{
//			if( mif_datum == MIF_ellipse[i][j] )
//			{
//				datum = MIF_ellipse[i][0];
//				return 0;
//			}
//			j++;
//		}
//		i++;
//	}
//	datum = 0;
//	return 0;
//}
//
//int_32 MIF_Adaptor::convertUnit( uint_32 mif_unitname, uint_32 &unitname ) const
//{
//	int i = 0;
//	 
//	while( MIF_unitname[i][0] != 0 )
//	{
//		if( MIF_unitname[i][1] == mif_unitname )
//		{
//			unitname = MIF_unitname[i][0];
//			return 0;
//		}
//		i++;
//	}
//	return 0;
//}
//
//int_32 MIF_Adaptor::convertType( uint_32 mif_protype, uint_32 &protype ) const 
//{
//	int i = 0;
//
//	while( MIF_protype[i][0] != -1 )
//	{
//		if( MIF_protype[i][1] == mif_protype )
//		{
//			protype = MIF_protype[i][0];
//			return 0;
//		}
//		i++;
//	}
//	return 0;
//}
//
//bool MIF_Adaptor::isTextLayer( const MIF_Obj_Vec & mif_data )
//{
//	uint_32 i;
//	MIF_Text *mif_text;
//	for( i = 0; i < mif_data.size(); i++ )
//	{
//		if( ( mif_text = dynamic_cast< MIF_Text *>( mif_data[i] ) ) != NULL )
//			break;
//	}
//	if( i < mif_data.size() )
//		return true;
//	return false;
//}
//
//int_32 MIF_Adaptor::addTextField( const MIF_Obj_Vec & mif_data, ATB_RecordSet *recordset )
//{
//	FieldInfoImpl *fieldinfo=new FieldInfoImpl;
//
//	//const wchar_t *name = wstring(L"TEXTFIELD").c_str();
//	int_32 flag = 0; 
//	int_32 len = 256;
//	ATB_Field_Type_enum type = ATTRIBUTE_FIELD_TYPE_STRING;
//
//	fieldinfo->setdata( (L"TEXTFIELD"), type, flag, len );
//	recordset->newFieldInfo( *fieldinfo );
//
//	uint_32 i;
//	MIF_Text *mif_text;
//	wchar_t * value = new wchar_t[128];
//	for( i = 0; i < mif_data.size(); i++ )
//	{
//		if( ( mif_text = dynamic_cast< MIF_Text *>( mif_data[i] ) ) != NULL )
//		{
//		
//			MultiByteToWideChar( 0, 0, mif_text->text.c_str(), -1, value, 128 * 2 );
//			const wchar_t *temp = value;
//			if( i <= recordset->getRecordNum() )
//				recordset->getRecord(i)->getItem(L"TEXTFIELD")->setStrVal( temp );
//			
//
//		}
//	}
//	delete value;
//
//	return 0;
//}
//
//uint_32 MIF_Adaptor::getGeoCrsType(uint_32 mif_datum) const
//{
//	int i = 0;
//
//	while( MIF_crstype[i][0] != -1 )
//	{
//		if( mif_datum == MIF_crstype[i][1] )
//		{
//			return MIF_crstype[i][0];
//		}
//		i++;
//	}
//	return 0;
//}
//int_32 MIF_Adaptor::setLayer( vector< p_Ref<GM_Object> > &gm_obj_vec, ATB_RecordSet &recordset ) 
//{
//	int_32 m_errinfo;
//	if( (int)( mif_struct.mif_data.size() ) == 0 )
//		setTableHead( &recordset );
//
//	point_array.clear();	
//		
//	for(int i = 0; i < gm_obj_vec.size(); i++)
//	{
//		GM_Object * gm_obj;
//        MIF_Object *mif_obj;
//		gm_obj = gm_obj_vec[i].getPtr();
//		m_errinfo = MIF_Adaptor::convertMIFObj(gm_obj,&mif_obj);
//		if( m_errinfo ) return m_errinfo;
//
//		mif_struct.mif_data.push_back( mif_obj );
//	}
//
//	getExtent(point_array);
//
//	point_array.clear();
//
//	mid_struct.clearAllFieldInfo();
//	mid_struct.copyAllFieldInfo( recordset);
//	m_errinfo = mid_struct.copyAllRecord( recordset );
//	if( m_errinfo == -1 ) return ERR_ADD_RECORD_ERR;
//	return 0; 
//
//}
//int_32 MIF_Adaptor::getExtent(vector<Pos2D> point_array)
//{
//	double x_min, y_min, x_max, y_max;
//
//	x_min = x_max = point_array[0].d0;
//	y_min = y_max = point_array[0].d1;
//
//	uint_32 i;
//	for( i = 1; i < point_array.size(); i++ )
//	{
//		if( point_array[i].d0 < x_min ) 
//			x_min = point_array[i].d0;
//		else if( point_array[i].d0 > x_max )
//			x_max = point_array[i].d0;
//		if( point_array[i].d1 < y_min )
//			y_min = point_array[i].d1;
//		else if( point_array[i].d1 > y_max )
//			y_max = point_array[i].d1;
//	}
//
//	extent.left_low.d0 = x_min;
//	extent.left_low.d1 = y_min;
//	extent.right_top.d0 = x_max;
//	extent.right_top.d1 = y_max;	
//	return 0;	
//}
//int_32 MIF_Adaptor::openHead( const string & filename )	//test, by hqs
//{
//	int_32 m_errinfo;
//	ifstream* p_MID_file = new ifstream();
//	ifstream* p_MIF_file = new ifstream();
//
//    ifstream& MID_file = *p_MID_file;
//	ifstream& MIF_file = *p_MIF_file;
//	string extName;
//
//	if(MID_file.is_open())
//		MID_file.close();
//	if(MIF_file.is_open())
//		MIF_file.close();
//
//	mif_filename = filename;
//	
//	fileOpen(MIF_file,filename.c_str(), ios::in);
//   // MIF_file.open( filename.c_str() , ios::in );
//	if( !MIF_file.is_open()) return ERR_MIFFILE_OPEN_ERR;
//	
//	extName = filename.substr( filename.rfind('.') + 1 );
//	transform(extName.begin(),extName.end(),extName.begin(),toupper);
//	if( extName != "MIF" ) return ERR_MIFFILE_OPEN_ERR; 
//
//	//m_errinfo = MIF_IO::loadMIFFile( MIF_file, mif_struct );
//	//MIF_file.close();
//	//if( m_errinfo ) return m_errinfo;
//	m_errinfo = MIF_IO::loadMIFHead( MIF_file, mif_struct.mif_header);
//	m_gm_cursor = new GMEntityCursor_MIF(MIF_file, this);	
//	if( m_errinfo ) return m_errinfo;
//	
//	string m_midfile = filename.substr( 0, filename.rfind('.'));
//	m_midfile = m_midfile + ".mid";
//
//	fileOpen(MID_file,m_midfile.c_str(), ios::in);
//	//MID_file.open( m_midfile.c_str(), ios::in );
//	if( MID_file.is_open() )
//	{
//		//mid_file = true;
//		//m_errinfo = MID_IO::loadMIDFile( MID_file, mif_struct.mif_header, mid_struct );
//		//MID_file.close();
//
//		//if( m_errinfo ) return m_errinfo;
//
//		m_errinfo = MID_IO::loadMIDHeadFile( MID_file, mif_struct.mif_header, mid_struct );
//		m_attr_cursor = new AttrEntityCursor_MIF(MID_file, this);
//		m_attr_cursor->setMIFHeader(mif_struct.mif_header);
//		m_attr_cursor->setFclsField(mid_struct);
//
//	}
//	else
//	{
//		return ERR_MIFFILE_OPEN_ERR;
//	}
//	if( isTextLayer( mif_struct.mif_data ) )
//	{
//		m_text_layer = true;
//		addTextField( mif_struct.mif_data, &mid_struct );
//	}
//
//	return 0;
//}
//
////openHead() must be called once and only once before 
//GMEntityCursor* MIF_Adaptor::getGMEntityCursor()
//{	
//	return m_gm_cursor;
//}
//
//AttrEntityCursor* MIF_Adaptor::getAttrEntityCursor()	
//{
//	return m_attr_cursor;
//}
//
//} // namespace GIS_Embedded
//
/////////////////////////////////////////////////////////
//#endif // #ifdef CONFIG_HAVE_MIF_FILE_SUPPORT
//
