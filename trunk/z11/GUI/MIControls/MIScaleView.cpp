//
//  MIScaleView.cpp
//  Gui
//
//  Created by slj on 13-5-14.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//

#include "MIScaleView.h"
#include "../../System/Device/MGDevice.h"
#include "../GuiContainer/GuiContainer.h"
#include "../../GraphicCore/MGImage.h"
#include "../MICustomControls/ImageResizableButton.h"
#include "assert.h"
#include <cmath>
#include <stdio.h>
#define            SCALE_VIEW_WILL_RELOAD_IMAGE_THREHOLD                (2/640.0f)

namespace z11
{
    MIScaleView::MIScaleView()
    {
        class_name = "MIScaleView";
        _view_flags.user_interaction_enabled = 0;
        
        scale_value = 1.5f;
        bkg_view = NULL;
        content_view = NULL;
    }
    
    MIScaleView::~MIScaleView()
    {
        
    }
    
    void MIScaleView::initWithFrame(MGRect frame)
    {
        MIView::initWithFrame(frame);
        
        bkg_view = new ImageResizableButton();
        bkg_view->_view_flags.user_interaction_enabled = 0;
        bkg_view->setCornerRadius(2);
        bkg_view->initWithFrame(MGRectMake(0.0f, 0.0f, frame.width, frame.height));
        bkg_view->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::lightGray()));
        addSubview(bkg_view);
        
        content_view = new MIImageView();
        content_view->_view_flags.user_interaction_enabled = 0;
        content_view->initWithFrame(bkg_view->getFrame());
        content_view->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::white()));
        addSubview(content_view);
    }
    
    void MIScaleView::setResizableBkgWithCapInsets(MGColor* color, const MGEdgeInsets &non_scale_edge/* = _UIEdgeInsets()*/)
    {
        assert(_view_flags.view_initialized == 1);
       
        bkg_view->setBackgroundColor(color);
        bkg_view->resizableWithCapInsets(non_scale_edge);
        
        MGRect content_rect = bkg_view->getFrame();
        content_rect.x += non_scale_edge.left;
        content_rect.y += non_scale_edge.top;
        content_rect.width -= non_scale_edge.left+non_scale_edge.right;
        content_rect.height -= non_scale_edge.top+non_scale_edge.bottom;
        content_view->setFrame(content_rect);
    }
    
    void MIScaleView::drawself()
    {
        MIView::drawself();
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if(graphic_engine == NULL)
        {
            return;
        }
        
        if(should_reload_capture_image)
        {
            should_reload_capture_image = false;
            MGImage* image = getCaptureImage();
            if(image != NULL)
            {
                content_view->setBackgroundColor(MGColor::fromImage(image));
            }
        }
    }
    
    void MIScaleView::showWithCaptureCenter(const MGPoint &capture_center)
    {
        hide();
    
        // set real center
        MGPoint text_scale_view_center = capture_center;
        MGRectTransition gui_trans = GuiContainer::container->gui_root->view->getRectTransition();
        MGRectTransition gui_subtrans = GuiContainer::container->gui_root->view->getSubRectTransition();
        text_scale_view_center.x -= gui_trans.axis_x+gui_subtrans.axis_x;
        text_scale_view_center.y -= gui_trans.axis_y+gui_subtrans.axis_y;
        
        // update image
        updateCaptureImage(capture_center);
        
        // init frame
        detachFromSuperview();
        GuiContainer::container->gui_root->view->addSubview(this);
        text_scale_view_center.y -= 1.2f*m_frame.height;
        setCenter(text_scale_view_center);
        show();
    }
    
    void MIScaleView::updateCaptureImage(const MGPoint &capture_center)
    {
        float threshold = SCALE_VIEW_WILL_RELOAD_IMAGE_THREHOLD*MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
        float delta_x = this->capture_center.x-capture_center.x;
        float delta_y = this->capture_center.y-capture_center.y;
        if(fabs(delta_x) > threshold ||
           fabs(delta_y) > threshold)
        {
            AUTO_LOCK_THIS;
            this->should_reload_capture_image = true;
            this->capture_center = capture_center;
        }
    }
    
    MGImage* MIScaleView::getCaptureImage()
    {
        // init capture rect
        int screen_width = MGDevice::getScreenWidth();
        int screen_height = MGDevice::getScreenHeight();
        
        MGRect capture_rect = content_view->getBounds();
        assert(capture_rect.width < screen_width && capture_rect.height < screen_height && scale_value > 1.0f);
        
        capture_rect.width /= scale_value;
        capture_rect.height /= scale_value;
        capture_rect.x = MAXVALUE(capture_center.x-capture_rect.width/2.0f, 0.0f);
        capture_rect.y = MAXVALUE(capture_center.y-capture_rect.height/2.0f, 0.0f);
        if(capture_rect.x+capture_rect.width > screen_width)
        {
            capture_rect.x = screen_width-capture_rect.width;
        }
        if(capture_rect.y+capture_rect.height > screen_height)
        {
            capture_rect.y = screen_height-capture_rect.height;
        }
        
        // capture
        int capture_start_x = capture_rect.x;
        int capture_start_y = screen_height-(capture_rect.y+capture_rect.height);
        int capture_width = capture_rect.width;
        int capture_height = capture_rect.height;
        
        int line_width = capture_width*4;
        unsigned char* data = new unsigned char[line_width*capture_height];
        glReadPixels(capture_start_x, capture_start_y, capture_width, capture_height, GL_RGBA,GL_UNSIGNED_BYTE, data);
        
        char header[256];
        int x, y;
        
        sprintf(header, "P6\n%d %d\n255\n", capture_width, capture_height);
        int header_size = int(strlen(header));
        int output_line_size = capture_width*3;
        unsigned int buffer_size = header_size+output_line_size*capture_height;
        unsigned char* buffer = new unsigned char[buffer_size];
        memcpy(buffer, header, header_size);
        unsigned char* pt = buffer+header_size;
        unsigned char* line_src = data+line_width*(capture_height-1);
        for (y = 0; y < capture_height; ++y)
        {
            unsigned char* src = line_src;
            for(x = 0; x < capture_width; ++x)
            {
                memcpy(pt, src, 3);
                pt += 3;
                src += 4;
            }
            line_src -= line_width;
        }
        
        MGData* _data = MGData::dataWithBytesNoCopy(buffer, buffer_size);
        MGImage* image = MGImage::imageWithData(_data);
        
        delete_and_set_null(_data);
        delete_and_set_null(data);
        
        return image;
    }
}
