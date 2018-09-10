//
//  AlertDialog.cpp
//  WaterMobileGIS
//
//  Created by MacOSDevelop on 13-1-22.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//
#include "MIAlertDialog.h"
#include "ImageResizableButton.h"
#include "../MIControls/MILabel.h"
#include "../MIControls/MITextView.h"
#include "../GUIContainer/GUIContainer.h"
#include "../../GraphicCore/MGFont.h"

#define       ALERT_DIALOG_INIT_TEXT                      "============ALERT_DIALOG_INIT_TEXT============"

#define       ALERT_DIALOG_CONTENT_WIDTH                  0.72f
#define       ALERT_DIALOG_BUTTON_BETWEEN                 0.02f

#define       ALERT_DIALOG_BUTTON_HEIGHT                  0.062f

#define       ALERT_DIALOG_TITLE_TO_TOP                   0.018f
#define       ALERT_DIALOG_TEXT_TO_TITLE                  0.018f
#define       ALERT_DIALOG_BUTTON_TO_TEXT                 0.018f
#define       ALERT_DIALOG_BOTTOM_TO_BUTTON               0.015f

#define       ALERT_DIALOG_TITLE_FONT_SIZE                0.030f
#define       ALERT_DIALOG_TEXT_FONT_SIZE                 0.025f


namespace z11
{
    MIAlertDialog* MIAlertDialog::alert_dialog = NULL;
    float MIAlertDialog::text_font_multiple = 1.0f;

    class MIAlertDialogDelegate : public MIViewController
	{
	public:
		MIAlertDialogDelegate()
		{
			class_name = "AlertDialogDelegate";
            positive_selected = false;
		}
        
		virtual ~MIAlertDialogDelegate()
		{
		}
        
		void positiveCall(MIObject* _sender)
		{
            positive_selected = true;
			MIButton *btn = dynamic_cast<MIButton*>(_sender);
			btn->titleLabel()->getFont()->setColor(RGBAColorMake(0,122,255));
            GuiContainer::container->foreignerSDKApiCall("remove top blend");
            startAnimation();
		}
        
