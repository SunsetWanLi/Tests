/* recordset.h : record set, attribte info 
 * 2003-05-27 
 */

#ifndef __RECORDSET_H__
#define __RECORDSET_H__

#include "Retiguide/Common/Types/basic_types.h"
#include "Retiguide/Common/CRS/basic_gm_type.h"

#include <string>
#include <vector>

namespace GIS_Embedded
{
//namespace Data
//{

enum ATB_Field_Type_enum
{
	ATTRIBUTE_FIELD_TYPE_UNKNOWN,
	ATTRIBUTE_FIELD_TYPE_LONG,
	ATTRIBUTE_FIELD_TYPE_DOUBLE,
	ATTRIBUTE_FILED_TYPE_FLOAT,
	ATTRIBUTE_FIELD_TYPE_BIGINT,
	ATTRIBUTE_FIELD_TYPE_STRING,
	ATTRIBUTE_FIELD_TYPE_BLOB
};

class ATB_FieldInfo
{
public:
	virtual ~ATB_FieldInfo() {}

	// operator == required: only if name, type, flag, len are all identical ==> return true;
	virtual bool operator ==(const ATB_FieldInfo &right) const = 0; 

	virtual int_32 setdata( const wchar_t * name, ATB_Field_Type_enum type, int_32 flag, int_32 len ) = 0;
	virtual const wchar_t * getName() const = 0;
	virtual ATB_Field_Type_enum getType()const = 0;
	virtual int_32 getFlag() const = 0;
	virtual int_32 getLen() const = 0;
};

class ATB_FieldItem
{
public:
	virtual ~ATB_FieldItem() {}

	virtual const ATB_FieldInfo * getFieldInfo() const = 0;
	virtual int_32 getLen() const = 0;
	virtual ATB_Field_Type_enum getType()const = 0;

	virtual int_32 setLongVal( long val ) = 0;
	virtual long getLongVal() const = 0;
	virtual int_32 setDoubleVal( double val ) = 0;
	virtual double getDoubleVal() const = 0;
	virtual int_32 setStrVal( const wchar_t * val ) = 0;
	virtual const wchar_t * getStrVal() const = 0;
	virtual int_32 setBigintVal( int_64 val ) = 0;
	virtual int_64 getBigintVal() const = 0;
	virtual int_32 setBlobVal( const byte_8 * blob, int_32 len ) = 0;
	virtual const byte_8 * getBlobVal() const = 0;
};

class ATB_RecordSet;

class ATB_Record
{
public:
	virtual ~ATB_Record() {} 

	virtual int_32 getFieldNum() const = 0;
	virtual const ATB_FieldInfo * getFieldInfo( int_32 index ) const = 0;
	virtual ATB_FieldItem * getItem( int_32 index ) = 0;
	virtual ATB_FieldItem * getItem( const wchar_t * name ) = 0;
	virtual const ATB_FieldItem * getItem( int_32 index ) const = 0;
	virtual const ATB_FieldItem * getItem( const wchar_t * name ) const = 0;

	virtual int_32 set_rs( ATB_RecordSet * rs ) = 0;
	virtual ATB_RecordSet *get_rs() const = 0;
	virtual void assign(const ATB_Record &record) = 0;
	virtual void assignWithId(const ATB_Record &record,const int_64 &id) = 0; 
};

class ATB_InfoHead
{
public:
	virtual int_32 getFieldNum() const = 0;
	virtual const ATB_FieldInfo * getFieldInfo( int_32 index ) const = 0;
	virtual const ATB_FieldInfo * getFieldInfo(const wchar_t *name) const = 0;
	virtual int_32 newFieldInfo( const ATB_FieldInfo & newfield ) = 0;	
	virtual int_32 delFieldInfo( int_32 index ) = 0;
	virtual int_32 delFieldInfo(const wchar_t *name) = 0;
	virtual void clear() = 0;
	virtual int_32 toString(std::vector<byte_8> *v_byte) = 0;
	virtual int_32 fromString(std::vector<byte_8> &v_byte) = 0;
};

class ATB_RecordSet
{
public:
	virtual ~ATB_RecordSet() {};
//	virtual ATB_RecordSet & operator = ( const ATB_RecordSet & rs ) = 0;
//	virtual ATB_RecordSet & operator += ( const ATB_RecordSet & rs ) = 0;

	virtual int_32 getFieldNum() const = 0;
	virtual const ATB_FieldInfo * getFieldInfo( int_32 index ) const = 0;
	virtual const ATB_FieldInfo * getFieldInfo(const wchar_t *name) const = 0;
	virtual int_32 newFieldInfo( const ATB_FieldInfo & newfield ) = 0;	
	virtual int_32 delFieldInfo( int_32 index ) = 0;
	virtual int_32 delFieldInfo(const wchar_t *name) = 0;

	// ����ֶ�����һ���������ݱ�������������ֶε����ݱ���Ϊ�����͵�ȱʡֵ,ԭֵ������
	virtual int_32 modFieldInfo( int_32 index, const ATB_FieldInfo & newfield ) = 0;
	// �����ֶ���Ϣ��ԭ���ֶ���Ϣ�����������Ҫ�滻�����ȵ���clear_FieldInfo��
	virtual int_32 copyAllFieldInfo( const ATB_RecordSet & rs ) = 0;
	virtual int_32 copyAllFieldInfo( const ATB_InfoHead &head) = 0;
	virtual int_32 clearAllFieldInfo() = 0;

	virtual bool cmpFieldInfo( const ATB_RecordSet & record ) = 0;

	virtual int_32 getRecordNum() const = 0;
	virtual ATB_Record * newRecord() = 0;
	virtual ATB_Record * getRecord( int_32 index ) = 0;
	virtual const ATB_Record * getRecord( int_32 index ) const = 0;
	virtual int_32 delRecord( int_32 index ) = 0;

	// ����ָ���ֶγ�Ϊһ���µ��ֶΣ���������ֶε�fieldinfo��type��һ����ʧ�ܡ�
	virtual int_32 copyRecord( const ATB_RecordSet & rs, int_32 rs_index ) = 0;
	// ����ĳ�ֶ�Ϊָ���ֶε�ֵ�����½�����������ֶε�fieldinfo��type��һ����ʧ�ܡ�
	virtual int_32 updateRecord( int_32 my_index, const ATB_RecordSet & rs, int_32 rs_index ) = 0;
	// �������м�¼��Ϣ��ԭ�еļ�¼��Ϣ�����������Ӧ��fieldinfo��name��type��һ����ʧ��
	virtual int_32 copyAllRecord( const ATB_RecordSet & rs ) = 0;
	virtual int_32 clearAllRecord() = 0;

	virtual int_32 toString(std::vector<byte_8> *v_byte) = 0;
	virtual int_32 fromString(std::vector<byte_8> &v_byte) = 0;
};

//} // namespace Data
} // namespace Retimap

#endif // __RECORDSET_H__
