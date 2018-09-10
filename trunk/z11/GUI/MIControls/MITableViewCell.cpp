//
//  MITableViewCell.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-8-17.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MITableViewCell.h"
#include "MITableView.h"
#include "../../GraphicCore/MGImage.h"
#include "../../System/Shared/MGLog.h"

namespace z11 {
	MITableViewCell::~MITableViewCell()
	{
		m_text_label = NULL;
	}
    MITableViewCell::MITableViewCell()
	{
        class_name = "MITableViewCell";
		enableClick();
		m_sytle=MITableViewCellStyleDefault;
		init();
    }
    MITableViewCell::MITableViewCell(const string & reuse_identifier)
		:m_reuse_identifier(reuse_identifier)
    {
		class_name = "MITableViewCell";
		#pragma message ("MITableViewCell::MITableViewCell(const string & reuse_identifier) has been Deprecated")
		m_sytle=MITableViewCellStyleDefault;
		init();
    }

	MITableViewCell::MITableViewCell( MITableViewCellStyle style,const string &reuse_identifier )
		:m_sytle(style),m_reuse_identifier(reuse_identifier)
	{
		class_name = "MITableViewCell";
		init();
	}

	void MITableViewCell::init()
	{
		m_background_color = MGColor::fromColorRef(RGBAColorRef::white());
		m_is_selected = false;
		m_is_highlighted = false;
		m_text_label = NULL;
		m_detail_text_label=NULL;
		m_image_view=NULL;
		m_accessoryType=MITableViewCellAccessoryNone;
		m_accessoryView=NULL;
		m_editingAccessoryType=MITableViewCellAccessoryNone;
	}

    void MITableViewCell::initWithFrame(MGRect frame)
    {
        MIView::initWithFrame(frame);
        m_text_label = new MILabel();
        m_text_label->initWithFrame(MGRectMake(0,0,frame.width,frame.height));
        m_text_label->setBackgroundAlphaInByte(0);
        m_text_label->autoClean();
        addSubview(m_text_label);
    }
    
    void MITableViewCell::drawself()
    {
		MITableView* table_view = static_cast<MITableView*>(parent());
		if (table_view == NULL) {
			return;
		}
        MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();
        if (graphic_engine == NULL) {
            return;
        }
        RGBAColorRef color_ref;
        // Cell不应有边框 //
		RGBAColorRef border_ref = RGBAColorRef(200,200,200);
        if (m_background_color != NULL) {
            color_ref = m_background_color->m_color;
            color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
        }else{
            color_ref = RGBAColorMake(0, 0, 0, (byte_8)(m_alpha * 255));
        }
        if (m_background_color != NULL && m_background_color->m_image != NULL) {
            if (m_is_selected && m_click_allowed) {
                color_ref = m_action_blend;
//                color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
            }
            m_background_color->m_image->sendDataToContext();
            graphic_engine->drawImage(m_background_color->m_image, m_bounds, color_ref, draw_image_mode);
        }else{
            //bool need_rect = (table_view->m_separator_style != MITableViewCellSeparatorStyleNone);
            RGBAColorRef blend_ref = RGBAColorMake(255, 255, 255, 255);
            if (m_is_selected && m_click_allowed) {
                blend_ref = m_action_blend;
            }
			if (table_view->style() == MITableViewStyleGrouped) {
				switch (positionInSection()) {
					case MITableViewCellPositionTitle:
						graphic_engine->drawFilledRect(m_bounds, color_ref, blend_ref);
						break;
					case MITableViewCellPositionHead:
						graphic_engine->drawFilledRoundAbovedRect(m_bounds, color_ref, _DEFAULT_ROUND_RATE_, blend_ref);
						graphic_engine->drawRoundAbovedRect(m_bounds, border_ref);
						break;
					case MITableViewCellPositionMiddle:
						graphic_engine->drawFilledRect(m_bounds, color_ref, blend_ref);
						graphic_engine->drawRect(m_bounds, border_ref);
						break;
					case MITableViewCellPositionTail:
						graphic_engine->drawFilledRoundBelowedRect(m_bounds, color_ref, _DEFAULT_ROUND_RATE_, blend_ref);
						graphic_engine->drawRoundBelowedRect(m_bounds, border_ref);
						break;
					case MITableViewCellPositionHeadAndTail:
						graphic_engine->drawFilledRoundRect(m_bounds, color_ref, _DEFAULT_ROUND_RATE_, blend_ref);
						graphic_engine->drawRoundRect(m_bounds, border_ref);
						break;
					default:
						break;
				}
			} else if (table_view->style() == MITableViewStylePlain) {
				switch (positionInSection()) {
					case MITableViewCellPositionTitle:
						graphic_engine->drawFilledRect(m_bounds, color_ref, blend_ref);
						break;
					default:
						graphic_engine->drawFilledRect(m_bounds, color_ref, blend_ref);
						//graphic_engine->drawRect(m_bounds, border_ref);
						break;
				}
			} else {
				graphic_engine->drawFilledRect(m_bounds, color_ref, blend_ref);
			}
        }
    }
    