        void negativeCall(MIObject* _sender)
        {
            positive_selected = false;
			MIButton *btn = dynamic_cast<MIButton*>(_sender);
			btn->titleLabel()->getFont()->setColor(RGBAColorMake(0,122,255));
            GuiContainer::container->foreignerSDKApiCall("remove top blend");
            startAnimation();
        }
        void buttonDown(MIObject* _sender)
		{
			MIButton *btn = dynamic_cast<MIButton*>(_sender);
			btn->titleLabel()->getFont()->setColor(RGBAColorRef::white());
		}
		void buttonUpOutside(MIObject* _sender)
		{
			MIButton *btn = dynamic_cast<MIButton*>(_sender);
			btn->titleLabel()->getFont()->setColor(RGBAColorMake(0,122,255));
		}
        void startAnimation()
        {
            MIAlertDialog* _view = dynamic_cast<MIAlertDialog*>(view);
            
            bool start_to_show = _view->isHide();
            float animation_duration = 0.24f;
            float content_show_duration = 0.70f;
            float alpha_in_black = 0.25;
            string animation_name_bkg_show = "AlertDialogDelegate::startAnimation: bkg_view";
            string animation_name_content_view = "AlertDialogDelegate::startAnimation: content_view";
            string animation_name_hide = "AlertDialogDelegate::startAnimation: bkg_view+content_view";
            if(start_to_show)
            {
                MIView::stopAnimation(animation_name_hide);
                // bkg_view
                _view->show();
                GuiContainer::container->gui_root->view->moveSubviewToBack(_view);
                _view->black_bkg_view->setAlpha(0.0f);
                MIView::beginAnimation(animation_name_bkg_show);
                MIView::setAnimationDuration(animation_duration);
                MIView::setAnimationCurve(MIViewAnimationOptionCurveLinear);
                _view->black_bkg_view->setAlpha(alpha_in_black);
                MIView::commitAnimation();
                
                // content_view
				if(_view->m_os_type!=MIAlertDialogDataOSTypeIOS7)
				{
					_view->content_view->setAlpha(1.0f);
					_view->content_view->setRectScale(MGRectScaleMake(0.6f, 0.6f, 1.0f));
				}
				else
				{
					_view->content_view->setAlpha(0);
					_view->content_view->setRectScale(MGRectScaleMake(1.1f, 1.1f, 1.0f));
				}
                MIView::beginAnimation(animation_name_content_view);
				if(_view->m_os_type!=MIAlertDialogDataOSTypeIOS7)
				{
					MIView::setAnimationDuration(content_show_duration);
					MIView::setAnimationCurve(MIViewAnimationOptionCurveDampingOnce);
				}
				else
				{
					MIView::setAnimationDuration(0.2f);
					MIView::setAnimationCurve(MIViewAnimationOptionCurveLinear);
				}
				if(_view->m_os_type!=MIAlertDialogDataOSTypeIOS7)
					_view->content_view->setRectScale(MGRectScaleMake(1.0f, 1.0f, 1.0f));
				else
				{
					_view->content_view->setAlpha(1);
					_view->content_view->setRectScale(MGRectScaleMake(1.0f, 1.0f, 1.0f));
				}
                MIView::commitAnimation();
            }
            else
            {
                GuiContainer::container->gui_root->view->_view_flags.user_interaction_enabled = 0;
                MIView::stopAnimation(animation_name_bkg_show);
                MIView::stopAnimation(animation_name_content_view);
                
                MIView::beginAnimation(animation_name_hide);
				if(_view->m_os_type!=MIAlertDialogDataOSTypeIOS7)
					MIView::setAnimationDuration(animation_duration);
				else
					MIView::setAnimationDuration(0.2f);
                MIView::setAnimationCurve(MIViewAnimationOptionCurveLinear);

				if(_view->m_os_type!=MIAlertDialogDataOSTypeIOS7)
					_view->content_view->setRectScale(MGRectScaleMake(0.0f, 0.0f, 1.0f));
				
                _view->content_view->setAlpha(0.0f);
                _view->black_bkg_view->setAlpha(0.0f);
                
                SELHandler handler;
                handler.sender = this;
                handler.sel_func = (SELFunc)&MIAlertDialogDelegate::animationDidStop;
                
                MIView::setAnimationDidStopSelector(this, handler);
                MIView::commitAnimation();
            }
        }
        
        void animationDidStop(MGObject* _sender)
        {
            GuiContainer::container->gui_root->view->_view_flags.user_interaction_enabled = 1;
            
            MIAlertDialog* _view = dynamic_cast<MIAlertDialog*>(view);
            if(_view->black_bkg_view->getAlpha() < 1e-3)
            {
                _view->hide();
                if(positive_selected)
                {
                    if(positive_responder != NULL)
                    {
                        (positive_responder->*positive_handler.sel_func)(positive_handler.sender);
                    }
                }
                else
                {
                    if(negative_responder != NULL)
                    {
                        (negative_responder->*negative_handler.sel_func)(negative_handler.sender);
                    }
                }
            }
            else
            {
                _view->show();
            }
        }
        
    public:
        bool positive_selected;
        SELHandler positive_handler;
        MIResponder* positive_responder;
        SELHandler negative_handler;
        MIResponder* negative_responder;
	};
    
    MIAlertDialogData::MIAlertDialogData()
    {
        title = ALERT_DIALOG_INIT_TEXT;
        message = ALERT_DIALOG_INIT_TEXT;
        font_color = RGBAColorRef::white();
        
        positive_text = ALERT_DIALOG_INIT_TEXT;
        negative_text = ALERT_DIALOG_INIT_TEXT;
        
        negative_responder = positive_responder = NULL;
    }
    
