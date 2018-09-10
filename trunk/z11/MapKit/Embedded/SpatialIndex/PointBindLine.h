#ifndef _EMBEDDED_SPATIALINDEX_POINT_BIND_LINE_H_
#define _EMBEDDED_SPATIALINDEX_POINT_BIND_LINE_H_
#include "../Tools/SpatialDataBase.h"

namespace Embedded
{
	namespace SpatialIndex
	{
		class StaticRTree;

		class PointBindLine : public IPointBindLine
		{
		public:
			PointBindLine(const std::vector<ISpatialIndex*>& layers, const ICondenseStrategy* cs);

			virtual void bind(const MapPos2D& p, Line** line, uint_64* id) const;
			virtual void bind(const MapPos2D& p, double course,Line** line, uint_64* id) const;

		private:
			class NNEntry
			{
			public:
				id_type m_id;
				IEntry* m_pEntry;
				double m_minDist;

				NNEntry(id_type id, IEntry* e, double f) : m_id(id), m_pEntry(e), m_minDist(f) {}
				~NNEntry() {}

				struct ascending : public std::binary_function<NNEntry*, NNEntry*, bool>
				{
					bool operator()(const NNEntry* __x, const NNEntry* __y) const { return __x->m_minDist > __y->m_minDist; }
				};
			}; // NNEntry

			class NNComparator : public INearestNeighborComparator
			{
			public:
				double getMinimumDistance(const IShape& query, const IShape& entry)
				{
					return query.getMinimumDistance(entry);
				}

				double getMinimumDistance(const IShape& query, const IData& data)
				{
					IShape* pS;
					data.getShape(&pS);
					double ret = query.getMinimumDistance(*pS);
					delete pS;
					return ret;
				}
			}; // NNComparator

		private:
			std::vector<StaticRTree*> m_layers;
			const ICondenseStrategy* m_condenseStrategy;

		}; // PointBindLine
	}
}

#endif // _EMBEDDED_SPATIALINDEX_POINT_BIND_LINE_H_