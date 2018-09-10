//
//  AlertDialog.h
//  WaterMobileGIS
//
//  Created by SLJ on 13-1-22.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//
#pragma once

#include "GUI/Base/MIView.h"

namespace z11
{
    class MILabel;
    class MITextView;
    class ImageResizableButton;
    class MIImageView;
  
	enum MIAlertDialogDataOSType
	{
		MIAlertDialogDataOSTypeIOS7,
		MIAlertDialogDataOSTypeOthers
	};
    class MIAlertDialogData
    {
    public:
        MIAlertDialogData();
        void setTitle(const string &title);
        void setMessage(const string &message);
        void setFontColor(const RGBAColorRef &font_color);
        void setPositiveButton(const string &text, MIResponder* responder = NULL, SELHandler handler = SELHandler());
        void setNegativeButton(const string &text, MIResponder* responder = NULL, SELHandler handler = SELHandler());
    private:
        string title;
        string message;
        RGBAColorRef font_color;
        
        string positive_text;
        SELHandler positive_handler;
        MIResponder* positive_responder;
        
        string negative_text;
        SELHandler negative_handler;
        MIResponder* negative_responder;
        
        friend class MIAlertDialog;
    };
    
    class MIAlertDialog : public MIView
    {
    protected:
        MIAlertDialog();
        virtual ~MIAlertDialog();
        virtual void initWithFrame(MGRect frame);
        virtual void drawself();        
    public:
        static void showDialog(const MIAlertDialogData &dialog, MIAlertDialogDataOSType os_type=MIAlertDialogDataOSTypeIOS7);
        static bool isHidden();
		static void hideDialog();
		//2017-9-20 用于更改message字体大小
		static void setTextFontMultiple(float multiple);
    private:
        float getHeightAdaptiveCoefficient();
        void resizeButtons();
        uint_32 getTitleFontSize();
        uint_32 getTextFontSize();
        void initTextView();
        void initAlertDialog();
        
    private:
        MIAlertDialogData dialog;
        MIImageView* black_bkg_view;
        MIImageView* content_view;
		MIImageView* horizontal_line_view;
		MIImageView* vertical_line_view;
        ImageResizableButton* content_bkg_view;
        MILabel* title_label;
        MITextView* message_text_view;
        ImageResizableButton* positive_button;
        ImageResizableButton* negative_button;
        MIAlertDialogDataOSType m_os_type;
		static float text_font_multiple;

        static MIAlertDialog* alert_dialog;
        
        friend class MIAlertDialogDelegate;
    };
}