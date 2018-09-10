#include "BindPoint.h"
#include "LowNetwork.h"
#include "../BasicType/Line.h"
#include "../Tools/SpatialMath.h"

using namespace Embedded;
using namespace Embedded::Network;

BindPoint::BindPoint(const MapPos2D& p, const IPointBindLine* bind, const LowNetwork* nw)
{
	Line* l;
	bind->bind(p, &l, &m_roadID);

	std::vector<MapPos2D> ctlpos;
	l->getPoints(&ctlpos);

	m_beginPos = ctlpos.front();
	m_endPos = ctlpos.back();

	m_beginID = nw->queryTopoNodeID(m_beginPos);
	m_endID = nw->queryTopoNodeID(m_endPos);

	SpatialMath::getNearestPosOnLineToPoint(p, ctlpos, &m_pos);
	m_beginLen = SpatialMath::getDisOnLinePosToLineEnd(m_pos, ctlpos, true);
	m_endLen = SpatialMath::getDisOnLinePosToLineEnd(m_pos, ctlpos, false);

	if(m_beginID == m_endID)
	{
		m_beginLen = m_endLen = 0;
	}

	delete l;
}

BindPoint::BindPoint(std::istream& fin)
{
	fin >> m_pos.d0 >> m_pos.d1 
		>> m_beginPos.d0 >> m_beginPos.d1 >> m_endPos.d0 >> m_endPos.d1 
		>> m_beginID >> m_endID 
		>> m_beginLen >> m_endLen 
		>> m_roadID;
}

void BindPoint::output(std::ostream& fout) const
{
	fout << std::setprecision(20) 
		<< m_pos.d0 << " " << m_pos.d1 << " " 
		<< m_beginPos.d0 << " " << m_beginPos.d1 << " " << m_endPos.d0 << " " << m_endPos.d1 << " "
		<< m_beginID << " " << m_endID << " " 
		<< m_beginLen << " " << m_endLen << " " 
		<< m_roadID << std::endl;
}

Embedded::Network::BindPoint::BindPoint() :m_beginID(0),m_endID(0),m_beginLen(0.0),m_endLen(0.0),m_roadID(0LL)
{

}

Embedded::Network::BindPoint::BindPoint( const MapPos2D& p,double course,const IPointBindLine* bind, const LowNetwork* nw )
{
	Line* l;
	bind->bind(p,course, &l, &m_roadID);

	std::vector<MapPos2D> ctlpos;
	l->getPoints(&ctlpos);

	m_beginPos = ctlpos.front();
	m_endPos = ctlpos.back();

	m_beginID = nw->queryTopoNodeID(m_beginPos);
	m_endID = nw->queryTopoNodeID(m_endPos);

	SpatialMath::getNearestPosOnLineToPoint(p, ctlpos, &m_pos);
	m_beginLen = SpatialMath::getDisOnLinePosToLineEnd(m_pos, ctlpos, true);
	m_endLen = SpatialMath::getDisOnLinePosToLineEnd(m_pos, ctlpos, false);

	if(m_beginID == m_endID)
	{
		m_beginLen = m_endLen = 0;
	}

	delete l;
}