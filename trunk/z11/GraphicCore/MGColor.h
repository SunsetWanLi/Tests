//
//  MGColor.h
//  GraphicCore
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011�� Zhejiang University. All rights reserved.
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
		 * @brief fromImage ��ָ���ļ�����MGColor
		 *
		 * @param filename ָ���ļ���
		 *
		 * @return �����MGColor
		 */
		static MGColor *fromImage(const string& filename);	//pattern using image 
		/**
		 * @brief fromImage ��ָ����MGImage ����MGColor
		 *
		 * @param image ָ����MGImage ,retain
		 *
		 * @return �����MGColor
		 */
		static MGColor *fromImage(MGImage *image); //retain
		/**
		 * @brief fromRGBA ����R,G,B,A��ֵ������MGColor
		 *
		 * @param r Red Value
		 * @param g Green Value
		 * @param b Blue Value
		 * @param a Alpha Value
		 *
		 * @return �����MGColor
		 */
		static MGColor *fromRGBA(byte_8 r,byte_8 g,byte_8 b, byte_8 a = 255);
		/**
		 * @brief fromColorRef ����RGBAColorRef������MGColor
		 *
		 * @param ref ָ����RGBAColorRef
		 *
		 * @return �����MGColor
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
