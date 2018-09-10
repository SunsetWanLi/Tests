

#include "CRS.h"
#include <assert.h>

namespace GIS_Embedded{

//CRS_Single::CRS_Single(CRS_Single& crs)
//{
//	this->_crsType = crs._crsType;
//	this->_strName = crs._strName;
//	this->_crsType = crs._crsType;
//	this->_cordinateSystem = crs._cordinateSystem;
//	this->_pDatum = crs._pDatum->clone();
//	//this->SetDatum(crs._pDatum);
//}

////////////////////////////////////////////////////////////////////////////
////	CoordinateSystemAxis

////////////////////////////////////////////////////////////////////////////////////
///CRS
//void CRS_Single::setSingleCrsType(CRS_SingleType crsSingleType)
//{
//	_crsSingleType = crsSingleType;
//}

//CRS_SingleType CRS_Single::getSingleCrsType() const
//{
//	return _crsSingleType;
//}

//void CRS_Single::setCoordinateSystem( const CoordinateSystem & refCs)
//{
//	_refCs = refCs;
//}
//
//CoordinateSystem CRS_Single::getCoordinateSystem() const
//{
//	return _refCs;
//}
//
//void CRS_RealWorld::setDatum( const CD_GeodesicDatum & datum )
//{
//	_datum = datum;
//}
//
//CD_GeodesicDatum CRS_RealWorld::getDatum( ) const
//{
//	return _datum;
//}
//
////void CRS_Single::setCrsType( CRS_Type crsType )
////{
////	_crsType = crsType ;
////}
//
////CRS_Type CRS_Single::getCrsType() const
////{
////	return _crsType ;
////}
//
//void CRS_RealWorld::setGeoCrsType(GCSType geoCrsType)
//{
//	_geoCrsType = geoCrsType;
//}
//
//GCSType CRS_RealWorld::getGeoCrsType() const
//{
//	return _geoCrsType;
//}

// from packable
//int_32 CRS_Single::packSize(const void *how_to ) const
//{
//	int_32 rt = 0 ;
//
//	if( !how_to )
//	{
//		rt += sizeof(PackHead) ;
//		rt += sizeof(uint_32) ; // how_to
//		rt += sizeof(int_32) * 3 ;
//		rt += sizeof(CD_GeodesicDatum) ;
//		rt += sizeof(CoordinateSystem) ;
//		rt += sizeof(uint_32 ) ;
//	}
//
//	return rt ;
//}
//
//int_32 CRS_Single::pack(byte_8 *buffer, int_32 buffer_size,
//						byte_8 **end_pos, const void *how_to ) const
//{
//	return 0;
	//int_32 sz = packSize( how_to ) ;

	//if ( sz > buffer_size )
	//	return  -1 ; // TODO

	//if( how_to == 0 )
	//{
	//	byte_8 * current = buffer ;
	//	PackHead * ph = ( PackHead * ) current ;

	//	ph->byte_order = SYS_ENDIAN_TAG;
	//	ph->is_segment = false;
	//	ph->size = sz;
	//	ph->type = PTC_CRS_SINGLE ;
	//	current += sizeof ( PackHead ) ;

	//	// how to
	//	* ( uint_32 * ) current = 0 ;
	//	current += sizeof ( uint_32 ) ;
	//
	//	*(int_32 * ) current = _crsType ;
	//	current += sizeof ( int_32 ) ;

	//	*(int_32 * ) current = _crsSingleType ;
	//	current += sizeof ( int_32 ) ;

	//	*(int_32 * ) current = _geoCrsType ;
	//	current += sizeof ( int_32 ) ;

	//	memcpy(current, &_datum, sizeof(CD_GeodesicDatum) ) ;
	//	current += sizeof(CD_GeodesicDatum) ;

	//	memcpy(current, &_refCs, sizeof(CoordinateSystem) ) ;
	//	current += sizeof(CoordinateSystem) ;
	//
	//	*(int_32 *)current = sz ;
	//	current += sizeof(int_32) ;
	//	*end_pos = current ;
	//}
	//else
	//{
	//	assert( 0 ) ;
	//}

