#ifndef GIS_Embeded_CRT_TRANSFORMATION_H_
#define GIS_Embeded_CRT_TRANSFORMATION_H_

#include "CRT.h"

namespace GIS_Embedded
{
;
// from Geocentric to Geocentric
class CRT_Transformation : public CRT 
{
public:
	CRT_Transformation()/*:CRT(CRT_TRANSFORMATION)*/ {};
	CRT_Transformation(const CRT_Transformation &right)
	{
		*this = right;
	}
	~CRT_Transformation()
	{
	}

	CRT_Transformation inverse() const;

	GIS_Embedded::Pos3D transDirectionPosion(const GIS_Embedded::Pos3D& posSource) const;

	void setParameters(bool blThreePara,double dShiftX,double dShiftY,double dShiftZ,
		double dRotateX = 0.0,double dRotateY = 0.0,double dRotateZ = 0.0,double dScale = 0.0);

	void setSevenPara(vector<GIS_Embedded::Pos3D>& posSourceArray, vector<GIS_Embedded::Pos3D>& posTargetArray);

	void setThreePara(GIS_Embedded::Pos3D& posSource,GIS_Embedded::Pos3D& posTarget);
	
	virtual bool operator == (const CRT &right) const;
	//{
	//	const CRT_Transformation * _right = dynamic_cast<const CRT_Transformation *>(&right);
	//	if(!_right)
	//		return false;
	//	return /*_crtType == _right->_crtType 
	//		&&*/ _dShiftX == _right->_dShiftX && _dShiftY == _right->_dShiftY && _dShiftZ == _right->_dShiftZ
	//		&& _dRotateX == _right->_dRotateX && _dRotateY == _right->_dRotateY && _dRotateZ == _right->_dRotateZ
	//		&& _dScale == _right->_dScale 
	//		&& _blThreePara == _right->_blThreePara;
	//}
	virtual bool isInversable() const
	{
		return true;
	}

	virtual CRT *createInverseCRT() const
	{
		return new CRT_Transformation(inverse());
	}
	virtual CRT* copy() const;

	virtual int_32 trans(const Pos3D &src, Pos3D *dest) const;
private:
	double _dShiftX,_dShiftY,_dShiftZ;		//three shift parameters
	double _dRotateX,_dRotateY,_dRotateZ;	//three rotation parameters
	double _dScale;
	bool   _blThreePara;
};

} // namespace
#endif //GIS_Embeded_CRT_TRANSFORMATION_H_