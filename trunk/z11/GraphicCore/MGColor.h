//
//  MGColor.h
//  GraphicCore
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include "../CommonBase/MGObject.h"
#include "GraphicStructure.h"
#include <string>

using namespace std;

namespace z11
{
	class MGImage;
    class MGColor : public MGObject
    {
    public:
        virtual ~MGColor();
		/**
		 * @brief fromImage 从指定文件构造MGColor
		 *
		 * @param filename 指定文件名
		 *
		 * @return 构造的MGColor
		 */
		static MGColor *fromImage(const string& filename);	//pattern using image 
		/**
		 * @brief fromImage 从指定的MGImage 构造MGColor
		 *
		 * @param image 指定的MGImage ,retain
		 *
		 * @return 构造的MGColor
		 */
		static MGColor *fromImage(MGImage *image); //retain
		/**
		 * @brief fromRGBA 根据R,G,B,A的值来构造MGColor
		 *
		 * @param r Red Value
		 * @param g Green Value
		 * @param b Blue Value
		 * @param a Alpha Value
		 *
		 * @return 构造的MGColor
		 */
		static MGColor *fromRGBA(byte_8 r,byte_8 g,byte_8 b, byte_8 a = 255);
		/**
		 * @brief fromColorRef 根据RGBAColorRef来构造MGColor
		 *
		 * @param ref 指定的RGBAColorRef
		 *
		 * @return 构造的MGColor
		 */
        static MGColor* fromColorRef(RGBAColorRef ref);
        
		void setImage(const string& filename);
		void setImage(MGImage *image);	//retain
		void setRGBA(byte_8 r,byte_8 g,byte_8 b, byte_8 a = 255);
        void setColorRef(RGBAColorRef ref);
        void setAlphaInByte(byte_8 _alpha);
    public:
        RGBAColorRef m_color;
		MGImage *m_image;

	private:
		MGColor();

    };
} //namespace z11
