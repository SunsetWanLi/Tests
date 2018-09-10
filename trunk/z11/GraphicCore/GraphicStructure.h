//
//  GraphicStructure.h
//  GraphicCore
//
//  Created by Wang ZhiPeng on 11-5-21.
//  Copyright 2011�?Zhejiang University. All rights reserved.
//
#pragma once

#include "../CommonBase/CommonDef.h"

/*字体*/
typedef struct FT_FaceRec_* FONT;

namespace z11
{
    class RGBAColorRef
    {
	public :
        bool   valid_src;
        byte_8 m_red_component;
        byte_8 m_green_component;
        byte_8 m_blue_component;
        byte_8 m_alpha_component;
        static RGBAColorRef white(byte_8 alpha = 255);
        static RGBAColorRef black(byte_8 alpha = 255);
        static RGBAColorRef lightBlack(byte_8 alpha = 255);
        static RGBAColorRef red(byte_8 alpha = 255);
        static RGBAColorRef green(byte_8 alpha = 255);
        static RGBAColorRef blue(byte_8 alpha = 255);
        static RGBAColorRef yellow(byte_8 alpha = 255);
        static RGBAColorRef skyBlue(byte_8 alpha = 255);
		static RGBAColorRef lightBlue(byte_8 alpha = 255);
		static RGBAColorRef darkBlue(byte_8 alpha = 255);
        static RGBAColorRef lightGray(byte_8 alpha = 255);
        static RGBAColorRef darkGray(byte_8 alpha = 255);
		
		RGBAColorRef(): m_red_component(0),
			m_green_component(0),
			m_blue_component(0),
			m_alpha_component(255),
            valid_src(false)
		{
		}
		
		/**
		 * @brief convertToRGB565 把MGColor转换为RGB565 数�?		 *
		 * @return 转换后的数�?		 */
		uint_16 convertToRGB565() const;
		uint_32 convertToARGB() const;
		
		RGBAColorRef( byte_8 _m_red_component,byte_8 _m_green_component,byte_8 _m_blue_component,byte_8 _m_alpha_component=255):
			m_red_component(_m_red_component),
			m_green_component(_m_green_component),
			m_blue_component(_m_blue_component),
			m_alpha_component(_m_alpha_component),
            valid_src(false)
		{
			
		}
		
		/**
		 * @brief fromRGB565 从指定的RGB565数值构造RGBAColorRef
		 *
		 * @param _color 指定的RGB565数�?		 *
		 * @return 构造的RGBAColorRef
		 */
		static RGBAColorRef fromRGB565(uint_16 _color);

		/**
		 * @brief fromRGB 从指定的RGB数值构�?RGBAColorRef
		 *
		 * @param _color 指定的RGB数�?		 *
		 * @return 构造的RGBAColorRef
		 */
		static RGBAColorRef fromRGB(uint_32 _color);
		static RGBAColorRef fromARGB(uint_32 _color);


        bool operator ==(RGBAColorRef rp)
        {
            return (m_red_component == rp.m_red_component &&
                    m_green_component == rp.m_green_component &&
                    m_blue_component == rp.m_blue_component &&
                    m_alpha_component == rp.m_alpha_component);
        }

        RGBAColorRef& operator = (RGBAColorRef rp)
        {
            m_red_component = rp.m_red_component;
            m_green_component = rp.m_green_component;
            m_blue_component = rp.m_blue_component;
            m_alpha_component = rp.m_alpha_component;
            valid_src = rp.valid_src;

            return *this;
        }
    };
    
	/**
	 * @brief RGBAColorMake 根据指定R，G，B，A的值构造RGBAColorRef
	 *
	 * @param r Red value
	 * @param g Green value
	 * @param b Blue value
	 * @param a Alpha value
	 *
	 * @return 构造的RGBAColorRef
	 */
    RGBAColorRef RGBAColorMake(byte_8 r, byte_8 g, byte_8 b, byte_8 a = 255, bool valid = false);


//GraphicEngine OpenGLES
    typedef struct _MGVertex{
        float x;
        float y;
        float z;
    }MGVertex;
    typedef struct _MGTexCoords{
        float x;
        float y;
    }MGTexCoords;
    
    _MGVertex MGVertexMake(float _x, float _y, float _z = 0.0);
    
    _MGTexCoords MGTexCoordsMake(float _x, float _y);
    
    //! a Point with a vertex point, a tex coord point and a color 4B
    typedef struct _V3F_C4B_T2F
    {
        //! vertices (3F)
        MGVertex		vertices;			// 12 bytes
        //! colors (4B)
        RGBAColorRef	colors;				// 4 bytes
        // tex coords (2F)
        MGTexCoords	tex_coords;			// 8 bytes
    } V3F_C4B_T2F;
    
    typedef struct _V3F_C4B
    {
        MGVertex vertices;
        RGBAColorRef colors;
    } V3F_C4B;
    
    //! 4 ccVertex3FTex2FColor4B
    typedef struct _V3F_C4B_T2F_Quad
    {
        //! top left
        V3F_C4B_T2F	tl;
        //! bottom left
        V3F_C4B_T2F	bl;
        //! top right
        V3F_C4B_T2F	tr;
        //! bottom right
        V3F_C4B_T2F	br;
    } V3F_C4B_T2F_Quad; 

} //namespace z11
