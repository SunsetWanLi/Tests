#include "./PyramidRequest.h"

namespace GIS_Embedded
{
	tagResult::~tagResult()
	{
		//for(z11::uint_32 i = 0; i< block_count ;++i)
		//{
		//	from ->leave(blocks[i].tile);	//改进,by hqs 2008-05-31			
		//}
		delete [] blocks;
		blocks = NULL;
		block_count = 0;
		from = NULL;
	}

} // end of namespace GIS_Embedded

