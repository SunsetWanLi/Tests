#include "MITemplateTableCell.h"
#include "../MILayoutEngine/MILayoutNode.h"
#include "../../System/Device/MGDevice.h"
#include "MIImageView.h"
#include "../Base/MIReusableQueue.h"
namespace z11
{
	MITemplateTableCell::MITemplateTableCell(void)
	{
        class_name = "MITemplateTableCell";
	}

	MITemplateTableCell::~MITemplateTableCell(void)
	{
	}

	void MITemplateTableCell::initWithFrame( MGRect frame )
	{
		MIView::initWithFrame(frame);
	}


	int_32 MITemplateDataSource::numberOfRowsInSection(MITableView * table_view, int_32 section)
	{
		if (layout_node!=NULL)
		{
			return layout_node->data_size;
		}
		else
			return 0;
		
	}
	MITableViewCell* MITemplateDataSource::cellForRowAtIndexPath(MITableView * table_view, const MGIndexPath& path)
	{
		//MGRect label_frame=MGRectMake(0,0,float(480),float(64));
		//MITemplateTableCell* cell = new MITemplateTableCell();
		//cell->initWithFrame(label_frame);
		//MILabel *m_label=new MILabel();
		//MIImageView *m_picture_view=new MIImageView();
		//m_label->setText(data["label_text"][row]);
		////cell->m_label->setFontSizeAndColor((uint_32)(18*MGDevice::getScreenHeight()/800));
		//m_label->setFontSizeAndColor((uint_32)(18*MGDevice::getScreenHeight()/800),RGBAColorRef::black());
		//m_picture_view->setBackgroundColor(MGColor::fromImage(MGBundle::mainBundle()->pathWithFileName("images/line_icon.png")));

		//m_picture_view->initWithFrame(MGRectMake(0,0,64,64));
		//cell->addSubview(m_picture_view);

		//m_label->initWithFrame(MGRectMake(64,0,400,64));
		//cell->addSubview(m_label);
		table_view->m_rowHeight=64;
		MITemplateTableCell *cell=(MITemplateTableCell*)(((MITableViewCellLayoutNode*)layout_node->subLayoutNode[0])->create(NULL, false, path.row()));
		return cell;


	}
	bool MITemplateDataSource::canEditRowAtIndexPath(MITableView * table_view, const MGIndexPath& path)
	{
		return false;
	}
	//float MITemplateDataSource::heightForRow(MITableView * table_view, int_32 row)
	//{
	//	return 64;
	//}

	MITemplateDataSource::MITemplateDataSource()
	{
		layout_node=NULL;
	}
    
    int_32 MITemplateDataSource::numberOfSections(z11::MITableView *table_view)
    {
        return 1;
    }
    
}
