//
//  MITableViewCell.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-8-17.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#pragma once

#include "MIControl.h"
#include "MILabel.h"
#include "MIImageView.h"

namespace z11 {

	typedef enum {
		MITableViewCellStyleDefault,
		MITableViewCellStyleValue1,
		MITableViewCellStyleValue2,
		MITableViewCellStyleSubtitle,
		MITableViewCellStyleNone
	} MITableViewCellStyle;


	typedef enum {
		MITableViewCellSeparatorStyleNone,
		MITableViewCellSeparatorStyleSingleLine,
		MITableViewCellSeparatorStyleSingleLineEtched
	} MITableViewCellSeparatorStyle;


	typedef enum {
		MITableViewCellAccessoryNone,
		MITableViewCellAccessoryDisclosureIndicator,
		MITableViewCellAccessoryDetailDisclosureButton,
		MITableViewCellAccessoryCheckmark
	} MITableViewCellAccessoryType;

	typedef enum {
		MITableViewCellSelectionStyleNone,
		MITableViewCellSelectionStyleBlue,
		MITableViewCellSelectionStyleGray
	} MITableViewCellSelectionStyle;

	typedef enum {
		MITableViewCellEditingStyleNone,
		MITableViewCellEditingStyleDelete,
		MITableViewCellEditingStyleInsert
	} MITableViewCellEditingStyle;

	typedef enum {
		MITableViewCellPositionTitle,
		MITableViewCellPositionHead,
		MITableViewCellPositionMiddle,
		MITableViewCellPositionTail,
		MITableViewCellPositionHeadAndTail,
	} MITableViewCellPosition;

	typedef uint_32 MITableViewCellStateMask;

    class MITableViewCell : public MIControl
    {
	//Initializing a UITableViewCell Object
    public:
        MITableViewCell();
		MITableViewCell(MITableViewCellStyle style,const string &reuseIdentifier);
        virtual ~MITableViewCell();
    public:
        const string & reuseIdentifier();
		void prepareForReuse();
    private:
        string m_reuse_identifier;
		MITableViewCellStyle m_sytle;
		void init();


    // Table View Cell Content //
    public:
		MILabel *textLabel() const;		//retain
		MILabel *detailTextLabel() const;	//retain
	private:
		MILabel* m_text_label;
		MILabel* m_detail_text_label;
	
	// Managing Images as Cell Content //
	public:
		MIImageView *imageView() const;	//retain
	private:
		MIImageView *m_image_view;
	
    //Accessing Views of the Cell Object //
	public:
		MIView		*contentView() ;
		//backgroundView  property
		//selectedBackgroundView  property
		//multipleSelectionBackgroundView  property

	//Managing Accessory Views //
	public:
		MITableViewCellAccessoryType m_accessoryType;
		MIView	*m_accessoryView;	//retain
		MITableViewCellAccessoryType m_editingAccessoryType;
		MIView *m_editingAccessoryView; //retain

	//Managing Cell Selection and Highlighting //
	public:
		bool isSelected();
		MITableViewCellSelectionStyle m_selectionStyle;
		void setSelected(bool selected);
		bool isHighlighted();
		void setHighlighted(bool highlighted );
	private:
		bool m_is_selected;
		bool m_is_highlighted;

	//Editing the Cell //
	public:
		bool isEditing();
		void setEditing(bool editing );
		MITableViewCellEditingStyle editingStyle();
		bool showingDeleteConfirmation();
		bool showsReorderControl();
	private:
		bool m_editing;
		bool m_editing_style;

	//Adjusting to State Transitions//
	public:
		void willTransitionToState(MITableViewCellStateMask state);
		void didTransitionToState(MITableViewCellStateMask state);

	//Managing Content Indentation //
	public:
		int_32 m_indentationLevel;
		float m_indentationWidth;
		bool m_shouldIndentWhileEditing;

	//Position in Section //
	public:
		MITableViewCellPosition positionInSection();
		void setPositionInSection(MITableViewCellPosition position);
	private:
		MITableViewCellPosition m_position_in_section;

	//Swith allowed to click//
	public:
		virtual void enableClick();
		virtual void disableClick();
	private:
		bool m_click_allowed;
    public:
        virtual void initWithFrame(MGRect frame);
        virtual void drawself();
    public:
        MIRESPONDER_TOUCH_DECLARE;

	//Deprecated 
		MITableViewCell(const string & reuse_identifier);
    };
} //namespace MITableViewCell