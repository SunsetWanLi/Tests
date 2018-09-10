#pragma  once
#include "TypeDefs.h"
#include "IBuffer.h"
#include <string>
#include <map>

using std::map;
using z11::MGWString;

namespace GIS_Embedded
{
	class GGPointBuffer : public IBuffer{
	private:
		map<z11::MGWString, GGPointBlock*> m_hash_blocks;		// 支持按类型查询
		map<z11::MGWString,GuiderGeoPoint*> m_hash_points;	// 支持景点名称查询

	public:	
		int saveRam(z11::MGWString &key,vector<GuiderGeoPoint*> *gg_points,int size);
		virtual int availableSize() const 
		{
			return 0;
		};
		virtual int spaceFree(int size)
		{
			return 0;
		};
		int getBlock(z11::MGWString&,GGPointBlock** block);
		int getGGPoint(z11::MGWString&, GuiderGeoPoint**);
	};
}