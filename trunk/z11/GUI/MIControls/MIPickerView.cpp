#include "MIPickerView.h"
#include "MILabel.h"
#include "MIImageView.h"
#include "MIScrollView.h"
#include "../../System/Device/MGDevice.h"
#include "../../System/Shared/MGLog.h"
#include "MITemplatePickerDelegate.h"
#include <stdio.h>
#include "../GestureRecognizer/MITapGestureRecognizer.h"
#include "../../System/Shared/Utility.h"

#define            PICKER_VIEW_CONNTENT_MARGIN                0.025f

namespace z11
{
#pragma mark - MIPickerViewImpl -
	class MIPickerViewImpl : public MIScrollView
	{
	public:
		MIPickerViewImpl()
		{
            class_name = "MIPickerViewImpl";
			m_component = 0;
			m_selected_row = 0;
			m_did_after_inertia_inScroll.sel_func = (SELFunc)&MIPickerViewImpl::bounceInScroll;
            
			tap_recognizer=NULL;
			tap_srcoll_enabled=true;
            _scroll_view_flags.vertical_bounce_enabled = 1;
            _scroll_view_flags.horizontal_bounce_enabled = 0;
            _scroll_view_flags.vertical_indicator_show = 0;
            _scroll_view_flags.horizontal_indicator_show = 0;
            _scroll_view_flags.inertia_enable = 1;
            _scroll_view_flags.decelarate_enable = 1;
            _scroll_view_flags.zoom_enable = 0;
            _scroll_view_flags.vertical_scroll_enabled = 1;
            _scroll_view_flags.horizontal_scroll_enabled = 0;
            _scroll_view_flags.passing_vertical_scroll_enable = 0;
            _scroll_view_flags.passing_horizontal_scroll_enable = 0;
            _scroll_view_flags.hide_keyboard_when_scrolling = 1;
		}
        
		~MIPickerViewImpl()
		{
			delete_and_set_null(tap_recognizer);
		}
        
        virtual void initWithFrame(MGRect frame)
		{
			MIScrollView::initWithFrame(frame);
			setBackgroundColor(MGColor::fromRGBA(255, 255, 255));

			tap_recognizer = new MITapGestureRecognizer();
			tap_recognizer->allowableMovement=6.0;
			tap_recognizer->numberOfTapsRequired=1;
			tap_recognizer->numberOfTouchesRequired=1;
			SELHandler _handler;
			_handler.sender = tap_recognizer;
			_handler.sel_func = (SELFunc)&MIPickerViewImpl::tapCall;
			tap_recognizer->initWithTarget(this, _handler);
			addGestureRecognizer(tap_recognizer);
		}
        
		virtual void drawself()
		{
			MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
            if(graphic_engine == NULL)
            {
                return;
            }
            MIPickerView* picker_view = dynamic_cast<MIPickerView*>(parent());
            if(picker_view == NULL)
            {
                return;
            }
            
            if(picker_view->image_shadow->isHide())
            {
                MGRect draw_rect = m_bounds;
                graphic_engine->drawFilledLightedRect(draw_rect, RGBAColorMake(255, 255, 255, m_alpha*255), 30, 150);
            }
            
            if(picker_view->image_glass->isHide())
            {
                MGRect draw_rect = picker_view->image_glass->getBounds();
                draw_rect.x = m_bounds.x;
                draw_rect.width = m_bounds.width;
                graphic_engine->drawFilledLightedRect(draw_rect, RGBAColorMake(180, 180, 255, m_alpha*255), 60, 120);
            }
		}
		virtual void scrollViewDraggingWithContentOffset(const MGPoint &targetContentOffset, const MGPoint &velocity)
		{
			MGPoint content_offset = MGPointMake(sub_transition.axis_x, sub_transition.axis_y);
			float scroll_offset_up = 0;
			float scroll_offset_down = 0;
			if(velocity.y > 1e-6)
			{
				scroll_offset_up = velocity.y*5;
			}
			else if(velocity.y < -1e-6)
			{
				scroll_offset_down = velocity.y*5;
			}
			else
			{
				content_offset = targetContentOffset;
			}

			for (size_t i = 0; i < m_subviews.size(); i++)
			{
				MGRect sub_frame = m_subviews[i]->getFrame();
				if (sub_frame.y+content_offset.y > -sub_frame.height-scroll_offset_up &&
					sub_frame.y+sub_frame.height+content_offset.y <= m_frame.height+sub_frame.height-scroll_offset_down)
				{
					m_subviews[i]->show();
				}
				else
				{
					m_subviews[i]->hide();
				}
			}
		}
        