    void MIAlertDialogData::setTitle(const string &title)
    {
        this->title = title;
    }
    
    void MIAlertDialogData::setMessage(const string &message)
    {
        this->message = message;
    }
    
    void MIAlertDialogData::setFontColor(const RGBAColorRef &font_color)
    {
        this->font_color = font_color;
    }
    
    void MIAlertDialogData::setPositiveButton(const string &text, MIResponder* responder/* = NULL*/, SELHandler handler/* = SELHandler()*/)
    {
        positive_text = text;
        positive_responder = responder;
        positive_handler = handler;
    }
    
    void MIAlertDialogData::setNegativeButton(const string &text, MIResponder* responder/* = NULL*/, SELHandler handler/* = SELHandler()*/)
    {
        negative_text = text;
        negative_responder = responder;
        negative_handler = handler;
    }
    
    MIAlertDialog::MIAlertDialog()
    {
        delegate = new MIAlertDialogDelegate();
        delegate->view = this;
        
        title_label = NULL;
        message_text_view = NULL;
        positive_button = NULL;
        negative_button = NULL;
		horizontal_line_view = NULL;
		vertical_line_view = NULL;
    }
    
    MIAlertDialog::~MIAlertDialog()
    {
		if (delegate->view == this)
			delegate->view = NULL;
        delete_and_set_null(delegate);
    }
    