	//return sz ;
//}

//int_32 CRS_Single::pack(byte_8 **result, const void *how_to )const
//{
//	int sz = packSize( how_to );
//	byte_8 *end_pos = NULL;
//	*result = new byte_8[sz];
//	return pack(*result, sz, &end_pos, how_to );
//}
//
//int_32 CRS_Single::unpack(const byte_8 *buffer, int_32 buf_size )
//{
//	return 0;
//
	//if (  !packageSizeMatch(buffer, true) )
	//	return -1 ; // TODO

	//int_32 ret = 0 ;
	//const PackHead *ph;

	//// check the package
	//const byte_8 * current = buffer ;
	//ph = ( PackHead * ) current ;

	//current += sizeof ( PackHead ) ;
	//if(ph->byte_order == SYS_ENDIAN_TAG) // small end for i386
	//{
	//	ret = ph->size ;
	//	if( ret > buf_size || ph->type != PTC_CRS_SINGLE )//
	//	{
	//		assert(0);
	//		return -1 ;	// bad package
	//	}
	//	if( *(const uint_32 *)current == 0)
	//		// use default unpack
	//	{
	//		current += sizeof ( uint_32 ) ;
	//
	//		setCrsType( (CRS_Type)(*(int *) current) ) ;
	//		current += sizeof(int) ;
	//
	//		setSingleCrsType( (CRS_SingleType)(*(int *) current ));
	//		current += sizeof(int) ;

	//		setGeoCrsType( (GCSType)(*(int *) current ));
	//		current += sizeof(int) ;

	//		setDatum( *(CD_GeodesicDatum *) current ) ;
	//		current += sizeof(CD_GeodesicDatum) ;

	//		setCoordinateSystem( *( CoordinateSystem * )current ) ;
	//		current += sizeof( CoordinateSystem ) ;
	//	}

	//}
	//else if((ph->byte_order ^ SYS_ENDIAN_TAG) == 0xff)
	//	// every field should reverse its uint_8 order
	//{
	//	assert(0);
	//	// to be continued...
	//	ret = -1;
	//}
	//else
	//{
	//	ret = -1;	// bad package
	//}

