#ifndef GIS_Embeded_CRT_GEOGRAPHY_TO_GEOGRAPHY_5480_H_
#define GIS_Embeded_CRT_GEOGRAPHY_TO_GEOGRAPHY_5480_H_

#include "CRT.h"

#include "Datum.h"

namespace GIS_Embedded {

	;

	class CRT_GeographyToGeography5480: public CRT/*, public Packable */
	{
	public:
		CRT_GeographyToGeography5480();
		CRT_GeographyToGeography5480(const CRT_GeographyToGeography5480& right);
		~CRT_GeographyToGeography5480();

		virtual bool operator == (const CRT &right) const ;
		virtual CRT *copy() const ;
		virtual bool isInversable() const { return false; }
		virtual CRT *createInverseCRT() const { return NULL;}

		// only Pos3D - sth. oop is unable to solve
		virtual int_32 trans(const Pos3D &src, Pos3D *dest) const;

		//virtual int_32 packSize(const void *how_to = NULL) const;
		//virtual int_32 pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to = NULL) const;
		//virtual int_32 pack(byte_8 **result, const void *how_to = NULL)const;
		//virtual int_32 unpack(const byte_8 *buffer, int_32 buf_size = PACK_IGNORE_BUF_SIZE);


	public:
		CD_Ellipsoid m_ellipsoid;
		double m_delta_x, m_delta_y, m_delta_z; //参心偏移量
		double m_DX,m_DY;
	};


}

#endif
