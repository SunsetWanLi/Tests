/* RecordSetImpl.h : record set implement
 * 2003-07-22
 */

#ifndef __RECORDSETIMPL_H__
#define __RECORDSETIMPL_H__

#include "RecordSet.h"

#include <string>
#include <vector>
using namespace std;

namespace GIS_Embedded
{
//namespace Data
//{

class RecordSetImpl;

class FieldInfoImpl : public ATB_FieldInfo
{
public:
	FieldInfoImpl();
	virtual ~FieldInfoImpl();
	
public: // ATB_FieldInfo
	virtual int_32 setdata( const wchar_t * name, ATB_Field_Type_enum type, int_32 flag, int_32 len );
	virtual const wchar_t * getName() const;
	virtual ATB_Field_Type_enum getType() const;
	virtual int_32 getFlag() const;
	virtual int_32 getLen() const;
	virtual bool operator ==(const ATB_FieldInfo &right) const;

public:
	FieldInfoImpl( const FieldInfoImpl & info );
	FieldInfoImpl & operator = ( const FieldInfoImpl & info );

private:
	std::wstring m_name;
	ATB_Field_Type_enum m_type;
	int_32 m_flag;
	int_32 m_len;		//if type is wchar, m_len is the length of the wchars not bytes!!!!!
};

class FieldItemImpl : public ATB_FieldItem
{
public:
	FieldItemImpl();
	virtual ~FieldItemImpl();
	
	FieldItemImpl( const FieldItemImpl & item );
	FieldItemImpl & operator = ( const FieldItemImpl & item );

public:
	virtual const ATB_FieldInfo * getFieldInfo() const;
	virtual int_32 getLen() const;
	virtual ATB_Field_Type_enum getType()const;

	virtual int_32 setLongVal( long val );
	virtual long getLongVal() const;
	virtual int_32 setDoubleVal( double val );
	virtual double getDoubleVal() const;
	virtual int_32 setStrVal( const wchar_t * val );
	virtual const wchar_t * getStrVal() const;
	virtual int_32	setBigintVal( int_64 val );
	virtual int_64	getBigintVal() const ; 
	virtual int_32 setBlobVal( const byte_8 * blob, int_32 len );
	virtual const byte_8 * getBlobVal() const;
	
public:
	// 如果新的field type和旧的不一样，则清除数据，初始化成缺省值。一样则保留。
	// m_len 长度相关的检查暂略。
	int set_fieldinfo( const ATB_FieldInfo * info );

	// 复制数据，如果类型不一样则出错。
	int_32 copy_from( const ATB_FieldItem * data );

protected:
	void clear_data();

private:
	ATB_Field_Type_enum m_type;
	int_32 m_len;  // data len, expecial for blob.  if type is wchar, m_len is the length of the wchars not bytes!!!!!
	const ATB_FieldInfo * m_info;
	union {
		long vl;
		double vd;
		wchar_t * vs;
		int_64 vi64;
		byte_8 * vb;
	} m_val;
};

class RecordImpl : public ATB_Record
{
public:
	RecordImpl();
	virtual ~RecordImpl();
	
