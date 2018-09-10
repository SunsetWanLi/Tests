//
//  ImageResizableButton.cpp
//  MultidimensionBeijing
//
//  Created by slj on 13-4-1.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//

#include "ImageResizableButton.h"
#include "GraphicCore/MGImage.h"
#include <cmath>
#include <assert.h>

namespace z11
{
    ImageResizableButton::ImageResizableButton()
    {
        class_name = "ImageResizableButton";
        edge_scale_enabled = false;
		m_draw_round_mode = RoundRectAll;
    }
    
    ImageResizableButton::~ImageResizableButton()
    {
        
    }
    
    void ImageResizableButton::initWithFrame(MGRect frame)
    {
        MIButton::initWithFrame(frame);
    }
    
    void ImageResizableButton::drawself()
    {
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if(graphic_engine == NULL)
        {
            return;
        }
        byte_8 alpha = m_alpha;
        RGBAColorRef color_ref;
        if (m_background_color != NULL)
        {
            color_ref = m_background_color->m_color;
            color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
            alpha = color_ref.m_alpha_component;
        }else
        {
            color_ref = RGBAColorMake(0, 0, 0, (byte_8)(m_alpha * 255));
            alpha = 0;
        }
        
        if(m_background_color != NULL && m_background_color->m_image != NULL)
        {
            m_background_color->m_image->sendDataToContext();
            if (m_is_enabled == false)
            {
                color_ref = RGBAColorRef::lightGray();
                color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
            }
            else if(m_is_clicked != 0 && (m_action_type & MIActionTypeBlend) != 0)
            {
                color_ref = m_action_blend;
            }
            else if((m_control_state & MIControlStateSelected) != 0)
            {
                color_ref = RGBAColorMake(128, 128, 128);
                color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
            }
            // draw resizable image here !!!
            do
            {
                MGRect draw_bounds[9];
                MGRect image_rect[9];
                bool should_draw[9];
                
                // init image_rect
                PixelInt image_width = m_background_color->m_image->getWidth();
                PixelInt image_height = m_background_color->m_image->getHeight();
                assert(non_scale_edge.left+non_scale_edge.right < image_width && non_scale_edge.top+non_scale_edge.bottom < image_height);
                vector<float> image_rect_start_x;
                image_rect_start_x.push_back(0.0f);
                image_rect_start_x.push_back(non_scale_edge.left);
                image_rect_start_x.push_back(image_width-non_scale_edge.right);
                vector<float> image_rect_start_y;
                image_rect_start_y.push_back(0.0f);
                image_rect_start_y.push_back(non_scale_edge.top);
                image_rect_start_y.push_back(image_height-non_scale_edge.bottom);
                vector<float> image_rect_width;
                image_rect_width.push_back(non_scale_edge.left);
                image_rect_width.push_back(image_width-non_scale_edge.left-non_scale_edge.right);
                image_rect_width.push_back(non_scale_edge.right);
                vector<float> image_rect_height;
                image_rect_height.push_back(non_scale_edge.top);
                image_rect_height.push_back(image_height-non_scale_edge.top-non_scale_edge.bottom);
                image_rect_height.push_back(non_scale_edge.bottom);
                
                for(int_32 index_x = 0; index_x < 3; ++index_x)
                {
                    for(int_32 index_y = 0; index_y < 3; ++index_y)
                    {
                        image_rect[index_x+index_y*3] = MGRectMake(image_rect_start_x[index_x], image_rect_start_y[index_y], image_rect_width[index_x], image_rect_height[index_y]);
                    }
                }
                
                // init image_draw_total_bounds
                MGRect image_draw_total_bounds = m_bounds;
                image_draw_total_bounds.x += draw_edge.left;
                image_draw_total_bounds.y += draw_edge.top;
                image_draw_total_bounds.width -= draw_edge.left+draw_edge.right;
                image_draw_total_bounds.height -= draw_edge.top+draw_edge.bottom;
                assert(image_draw_total_bounds.width > 1e-6 && image_draw_total_bounds.height > 1e-6);
                
                // init draw_bounds
                float scale_x = 1.0f*image_draw_total_bounds.width/image_width;
                float scale_y = 1.0f*image_draw_total_bounds.height/image_height;
                if(!edge_scale_enabled)
                {
                    scale_x = scale_y = 1.0f;
                }
                if(scale_x < scale_y)
                {
                    for(int_32 index = 0; index < 9; ++index)
                    {
                        draw_bounds[index].width = image_rect[index].width*scale_x;
                        draw_bounds[index].height = image_rect[index].height*scale_x;
                    }
                }
                else
                {
                    for(int_32 index = 0; index < 9; ++index)
                    {
                        draw_bounds[index].width = image_rect[index].width*scale_y;
                        draw_bounds[index].height = image_rect[index].height*scale_y;
                    }
                }
                draw_bounds[1].width = image_draw_total_bounds.width-draw_bounds[0].width-draw_bounds[2].width;
                draw_bounds[4].width = draw_bounds[1].width;
                draw_bounds[7].width = draw_bounds[1].width;
                
                draw_bounds[3].height = image_draw_total_bounds.height-draw_bounds[0].height-draw_bounds[6].height;
                draw_bounds[4].height = draw_bounds[3].height;
                draw_bounds[5].height = draw_bounds[3].height;
                
                draw_bounds[0].x = draw_bounds[3].x = draw_bounds[6].x = image_draw_total_bounds.x;
                draw_bounds[1].x = draw_bounds[4].x = draw_bounds[7].x = draw_bounds[0].x+draw_bounds[0].width;
                draw_bounds[2].x = draw_bounds[5].x = draw_bounds[8].x = draw_bounds[1].x+draw_bounds[1].width;
                
                draw_bounds[0].y = draw_bounds[1].y = draw_bounds[2].y = image_draw_total_bounds.y;
                draw_bounds[3].y = draw_bounds[4].y = draw_bounds[5].y = draw_bounds[0].y+draw_bounds[0].height;
                draw_bounds[6].y = draw_bounds[7].y = draw_bounds[8].y = draw_bounds[3].y+draw_bounds[3].height;
                
                for(int_32 index = 0; index < 9; ++index)
                {
                    if(isZero(draw_bounds[index].width) || isZero(draw_bounds[index].height) ||
                       isZero(image_rect[index].height) || isZero(image_rect[index].height))
                    {
                        should_draw[index] = false;
                    }
                    else
                    {
                        should_draw[index] = true;
                    }
                }
                
                for(int_32 index = 0; index < 9; ++index)
                {
                    if(should_draw[index])
                    {
                        graphic_engine->drawImage(m_background_color->m_image, draw_bounds[index], color_ref, ImageCustomFit, image_rect[index]);
                    }
                }
            }
            while(false);
        }
        else
        {
            MGRect rect = m_bounds;
            ++rect.x;
            ++rect.y;
            rect.width -= 2;
            rect.height -= 2;
            if (m_is_enabled == false)
            {
                graphic_engine->drawFilledRoundRect(rect, color_ref, cornerRadius, RGBAColorRef::lightGray(),m_draw_round_mode);
            }
            else if(m_is_clicked == 0 || (m_action_type & MIActionTypeBlend) == 0)
            {
                graphic_engine->drawFilledRoundRect(rect, color_ref, cornerRadius,RGBAColorMake(255,255,255,255),m_draw_round_mode);
            }
            else
            {
                graphic_engine->drawFilledRoundRect(rect, color_ref, cornerRadius, m_action_blend,m_draw_round_mode);
            }
            color_ref=RGBAColorRef::darkGray();
            color_ref.m_alpha_component=(byte_8)alpha;
            graphic_engine->drawRoundRect(m_bounds, color_ref, cornerRadius,1.0f,RGBAColorMake(255,255,255,255),m_draw_round_mode);
        }
    }
    
    void ImageResizableButton::resizableWithCapInsets(const MGEdgeInsets &non_scale_edge, const MGEdgeInsets &draw_edge/* = _UIEdgeInsets()*/)
    {
        AUTO_LOCK_THIS;
        
        this->non_scale_edge = non_scale_edge;
        this->draw_edge = draw_edge;
    }
    
    inline bool ImageResizableButton::isZero(float value)
    {
        return (fabs(value) < 1e-6) ? true : false;
    }
}