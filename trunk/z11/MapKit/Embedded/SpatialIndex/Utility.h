#ifndef _EMBEDDED_SPATIALINDEX_UTILITY_H_
#define _EMBEDDED_SPATIALINDEX_UTILITY_H_
#include "../Tools/SpatialDataBase.h"

namespace Embedded
{
	namespace SpatialIndex
	{
		enum RangeQueryType
		{
			ContainmentQuery	= 0x1,
			IntersectionQuery	= 0x2,
		};

		const uint_32 NODE_CAPACITY = (PUBLISH_PAGE_SIZE - 2 - 16) / 27;		// (4096 - 2 - 16) / 27

		extern ISpatialIndex* returnStaticRTree(IStorageManager& sm, PropertySet& ps);
		extern ISpatialIndex* createNewStaticRTree(
			IStorageManager& sm, 
			id_type& indexIdentifier,
			uint_32 condenseStrategy
			);

		extern ISpatialIndex* loadStaticRTree(IStorageManager& in, id_type indexIdentifier);

		class StaticRTree;
	}

	typedef SpatialIndex::StaticRTree STATIC_LAYER;
}

#endif // _EMBEDDED_SPATIALINDEX_UTILITY_H_