    uint_32 MIAlertDialog::getTitleFontSize()
    {
        return ALERT_DIALOG_TITLE_FONT_SIZE*getHeightAdaptiveCoefficient()*(MAXVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight()));
    }
    
    uint_32 MIAlertDialog::getTextFontSize()
    {
        return ALERT_DIALOG_TEXT_FONT_SIZE*getHeightAdaptiveCoefficient()*(MAXVALUE(MGDevice::getScreenWidth(), MGDevice::getScreenHeight()));
    }
    
    void MIAlertDialog::initWithFrame(MGRect frame)
    {
        float screen_width = MGDevice::getScreenWidth();
        float screen_height = MGDevice::getScreenHeight();
        MIView::initWithFrame(MGRectMake(0.0f, 0.0f, screen_width, screen_height));
        MIAlertDialogDelegate* controller = dynamic_cast<MIAlertDialogDelegate*>(delegate);
        EventHandler handler = {controller, NULL};
        
        // init params
        float height_adaptive = getHeightAdaptiveCoefficient();
        float title_font_size = getTitleFontSize();
        float text_font_size = getTextFontSize()*text_font_multiple;
        
        float content_width = ALERT_DIALOG_CONTENT_WIDTH*MINVALUE(screen_width, screen_height);
        float content_start_x = (screen_width-content_width)/2.0f;
        float text_start_x = text_font_size;
        float common_height = ALERT_DIALOG_BUTTON_HEIGHT*height_adaptive*(MAXVALUE(screen_width, screen_height));
        if(m_os_type==MIAlertDialogDataOSTypeIOS7)
			common_height = 0.075*height_adaptive*(MAXVALUE(screen_width, screen_height));
        // bkg_view
        black_bkg_view = new MIImageView();
        black_bkg_view->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::black()));
        black_bkg_view->initWithFrame(MGRectMake(0.0f, 0.0f, screen_width, screen_height));
        addSubview(black_bkg_view);
        
        // content_view
        content_view = new MIImageView();
        content_view->initWithFrame(MGRectMake(content_start_x, 0.0f, content_width, screen_height));
        content_view->setBackgroundColor(MGColor::fromRGBA(0, 0, 0, 0));
        addSubview(content_view);
        
        // content_bkg_view
        content_bkg_view = new ImageResizableButton();
        content_bkg_view->_view_flags.user_interaction_enabled = 0;
        content_bkg_view->initWithFrame(MGRectMake(0.0f, 0.0f, content_width, screen_height));
        content_view->addSubview(content_bkg_view);
		if(m_os_type==MIAlertDialogDataOSTypeIOS7)
			content_bkg_view->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::white()));
		else
			content_bkg_view->setBackgroundColor(MGColor::fromImage(MGBundle::mainBundle()->drawablePathWithFileName("controls/alert_dialog/content_bkg.png")));
        // title_label
        title_label = new MILabel();
        title_label->setFontSizeAndColor(title_font_size, dialog.font_color);
        title_label->setTextAlignment(MITextAlignmentCenterMiddle);
        title_label->initWithFrame(MGRectMake(text_start_x, 0.0f, content_width-2*text_font_size, common_height));
        content_view->addSubview(title_label);
        
        // message_text_view
        initTextView();
        message_text_view->initWithFrame(MGRectMake(text_start_x, 0.0f, content_width-2*text_font_size, common_height));
        content_view->addSubview(message_text_view);
        
        // positive_button
        positive_button = new ImageResizableButton();
		if(m_os_type!=MIAlertDialogDataOSTypeIOS7)
			positive_button->setBackgroundColor(MGColor::fromImage(MGBundle::mainBundle()->drawablePathWithFileName("controls/alert_dialog/button_bkg.png")));
		else
		{
			positive_button->setDrawRoundMode(RoundRectLeftBottom);
			positive_button->setBackgroundColor(MGColor::fromRGBA(0,0,0,0));
		}
		positive_button->titleLabel()->setFontSizeAndColor(title_font_size, dialog.font_color);
        positive_button->titleLabel()->setTextAlignment(MITextAlignmentCenterMiddle);
        positive_button->initWithFrame(MGRectMake(0.0f, 0.0f, screen_width, common_height));
        content_view->addSubview(positive_button);
        handler.event_func = (EventFunc)&MIAlertDialogDelegate::positiveCall;
        positive_button->registerEventHandler(MIControlEventTouchUpInside, handler);
        
        // negative_button
        negative_button = new ImageResizableButton();
		if(m_os_type!=MIAlertDialogDataOSTypeIOS7)
			negative_button->setBackgroundColor(MGColor::fromImage(MGBundle::mainBundle()->drawablePathWithFileName("controls/alert_dialog/button_bkg.png")));
		else
		{
			negative_button->setDrawRoundMode(RoundRectRightBottom);
			negative_button->setBackgroundColor(MGColor::fromRGBA(0,0,0,0));
		}
		negative_button->titleLabel()->setFontSizeAndColor(title_font_size, dialog.font_color);
        negative_button->titleLabel()->setTextAlignment(MITextAlignmentCenterMiddle);
        negative_button->initWithFrame(MGRectMake(0.0f, 0.0f, screen_width, common_height));
        content_view->addSubview(negative_button);
        handler.event_func = (EventFunc)&MIAlertDialogDelegate::negativeCall;
        negative_button->registerEventHandler(MIControlEventTouchUpInside, handler);
        
		if(m_os_type==MIAlertDialogDataOSTypeIOS7)
		{
			handler.event_func = (EventFunc)&MIAlertDialogDelegate::buttonDown;
			positive_button->registerEventHandler(MIControlEventTouchDown, handler);
			negative_button->registerEventHandler(MIControlEventTouchDown, handler);
			handler.event_func = (EventFunc)&MIAlertDialogDelegate::buttonUpOutside;
			positive_button->registerEventHandler(MIControlEventTouchUpOutside, handler);
			negative_button->registerEventHandler(MIControlEventTouchUpOutside, handler);
		}

        resizeButtons();
		//窗口显示完毕后重置字体大小
		text_font_multiple = 1.0f;
    }
    
    void MIAlertDialog::initTextView()
    {
        if(message_text_view != NULL)
        {
            message_text_view->detachFromSuperview();
            delete_and_set_null(message_text_view);
        }
        message_text_view = new MITextView();
        message_text_view->setFontSizeAndColor(getTextFontSize()*text_font_multiple, dialog.font_color);
        message_text_view->setBackgroundColor(MGColor::fromRGBA(0, 0, 0, 0));
        message_text_view->_view_flags.user_interaction_enabled = 0;
    }
    
    void MIAlertDialog::resizeButtons()
    {
		content_bkg_view->resizableWithCapInsets(MGEdgeInsetsMake(21, 21, 21, 21));
		content_bkg_view->setEdgeScaleEnabled(true);
		positive_button->resizableWithCapInsets(MGEdgeInsetsMake(10, 10, 10, 10));
		positive_button->setEdgeScaleEnabled(true);
		negative_button->resizableWithCapInsets(MGEdgeInsetsMake(10, 10, 10, 10));
		negative_button->setEdgeScaleEnabled(true);

		if(m_os_type==MIAlertDialogDataOSTypeIOS7)
		{
			positive_button->setCornerRadius(10);
			negative_button->setCornerRadius(10);
			positive_button->setBorderEnabled(false);
			negative_button->setBorderEnabled(false);
		}
  
    }
    
    void MIAlertDialog::drawself()
    {
        MIView::drawself();
    }
    
    void MIAlertDialog::showDialog(const MIAlertDialogData &dialog, MIAlertDialogDataOSType os_type)
    {
        MIEvent *event = new MIEvent();
        event->_event_type = RESIGN_FIRST_RESPONDER_EVENT;
        GuiContainer::container->preserveEvent(event);
        
        if(alert_dialog == NULL)
        {
            alert_dialog = new MIAlertDialog();
			alert_dialog->m_os_type = os_type;
            alert_dialog->initWithFrame(MGRectMake(0.0f, 0.0f, 100.0f, 100.0f));
            GuiContainer::container->gui_root->view->addSubview(alert_dialog);
            alert_dialog->hide();
        }
        if (alert_dialog->isHidden()) {
            alert_dialog->dialog = dialog;
            if(os_type==MIAlertDialogDataOSTypeIOS7)
                alert_dialog->dialog.font_color = RGBAColorRef::black();
            alert_dialog->initAlertDialog();
        }
        GuiContainer::container->foreignerSDKApiCall("add top blend");
        (dynamic_cast<MIAlertDialogDelegate*>(alert_dialog->delegate))->startAnimation();
    }
    bool MIAlertDialog::isHidden()
	{
		bool ret = true;
		if(alert_dialog!=NULL)
		{
			if(!alert_dialog->isHide())
				ret = false;
		}
		return ret;
	}
	void MIAlertDialog::hideDialog()
	{
		if(alert_dialog==NULL)
			return;
		(dynamic_cast<MIAlertDialogDelegate*>(alert_dialog->delegate))->negativeCall(alert_dialog->negative_button);
	}
    float MIAlertDialog::getHeightAdaptiveCoefficient()
    {
        return MINVALUE(1.5f*(float)MGDevice::getScreenWidth()/(float)MGDevice::getScreenHeight(), 1.0f);
    }
    
    void MIAlertDialog::initAlertDialog()
    {
        MIAlertDialogDelegate* controller = dynamic_cast<MIAlertDialogDelegate*>(delegate);
        controller->positive_handler = dialog.positive_handler;
        controller->positive_responder = dialog.positive_responder;
        controller->negative_handler = dialog.negative_handler;
        controller->negative_responder = dialog.negative_responder;
        controller->positive_selected = false;
        
        // init font_color
        title_label->getFont()->setColor(dialog.font_color);
        message_text_view->getFont()->setColor(dialog.font_color);
        positive_button->titleLabel()->getFont()->setColor(dialog.font_color);
        negative_button->titleLabel()->getFont()->setColor(dialog.font_color);
        if(m_os_type==MIAlertDialogDataOSTypeIOS7)
		{
			positive_button->titleLabel()->getFont()->setColor(RGBAColorMake(0,122,255));
			negative_button->titleLabel()->getFont()->setColor(RGBAColorMake(0,122,255));
			positive_button->setActionBlend(RGBAColorMake(0,122,255));
			negative_button->setActionBlend(RGBAColorMake(0,122,255));
		}
        // init params
        float screen_width = MGDevice::getScreenHeight();
        float screen_height = MGDevice::getScreenHeight();
        float height_adaptive_coefficient = getHeightAdaptiveCoefficient();
        
        float title_label_height = ALERT_DIALOG_BUTTON_HEIGHT*height_adaptive_coefficient*screen_height;
        float TITLE_TO_TOP = ALERT_DIALOG_TITLE_TO_TOP*height_adaptive_coefficient*screen_height;
        float TEXT_TO_TITLE = ALERT_DIALOG_TEXT_TO_TITLE*height_adaptive_coefficient*screen_height;
        float BUTTON_TO_TEXT = ALERT_DIALOG_BUTTON_TO_TEXT*height_adaptive_coefficient*screen_height;
        float BOTTOM_TO_BUTTON = ALERT_DIALOG_BOTTOM_TO_BUTTON*height_adaptive_coefficient*screen_height;
        if(m_os_type==MIAlertDialogDataOSTypeIOS7)
			BOTTOM_TO_BUTTON=0;
        float start_y = TITLE_TO_TOP;
		float line_y = start_y;
        
        // title label
        if(dialog.title != ALERT_DIALOG_INIT_TEXT)
        {
            title_label->show();
            MGRect title_label_frame = title_label->getFrame();
            title_label_frame.y = start_y;
            title_label->setFrame(title_label_frame);
            title_label->setText(dialog.title);
            
            start_y += title_label_height+TEXT_TO_TITLE;
        }
        else
        {
            title_label->hide();
            start_y = TEXT_TO_TITLE;
        }
        
        // message_text_view
        do
        {
            // set text
            MGRect message_text_frame = message_text_view->getFrame();
            initTextView();
            if(dialog.message == ALERT_DIALOG_INIT_TEXT)
            {
                message_text_view->setText(" ");
            }
            else
            {
                message_text_view->setText(dialog.message);
            }
            
            // set frame
            message_text_frame.y = start_y;
            MGFont* font = message_text_view->getFont();
            MGRect text_rect = font->getStringBoundForWidth(message_text_view->getText(), message_text_frame.width-MITEXTVIEW_DEFAULT_BORDER_VALUE*2);
			text_rect.height *= 1.2f;
			if (text_rect.height > 0 && text_rect.height < font->getFontSize() * 1.2f)
			{
				text_rect.height = font->getFontSize() * 1.2f;
			}
			message_text_frame.height = text_rect.height + MITEXTVIEW_DEFAULT_BORDER_VALUE*2;
            message_text_view->initWithFrame(message_text_frame);
            content_view->addSubview(message_text_view);
            
            start_y += message_text_frame.height+BUTTON_TO_TEXT;
        }
        while(false);
        
        // bottom buttons
        do
        {
            // positive_button
            if(dialog.positive_text != ALERT_DIALOG_INIT_TEXT)
            {
                positive_button->titleLabel()->setText(dialog.positive_text);
                positive_button->show();
            }
            else
            {
                positive_button->hide();
            }
            
            // negative_button
            if(dialog.negative_text != ALERT_DIALOG_INIT_TEXT)
            {
                negative_button->titleLabel()->setText(dialog.negative_text);
                negative_button->show();
            }
            else
            {
                negative_button->hide();
            }
            
            // init visibility and width
            if(positive_button->isHide() && negative_button->isHide())
            {
                positive_button->show();
            }
            float button_margin = ALERT_DIALOG_BUTTON_BETWEEN*MINVALUE(screen_width, screen_height);
			if(m_os_type==MIAlertDialogDataOSTypeIOS7)
				button_margin=0;
            if(!positive_button->isHide() && !negative_button->isHide())
            {
                float button_width = (content_view->getFrame().width-3*button_margin)/2.0f;
                MGRect positive_button_frame = positive_button->getFrame();
                MGRect negative_button_frame = negative_button->getFrame();
                positive_button_frame.x = button_margin;
                positive_button_frame.width = button_width;
                positive_button_frame.y = start_y;
                negative_button_frame.x = positive_button_frame.x+positive_button_frame.width+button_margin;
                negative_button_frame.width = button_width;
                negative_button_frame.y = start_y;
                positive_button->setFrame(positive_button_frame);
                positive_button->titleLabel()->setFrame(MGRectMake(0.0f, 0.0f, positive_button_frame.width, positive_button_frame.height));
                negative_button->setFrame(negative_button_frame);
                negative_button->titleLabel()->setFrame(MGRectMake(0.0f, 0.0f, negative_button_frame.width, negative_button_frame.height));
            }
            else
            {
                float button_width = content_view->getFrame().width-2*button_margin;
                MIButton* visible_button = positive_button;
                if(visible_button->isHide())
                {
                    visible_button = negative_button;
                }
                visible_button->show();
                MGRect button_frame = visible_button->getFrame();
                button_frame.x = button_margin;
                button_frame.width = button_width;
                button_frame.y = start_y;
                visible_button->setFrame(button_frame);
                visible_button->titleLabel()->setFrame(MGRectMake(0.0f, 0.0f, button_frame.width, button_frame.height));
            }
            
			if(m_os_type==MIAlertDialogDataOSTypeIOS7)
			{
				if(horizontal_line_view==NULL)
				{
					horizontal_line_view = new MIImageView();
					horizontal_line_view->initWithFrame(MGRectMake(0,start_y,content_bkg_view->getFrame().width,screen_height*1.0/960.0*height_adaptive_coefficient));	
					content_view->addSubview(horizontal_line_view);
					horizontal_line_view->_view_flags.user_interaction_enabled=0;
				}
				horizontal_line_view->hide();
			}
			line_y = start_y;
			start_y += positive_button->getFrame().height+BOTTOM_TO_BUTTON;
        }
        while(false);
        //start_y += 0.05*screen_height*getHeightAdaptiveCoefficient();
        MGRect dialog_frame = content_view->getFrame();
        dialog_frame.height = start_y;
        dialog_frame.y = (screen_height-dialog_frame.height)/2.0f;
        content_view->setFrame(dialog_frame);
        content_bkg_view->setFrame(MGRectMake(0.0f, 0.0f, dialog_frame.width, dialog_frame.height));
		if(m_os_type==MIAlertDialogDataOSTypeIOS7)
		{
			if(horizontal_line_view!=NULL)
			{
				if(vertical_line_view!=NULL)
					vertical_line_view->hide();
				if(!positive_button->isHide() && !negative_button->isHide())
				{
					if(vertical_line_view==NULL)
					{
						vertical_line_view = new MIImageView();
						vertical_line_view->initWithFrame(MGRectMake(positive_button->getFrame().x+positive_button->getFrame().width,line_y,1.0,positive_button->getFrame().height));
						content_view->addSubview(vertical_line_view);
						vertical_line_view->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::darkGray()));
					}
					vertical_line_view->setFrame(MGRectMake(positive_button->getFrame().x+positive_button->getFrame().width,line_y,1.0,positive_button->getFrame().height));
					vertical_line_view->show();
				}
				horizontal_line_view->setFrame(MGRectMake(0,line_y,content_bkg_view->getFrame().width,1.0));
				horizontal_line_view->setBackgroundColor(MGColor::fromColorRef(RGBAColorRef::darkGray()));
				
				horizontal_line_view->show();
			}
		}
        resizeButtons();
		text_font_multiple =1.0f;
    }

	void MIAlertDialog::setTextFontMultiple(float multiple)
	{
		text_font_multiple = multiple;
	}
}
