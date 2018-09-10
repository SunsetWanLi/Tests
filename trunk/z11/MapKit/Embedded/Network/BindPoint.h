#ifndef _EMBEDDED_NETWORK_BIND_POINT_H_
#define _EMBEDDED_NETWORK_BIND_POINT_H_
#include "../Tools/SpatialDataBase.h"

namespace Embedded
{
	namespace Network
	{
		class LowNetwork;

		class BindPoint
		{
		public:
			MapPos2D m_pos;
			MapPos2D m_beginPos, m_endPos;
			uint_32 m_beginID, m_endID;
			double m_beginLen, m_endLen;
			uint_64 m_roadID;

		public:
			BindPoint(const MapPos2D& p, const IPointBindLine* bind, const LowNetwork* nw);
			BindPoint(const MapPos2D& p,double course, const IPointBindLine* bind, const LowNetwork* nw);
			BindPoint(std::istream& fin);
			BindPoint();

			void output(std::ostream& fout) const;
		}; // BindPoint
	}
}

#endif // _EMBEDDED_NETWORK_BIND_POINT_H_