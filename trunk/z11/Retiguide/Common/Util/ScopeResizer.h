#ifndef _COMMON_UTIL_SCOPERESIZER_H
#define _COMMON_UTIL_SCOPERESIZER_H

#include "../Share/Scope.h"

namespace GIS_Embedded
{

	class ScopeResizer
	{

		private:
			double _s_max;  //最大面积
			double _s_min;  //最小面积
			double _min_width;  //最小宽度
			double _min_height;  //最小高度
			double _max_width;
			double _max_height;
			double _a;  //
			double _b;
			double _precision;
		
		public:

			ScopeResizer();

			~ScopeResizer();

			void setSize(double precision,double max_width,double max_height,double min_width,double min_height);

			Scope* resizeScope(Scope& view_scope,Scope& scope);
	   
			Scope* resizeScope(Scope& screen_scope,double zoom_level);

	};


}

#endif