	RecordImpl( const RecordImpl &model );

public: // ATB_Record
	virtual int_32 getFieldNum() const;
	virtual const ATB_FieldInfo * getFieldInfo( int_32 index ) const;
	virtual ATB_FieldItem * getItem( int_32 index ) ;
	virtual ATB_FieldItem * getItem( const wchar_t * name ) ;
	virtual const ATB_FieldItem * getItem( int_32 index ) const ;
	virtual const ATB_FieldItem * getItem( const wchar_t * name ) const ;

public:
	virtual int set_rs( ATB_RecordSet * rs );
	virtual ATB_RecordSet *get_rs() const;
	virtual void assign(const ATB_Record &record);
	virtual void assignWithId(const ATB_Record &record,const int_64 &id);
	int add_field( FieldInfoImpl * field );
	int del_field( int_32 index );

protected:
	std::vector< FieldItemImpl > m_items;
	RecordSetImpl * m_rs;
};

class InfoHeadImpl : public ATB_InfoHead
{
public:
	virtual ~InfoHeadImpl(){}
	virtual int_32 getFieldNum() const;
	virtual const ATB_FieldInfo * getFieldInfo( int_32 index ) const;
	virtual const ATB_FieldInfo * getFieldInfo(const wchar_t *name) const;
	virtual int_32 newFieldInfo( const ATB_FieldInfo & newfield );	
	virtual int_32 delFieldInfo( int_32 index );
	virtual int_32 delFieldInfo(const wchar_t *name);
	virtual void clear();
	virtual int_32 toString(std::vector<byte_8> *v_byte);
	virtual int_32 fromString(std::vector<byte_8> &v_byte);
private:
	std::vector< FieldInfoImpl > m_fieldinfo;
};

class RecordSetImpl : public ATB_RecordSet
{
public:
	RecordSetImpl(void);
	virtual ~RecordSetImpl(void);
	virtual RecordSetImpl & operator = ( const RecordSetImpl & rs );
	virtual RecordSetImpl & operator += ( const RecordSetImpl & rs );

	RecordSetImpl( const RecordSetImpl & rs );

public: // ATB_RecordSet
	virtual int_32 getFieldNum() const;
	virtual const ATB_FieldInfo * getFieldInfo( int_32 index ) const;
	virtual const ATB_FieldInfo * getFieldInfo(const wchar_t *name) const;
	virtual int_32 newFieldInfo( const ATB_FieldInfo & newfield );
	virtual int_32 delFieldInfo( int_32 index );
	virtual int_32 delFieldInfo(const wchar_t *name);

	virtual int_32 getRecordNum() const;
	virtual ATB_Record * getRecord( int_32 index );
	virtual const ATB_Record * getRecord( int_32 index ) const ;
	virtual ATB_Record * newRecord();
	virtual int_32 delRecord( int_32 index );

public: 
	// 如果字段类型一样，则数据保留，否则该列字段的数据被清空。
	virtual int_32 modFieldInfo( int_32 index, const ATB_FieldInfo & newfield );
	// 复制字段信息，原有字段信息保留。如果需要替换，请先调用clear_FieldInfo。
	virtual int_32 copyAllFieldInfo( const ATB_RecordSet & rs );
	virtual int_32 copyAllFieldInfo( const ATB_InfoHead &head);
	virtual int_32 clearAllFieldInfo();
	
	virtual bool cmpFieldInfo( const ATB_RecordSet & rs	);

	// 复制指定字段成为一条新的字段，如果两个字段的fieldinfo的type不一致则失败。
	virtual int_32 copyRecord( const ATB_RecordSet & rs, int_32 rs_index );
	// 更新某字段为指定字段的值，不新建，如果两个字段的fieldinfo的type不一致则失败。
	virtual int_32 updateRecord( int_32 my_index, const ATB_RecordSet & rs, int_32 rs_index );
	// 复制所有记录信息，原有的记录信息保留，如果对应的fieldinfo的name和type不一致则失败
	virtual int_32 copyAllRecord( const ATB_RecordSet & rs );

	// 清空数据
	virtual int_32 clearAllRecord();

	virtual int_32 toString(std::vector<byte_8> *v_byte);
	virtual int_32 fromString(std::vector<byte_8> &v_byte);

public: // easy interface
	int_32 newFieldInfo( const wchar_t * name, ATB_Field_Type_enum type, int_32 flag, int_32 len );
	int_32 modFieldInfo( int_32 index, const wchar_t * name, ATB_Field_Type_enum type, int_32 flag, int_32 len );
	void assignFieldInfo(const vector<ATB_FieldInfo*> &fields);
private:
	std::vector< FieldInfoImpl > m_fieldinfo;
	std::vector< RecordImpl > m_records;
};

//} // Data
} // namespace RetiMap

#endif // __RECORDSETIMPL_H__