		void bounceInScroll(MGObject* _sender)
		{
			//MGRectTransition transition_now =  sub_transition;
			MIPickerView* parent_view = static_cast<MIPickerView*>(parent());
			MGSize cell_size = parent_view->rowSizeForComponent(m_component);
			int row_now = 0;
			if(sub_transition.axis_y > m_content_size.top + displacement_size.top)
            {
				//transition_now.axis_y = m_content_size.top + displacement_size.top;
			}
			else if(m_frame.height - sub_transition.axis_y > m_content_size.bottom + displacement_size.bottom)
            {
				//transition_now.axis_y = m_frame.height - m_content_size.bottom - displacement_size.bottom;
				row_now = parent_view->numberOfRowsInComponent(m_component)-1;
			}
			else
			{
				int didselectrow = ((long)sub_transition.axis_y-cell_size.height/2)/cell_size.height;
				//transition_now.axis_y = didselectrow*cell_size.height;
				row_now = -didselectrow;
			}
			parent_view->changeSelectedRowInComponent(row_now, m_component);			
		}
		void tapCall(MGObject *_sender)
		{
			if(!tap_srcoll_enabled)
				return;
			MIPickerView* parent_view = static_cast<MIPickerView*>(parent());
			MGSize cell_size = parent_view->rowSizeForComponent(m_component);
			MGRectTransition transition_now =  sub_transition;
			float _tmp = transition_now.axis_y/cell_size.height;
			if(_tmp<0)
				_tmp=-_tmp;
			if(_tmp<0.1)
				setSubRectTransition(MGRectTransitionMake(transition_now.axis_x,0,transition_now.axis_z));
			transition_now =  sub_transition;
			float d=transition_now.axis_y/cell_size.height;
			string tmp = Utility::BuiltInTypeToString<float>(d);
			if(tmp.find(".")!=string::npos)
				return;
			MITapGestureRecognizer *tap = dynamic_cast<MITapGestureRecognizer*>(_sender);	
			int now_row = Utility::BuiltInTypeFromString<int>(tmp);
			if(now_row<0)
				now_row=-now_row;
			MGPoint tap_point = tap->getLastTap().locationInView(this);  
			MGPoint mid = MGPointMake(m_center.y-cell_size.height/2,m_center.y+cell_size.height/2);		
			if(tap_point.y>=mid.x&&tap_point.y<=mid.y){
				return;
			}
			int delta_row =0;
			if(tap_point.y<mid.x){
				delta_row = (int) (mid.x-tap_point.y)/cell_size.height ;
				delta_row++;
				transition_now.axis_y +=((float) delta_row)*cell_size.height;
				now_row = now_row-delta_row;
			}
			else if(tap_point.y>mid.y){
				delta_row=(int) (tap_point.y-mid.y)/cell_size.height ;
				delta_row++;
				transition_now.axis_y -= delta_row*cell_size.height;
				now_row = now_row+delta_row;
			}
			float max_trans = parent_view->numberOfRowsInComponent(m_component)-1.0;
			max_trans *=(cell_size.height);
			if(transition_now.axis_y<0&&(-transition_now.axis_y)>max_trans)
				return;
			if(now_row>=0&&now_row<parent_view->numberOfRowsInComponent(m_component)){
				tap_srcoll_enabled=false;
				m_selected_row=now_row;
				char pointer[128];
				sprintf(pointer, "%p", tap_recognizer);
				string str_pointer = pointer;
				MIView::beginAnimation("Picker View Tap In Scroll." + str_pointer);
				MIView::setAnimationDuration(0.2);
				setSubRectTransition(transition_now);
				SELHandler _handler;
				_handler.sel_func = (SELFunc)&MIPickerViewImpl::AfterTapCall;
				_handler.sender=tap_recognizer;
				MIView::setAnimationDidStopSelector(this,_handler);
				MIView::commitAnimation();
			}
		}
		void AfterTapCall(MGObject* _sender)
		{
			MIPickerView* parent_view = static_cast<MIPickerView*>(parent());
			parent_view->delegate->didSelectRowInComponent(parent_view,m_selected_row,m_component);
			tap_srcoll_enabled=true;
		}
	public:
		int m_component;
		int m_selected_row;
		bool tap_srcoll_enabled;
		MITapGestureRecognizer *tap_recognizer;
	};
    
#pragma mark - constructor -
	MIPickerView::MIPickerView()
	{
        class_name = "MIPickerView";
        _view_flags.clip_subviews = 1;
        
		dataSource = NULL;
		delegate = NULL;
		m_showsSelectionIndicator = false;
        
        // init border
        float border_value = PICKER_VIEW_CONNTENT_MARGIN*MINVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight());
        border_rect = MGRect2(border_value, border_value, border_value, border_value);
        
