#pragma once
#include "MITableView.h"
namespace z11
{
	class MITableViewLayoutNode;
class MITemplateTableCell:public MITableViewCell
{
public:
	MITemplateTableCell(void);
	virtual ~MITemplateTableCell(void);
	virtual void initWithFrame(MGRect frame);
};

class MITemplateDataSource:public MITableViewDataSource
{
public:
	MITemplateDataSource();
public:
	MITABLEVIEW_DATASOURCE_DECLARE;
	MITableViewLayoutNode *layout_node;
	std::map< std::string , std::vector<std::string> > data;
};

}