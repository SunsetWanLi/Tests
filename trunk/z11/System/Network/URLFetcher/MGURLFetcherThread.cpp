#include "MGURLFetcherThread.h"

#include "../../Shared/MGLog.h"
#include "../../IO/MGBundle.h"
namespace z11{
	MGURLFetcherThread::MGURLFetcherThread()
		:m_fetcher_semaphore(0)
	{
		MGLogD("MGURLFetcherThread::MGURLFetcherThread");
		m_download_folder=MGBundle::mainBundle()->tmpPathWithFileName("");
	}
	MGURLFetcherThread::~MGURLFetcherThread()
	{
		requestExitAndWait();
		MGLogD("MGURLFetcherThread::~MGURLFetcherThread");
	}


	bool MGURLFetcherThread::threadLoop()
	{
		m_fetcher_semaphore.Wait();
		MGURLFetcherRequest *request=NULL;
		{
			AUTO_LOCK_THIS;
			if (!requests.empty())
			{
				request=requests.front();
				requests.pop();
			}
		}
		if (request!=NULL)
		{
			
		}
		
		return true;
	}

	void MGURLFetcherThread::setDownloadFolder( const std::string &download_folder )
	{
		m_download_folder=download_folder;
	}

	void MGURLFetcherThread::addTask( MGURLFetcherRequest *request )
	{
		AUTO_LOCK_THIS;
		requests.push(request);
		m_fetcher_semaphore.Signal();
	}

	void MGURLFetcherThread::requestExit()
	{
		m_fetcher_semaphore.Signal();
	}
}
