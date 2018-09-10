#include "LookUpTable.h"
#include <string.h>

using namespace Embedded;
using namespace Embedded::Network;

double LookUpTable::queryMinDistance(IStorageManager* sm, uint_32 startPage, uint_32 gridNum,
									   uint_32 sGridID, uint_32 dGridID, uint_32 pageSize)
{
	uint_32 pos = (dGridID * gridNum + sGridID) * DIS_SIZE;
	uint_32 page = pos / pageSize,
		offset = pos - page * pageSize;
	page += startPage;

	uint_8* data;
	uint_32 len;
	sm->loadByteArray(static_cast<id_type>(page), len, &data);

	uint_16 dis = 0;
	memcpy(&dis, data + offset, DIS_SIZE);
	delete[] data;

	return decodeDis(dis);
}

void LookUpTable::loadTable(IStorageManager* sm, uint_32 startPage, uint_32 gridNum,
							  uint_32 destGridID, uint_8** data, uint_32 pageSize)
{
	uint_32 pos = destGridID * gridNum * DIS_SIZE;
	uint_32 page = pos / pageSize,
		offset = pos - page * pageSize;
	page += startPage;

	uint_32 len = gridNum * DIS_SIZE;
	*data = new uint_8[len];

	uint_8* ptrDest = *data;
	uint_32 cLen;
	uint_32 cRem = len, pageRem;

	do
	{
		uint_8* dataSrc;
		uint_32 lenSrc;
		sm->loadByteArray(static_cast<id_type>(page), lenSrc, &dataSrc);

		uint_8* ptrSrc = dataSrc + offset;

		pageRem = pageSize - offset;
		cLen = (cRem > pageRem) ? pageRem : cRem;
		memcpy(ptrDest, ptrSrc, cLen);

		ptrDest += cLen;
		cRem -= cLen;
		++page;
		offset = 0;

		delete[] dataSrc;
	} while (cRem != 0);
}

double LookUpTable::queryMinDistance(uint_32 srcGridID, const uint_8* table)
{
	uint_16 dis = 0;
	memcpy(&dis, table + srcGridID * DIS_SIZE, DIS_SIZE);
	return decodeDis(dis);
}

uint_32 LookUpTable::decodeDis(uint_16 len)
{
	if (len <= DIS_16GAP)
		return len;
	else if (len <= DIS_128GAP)
		return static_cast<uint_32>(len ^ DIS_128CARRY) << 3;
	else if (len <= DIS_1024GAP)
		return static_cast<uint_32>(len ^ DIS_1024CARRY) << 6;
	else if (len < DIS_8192GAP)
		return static_cast<uint_32>(len ^ DIS_8192CARRY) << 9;
	else
		return MAX_DIS;
}