    // touch event //
    //触摸事件的响应 
    MIEventHandleState MITableViewCell::touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        m_is_selected = true;
        m_is_highlighted = true;
        return MIEventHandleStateDone;
    }
    MIEventHandleState MITableViewCell::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        return MIView::touchesMoved(touches, event);
    }
    MIEventHandleState MITableViewCell::touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        m_is_selected = false;
        m_is_highlighted = false;
        if (m_superview != NULL) {
            MGIndexPath path = ((MITableView*)m_superview)->indexPathForCell(this);
            int row = path.row();
            if (row >= 0 && ((MITableView*)m_superview)->m_tableview_delegate != NULL) {
                path = ((MITableView*)m_superview)->m_tableview_delegate->willSelectRowAtIndexPath((MITableView*)m_superview, path);
                row = path.row();
                if (row >= 0) {
                    ((MITableView*)m_superview)->m_tableview_delegate->didSelectRowAtIndexPath((MITableView*)m_superview, path);
                }
            }
        }
        return MIEventHandleStateDone;
    }
    MIEventHandleState MITableViewCell::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        m_is_selected = false;
        m_is_highlighted = false;
        return MIEventHandleStatePass;
    }
    //事件响应 
    MIEventHandleState MITableViewCell::handleEvent(const MIEvent* const event)
    {
        return MIEventHandleStateDone;
    }

	MILabel * MITableViewCell::textLabel() const
	{
		assert(m_text_label!=NULL);
		return m_text_label;
	}

	MILabel * MITableViewCell::detailTextLabel() const
	{
		assert(m_detail_text_label!=NULL);
		return m_detail_text_label;
	}

	MIImageView * MITableViewCell::imageView() const
	{
		assert(m_image_view!=NULL);
		return m_image_view;
	}

	MIView		* MITableViewCell::contentView() 
	{
		return this;
	}

	bool MITableViewCell::isSelected()
	{
		return m_is_selected;
	}

	void MITableViewCell::setSelected( bool selected )
	{
		m_is_selected=selected;
	}

	bool MITableViewCell::isHighlighted()
	{
		return m_is_highlighted;
	}

	void MITableViewCell::setHighlighted( bool highlighted )
	{
		m_is_highlighted=highlighted;
	}

	bool MITableViewCell::isEditing()
	{
		return m_editing;
	}

	void MITableViewCell::setEditing( bool editing )
	{
		m_editing=editing;
	}

	z11::MITableViewCellEditingStyle MITableViewCell::editingStyle()
	{
		return MITableViewCellEditingStyleNone;
	}

	bool MITableViewCell::showingDeleteConfirmation()
	{
		return false;
	}

	bool MITableViewCell::showsReorderControl()
	{
		return false;
	}

	void MITableViewCell::willTransitionToState( MITableViewCellStateMask state )
	{

	}

	void MITableViewCell::didTransitionToState( MITableViewCellStateMask state )
	{

	}

	const string & MITableViewCell::reuseIdentifier()
	{
		return m_reuse_identifier;
	}

	void MITableViewCell::prepareForReuse()
	{

	}

	MITableViewCellPosition MITableViewCell::positionInSection()
	{
		return m_position_in_section;
	}

	void MITableViewCell::setPositionInSection(MITableViewCellPosition position)
	{
		m_position_in_section = position;
	}

	void MITableViewCell::enableClick()
	{
		m_click_allowed = true;
	}

	void MITableViewCell::disableClick()
	{
		m_click_allowed = false;
	}
















} //namespace z11