        // shadow and glass
        image_shadow = new MIImageView();
        image_shadow->_view_flags.user_interaction_enabled = 0;
        image_shadow->hide();
        image_glass = new MIImageView();
        image_glass->_view_flags.user_interaction_enabled = 0;
        image_glass->hide();
	}

	MIPickerView::~MIPickerView()
	{
		MITemplatePickerDataSource *template_data_src = dynamic_cast<MITemplatePickerDataSource*>(dataSource);
		MITemplatePickerDelegate *template_delegate = dynamic_cast<MITemplatePickerDelegate*>(delegate);
		dataSource = NULL;
		delete_and_set_null(template_data_src);
		delegate = NULL;
		delete_and_set_null(template_delegate);
	}
    
#pragma mark - init -
    void MIPickerView::initWithFrame(MGRect frame)
	{
		MIView::initWithFrame(frame);
        
        image_shadow->initWithFrame(MGRectMake(border_rect.left, border_rect.top, frame.width-border_rect.left-border_rect.right, frame.height-border_rect.top-border_rect.bottom));
        addSubview(image_shadow);
        
        image_glass->initWithFrame(MGRectMake(-1.0f, -1.0f, 1.0f, 1.0f));
        addSubview(image_glass);
        
		reloadAllComponents();
	}
    
    void MIPickerView::drawself()
	{
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if(graphic_engine == NULL)
        {
            return;
        }
        
        // draw background
        if(m_background_color == NULL)
        {
            MGGraphicEngine* engine = MGGraphicEngine::sharedGraphicEngine();
            MGRect rect = m_bounds;
            rect.height /= 2.0f;
            engine->drawFilledLightedRect(rect, RGBAColorMake(135, 141, 157, m_alpha*255), 90, 30);
            rect.y+=rect.height;
            engine->drawFilledLightedRect(rect, RGBAColorMake(40, 40, 50, m_alpha*255), 90, 30);
        }
		else
        {
            MIControl::drawself();
        }
	}
    
    void MIPickerView::setShadowColor(MGColor* color)
    {
        image_shadow->setBackgroundColor(color);
        if(color == NULL)
        {
            image_shadow->hide();
        }
        else
        {
            image_shadow->show();
        }
    }
    
    void MIPickerView::setGlassColor(MGColor* color)
    {
        image_glass->setBackgroundColor(color);
        if(color == NULL)
        {
            image_glass->hide();
        }
        else
        {
            image_glass->show();
        }
    }
    
    void MIPickerView::setBorder(const MGRect2 &border_rect)
    {
        this->border_rect = border_rect;
        if(image_shadow->_view_flags.view_initialized)
        {
            image_shadow->setFrame(MGRectMake(border_rect.left, border_rect.top, m_frame.width-border_rect.left-border_rect.right, m_frame.height-border_rect.top-border_rect.bottom));
        }
    }
    
    const MGRect2 & MIPickerView::getBorder()
    {
        return border_rect;
    }
    
