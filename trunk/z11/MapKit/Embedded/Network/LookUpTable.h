#ifndef _EMBEDDED_NETWORK_LOOKUP_TABLE_H_
#define _EMBEDDED_NETWORK_LOOKUP_TABLE_H_
#include "../Tools/SpatialDataBase.h"

namespace Embedded
{
	namespace Network
	{
		class LookUpTable
		{
		public:
			static double queryMinDistance(IStorageManager* sm, uint_32 startPage, uint_32 gridNum,
				uint_32 sGridID, uint_32 dGridID, uint_32 pageSize);
			static void loadTable(IStorageManager* sm, uint_32 startPage, uint_32 gridNum,
				uint_32 destGridID, uint_8** data, uint_32 pageSize);
			static double queryMinDistance(uint_32 srcGridID, const uint_8* table);

		private:
			static uint_32 decodeDis(uint_16 len);

		private:
			static const uint_32 MAX_DIS			= 0xffff;
			static const uint_32 DIS_SIZE		= 2;

			static const uint_16 DIS_16CARRY	= 0x0000;
			static const uint_16 DIS_128CARRY	= 0x4000;
			static const uint_16 DIS_1024CARRY	= 0x8000;
			static const uint_16 DIS_8192CARRY	= 0xC000;

			static const uint_16 DIS_16GAP		= 0x3fff;
			static const uint_16 DIS_128GAP	= 0x7fff;
			static const uint_16 DIS_1024GAP	= 0xBfff;
			static const uint_16 DIS_8192GAP	= 0xffff;

		}; // LookUpTable
	}
}

#endif // _EMBEDDED_NETWORK_LOOKUP_TABLE_H_
