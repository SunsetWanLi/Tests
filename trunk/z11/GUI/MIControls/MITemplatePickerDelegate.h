#pragma once
#include "MIPickerView.h"
namespace z11
{
	class MITemplatePickerDelegate:public MIPickerViewDelegate
	{
	public :
		MITemplatePickerDelegate();
		//Setting the Dimensions of the Picker View
		virtual float rowHeightForComponent(MIPickerView *picker_view,int component);
		virtual float widthForComponent(MIPickerView *picker_view,int component);

		//Setting the Content of Component Rows
		virtual string titleForRowForComponent(MIPickerView *picker_view,int row,int component);
		
		virtual void didSelectRowInComponent(MIPickerView *picker_view,int row,int component);

	public:
		vector<MGSize> component_size;
	};
	class MGMultipleIndexDictionary;

	class MITemplatePickerDataSource:public MIPickerViewDataSource
	{
	public:
		MITemplatePickerDataSource();
		virtual int numberOfComponentsInPickerView(MIPickerView *view);
		virtual int numberOfRowsInComponent(MIPickerView *view,int component);
	public:
		MGMultipleIndexDictionary *data;
	private:
		friend class MITemplatePickerDelegate;
		MGMultipleIndexDictionary *getDictionary(MIPickerView *view,int component);
	};


}