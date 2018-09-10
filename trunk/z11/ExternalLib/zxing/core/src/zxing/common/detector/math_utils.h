// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
#ifndef __ZXING_COMMON_DETECTOR_MATHUTILS_H__
#define __ZXING_COMMON_DETECTOR_MATHUTILS_H__
/*
 *  Copyright 2012 ZXing authors All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <math.h>

namespace zxing { namespace common { namespace detector { namespace math_utils {
/**
 * Ends up being a bit faster than {@link Math#round(float)}. This merely rounds its
 * argument to the nearest int, where x.5 rounds up to x+1.
 */
	template<class T>
	const T& max(const T& a, const T& b)
	{
		return (a < b) ? b : a;
	}

	template<class T>
	const T& min(const T& a, const T& b)
	{
		return (b < a) ? b : a;
	}

inline int round(float d) {
  return (int) (d + 0.5f);
}

inline float distance(float aX, float aY, float bX, float bY) {
  float xDiff = aX - bX;
  float yDiff = aY - bY;
  return (float) sqrt(xDiff * xDiff + yDiff * yDiff);
}

inline float distance(int aX, int aY, int bX, int bY) {
  int xDiff = aX - bX;
  int yDiff = aY - bY;
  return (float) sqrt((double)xDiff * xDiff + yDiff * yDiff);
}

}}}}

#ifdef WIN32
	#include <limits>
	#ifndef NAN
		static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
		//#define NAN (*(const float *) __nan)
		#define NAN std::numeric_limits<float>::min();
		inline bool isnan(float num)
		{
			return num==NAN;
		}
	#endif
	#ifndef INFINITY
		#define INFINITY std::numeric_limits<float>::infinity()
	#endif
	#ifndef fmax
		#include <utility>
		#define fmax(a,b) zxing::common::detector::math_utils::max((a),(b))
	#endif

#endif

#endif
