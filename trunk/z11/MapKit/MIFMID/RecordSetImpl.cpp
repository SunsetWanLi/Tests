/* RecordSetImpl.cpp : implement of RecordSet
 * 2003-07-22
 */

//#include "StdAfx.h"
#pragma warning(disable:4996)
#pragma warning(disable:4267)
#pragma warning(disable:4018)

#include <assert.h>

#include "RecordSetImpl.h"
#include "PackMacro.h"
//#include "../Misc/PackMacro.h"
//#include "../common_config.h"
//#include "../Log/LogLib.h"

#if defined(_WIN32) || defined(_WIN64) 

#include <winsock2.h>
#include <windows.h>

#endif

#include <memory.h>

#ifdef _DEBUG
#include <iostream>
#endif 

using namespace std;

namespace GIS_Embedded
{
//namespace Data
//{
////////////////////////////////////////////////////////////////////////////////
////	FieldInfo
FieldInfoImpl::FieldInfoImpl()
{

}

FieldInfoImpl::~FieldInfoImpl()
{

}

FieldInfoImpl::FieldInfoImpl( const FieldInfoImpl & info )
{
	operator = ( info );
}

FieldInfoImpl & FieldInfoImpl::operator = ( const FieldInfoImpl & info )
{
	if ( this == &info )
		return *this;

	m_name = info.m_name;
	m_type = info.m_type;
	m_flag = info.m_flag;
	m_len = info.m_len;
	return *this;
}

bool FieldInfoImpl::operator ==(const ATB_FieldInfo &right) const
{
	const FieldInfoImpl * p_right = dynamic_cast<const FieldInfoImpl *>(&right);
	if(p_right != NULL)
	{
		std::wstring l_name = m_name;
		std::wstring r_name = p_right->getName();

		if( l_name == r_name
			&& m_type == p_right->getType()
			&& m_flag == p_right->getFlag()
			&& m_len == p_right->getLen() )
		{
			return true;
		}
	}

	return false;
}

int_32 FieldInfoImpl::setdata( const wchar_t * name, ATB_Field_Type_enum type, int_32 flag, int_32 len )
{
	m_name = name;
	m_type = type;
	m_flag = flag;

	switch( m_type )
	{
	case ATTRIBUTE_FIELD_TYPE_LONG:
		m_len = sizeof( int_32 );
		break;
	case ATTRIBUTE_FIELD_TYPE_DOUBLE:
		m_len = sizeof( double );
		break;
	case ATTRIBUTE_FIELD_TYPE_STRING:
		m_len = ( (len+1)/2 ) *2;
		break;
	case ATTRIBUTE_FIELD_TYPE_BIGINT:
		m_len = sizeof( int_64 );
		break;
	case ATTRIBUTE_FIELD_TYPE_BLOB :
		m_len = ((len+1)/2)*2;	//to be even
		break;
	case ATTRIBUTE_FIELD_TYPE_UNKNOWN:
	default:
		//assert(0);
		m_len = 0;
		break;
	}

	return 0;
}

const wchar_t * FieldInfoImpl::getName() const
{
	return m_name.c_str();
}

ATB_Field_Type_enum FieldInfoImpl::getType() const
{
	return m_type;
}

int_32 FieldInfoImpl::getFlag() const
{
	return m_flag;
}

int_32 FieldInfoImpl::getLen() const
{
	return m_len;
}

///////////////////////////////////////////////////////////////////////////////
////	FileItem
FieldItemImpl::FieldItemImpl()
{
	m_type = ATTRIBUTE_FIELD_TYPE_UNKNOWN;
	m_len = 0;
	m_info = NULL;
	m_val.vi64 = 0;
}

FieldItemImpl::~FieldItemImpl()
{
	clear_data();
}

void FieldItemImpl::clear_data()
{
	if( m_type == ATTRIBUTE_FIELD_TYPE_STRING )
		delete m_val.vs, m_val.vs = NULL, m_len = 0;
	else if( m_type == ATTRIBUTE_FIELD_TYPE_BLOB )
		delete m_val.vb, m_val.vb = NULL, m_len = 0;
}

FieldItemImpl::FieldItemImpl( const FieldItemImpl & item )
{
	m_type = ATTRIBUTE_FIELD_TYPE_UNKNOWN;
	operator = ( item );
}

FieldItemImpl & FieldItemImpl::operator = ( const FieldItemImpl & item )
{
	if ( this == &item )
		return *this;

	clear_data();

	m_type = item.m_type;
	m_info = item.m_info;
	m_len = item.getLen();

	switch( m_type )
	{
	case ATTRIBUTE_FIELD_TYPE_LONG:
		m_val.vl = item.getLongVal();
		break;
	case ATTRIBUTE_FIELD_TYPE_DOUBLE:
		m_val.vd = item.getDoubleVal();
		break;
	case ATTRIBUTE_FIELD_TYPE_STRING:
		m_val.vs = new wchar_t[ m_len/2 + 1 ];
		if(!m_val.vs)
		{
			//ERROR_LOG(L"RecordSet", L"NULL string field pointer.");
			assert(0);
			break;
		}
		wcscpy( m_val.vs, item.getStrVal() );
		break;
	case ATTRIBUTE_FIELD_TYPE_BIGINT:
		m_val.vi64 = item.getBigintVal();
		break;
	case ATTRIBUTE_FIELD_TYPE_BLOB :
		if ( m_len == 0 )
			m_val.vb = NULL;
		else{
			m_val.vb = new byte_8[ m_len ];
			memcpy( m_val.vb, item.getBlobVal(), m_len );
		}
		break;
	case ATTRIBUTE_FIELD_TYPE_UNKNOWN:
	default:
		m_type = ATTRIBUTE_FIELD_TYPE_UNKNOWN;
		m_len = 0;
		//assert(0);
		break;
	}

	return *this;
}

int_32 FieldItemImpl::copy_from( const ATB_FieldItem * data )
{
	if( data == NULL )//|| m_type != data->getFieldInfo()->getType() )
		return -1;

	clear_data();
	m_len = data->getLen();
	m_type = data->getType(); // should this be written?

	switch( m_type )
	{
	case ATTRIBUTE_FIELD_TYPE_LONG:
		m_val.vl = data->getLongVal();
		m_len = sizeof(long);
		break;
	case ATTRIBUTE_FIELD_TYPE_DOUBLE:
		m_val.vd = data->getDoubleVal();
		m_len = sizeof(double);
		break;
	case ATTRIBUTE_FIELD_TYPE_STRING:
		m_val.vs = new wchar_t[ m_len/2 + 1 ];
		if(m_len == 0)
		{
			wcscpy( m_val.vs, L"");
//			m_len = 
		}
		else
			wcscpy( m_val.vs, data->getStrVal() );
		break;
	case ATTRIBUTE_FIELD_TYPE_BIGINT:
		m_val.vi64 = data->getBigintVal();
		m_len = sizeof(int_64);
		break;
	case ATTRIBUTE_FIELD_TYPE_BLOB :
		if ( m_len == 0 )
			m_val.vb = NULL;
		else{
			m_val.vb = new byte_8[ m_len ];
			memcpy( m_val.vb, data->getBlobVal(), m_len );
		}
		break;
	case ATTRIBUTE_FIELD_TYPE_UNKNOWN:
	default:
		m_type = ATTRIBUTE_FIELD_TYPE_UNKNOWN;
		m_len = 0;
		//assert(0);
		break;
	}
	return 0;
}

int_32 FieldItemImpl::getLen() const
{
	return m_len;
}

ATB_Field_Type_enum FieldItemImpl::getType() const
{
	return m_type;
}

const ATB_FieldInfo * FieldItemImpl::getFieldInfo() const
{
	return m_info;
}

int_32 FieldItemImpl::setLongVal( long val )
{
	if( m_type == ATTRIBUTE_FIELD_TYPE_LONG ) 
	{
		m_val.vl = val;
		return 0;
	}
	return -1; // TODO : how to do error
}

long FieldItemImpl::getLongVal() const
{
	if( m_type == ATTRIBUTE_FIELD_TYPE_LONG )
		return m_val.vl;
	else
		return -1; // TODO : how to do error
}

int_32 FieldItemImpl::setDoubleVal( double val )
{
	if( m_type == ATTRIBUTE_FIELD_TYPE_DOUBLE )
	{
		m_val.vd = val;
		return 0;
	}
	return -1; // TODO : how to do error
}

double FieldItemImpl::getDoubleVal() const
{
	if( m_type == ATTRIBUTE_FIELD_TYPE_DOUBLE )
		return m_val.vd;
	else
		return 0.0; // TODO : how to do error
}

int_32 FieldItemImpl::setStrVal( const wchar_t * val )
{
	wstring temp_str;

	if( m_type == ATTRIBUTE_FIELD_TYPE_STRING )
	{
		if( m_val.vs != NULL )
			delete m_val.vs, m_val.vs = NULL;
		uint_32 len = 0; // zgr: 未初始化为0，则当val == NULL时出错 

		if ( val ==	NULL )
		{
			m_len = 0;
		}
		else
		{
			len = wcslen( val ) * 2;
			temp_str = val;;
		}

		if ( len > m_info->getLen() )
			m_len = m_info->getLen();
		else
			m_len = len;

		m_val.vs = new wchar_t[m_len/2 + 1];
		
		wcscpy( m_val.vs, temp_str.substr(0, m_len/2).c_str() );

		return 0;
	}
	return -1; // TODO : how to do error
}

const wchar_t * FieldItemImpl::getStrVal() const
{
	if( m_type == ATTRIBUTE_FIELD_TYPE_STRING )
		return m_val.vs;
	else 
		return NULL; // TODO : how to do error
}

int_32	FieldItemImpl::setBigintVal( int_64 val )
{
	if( m_type == ATTRIBUTE_FIELD_TYPE_BIGINT )
	{
		m_val.vi64 = val;
		return 0;
	}
	return -1; // TODO : how to do error
}

int_64	FieldItemImpl::getBigintVal() const 
{
	if( m_type == ATTRIBUTE_FIELD_TYPE_BIGINT )
		return m_val.vi64;
	else
		return -1; // TODO : how to do error
}

int_32 FieldItemImpl::setBlobVal( const byte_8 * blob, int_32 len )
{
	if( m_type == ATTRIBUTE_FIELD_TYPE_BLOB )
	{
		if( m_val.vb != NULL )
			delete m_val.vb, m_val.vb = NULL, m_len = 0;

		if ( blob == NULL )
			return 0;

		m_val.vb = new byte_8[len];
		memcpy( m_val.vb, blob, len );
		m_len = len;
		return 0;
	}
	return -1; // TODO : how to do error
}

const byte_8 * FieldItemImpl::getBlobVal() const
{
	if( m_type == ATTRIBUTE_FIELD_TYPE_BLOB )
		return m_val.vb;
	else
		return NULL; // TODO : how to do error
}

int FieldItemImpl::set_fieldinfo( const ATB_FieldInfo * info )
{
	if( info == NULL )
		return -1;

	if( info->getType() == m_type )
	{
		m_info = info;
		return 0;
	}

	clear_data();

	m_info = info;
	m_type = info->getType();
	m_len = info->getLen();
	switch( m_type )
	{
	case ATTRIBUTE_FIELD_TYPE_LONG:
		m_val.vl = 0;
		break;
	case ATTRIBUTE_FIELD_TYPE_DOUBLE:
		m_val.vd = 0.0;
		break;
	case ATTRIBUTE_FIELD_TYPE_STRING:
		m_len = 0;
		m_val.vs = new wchar_t[1];
		m_val.vs[0] = L'\0';
		break;
	case ATTRIBUTE_FIELD_TYPE_BIGINT:
		m_val.vi64 = 0;
		break;
	case ATTRIBUTE_FIELD_TYPE_BLOB :
		m_len = 0;
		m_val.vb = NULL;
		break;
	case ATTRIBUTE_FIELD_TYPE_UNKNOWN:
	default:
		m_type = ATTRIBUTE_FIELD_TYPE_UNKNOWN;
		m_len = 0;
		//ERROR_LOG(L"RecordSet", L"Attribute field type unknown! (" << info->getName() << L")");
		assert(0);
		return -1;
		break;
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
////	Record
RecordImpl::RecordImpl()
{
	m_rs = NULL;
}

RecordImpl::~RecordImpl()
{

}

int_32 RecordImpl::getFieldNum() const
{
	if( m_rs == NULL )
		return 0;
	return m_rs->getFieldNum();
}

const ATB_FieldInfo * RecordImpl::getFieldInfo( int_32 index ) const
{
	if( m_rs == NULL )
	{
		//ERROR_LOG(L"Record", L"Record NOT belong to a recordset.");
		assert(0);
		return 0;
	}
	return m_rs->getFieldInfo( index );
}

ATB_FieldItem * RecordImpl::getItem( int_32 index ) 
{
//	if( m_rs == NULL )
//		assert(0);
	if( index < 0 || index >= (int_32)m_items.size() )
		return NULL;
	return &(m_items[index]);
}

ATB_FieldItem * RecordImpl::getItem( const wchar_t * name ) 
{
//	if( m_rs == NULL )
//		assert(0);
	int i, size;
	size = m_rs->getFieldNum();
	for( i = 0; i < size; i++ )
		if( wcscmp( name, m_rs->getFieldInfo(i)->getName() ) == 0 )
			return &(m_items[i]);
	return NULL;
}

const ATB_FieldItem * RecordImpl::getItem( int_32 index ) const
{
//	if( m_rs == NULL )
//		assert(0);
	if( index < 0 || index >= (int_32)m_items.size() )
		return NULL;
	return &(m_items[index]);
}

const ATB_FieldItem * RecordImpl::getItem( const wchar_t * name ) const 
{
//	if( m_rs == NULL )
//		assert(0);
	int i, size;
	size = m_rs->getFieldNum();
	for( i = 0; i < size; i++ )
		if( wcscmp( name, m_rs->getFieldInfo(i)->getName() ) == 0 )
			return &(m_items[i]);
	return NULL;
}

int RecordImpl::set_rs( ATB_RecordSet * rs )
{
	assert( dynamic_cast<RecordSetImpl*>( rs ) );

	m_rs = dynamic_cast<RecordSetImpl*>( rs );

	int i, size;
	size = m_rs->getFieldNum();
	m_items.resize( size );
	for( i = 0; i < size; i++ )
		m_items[i].set_fieldinfo( m_rs->getFieldInfo( i ) );
	return 0;
}


RecordImpl::RecordImpl( const RecordImpl &model )
{
	m_items.resize( model.getFieldNum() );

	m_rs = model.m_rs;

	for ( uint_32 i = 0; i < m_items.size(); i++ )
	{
		m_items[i] = model.m_items[i];
	}
}

ATB_RecordSet *RecordImpl::get_rs() const
{
	return m_rs;
}

int RecordImpl::add_field( FieldInfoImpl * field )
{
	if( m_rs == NULL )
	{
		//ERROR_LOG(L"Record", L"Record NOT belongs to a recordset.");
		assert(0);
		return -1;
	}
	uint_32 size;
	size = m_items.size();
	m_items.resize( size + 1 );
	m_items[size].set_fieldinfo( field );
	return 0;
}

int RecordImpl::del_field( int_32 index )
{
	if( m_rs == NULL )
	{
		//ERROR_LOG(L"Record", L"Record NOT belongs to a recordset.");
		assert(0);
		return -1;
	}
	if( index < 0 || index >= (int_32)m_items.size() )
		return -1;
	m_items.erase( m_items.begin() + index );
	return 0;
}

void RecordImpl::assign(const ATB_Record &record)
{
	int_32 item_sum = record.getFieldNum();
	if(item_sum == 0)
		return;

	m_items.resize(item_sum);
	for(int_32 i = 0; i < item_sum; i ++)
	{
		m_items[i].copy_from(record.getItem(i));	
	}
}
void RecordImpl::assignWithId(const ATB_Record &record,const int_64 &id)
{
	int_32 item_sum = record.getFieldNum();
	if(item_sum == 0)
		return;

	m_items.resize(item_sum + 1);
	for(int_32 i = 0; i < item_sum; i ++)
	{
		m_items[i].copy_from(record.getItem(i));	
	}
	m_items[item_sum].setBigintVal(id);
}
//////////////////////////////////////////////////////////////////////////////
/////	RecordSet
RecordSetImpl::RecordSetImpl(void)
{
}

RecordSetImpl::~RecordSetImpl(void)
{
}

int_32 RecordSetImpl::getFieldNum() const
{
	return (int_32)m_fieldinfo.size();
}

int_32 RecordSetImpl::newFieldInfo( const ATB_FieldInfo & newfield )
{
	uint_32 i;
//	FieldInfoImpl * info;
//	size = m_fieldinfo.size();
//	m_fieldinfo.resize( size + 1 );
//	info = &(m_fieldinfo[size]);
//	info->setdata( newfield->getName(), newfield->getType(), newfield->getFlag(), newfield->getLen() );
	uint_32	size = m_fieldinfo.size();
	for( i = 0; i < size; i++ )
		if( wcscmp( getFieldInfo(i)->getName(), newfield.getName() ) == 0 )
			return i;

	m_fieldinfo.push_back( dynamic_cast<const FieldInfoImpl &>( newfield ) );
	for( i = 0; i < m_records.size(); i++ )
	{
		// NOTE : we need to initlize all fielditem, for fieldinfo changed!
		// record's add_field() can omit, for we add AT BACK SIDE, set_rs() will do it for us.
		// m_records[i].add_field( info );
		m_records[i].set_rs( this );
	}
	
	return 0;
}

int_32 RecordSetImpl::delFieldInfo( int_32 index )
{
	if( index < 0 || index >= (int_32)m_fieldinfo.size() )
		return -1;
	m_fieldinfo.erase( m_fieldinfo.begin() + index );
	uint_32 i;
	for( i = 0; i < m_records.size(); i++ )
	{
		// NOTE : we need to initlize all fielditem, for fieldinfo changed!, set_rs() can do this.
		// !!! order can not change, first fieldinfo erase, then record erase, at last set_rs()
		m_records[i].del_field( index );
		m_records[i].set_rs( this );
	}
	return 0;
}

int_32 RecordSetImpl::delFieldInfo( const wchar_t *name )
{
	uint_32 i;
	uint_32	size = m_fieldinfo.size();
	for( i = 0; i < size; i++ )
		if( wcscmp( name , getFieldInfo(i)->getName()) == 0 )
			return delFieldInfo(i);
	return 0;
}

const ATB_FieldInfo * RecordSetImpl::getFieldInfo( int_32 index ) const
{
	if( index < 0 || index >= (int_32)m_fieldinfo.size() )
		return NULL;
	return &(m_fieldinfo[index]);
}

const ATB_FieldInfo * RecordSetImpl::getFieldInfo(const wchar_t *name) const
{
	uint_32 i;
	uint_32	size = m_fieldinfo.size();
	for( i = 0; i < size; i++ )
	{
		if( wcscmp( name , getFieldInfo(i)->getName()) == 0 )
		{
			return getFieldInfo(i);
		}
	}
	return NULL;
}

int_32 RecordSetImpl::getRecordNum() const
{
	return (int_32)m_records.size();
}

ATB_Record * RecordSetImpl::getRecord( int_32 index ) 
{
	if( index < 0 || index >= (int_32)m_records.size() )
		return NULL;
	return &(m_records[index]);
}

const ATB_Record * RecordSetImpl::getRecord( int_32 index ) const
{
	if( index < 0 || index >= (int_32)m_records.size() )
		return NULL;
	return &(m_records[index]);
}

ATB_Record * RecordSetImpl::newRecord()
{
	uint_32 size;
	size = m_records.size();
	m_records.resize( size + 1 );
	m_records[size].set_rs( this );
	return &(m_records[size]);
}

int_32 RecordSetImpl::delRecord( int_32 index )
{
	if( index < 0 || index >= (int_32)m_records.size() )
		return -1;
	m_records.erase( m_records.begin() + index );
	return 0;
}

RecordSetImpl::RecordSetImpl( const RecordSetImpl & rs )
{
	m_fieldinfo.resize(0);
	m_records.resize(0);

	copyAllFieldInfo( rs );
	copyAllRecord( rs );
}

RecordSetImpl & RecordSetImpl::operator = ( const RecordSetImpl & rs )
{
	clearAllRecord();
	clearAllFieldInfo();
	copyAllFieldInfo( rs );
	copyAllRecord( rs );
	return *this;
}

RecordSetImpl & RecordSetImpl::operator += ( const RecordSetImpl & rs )
{
	copyAllRecord( rs );
	return *this;
}

// 清空数据
int_32 RecordSetImpl::clearAllFieldInfo()
{
	m_fieldinfo.clear();
	uint_32 i;
	for( i = 0; i < m_records.size(); i++ )
		m_records[i].set_rs( this );
	return 0;	
}

int_32 RecordSetImpl::clearAllRecord()
{
	m_records.clear();
	return 0;
}

// 如果字段类型一样，则数据保留，否则该列字段的数据被清空。
int_32 RecordSetImpl::modFieldInfo( int_32 index, const ATB_FieldInfo & newfield )
{
	if( index < 0 || index >= (int_32)m_fieldinfo.size() )
		return -1;
	m_fieldinfo[index] = dynamic_cast<const FieldInfoImpl &>( newfield );
	uint_32 i;
	for( i = 0; i < m_records.size(); i++ )
	{
		// NOTE : we need to update that field, update all 
		m_records[i].set_rs( this );
	}
	return 0;
}

// 复制字段信息，原有字段信息保留。如果需要替换，请先调用clear_FieldInfo。
int_32 RecordSetImpl::copyAllFieldInfo( const ATB_RecordSet & rs )
{
	int_32 i, fieldnum;
	fieldnum = rs.getFieldNum();
	for( i = 0; i < fieldnum; i++ )
		newFieldInfo( *(rs.getFieldInfo(i)) );
	return 0;
}

int_32 RecordSetImpl::copyAllFieldInfo( const ATB_InfoHead &head)
{
	int_32 i, fieldnum;
	fieldnum = head.getFieldNum();
	for( i = 0; i < fieldnum; i++ )
		newFieldInfo( *(head.getFieldInfo(i)) );
	return 0;
}

// 复制所有记录信息，原有的记录信息保留，如果对应的fieldinfo的name和type不一致则失败
int_32 RecordSetImpl::copyAllRecord( const ATB_RecordSet & rs )
{
	if( cmpFieldInfo( rs ) == false )
		return -1;
	int_32 i, rsnum;
	rsnum = rs.getRecordNum();
	for( i = 0; i < rsnum; i++ )
		copyRecord( rs, i );
	return 0;
}

// 由于record不能够脱离RecordSet独立存在，所以下面的record参数是从某个recordset用getRecord()得到
// 复制指定字段成为一条新的字段，如果两个字段的fieldinfo的type不一致则失败。
int_32 RecordSetImpl::copyRecord( const ATB_RecordSet & rs, int_32 rs_index )
{
	if( cmpFieldInfo( rs ) == false || rs_index < 0 || rs_index >= rs.getRecordNum() )
		return -1;

	int_32 result;
	uint_32 rsnum;
	rsnum = m_records.size();
	m_records.resize( rsnum + 1 );
	m_records[rsnum].set_rs(this);
	if( ( result = updateRecord( (int_32)rsnum, rs, rs_index ) ) < 0 )
		m_records.erase( m_records.begin() + rsnum );
	return result;
}

// 更新某字段为指定字段的值，不新建，如果两个字段的fieldinfo的type不一致则失败。
int_32 RecordSetImpl::updateRecord( int_32 my_index, const ATB_RecordSet & rs, int_32 rs_index )
{
	if( my_index < 0 || my_index >= (int_32)m_records.size() || rs_index < 0
		|| rs_index >= rs.getRecordNum() || cmpFieldInfo( rs ) == false )
		return -1;

	int_32 i;
	for( i = 0; i < getFieldNum(); i++ )
	{
		FieldItemImpl * rii;
		rii = dynamic_cast< FieldItemImpl * >( m_records[my_index].getItem( i ) );
		if( rii == NULL )
			return -1;
		rii->copy_from( rs.getRecord(rs_index)->getItem(i) );
	}
	return 0;
}

bool RecordSetImpl::cmpFieldInfo( const ATB_RecordSet & rs )
{
	if( m_fieldinfo.size() != rs.getFieldNum() )
		return false;
	uint_32 i;
	for( i = 0; i < m_fieldinfo.size(); i++ )
		if( m_fieldinfo[i].getType() != rs.getFieldInfo((long)i)->getType() 
			|| wcscmp( m_fieldinfo[i].getName(), rs.getFieldInfo((long)i)->getName() ) != 0 )
			return false;
	return true;
}

int_32 RecordSetImpl::newFieldInfo( const wchar_t * name, ATB_Field_Type_enum type, int_32 flag, int_32 len )
{
	uint_32 i, size;
	size = m_fieldinfo.size();

	for( i = 0; i < size; i++ )
		if( wcscmp( getFieldInfo(i)->getName(), name) == 0 )
			return i;

	m_fieldinfo.resize( size + 1 );
	m_fieldinfo[size].setdata( name, type, flag, len );
	for( i = 0; i < m_records.size(); i++ )
	{
		// NOTE : we need to initlize all fielditem, for fieldinfo changed!
		// record's add_field() can omit, for we add AT BACK SIDE, set_rs() will do it for us.
		// m_records[i].add_field( info );
		m_records[i].set_rs( this );
	}
	return 0;
}

// 如果字段类型一样，则数据保留，否则该列字段的数据被清空。
int_32 RecordSetImpl::modFieldInfo( int_32 index, const wchar_t * name, ATB_Field_Type_enum type, int_32 flag, int_32 len )
{
	if( index < 0 || index >= (int_32)m_fieldinfo.size() )
		return -1;
	m_fieldinfo[index].setdata( name, type, flag, len );
	uint_32 i;
	for( i = 0; i < m_records.size(); i++ )
	{
		// NOTE : we need to update that field, update all 
		m_records[i].set_rs( this );
	}
	return 0;
}

void RecordSetImpl::assignFieldInfo(const vector<ATB_FieldInfo*> &fields)
{
	clearAllFieldInfo();
	for(vector<ATB_FieldInfo*>::const_iterator it = fields.begin(); it != fields.end(); ++it)
	{
		newFieldInfo(**it);
	}
}

int_32 InfoHeadImpl::getFieldNum() const
{
	return (int_32)m_fieldinfo.size();
}

const ATB_FieldInfo *InfoHeadImpl::getFieldInfo(int index) const
{
	if(index < 0 || index >= m_fieldinfo.size())
		return NULL;
	return &m_fieldinfo[index];
}

const ATB_FieldInfo *InfoHeadImpl::getFieldInfo(const wchar_t *name) const
{
	uint_32 i;
	uint_32	size = m_fieldinfo.size();
	for( i = 0; i < size; i++ )
	{
		if( wcscmp( name , getFieldInfo(i)->getName()) == 0 )
		{
			return getFieldInfo(i);
		}
	}
	return NULL;	
}

int_32 InfoHeadImpl::newFieldInfo(const ATB_FieldInfo &newfield)
{
	uint_32 i;
	uint_32	size = m_fieldinfo.size();
	for( i = 0; i < size; i++ )
		if( wcscmp( getFieldInfo(i)->getName(), newfield.getName() ) == 0 )
			return i;

	m_fieldinfo.push_back( dynamic_cast<const FieldInfoImpl &>( newfield ) );
	
	return 0;

}

int_32 InfoHeadImpl::delFieldInfo(int index)
{
	if( index < 0 || index >= (int_32)m_fieldinfo.size() )
		return -1;
	m_fieldinfo.erase( m_fieldinfo.begin() + index );
	return 0;
}

int_32 InfoHeadImpl::delFieldInfo(const wchar_t *name)
{
	uint_32 i;
	uint_32	size = m_fieldinfo.size();
	for( i = 0; i < size; i++ )
		if( wcscmp( name , getFieldInfo(i)->getName()) == 0 )
			return delFieldInfo(i);
	return 0;
}

void InfoHeadImpl::clear()
{
	m_fieldinfo.clear();
}

// packagesize | field count| {name | type | flag | len}*
int_32 InfoHeadImpl::toString(vector<byte_8> *v_byte)
{
	uint_32 count = 4;	// size of the vector
	count += 4;			// field count
	for(uint_32 i = 0; i < m_fieldinfo.size(); i ++)
	{
		count += 4 // length of name
			+ (wstring(m_fieldinfo[i].getName()).size() + 1) * 2	// name
			+ 4 // type
			+ 4 // flag
			+ 4 // len
			;
	}
	v_byte->resize(count);
	byte_8 *ptr = &v_byte->at(0);
	
	PACK_INT_32(ptr, count);
	PACK_INT_32(ptr, m_fieldinfo.size());
	for(uint_32 i = 0; i < m_fieldinfo.size(); i ++)
	{
		PACK_PTR(ptr, m_fieldinfo[i].getName(), (wstring(m_fieldinfo[i].getName()).size() + 1) * 2);
		PACK_INT_32(ptr, m_fieldinfo[i].getType());
		PACK_INT_32(ptr, m_fieldinfo[i].getFlag());
		PACK_INT_32(ptr, m_fieldinfo[i].getLen());
	}
	return 0;
}

int_32 InfoHeadImpl::fromString(vector<byte_8> &v_byte)
{
	byte_8 *ptr = &v_byte[0];
	if(*(uint_32*)ptr != v_byte.size())
	{
		//ERROR_LOG(L"InfoHead", L"Invalid stream format, header and block size NOT match.");
		assert(0);
		return -1;
	}
	//assert(*(uint_32*)ptr == v_byte.size());
	ptr += 4;

	uint_32 field_count, str_sz, _type, flag, len;
	ATB_Field_Type_enum type;
	UNPACK_INT_32(ptr, field_count);
	m_fieldinfo.resize(field_count);
	vector<wchar_t> temp;
	for(uint_32 i = 0; i < field_count; i ++)
	{
		UNPACK_INT_32(ptr, str_sz);
		temp.resize(str_sz / sizeof(wchar_t));
		UNPACK_PTR(&temp[0], ptr, str_sz);
		UNPACK_INT_32(ptr, _type);	type = (ATB_Field_Type_enum)_type;
		UNPACK_INT_32(ptr, flag);
		UNPACK_INT_32(ptr, len);

		m_fieldinfo[i].setdata(&temp[0], type, flag, len);
	}
	return 0;
}

int_32 RecordSetImpl::toString(std::vector<byte_8> *v_byte)
{
	//uint_32 count = 4;	// size of the vector
	//count += 4;			// field count
	//for(uint_32 i = 0; i < m_fieldinfo.size(); i ++)	// field info
	//{
	//	count += 4 // length of name
	//		+ (wstring(m_fieldinfo[i].getName()).size() + 1) * 2	// name
	//		+ 4 // type
	//		+ 4 // flag
	//		+ 4 // len
	//		;
	//}
	//count += 4;	// record count
	//for(uint_32 i = 0; i < m_records.size(); i ++)
	//{
	//	//switch
	//}


	//v_byte->resize(count);
	//byte_8 *ptr = &v_byte->at(0);
	//
	//PACK_INT_32(ptr, count);
	//PACK_INT_32(ptr, m_fieldinfo.size());
	//for(uint_32 i = 0; i < m_fieldinfo.size(); i ++)
	//{
	//	PACK_PTR(ptr, m_fieldinfo[i].getName(), (wstring(m_fieldinfo[i].getName()).size() + 1) * 2);
	//	PACK_INT_32(ptr, m_fieldinfo[i].getType());
	//	PACK_INT_32(ptr, m_fieldinfo[i].getFlag());
	//	PACK_INT_32(ptr, m_fieldinfo[i].getLen());
	//}
	assert(0);
//#pragma CMPL_MESSAGE(RecordSetImpl::toString()方法未实现)
	// TODO: 
	return 0;	
}

int_32 RecordSetImpl::fromString(std::vector<byte_8> &v_byte)
{
	assert(0);
//#pragma CMPL_MESSAGE(RecordSetImpl::fromString()方法未实现)
	return 0;
}
//} // Data
///////////////////////////////////////////////////////////////////////////
} // namespace IS_GIS