	//return ret ;

//}

//CRS * CRS_Single::copy() const
//{
//	CRS_Single * ret = new CRS_Single;
//
//	//ret->setCrsType( this->getCrsType() );
//	ret->setSingleCrsType( this->getSingleCrsType() );
//	ret->setGeoCrsType(this->getGeoCrsType() );
//	ret->setCoordinateSystem( this->getCoordinateSystem() );
//	ret->setDatum( this->getDatum() );
//
//	return ret;
//}

//bool CRS_Single::operator ==( const CRS& crs) const
//{
//	const CRS_Single* crs_s = dynamic_cast< const CRS_Single*> (&crs);
//
//	if(crs_s == NULL)
//		return false;
//
//	return //this->_crsType == crs_s->_crsType && this->_crsSingleType == crs_s->_crsSingleType
//		this->getSingleCrsType() == crs_s->getSingleCrsType()
//		&& this->_geoCrsType == crs_s->_geoCrsType && this->_datum == crs_s->_datum
//		&& this->_refCs == crs_s->_refCs;
//}
//
/////////////////////////////////////////////////////////////////////////////
//int_32 CRS_RealWorld::packPart(byte_8 *buffer, int_32 buffer_size, int_32 pack_type_code, const void *how_to) const
//{
//	int_32 sz = dynamic_cast<const Packable*>(this)->packSize( how_to ) ;
//
//	if ( sz > buffer_size )
//		return  -1 ; // TODO
//
//	byte_8 * current = buffer ;
//
//	if( how_to == 0 )
//	{
//		PackHead * ph = ( PackHead * ) current ;
//
//		ph->byte_order = SYS_ENDIAN_TAG;
//		ph->is_segment = false;
//		ph->size = sz;
//		ph->type = pack_type_code;
//		current += sizeof ( PackHead ) ;
//
//		// how to
//		* ( uint_32 * ) current = 0 ;
//		current += sizeof ( uint_32 ) ;
//
//		*(int_32 * ) current = getCrsType() ;
//		current += sizeof ( int_32 ) ;
//
//		*(int_32 * ) current = getSingleCrsType() ;
//		current += sizeof ( int_32 ) ;
//
//		*(int_32 * ) current = getGeoCrsType() ;
//		current += sizeof ( int_32 ) ;
//
//		memcpy(current, &_datum, sizeof(CD_GeodesicDatum) ) ;
//		current += sizeof(CD_GeodesicDatum) ;
//
//		memcpy(current, &_refCs, sizeof(CoordinateSystem) ) ;
//		current += sizeof(CoordinateSystem) ;
//
////		*(int_32 *)current = sz ;
////		current += sizeof(int_32) ;
////		*end_pos = current ;
//	}
//	else
//	{
//		assert( 0 ) ;
//	}
//
//	return current - buffer;
//}
//
//int_32 CRS_RealWorld::packPartSize(const void *how_to) const
//{
//	if(how_to)
//	{
//		assert(0);
//		return -1;	// TODO
//	}
//	else
//	{
//		return sizeof(PackHead)
//			+ sizeof (uint_32 )	// how_to
//			+ sizeof (int_32)	// crs type, simple or compound, in int_32
//			+ sizeof (int_32)	// single crs type, in int_32
//			+ sizeof (int_32)	// geo crs type, in int_32
//			+ sizeof (CD_GeodesicDatum)
//			+ sizeof (CoordinateSystem);
//	}
//}
//
//int_32 CRS_RealWorld::unpackPart(const byte_8 *buffer, int_32 pack_type_code)//, int_32 buf_size);
//{
//	int_32 ret = 0 ;
//	const PackHead *ph;
//
//	// check the package
//	const byte_8 * current = buffer ;
//	ph = ( PackHead * ) current ;
//	current += sizeof ( PackHead ) ;
//
//	if(ph->byte_order == SYS_ENDIAN_TAG) // small end for i386
//	{
//		//ret = ph->size ;
//		//if( ret > buf_size || ph->type != PTC_CRS_SINGLE )//
//		if(ph->type != pack_type_code)
//		{
//			assert(0);
//			return -1 ;	// bad package
//		}
//		if( *(const uint_32 *)current == 0)
//			// use default unpack
//		{
//			current += sizeof ( uint_32 ) ; // how_to
//
//			//setCrsType( (CRS_Type)(*(int *) current) ) ;
//			current += sizeof(int) ;	// crs_type
//
//			//setSingleCrsType( (CRS_SingleType)(*(int *) current ));
//			current += sizeof(int) ;	// single_crs_type
//
//			setGeoCrsType( (GCSType)(*(int *) current ));
//			current += sizeof(int) ;
//
//			setDatum( *(CD_GeodesicDatum *) current ) ;
//			current += sizeof(CD_GeodesicDatum) ;
//
//			setCoordinateSystem( *( CoordinateSystem * )current ) ;
//			current += sizeof( CoordinateSystem ) ;
//		}
//
//	}
//	else if((ph->byte_order ^ SYS_ENDIAN_TAG) == 0xff)
//		// every field should reverse its uint_8 order
//	{
//		assert(0);
//		// to be continued...
//		ret = -1;
//	}
//	else
//	{
//		ret = -1;	// bad package
//	}
//
//	return current - buffer;
//	//return ret ;
//}
//
//void CRS_Project::setOriginPos(GIS_Embedded::Pos2D originPos)
//{
//	_originPos = originPos;
//}
//
//GIS_Embedded::Pos2D CRS_Project::getOriginPos() const
//{
//	return _originPos;
//}
//
//CRS* CRS_Project::copy() const
//{
//	CRS_Project* crs = new CRS_Project;
//
//	crs->_projectType = this->_projectType;
//	crs->_originPos = this->_originPos;
//	//crs->setCrsType( this->getCrsType() );
//	//crs->setSingleCrsType( this->getSingleCrsType() );
//	crs->setGeoCrsType(this->getGeoCrsType() );
//	crs->setCoordinateSystem( this->getCoordinateSystem() );
//	crs->setDatum( this->getDatum() );
//
//	return crs;
//}
//
//bool CRS_Project::operator ==( const CRS& crs) const
//{
//	const CRS_Project* crs_p = dynamic_cast< const CRS_Project*> (&crs);
//
//	if(crs_p == NULL)
//		return false;
//
//	return this->_projectType == crs_p->_projectType && this->_originPos == crs_p->_originPos;
//}
//
//int_32 CRS_Project::packSize(const void *how_to) const
//{
//	if(how_to)
//	{
//		assert(0);
//		return -1;	//TODO
//	}
//
//	return CRS_RealWorld::packPartSize(how_to)
//		+ sizeof(int_32)	// prj type, in int_32
//		+ sizeof(Pos2D)		// origin pos
//		+ sizeof(int_32);	// end check
//}
//
//int_32 CRS_Project::pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to) const
//{
//	byte_8 *current = buffer;
//	int_32 temp = 0;
//	int_32 size = 0;
//
//	assert(how_to == 0);
//
//	if(buffer_size < (size = packSize(how_to)))
//	{
//		return -1;
//	}
//
//	current += packPart(current, buffer_size, PTC_CRS_PROJECT, how_to);
//
//	*(int_32 *)current = _projectType;
//	current += sizeof(int_32);
//
//	*(Pos2D *)current = _originPos;
//	current += sizeof(Pos2D);
//
//	*(int_32 *)current = size;
//	current += sizeof(int_32);
//
//	*end_pos = current;
//
//	return size;
//}
//
//int_32 CRS_Project::pack(byte_8 **result, const void *how_to) const
//{
//	int_32 size = packSize();
//	byte_8 *next_end = NULL;
//
//	*result = new byte_8[size];
//	return pack(*result, size, &next_end, how_to);
//}
//
//int_32 CRS_Project::unpack(const byte_8 *buffer, int_32 buf_size)
//{
//	if (  !packageSizeMatch(buffer, true) )
//		return -1 ; // TODO
//
//	const byte_8 *current = buffer;
//	current += CRS_RealWorld::unpackPart(current, PTC_CRS_PROJECT);
//
//	_projectType = (PrjType)*(int *)(current);
//	current += sizeof(int_32);
//
//	_originPos = *(Pos2D*)(current);
//	current += sizeof(Pos2D);
//
//	current += sizeof(int_32);	// end check
//
//	return current - buffer;
//}
//
//CRS * CRS_Geography::copy() const
//{
//	CRS_Geography * ret = new CRS_Geography;
//
//	//ret->setCrsType( this->getCrsType() );
//	//ret->setSingleCrsType( this->getSingleCrsType() );
//	ret->setGeoCrsType(this->getGeoCrsType() );
//	ret->setCoordinateSystem( this->getCoordinateSystem() );
//	ret->setDatum( this->getDatum() );
//
//	return ret;
//}
//
//bool CRS_Geography::operator == (const CRS &crs) const
//{
//	const CRS_Geography * crs_g = dynamic_cast< const CRS_Geography*> (&crs);
//
//	if(crs_g == NULL)
//		return false;
//
//	return
//		this->_geoCrsType == crs_g->_geoCrsType
//		&& this->_datum == crs_g->_datum
//		&& this->_refCs == crs_g->_refCs;
//}
//
//int_32 CRS_Geography::packSize(const void *how_to) const
//{
//	if(how_to)
//	{
//		assert(0);
//		return -1;	//TODO
//	}
//	else
//	{
//		return CRS_RealWorld::packPartSize(how_to)
//			+ sizeof(int_32);	// end check
//	}
//}
//
//int_32 CRS_Geography::pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to) const
//{
//	int_32 size = packSize(how_to);
//	byte_8 *current = buffer;
//
//	if(buffer_size < size || how_to)
//	{
//		assert(0);
//		return -1;	//TODO
//	}
//
//	current += CRS_RealWorld::packPart(current, buffer_size, PTC_CRS_GEOGRAPHY, how_to);
//	*(int_32 *)current = size;
//	current += sizeof(int_32);
//
//	return size;
//}
//
//int_32 CRS_Geography::pack(byte_8 **result, const void *how_to) const
//{
//	int_32 size = packSize(how_to);
//	*result = new byte_8[size];
//	byte_8 *next_begin = NULL;
//
//	return pack(*result, size, &next_begin, how_to);
//}
//
//int_32 CRS_Geography::unpack(const byte_8 *buffer, int_32 buf_size)
//{
//	if (  !packageSizeMatch(buffer, true) )
//		return -1 ; // TODO
//
//	const byte_8 *current = buffer;
//	current += CRS_RealWorld::unpackPart(current, PTC_CRS_GEOGRAPHY);
//
//	current += sizeof(int_32);	// end check
//
//	return current - buffer;
//}

}//namespace GIS_Embedded
