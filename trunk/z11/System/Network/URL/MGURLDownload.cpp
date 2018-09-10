
#include <stdio.h>
#include "MGURLDownload.h"
#include "../../IO/MGBundle.h"
#include "../../Shared/MGLog.h"
#include "MGURLDownloadThread.h"
#include "../../Shared/Utility.h"
using namespace std;
using namespace z11;

namespace z11
{
	

	MGURLDownload::MGURLDownload()
	{
		m_request=NULL;
		m_delegate=NULL;
		m_deletesFileUponFailure=true;
		m_allowOverwrite=true;
		impl=NULL;
		m_receive_bytes=0;
		m_connection=NULL;
		resume_mode=false;
		impl=new MGURLDownloadImpl(this);
	}

	MGURLDownload::~MGURLDownload()
	{
		delete_and_set_null(impl);
		//MGLogD("~MGURLDownload");
        release_and_set_null(m_request);
		//m_request->release();
	}
	void MGURLDownload::initWithRequest(const MGURLRequest *request,MGURLDownloadDelegate *delegate )
	{
		m_delegate=delegate;
		resume_mode=false;
		m_connection=MGURLConnection::connectionWithRequest(request,impl);
	}

	MGData * MGURLDownload::resumeData()
	{
		stringstream ss;
		ss<<"URL "<<request()->URL()->absoluteString()<<endl;
		ss<<"TimeoutInterval "<<request()->timeoutInterval()<<endl;
		ss<<"ResumeFrom "<<m_receive_bytes<<endl;
		vector<string> keys=request()->allHTTPHeaderFields()->allKeys();
		for (size_t i=0;i<keys.size();i++)
		{
			if (request()->allHTTPHeaderFields()->objectForKey(keys[i])!="")
			{
				ss<<keys[i]<<" "<<request()->allHTTPHeaderFields()->objectForKey(keys[i])<<endl;
			}
		}
		return MGData::dataWithBytes(ss.str().c_str(),ss.str().length()+1);
	}

	void MGURLDownload::setDeletesFileUponFailure( bool _deletesFileUponFailure )
	{
		m_deletesFileUponFailure=_deletesFileUponFailure;
	}

	bool MGURLDownload::deletesFileUponFailure()
	{
		return m_deletesFileUponFailure;
	}

	MGURLRequest * MGURLDownload::request()
	{
		return m_connection->request();
	}

	void MGURLDownload::setDestination( const std::string&path)
	{
		m_download_path=path;
	}

	void MGURLDownload::setAllowOverwrite( bool allowOverwrite )
	{
		m_allowOverwrite=allowOverwrite;
	}


	void MGURLDownload::decideDestinationWithSuggestedFilename( const std::string &file_name )
	{
		if (m_delegate!=NULL)
		{
			m_delegate->downloadDecideDestinationWithSuggestedFilename(this,file_name);
		}
		else
		{
			if(!Utility::isFileORDirExist(MGBundle::mainBundle()->tmpPathWithFileName("downloads/")))
			{
				Utility::createDirOnPath(MGBundle::mainBundle()->tmpPathWithFileName("downloads/"));
			}
			setDestination(MGBundle::mainBundle()->tmpPathWithFileName("downloads/"+file_name));
		}
	}

	void MGURLDownload::startDownloadThread()
	{
		//MGURLDownloadThread::start();
	}

	void MGURLDownload::stopDownloadThread()
	{
		//MGURLDownloadThread::stop();
	}

	std::string MGURLDownload::getDestinationPath()
	{
		return m_download_path;
	}

	void MGURLDownload::cancel()
	{
		m_connection->cancel();
		//if (impl!=NULL)
		//{
		//	impl->cancel();
		//}
	}

	void MGURLDownload::initWithResumeData( MGData *resume_data,MGURLDownloadDelegate *delegate,const string &path )
	{
		resume_mode=true;
		m_delegate=delegate;
		string url;
		int timeout_sec=0;

		string resume_key;
		string resume_value;
		stringstream ss;
		ss<<(char *)resume_data->bytes();
		//ss.read((char *)resume_data->bytes(),resume_data->length());
		ss>>resume_key>>resume_value;
		if (resume_key=="URL")
		{
			url=resume_value;
		}
		ss>>resume_key>>resume_value;
		if (resume_key=="TimeoutInterval")
		{
			sscanf(resume_value.c_str(),"%d",&timeout_sec);
		}
		size_t file_size=0;
		ss>>resume_key>>resume_value;
		if (resume_key=="ResumeFrom")
		{
			sscanf(resume_value.c_str(),"%ld",&file_size);
		}
		MGURLRequest *request=MGURLRequest::requestWithURL(Ref<MGURL>(MGURL::URLWithString(url)),timeout_sec);
		while (ss>>resume_key>>resume_value)
		{
			request->setValueForHTTPHeaderField(resume_value,resume_key);
		}
		{
			FILE *file=fopen(path.c_str(),"rb");
			if (file!=NULL)
			{
				fseek(file,0,SEEK_END);
				file_size=ftell(file);
				fclose(file);
			}
			stringstream ss;
			ss<<file_size<<"-";
			request->setValueForHTTPHeaderField(ss.str(),"Range");
		}
		delete_and_set_null(m_connection);
		m_connection=MGURLConnection::connectionWithRequest(request,impl);
		//stringstream ss;
		//ss<<m_receive_bytes<<"-";
		//m_request->setValueForHTTPHeaderField(ss.str(),"Range");
		//MGURLDownloadThread::enqueue(this);
	}


	void MGURLDownloadDelegate::downloadDecideDestinationWithSuggestedFilename( MGURLDownload *download,const std::string &file_name )
	{
		if(!Utility::isFileORDirExist(MGBundle::mainBundle()->tmpPathWithFileName("downloads/")))
		{
			Utility::createDirOnPath(MGBundle::mainBundle()->tmpPathWithFileName("downloads/"));
		}
		download->setDestination(MGBundle::mainBundle()->tmpPathWithFileName("downloads/"+file_name));
	}

	void MGURLDownloadDelegate::downloadDidReceiveData( MGURLDownload *download,int receive_bytes,int total_bytes )
	{
	}

	void MGURLDownloadDelegate::downloadDidFinishDownloading( MGURLDownload *download )
	{

	}

	void MGURLDownloadDelegate::downloadDidCancelDownloading( MGURLDownload *download )
	{

	}

	void MGURLDownloadDelegate::downloadDidFailWithError( MGURLDownload *download,const MGError&error )
	{

	}

}
