#include "MITemplatePickerDelegate.h"
#include "../Controllers/MIViewController.h"
#include "../MILayoutEngine/MILayoutNode.h"
namespace z11
{
	int MITemplatePickerDataSource::numberOfComponentsInPickerView( MIPickerView *view )
	{
		MITemplatePickerDelegate *delegate=dynamic_cast<MITemplatePickerDelegate *> (view->getDelegate());
		if (delegate!=NULL)
		{
			return delegate->component_size.size();
		}
		return 0;
	}

	int MITemplatePickerDataSource::numberOfRowsInComponent( MIPickerView *view,int component )
	{
		MITemplatePickerDataSource *src=this;
		if (src!=NULL)
		{
			MGMultipleIndexDictionary *dic=src->getDictionary(view,component);
			if (dic!=NULL)
			{
				return dic->m_strings.size();
			}
		}
		return 0;
	}

	std::string MITemplatePickerDelegate::titleForRowForComponent( MIPickerView *view,int row,int component )
	{
		MITemplatePickerDataSource *src=dynamic_cast<MITemplatePickerDataSource *> (view->getDataSource());
		if (src!=NULL)
		{
			MGMultipleIndexDictionary *dic=src->getDictionary(view,component);
			if (dic!=NULL)
			{
				return dic->m_strings[row];
			}
		}
		return "";
	}
	MITemplatePickerDataSource::MITemplatePickerDataSource()
	{

	}

	MGMultipleIndexDictionary * MITemplatePickerDataSource::getDictionary( MIPickerView *view,int component )
	{
		MGMultipleIndexDictionary *dic=data;
		int i;
		for (i=0;i<component&&dic!=NULL;i++)
		{
			dic=dic->objectForIndex(view->selectedRowInComponent(i));
		}
		return dic;
	}

	MITemplatePickerDelegate::MITemplatePickerDelegate()
	{

	}

	float MITemplatePickerDelegate::rowHeightForComponent( MIPickerView *picker_view,int component )
	{
		return component_size.at(component).height;
	}

	float MITemplatePickerDelegate::widthForComponent( MIPickerView *picker_view,int component )
	{
		return component_size.at(component).width;
	}


	void MITemplatePickerDelegate::didSelectRowInComponent( MIPickerView *picker_view,int row,int component )
	{
	}

}
