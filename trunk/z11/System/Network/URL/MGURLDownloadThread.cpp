#include "MGURLDownloadThread.h"
#include "MGURLConnection.h"
#include "../../Shared/MGLog.h"
#include "../../IO/MGFileManager.h"
#include "../../Shared/Utility.h"
#include <stdio.h>
using namespace std;
using namespace z11;

namespace z11
{




	 MGURLDownloadThread *MGURLDownloadThread::_instance=NULL;
	 std::queue<MGURLDownload*> MGURLDownloadThread::download_queue;
	 MGMutex MGURLDownloadThread::m_queue_locker;
	 int MGURLDownloadThread::start_num=0;
	MGURLDownloadThread::~MGURLDownloadThread()
	{
		requestExitAndWait();
	}

	void MGURLDownloadThread::start()
	{
		AUTO_LOCK(&m_queue_locker);
		start_num++;
		if (_instance==NULL)
		{
			_instance=new MGURLDownloadThread();
			_instance->run("MGURLDownloadThread");
		}
	}

	void MGURLDownloadThread::stop()
	{
		AUTO_LOCK(&m_queue_locker);
		start_num--;
		if (start_num==0)
		{
			if (_instance!=NULL)
			{
				_instance->requestExitAndWait();
				delete_and_set_null(_instance);
			}
		}
	}

	void MGURLDownloadThread::enqueue( MGURLDownload *download )
	{
		AUTO_LOCK(&m_queue_locker);
		download_queue.push(download);
		_instance->m_queue_semphore.Signal();
	}

	bool MGURLDownloadThread::threadLoop()
	{
		m_queue_semphore.Wait();
		if(!download_queue.empty())
		{

			AUTO_LOCK(&m_queue_locker);
			MGURLDownload *download=download_queue.front();
			download_queue.pop();
			if (download!=NULL)
			{
				MGURLDownloadImpl impl(download);
				//delete_and_set_null(download);
			}
		}
		return true;
	}

	MGURLDownloadThread::MGURLDownloadThread() :m_queue_semphore(0)
	{

	}

	void MGURLDownloadThread::requestExit()
	{
		MGThread::requestExit();
		//avoid MGURLDownloadThread be blocked,and unable quit
		m_queue_semphore.Signal();
	}
	
	/************************************************************************/
	/*                      MGURLDownloadImpl								*/
	/************************************************************************/

	MGURLDownloadImpl::MGURLDownloadImpl( MGURLDownload *download )
	{
		assert(download!=NULL);
		m_download=download;
		file=NULL;
		//m_connection=new MGURLConnection();
		//m_download->impl=this;
		//m_connection->initWithRequest(download->request(),this);
	}

	MGURLDownloadImpl::~MGURLDownloadImpl()
	{
		if (file!=NULL)
		{
			fclose(file);
		}
		//m_download->m_receive_bytes=m_connection->receive_bytes;
		//m_download->m_request=NULL;
		//m_download->impl=NULL;
		//delete_and_set_null(m_download);
		//delete_and_set_null(m_connection);
	}

	void MGURLDownloadImpl::connectionDidReceiveResponse( MGURLConnection *connection,MGURLResponse *response )
	{
		string dispostion=connection->header_response_map.objectForKey("Content-Disposition");
		string suggest_file_name("UnKown");
		if (dispostion=="")
		{
			string url=connection->m_request->URL()->absoluteString();
			size_t pos=url.find_last_of('/');
			if (pos!=string::npos)
			{
				suggest_file_name=url.substr(pos+1);
			}
		}
		else
		{
			size_t pos=dispostion.find_first_of('"')+1;
			suggest_file_name=dispostion.substr(pos,dispostion.find_last_of('"')-pos);
		}
		m_download->decideDestinationWithSuggestedFilename(suggest_file_name);
		if (m_download->m_allowOverwrite || !MGFileManager::defaultManager()->fileExistsAtPath(m_download->m_download_path))
		{
			//Utility::delAFile(m_download->m_download_path);
			file=fopen(m_download->m_download_path.c_str(),"wb");
		}
		else
		{
			if (m_download->resume_mode)
			{
				file=fopen(m_download->m_download_path.c_str(),"ab");
				fseek(file,0,SEEK_END);
			}
			else
			{
				MGLogD("file %s is already exists.",m_download->m_download_path.c_str());
				file=NULL;
				connection->cancel();
			}
		}
	}

	void MGURLDownloadImpl::connectionDidReceiveData( MGURLConnection *connection,MGData *data )
	{
		assert(file!=NULL);
		fwrite(data->bytes(),data->length(),1,file);
		m_download->m_receive_bytes+=data->length();
		if (m_download->m_delegate!=NULL)
		{
			m_download->m_delegate->downloadDidReceiveData(m_download,connection->receive_bytes,connection->total_bytes);
		}
	}

	void MGURLDownloadImpl::connectionDidFinishLoading( MGURLConnection *connection )
	{
		if (file!=NULL)
		{
			fclose(file);
			file=NULL;
		}
		if (m_download->m_delegate!=NULL)
		{
			m_download->m_delegate->downloadDidFinishDownloading(m_download);
		}
	}

	void MGURLDownloadImpl::connectionDidCancelLoading( MGURLConnection *connection )
	{
		if (file!=NULL)
		{
			fclose(file);
			file=NULL;
		}
		if (m_download->m_delegate!=NULL)
		{
			m_download->m_delegate->downloadDidCancelDownloading(m_download);
		}
	}

	void MGURLDownloadImpl::connectionDidFailWithError( MGURLConnection *connection,const MGError&error )
	{
		if (file!=NULL)
		{
			fclose(file);
			file=NULL;
		}
		if (m_download->m_delegate!=NULL)
		{
			m_download->m_delegate->downloadDidFailWithError(m_download,error);
		}
	}

	//void MGURLDownloadImpl::cancel()
	//{
	//	if (m_connection!=NULL)
	//	{
	//		m_connection->cancel();
	//	}
	//}
}
