#ifndef GIS_Embeded_CRT_PROJECTION_GAUSS_STRIPE_H_
#define GIS_Embeded_CRT_PROJECTION_GAUSS_STRIPE_H_

#include "CRT_Projection.h"
 
#include "Datum.h"
namespace GIS_Embedded {
	;

	class CRT_ProjectionGauss_Zoned: public CRT_Projection/*, public Packable*/
	{
	public:
		CRT_ProjectionGauss_Zoned();
		CRT_ProjectionGauss_Zoned(const CRT_ProjectionGauss_Zoned& right);
		virtual ~CRT_ProjectionGauss_Zoned();

		virtual CRT *copy() const;
		virtual PrjType prjType() const { return PRJ_GAUSS_ZONED;}
		virtual bool isInversable() const { return true; }
		virtual CRT *createInverseCRT() const;


		virtual bool operator == (const CRT& right)const;
		virtual int_32 trans(const Pos3D &src, Pos3D *dest) const; 
		
		 
		

		//virtual int_32 packSize(const void *how_to = NULL) const;
		//virtual int_32 pack(byte_8 *buffer, int_32 buffer_size, byte_8 **end_pos, const void *how_to = NULL) const;
		//virtual int_32 pack(byte_8 **result, const void *how_to = NULL)const;
		//virtual int_32 unpack(const byte_8 *buffer, int_32 buf_size = PACK_IGNORE_BUF_SIZE);

	public:
		CD_Ellipsoid m_ellipsoid; // 椭球
		bool m_b3du;
		bool m_b6du;
		
	};



	class CRT_ProjectionGauss_Zoned_Inverse: public CRT
	{
	public:
		CRT_ProjectionGauss_Zoned_Inverse();
		CRT_ProjectionGauss_Zoned_Inverse(const CRT_ProjectionGauss_Zoned_Inverse& right);
		CRT_ProjectionGauss_Zoned_Inverse(const CRT_ProjectionGauss_Zoned& right);
		
		virtual CRT *copy() const;
		virtual bool operator == (const CRT &right) const;
		virtual bool isInversable() const {	return true;	}
		virtual CRT *createInverseCRT() const;
		virtual int_32 trans(const Pos3D &src, Pos3D *dest) const;

	public:
		CRT_ProjectionGauss_Zoned	m_prj_gauss_zoned;

	};

	
}

#endif 