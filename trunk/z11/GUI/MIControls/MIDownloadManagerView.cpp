#include "MIDownloadManagerView.h"
#include "MITableView.h"
#include "../../System/IO/MGBundle.h"
namespace z11
{


	MIDownloadManagerView::MIDownloadManagerView(void)
	{
		m_table_view=NULL;
		delegate=NULL;
		m_download_folder=MGBundle::mainBundle()->tmpPathWithFileName("");
	}

	MIDownloadManagerView::~MIDownloadManagerView(void)
	{
	}

	void MIDownloadManagerView::initWithFrame(MGRect frame)
	{
		MIView::initWithFrame(frame);
		m_table_view=new MITableView();
	}
	void MIDownloadManagerView::drawself()
	{

	}

	void MIDownloadManagerView::setDownloadFolder( const std::string &download_folder )
	{
		m_download_folder=download_folder;
	}

	void MIDownloadManagerView::addTask( const std::string &url )
	{

	}

	void MIDownloadManagerView::addTask( const std::string &url,const std::string &name )
	{

	}
}