#pragma mark - delegate -
    void MIPickerView::setDelegate(MIPickerViewDelegate* _delegate)
	{
		delegate = _delegate;
	}
    
	void MIPickerView::setDataSource(MIPickerViewDataSource* _src)
	{
		dataSource = _src;
	}

    MIPickerViewDelegate* MIPickerView::getDelegate()
	{
		return delegate;
	}
    
	MIPickerViewDataSource* MIPickerView::getDataSource()
	{
		return dataSource;
	}

#pragma mark - rows -
	int MIPickerView::numberOfComponents()
	{
        int ret = 0;
		if(dataSource != NULL)
		{
			ret = dataSource->numberOfComponentsInPickerView(this);
		}
        return ret;
	}

	int MIPickerView::numberOfRowsInComponent(int component)
	{
        int ret = 0;
		if(dataSource != NULL)
		{
			ret = dataSource->numberOfRowsInComponent(this, component);
		}
        return ret;
	}

	MGSize MIPickerView::rowSizeForComponent(int component)
	{
		MGSize size;
		if(delegate != NULL)
		{
			size.width = delegate->widthForComponent(this, component);
			size.height = delegate->rowHeightForComponent(this, component);
		}
		return size;
	}
    
    MIView* MIPickerView::viewForRowForComponent(int row, int component)
	{
		return m_components[component]->m_subviews[row];
	}
    
#pragma mark - load rows -
	void MIPickerView::reloadAllComponents()
	{
        assert(dataSource != NULL && delegate != NULL);
        
		for(size_t i = 0; i < m_components.size(); ++i)
		{
			m_components[i]->detachFromSuperview();
			delete_and_set_null(m_components[i]);
		}
		m_components.clear();
        
        int row_num = numberOfComponents();
		for(int i = 0; i < row_num; ++i)
		{
			reloadComponent(i);
		}
        
        if(row_num > 0)
        {
            float glass_height = delegate->rowHeightForComponent(this, 0);
            float glass_start_y = (m_frame.height-glass_height)/2.0f;
            image_glass->setFrame(MGRectMake(border_rect.left, glass_start_y, m_frame.width-border_rect.left-border_rect.right, glass_height));
        }
        else
        {
            image_glass->setFrame(MGRectMake(-1.0f, -1.0f, 1.0f, 1.0f));
        }
        image_glass->detachFromSuperview();
        addSubview(image_glass);
	}

	void MIPickerView::reloadComponent(int component)
	{
		assert(dataSource != NULL && delegate != NULL);
        // component should not be bigger than size of m_components
        if (m_components.size() < component) {
            return;
        }

        MIPickerViewImpl* impl = new MIPickerViewImpl();
        
        if(m_components.size() == component)
        {
            // load
            MGRect current_content_size = getCurrentComponentsRect();
            MGRect frame;
            frame.x = current_content_size.x+current_content_size.width;
            frame.y = current_content_size.y;
            frame.width = delegate->widthForComponent(this, component);
            frame.height = current_content_size.height;
            
            if(component == dataSource->numberOfComponentsInPickerView(this)-1)
            {
                float width_remain = m_bounds.width-current_content_size.width-border_rect.left-border_rect.right;
                frame.width = width_remain;
            }
            
            impl->initWithFrame(frame);
            impl->m_selected_row = 0;
            m_components.push_back(impl);
        }
        else
        {   // reload
            impl->m_selected_row = 0;
            MGRect frame = m_components[component]->getFrame();
            m_components[component]->detachFromSuperview();
            delete_and_set_null(m_components[component]);
            impl->initWithFrame(frame);
            m_components[component] = impl;
        }
        impl->m_component = component;
        float row_height = delegate->rowHeightForComponent(this, component);
        float scroll_total_height = row_height*numberOfRowsInComponent(component);
        
        MGRect m_components_rect = image_shadow->getFrame();
        impl->displacement_size.bottom = (m_components_rect.height-row_height)/2.0f;
        if(scroll_total_height < m_components_rect.height/2.0f)
        {
            impl->displacement_size.bottom -= m_components_rect.height/2.0f-scroll_total_height;
        }
        
        addSubview(impl);
        
        float label_start_y = (m_bounds.height-delegate->rowHeightForComponent(this, component))/2.0f-border_rect.top;
        for (int row = 0; row < numberOfRowsInComponent(component); ++row)
        {
            MILabel* label = new MILabel();
            label->initWithFrame(MGRectMake(0.0f, label_start_y+row*delegate->rowHeightForComponent(this, component), impl->getFrame().width, delegate->rowHeightForComponent(this, component)));
            label->setText(delegate->titleForRowForComponent(this, row, component));
            label->setTextAlignment(MITextAlignmentCenterMiddle);
            label->setFontSizeAndColor(label->getFrame().height*0.42f);
            impl->addSubview(label);
        }
	}

	MGRect MIPickerView::getCurrentComponentsRect()
	{
		MGRect rect = MGRectMake(border_rect.left, border_rect.top, 0.0f, m_bounds.height-border_rect.top-border_rect.bottom);

		for(uint_32 component = 0; component < m_components.size(); ++component)
		{
			rect.width += delegate->widthForComponent(this, component)+1;
		}

		return rect;
	}

