//////////////////////////////////////////////////////////////////////////
////   COPYRIGHT NOTICE 
////    Copyright (c) 2011 Zhejiang University. 
////    All rights reserved.  
////  
//// @file    (MIPickerView.h)  
//// @brief (本文件实现的功能的简述)  
////  
////(本文件实现的功能的详述)  
////  
//// @version 0.1   (版本声明)  
//// @author        (Gu RongFang)  
//// @date          (11-11-11)  
////  
////  
////    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#pragma once
#include "MIControl.h"

namespace z11
{
	typedef enum
    {
		MIControlContentVerticalAlignmentCenter         =   0,
		MIControlContentVerticalAlignmentTop            =   1,
		MIControlContentVerticalAlignmentBottom         =   2,
		MIControlContentVerticalAlignmentFill           =   3,
	}
    MIControlContentVerticalAlignment;

	typedef enum
    {
		MIControlContentHorizontalAlignmentCenter       =   0,
		MIControlContentHorizontalAlignmentLeft         =   1,
		MIControlContentHorizontalAlignmentRight        =   2,
		MIControlContentHorizontalAlignmentFill         =   3,
	}
    MIControlContentHorizontalAlignment;

	class MIPickerViewImpl;
	class MIPickerViewDelegate;
	class MIPickerViewDataSource;
    class MIImageView;
    
	class MIPickerView : public MIControl
	{
	public:
        // Constructor
        MIPickerView();
		virtual ~MIPickerView();
        
        // init
        void initWithFrame(MGRect frame);
        void setShadowColor(MGColor* color);
        void setGlassColor(MGColor* color);
        void setBorder(const MGRect2 &border_rect);
        const MGRect2 & getBorder();
        
		// Getting the Dimensions of the View Picker
		int numberOfComponents();
		int numberOfRowsInComponent(int component);
		MGSize rowSizeForComponent(int component);

		// Reloading the View Picker
		void reloadAllComponents();
		void reloadComponent(int component);

		// Selecting Rows in the View Picker
		void selectRowInComponent(int row, int component, bool animated = true);
		int selectedRowInComponent(int component);

		// Returning the View for a Row and Component
		MIView* viewForRowForComponent(int row, int component);

		// Specifying the Delegate
		void setDelegate(MIPickerViewDelegate* _delegate);
		MIPickerViewDelegate* getDelegate();

		// Specifying the Data Source
		void setDataSource(MIPickerViewDataSource* _src);
		MIPickerViewDataSource* getDataSource();

		// Managing the Appearance of the Picker View
		MGPROPERTY(bool, m_showsSelectionIndicator, ShowsSelectionIndicator);

		MIControlContentVerticalAlignment contentVerticalAlignment;
		MIControlContentHorizontalAlignment contentHorizontalAlignment;

	protected:
		virtual void drawself();
		void didSelectRowInComponent(MIPickerView* picker_view, int row, int component);
		void changeSelectedRowInComponent(int row, int component, bool animated = true);
        MGRect getCurrentComponentsRect();
        
	private:
		MIPickerViewDelegate* delegate;
		MIPickerViewDataSource* dataSource;
		vector<MIPickerViewImpl*> m_components;
        
        MGRect2 border_rect;
        MIImageView* image_shadow;
        MIImageView* image_glass;
        
		friend class MIPickerViewImpl;
	};

	class MIPickerViewDelegate
	{
	public:
		// Setting the Dimensions of the Picker View
		virtual float rowHeightForComponent(MIPickerView* picker_view, int component) = 0;
		virtual float widthForComponent(MIPickerView* picker_view, int component) = 0;

		// Setting the Content of Component Rows
		virtual string titleForRowForComponent(MIPickerView* picker_view, int row, int component) = 0;
		// virtual MIView* viewForRowForComponentReusingView(MIPickerView* picker_view, int row, int component, MIView* view) = 0;
		virtual void didSelectRowInComponent(MIPickerView* picker_view, int row, int component) = 0;
	};

	class MIPickerViewDataSource 
	{
	public:
		virtual int numberOfComponentsInPickerView(MIPickerView* view) = 0;
		virtual int numberOfRowsInComponent(MIPickerView* view, int component) = 0;
	};

#define MIPICKERVIEW_DELEGAGE_DECLARE \
	virtual float rowHeightForComponent(MIPickerView* picker_view, int component); \
	virtual float widthForComponent(MIPickerView* picker_view, int component); \
	virtual string titleForRowForComponent(MIPickerView* picker_view, int row, int component); \
	virtual void didSelectRowInComponent(MIPickerView* picker_view, int row, int component);

#define MIPICKERVIEW_DATASOURCE_DECLARE \
	virtual int numberOfComponentsInPickerView(MIPickerView* view); \
	virtual int numberOfRowsInComponent(MIPickerView* view, int component); 

	enum PickerHandlerType
	{
		PICKER_VIEW_DID_SELECT_ROW = MIControlEventSystemReserved+100
	};
    
	class MIPickerHander : public MIObject
	{
	public:
		MIPickerHander(MIPickerView* _view, int _row, int _component) : view(_view), row(_row), component(_component)
		{

		}
        
		MIPickerHander() : view(NULL), row(0), component(0)
		{

		}
        
    public:
        MIPickerView *view;
		int row;
		int component;
	};
}
