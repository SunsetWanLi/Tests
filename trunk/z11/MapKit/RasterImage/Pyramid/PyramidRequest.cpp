#include "PyramidRequest.h"
//#include "./PyramidTileIndex.h"
//#include "./PyramidExceptionDef.h"
//#include "./PyramidStringConvert.h"
//#include "./PyramidThreadBase.h"

namespace Raster_GIS
{
namespace Pyramid_Space
{
//	namespace MapRequest
//	{

		//inline int getThreadPrio(Priority prio)
		//{
		//	switch(prio)
		//	{
		//	case SPECIAL:
		//		return Thread::THREAD_TIME_CRITICAL;
		//		break;
		//	case HIGHER:
		//		return Thread::THREAD_ABOVE_NORMAL;
		//		break;
		//	case NORMAL:
		//		return Thread::THREAD_NORMAL;
		//		break;
		//	case MOST_POSSIBLE:
		//	case POSSIBLE:
		//	case GUESS:
		//		return Thread::THREAD_BELOW_NORMAL;
		//		break;
		//	case WHEN_IDLE:
		//		return Thread::THREAD_IDLE;
		//		break;
		//	default:
		//		throw unknown_priority(L"@ Request.cpp getThreadPrio()");
		//	} // switch
		//} // getThreadPrio(Priority )

		tagResult::~tagResult()
		{
			//for(unsigned int i = 0; i< block_count ;++i)
			//{
			//	from ->leave(blocks[i].tile);	//¸Ä½ø,by hqs 2008-05-31			
			//}
			delete [] blocks;
			blocks = NULL;
			block_count = 0;
			from = NULL;
		}


//	} // end of namespace MapRequest
} // end of namespace Pyramid_Space
} // end of namespace Raster_GIS

