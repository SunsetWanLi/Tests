///* MIF_Adaptor.h : FileDataSrc -- mif adaptor 
// * 2003-09-27
// */
//
//#ifndef __MIF_ADAPTOR_H__
//#define __MIF_ADAPTOR_H__
//
////#include "FDS_config.h"
//#define CONFIG_HAVE_MIF_FILE_SUPPORT
//#ifdef CONFIG_HAVE_MIF_FILE_SUPPORT
///////////////////////////////////////////////////
//
//#include <string>
//#include <map>
//#include "Retiguide/Common/crs/CRS_Factory.h"
//#include "Retiguide/Common/crs/CRS_Geography.h"
//#include "Retiguide/Common/crs/CRS_Generic.h"
//#include "Retiguide/Common/crs/CRS_Geocentric.h"
//#include "Retiguide/Common/crs/CRS_Project.h"
//#include "Retiguide/Common/crs/CRT_Projection.h"
//#include "Retiguide/Common/crs/CRT_ProjectionMercatorTransverse.h"
//
//
//#include "MIF_StructDef.h"
////#include "FDS_Adaptor_if.h"
//
//using namespace std;
//
//namespace GIS_Embedded
//{
//;
//class GeoText;
//
//class GMEntityCursor_MIF: public GMEntityCursor
//{
//public:
//	GMEntityCursor_MIF(ifstream& file, FDS_Adaptor* adaptor):GMEntityCursor(file, adaptor){}
//	virtual ~GMEntityCursor_MIF(){}
//public:
//	//virtual GM_Object* getGM();	//only 1 element one fetch
//	virtual int_32 getGMs(int max_n, vector<GM_Object*>* gms,vector<int>* gm_type); //return valid_count
//};
//
//class AttrEntityCursor_MIF: public AttrEntityCursor
//{
//public:
//	AttrEntityCursor_MIF(ifstream& file, FDS_Adaptor* adaptor):AttrEntityCursor(file, adaptor), m_rs(0), m_mif_header(0){}
//	virtual ~AttrEntityCursor_MIF(){delete m_rs;delete m_mif_header;}
//public:
//	//virtual int_32 getAttr(ATB_RecordSet* attr);	//only 1 element one fetch
//	virtual int_32 getAttrs(int max_n, ATB_RecordSet* attrs,const vector<int> &gm_type); //return valid_count
//	virtual void setFclsField(const ATB_RecordSet& attr);
//	virtual ATB_RecordSet* getFclsField()const;
//	/*virtual*/ void setMIFHeader(const MIF_Header& header);
//private:
//	ATB_RecordSet* m_rs;
//	MIF_Header* m_mif_header;	
//};
//
//class MIF_Adaptor : public FDS_Adaptor
//{
//public:
//	MIF_Adaptor();
//	virtual ~MIF_Adaptor();
//
//	virtual int_32 open( const string & filename );
//	virtual int_32 export( const string &filename );
//	//只打开头及判断是否有ID文件等，完成初始化
//	//int_32 openPartAndInit( const string &filename,bool *isExistID, long * idcount ) ;
//	virtual int_32 flush( bool file_updata );  
//	
//	virtual int_32 addToFileEnd( bool file_updata ) ;//added by zxj 2004-11-5
//
//	virtual int_32 getFileName( string & filename ) const;
//	virtual int_32 setFileName( const string & filename ) ;
//	virtual bool textLayer();
//
//	virtual int_32 clear();
//	virtual int_32 getMeta() const;
//	virtual int_32 setMeta();
//	virtual int_32 getCRS( CRS ** crs ) const;
//	virtual int_32 setCRS( CRS * crs ,Rect2D &extent1);
//
//	virtual int_32 getTableHead( ATB_RecordSet *tablehead ) const;
//	virtual int_32 setTableHead( const ATB_RecordSet *tablehead );
//	
//	virtual int_32 addEntity( GM_Object *gm_obj, const ATB_RecordSet *record );
//	virtual int_32 addEntitySet( const vector< GM_Object *> *gm_obj_vec, const ATB_RecordSet *record );
//
//	virtual int_32 delEntity( int index );
//	virtual int_32 modEntity( int index,  GM_Object *gm_obj, const ATB_RecordSet *recordset );
//
//	virtual int_32 getEntityNum( long &entity_num ) const ;
//	virtual int_32 getEntityGM( int index, GM_Object **gm_obj ) const;
//	virtual int_32 getEntityAttr( int index, ATB_RecordSet *record ) const;
//	virtual int_32 getAllEntityGM( vector< GM_Object *> *gm_obj_vec ) const;
//	virtual int_32 getAllEntityGM( const vector< GO_ID > &ids,vector< GM_Object *> *gm_obj_vec ) const ;
//	virtual int_32 getAllEntityText(vector< GeoText *> *text_vec) const;
//	virtual int_32 getRangeGM(const GO_ID &start_id, const GO_ID &end_id, const int_32 &max_pack_size, const vector< GO_ID > &ids, vector < GM_Object * >  *retgm, GO_ID *next_id);
//	virtual int_32 getAllEntityAttr( ATB_RecordSet *recordset) const;
//
//	virtual int_32 getPointLayer( vector< GM_Object *> *gm_obj_vec, ATB_RecordSet *recordset ) const { return 0; };
//	virtual int_32 getPlineLayer( vector< GM_Object *> *gm_obj_vec, ATB_RecordSet *recordset ) const { return 0; };
//	virtual int_32 getPolygonLayer( vector< GM_Object *> *gm_obj_vec, ATB_RecordSet *recordset ) const { return 0; };
//
//	virtual int_32 getAllEntityNW( vector< NW_Object *> *nw_obj_vec, ATB_RecordSet *recordset ) const;
//	
//	virtual int_32 getAllEntityTP( vector< TP_Object *> *tp_obj_vec, ATB_RecordSet *recordset ) const;
//
//	virtual int_32 setLayer( vector< p_Ref<GM_Object> > &gm_obj_vec, ATB_RecordSet &recordset );
//
//	virtual int_32 getExtent(vector<Pos2D> point_array);
//
//	
//
//	
//private:
//    int_32 convertMIFObj( const GM_Object * gm_obj, MIF_Object **mif_obj ) ;	
//	int_32 convertGMObj( MIF_Object * mif_obj, GM_Object **gm_obj ) const;
//	int_32 convertTextObj( MIF_Object *mif_obj, GeoText **text) const;
//	int_32 convertGMObj( MIF_Object * mif_obj, GO_ID id, GM_Object **gm_obj ) const;
//
//	int_32 convertDatum( uint_32 mif_datum, uint_32 &datum ) const;
//	int_32 convertUnit(uint_32 mif_unitname, uint_32 &unitname ) const;
//	int_32 convertType(uint_32 mif_protype, uint_32 &protype ) const;
//	uint_32 getGeoCrsType(uint_32 mif_datum) const;
//
//	bool isTextLayer( const MIF_Obj_Vec & mif_data );
//	int_32 addTextField( const MIF_Obj_Vec & mif_data, ATB_RecordSet *recordset  );
//	uint_32 getIndexByID(const GO_ID &id, const vector< GO_ID > &ids );
//
//private:
//	string mif_filename;
//	bool mid_file;
//	bool m_text_layer;
//    MIF_Struct mif_struct;
//	RecordSetImpl mid_struct;
//public:
//	// patch: for multiple pline, invalid when aggregate is supported
//	// zzj, Oct. 20
//
//	// to record how many parts a multi-pline contains
//	mutable map<int_32, int_32> m_multi_part_num;
//	// where the last read multiple obj data is stored
//	mutable vector<GM_Object*> m_current_multi_vector;
//	// whether the last got obj is a multiple
//	mutable bool m_is_last_multiple;
//
//	Rect2D extent;
//	vector< Pos2D > point_array;
//public:
//	virtual int_32 openHead( const string & filename );	
//	//openHead() must be called once and only once before 
//	virtual GMEntityCursor* getGMEntityCursor();	
//	virtual AttrEntityCursor* getAttrEntityCursor();
//	//new method
//	/*virtual */int_32 getAllEntityGM(const MIF_Struct& mif_struct, vector< GM_Object *> *gm_obj_vec) const;
//	/*virtual*/ int_32 getAllEntityAttr(const RecordSetImpl& rs, ATB_RecordSet *recordset) const;
//public:
//	int_32 exportHead( const string & filename );
//	int_32 exportData( bool is_end);
//private:
//	ofstream m_mif_file;
//	ofstream m_mid_file;
//private:
//	GMEntityCursor_MIF* m_gm_cursor;
//	AttrEntityCursor_MIF* m_attr_cursor;
//};
//
//
//}  // namespace GIS_Embedded
//
//////////////////////////////////////////////////////////////
//#endif // CONFIG_HAVE_MIF_FILE_SUPPORT
//
//#endif // __MIF_ADAPTOR_H__