#pragma mark - select row -
    void MIPickerView::selectRowInComponent(int row, int component, bool animated)
	{
		changeSelectedRowInComponent(row, component, animated);
	}
    
	int MIPickerView::selectedRowInComponent(int component)
	{
		return m_components[component]->m_selected_row;
	}
    
	void MIPickerView::didSelectRowInComponent(MIPickerView* picker_view, int row, int component)
	{
		EventsMapIterator it = events_dispatch_map.find(PICKER_VIEW_DID_SELECT_ROW);
		if (it != events_dispatch_map.end())
        {
			assert(it->second.view_controller != NULL);
			assert(it->second.event_func != NULL);
			EventFunc func = it->second.event_func;
			MIPickerHander did_select_row_in_component(this, row, component);
			((it->second.view_controller)->*func)(&did_select_row_in_component);
		}
		else if(delegate != NULL)
		{
			delegate->didSelectRowInComponent(picker_view, row, component);
		}
	}

	void MIPickerView::changeSelectedRowInComponent(int row, int component, bool animated)
	{
        if (component >= m_components.size())
        {
            return;
        }
		MIPickerViewImpl* impl = m_components[component];
		MGRectTransition transition_now = impl->getSubRectTransition();
		transition_now.axis_y = impl->displacement_size.top-row*rowSizeForComponent(component).height;
		float duration = 0.2f;
		if(transition_now == impl->getSubRectTransition())
        {
			duration = 0.0f;
		}
		if(animated)
		{
			char pointer[128];
			sprintf(pointer, "%p", impl);
			string str_pointer = pointer;
			MIView::beginAnimation("Picker View Bounce In Scroll." + str_pointer);
			MIView::setAnimationDuration(duration);
			impl->setSubRectTransition(transition_now);
			MIView::commitAnimation();
		}
		else
		{
			impl->setSubRectTransition(transition_now);
		}
        
		MGPoint target_offset = MGPointMake(transition_now.axis_x,transition_now.axis_y);
        impl->scrollViewDraggingWithContentOffset(target_offset,MGPoint());
        m_components[component]->m_selected_row = row;
        didSelectRowInComponent(this, row, component);
	}
}
