#include "MGURLConnection.h"
#include "../../Shared/MGLog.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <sstream>
#include "../../../CommonBase/MGMutableData.h"
#include "../../Thread/MGThread.h"
using namespace std;
using namespace z11;

namespace z11
{
	class MGURLConnectionThread:public MGThread
	{
	public:
		MGURLConnection *m_connection;
		MGURLConnectionThread(MGURLConnection *connection)
		{
			m_connection=connection;
		}
		~MGURLConnectionThread()
		{
		}
		virtual bool threadLoop()
		{
			m_connection->start();
			return false;
		}
	};
	size_t MGURLConnectionHeaderCallback(MGURLConnection *connection, char* ptr, size_t size, size_t nmemb)
	{
		if (connection!=NULL)
		{
			if (strcmp(ptr,"\r\n")!=0)
			{
				string response_line(ptr);
				size_t pos=response_line.find_first_of(':');
				if (pos!=string::npos)
				{
					connection->header_response_map.setObjectForKey(response_line.substr(pos+2,response_line.size()-pos-4),response_line.substr(0,pos));
				}
				//MGLogD("%s",ptr);
			}
			else
			{
				connection->total_bytes=0;
				std::string content_length=connection->header_response_map.objectForKey("Content-Length");
				if(content_length!="")
				{

					sscanf(content_length.c_str(),"%d",&(connection->total_bytes));
				}
				if (connection->total_bytes==0)
				{
					MGLogD("MGURLConnection Error:Unknown connection size.");
				}
				connection->m_response=MGURLResponse::initWithURL(connection->m_request->URL(),
					connection->header_response_map.objectForKey("Content-Type"),connection->total_bytes,connection->header_response_map.objectForKey("Content-Encoding")
				);
				connection->m_response->allHTTPHeaderFields()->initWithDictionary(&connection->header_response_map);
				if (connection->m_delegate!=NULL)
				{
					connection->m_delegate->connectionDidReceiveResponse(connection,connection->m_response);
				}
				//connection->receive_bytes=0;
			}
		}
		return size*nmemb;
		//return fwrite(ptr, size, nmemb, f);
	};
	
	size_t MGURLConnectionWriteCallback(MGURLConnection *connection, char* ptr, size_t size, size_t nmemb)
	{
		AUTO_LOCK(&connection->write_locker);
		if (connection->m_state==MGURLConnectionStateCancel)
		{
			return 0;
		}
		
		if (connection!=NULL&&connection->m_delegate!=NULL)
		{
			MGData *data=MGData::dataWithBytesNoCopy(ptr,size*nmemb,false);
			connection->m_delegate->connectionDidReceiveData(connection,data);
			connection->receive_bytes+=size*nmemb;
			if (connection->receive_bytes>=connection->total_bytes)
			{
				connection->m_delegate->connectionDidFinishLoading(connection);
			}
			delete_and_set_null(data);
		}
		return size*nmemb;
	}
	size_t MGURLConnectionReadCallback(MGURLConnection *connection, char* ptr, size_t size, size_t nmemb)
	{
		if (connection->m_state==MGURLConnectionStateCancel)
		{
			return 0;
		}

		if (connection!=NULL)
		{
			return connection->m_data_reader->read(ptr,size*nmemb);
		}
		return 0;
	}
	int MGURLConnectionDebugCallback(MGURLConnection *connection,curl_infotype type, char *str, size_t len)
	{
		if (connection!=NULL)
		{
			MGLogD("DebugOutput %d:%s",type,str);
		}
		return 0;
	}
	MGURLConnection::~MGURLConnection()
	{
		if (m_thread!=NULL)
		{
			m_thread->requestExitAndWait();
			delete_and_set_null(m_thread);
		}
		m_request->release();
		delete_and_set_null(easy_request);
		delete_and_set_null(m_response);
		delete_and_set_null(m_data_reader);
	}


	MGURLConnection* MGURLConnection::connectionWithRequest(const  MGURLRequest *request,MGURLConnectionDelegate *delegate )
	{
		MGURLConnection* connection=new MGURLConnection();
		connection->m_request=MGURLRequest::requestWithRequest(request);
		connection->m_delegate=delegate;
		connection->m_thread=new MGURLConnectionThread(connection);
		connection->m_thread->run("MGURLConnectionThread");
		return connection;
	}

	MGURLConnection::MGURLConnection( MGURLRequest *request,MGURLConnectionDelegate *delegate  )
	{
		m_request=request;
		m_delegate=delegate;
		receive_bytes=0;
		m_response=NULL;
		easy_request=NULL;
		m_thread=NULL;
		m_data_reader=NULL;
		start();
	}

	MGURLConnection::MGURLConnection()
	{
		m_request=NULL;
		m_delegate=NULL;
		m_response=NULL;
		easy_request=NULL;
		receive_bytes=0;
		m_thread=NULL;
		m_data_reader=NULL;
	}

	class SynchronousConnectionDelegate:public MGURLConnectionDelegate
	{
	public:
		MGMutableData *recv_data;
		SynchronousConnectionDelegate()
		{
			recv_data=NULL;
		}
		~SynchronousConnectionDelegate()
		{
			delete_and_set_null(recv_data);
		}
		virtual void connectionDidReceiveResponse(MGURLConnection *connection,MGURLResponse *response)
		{
			if (response!=NULL)
			{
				if (recv_data==NULL)
				{
					recv_data=MGMutableData::dataWithCapacity(response->expectedContentLength());
				}
			}
		}
		virtual void connectionDidReceiveData(MGURLConnection *connection,MGData *data)
		{
			if (recv_data!=NULL)
			{
				recv_data->appendData(data);
			}
		}
		virtual void connectionDidFinishLoading(MGURLConnection *connection)
		{
			//do nothing
		}
		virtual void connectionDidCancelLoading(MGURLConnection *connection)
		{
			//do nothing
		}
	};
	MGData * MGURLConnection::sendSynchronousRequest(const MGURLRequest *request,MGURLResponse **response )
	{
		SynchronousConnectionDelegate connection_delegate;
		MGURLConnection connection;
		connection.m_request=MGURLRequest::requestWithRequest(request);
		connection.m_delegate=&connection_delegate;
		connection.start();
		if (connection.m_response!=NULL&&response!=NULL)
		{
			*response=MGURLResponse::responseWithResponse(connection.m_response);
		}
		if (connection_delegate.recv_data!=NULL&&connection_delegate.recv_data->length())
		{
			return MGData::dataWithData(connection_delegate.recv_data);
		}
		else
			return NULL;
	}

	void MGURLConnection::initWithRequest(const  MGURLRequest *request,MGURLConnectionDelegate *delegate,bool startImmediately )
	{
		m_request=MGURLRequest::requestWithRequest(request);
		m_delegate=delegate;
		if (startImmediately)
		{
			start();
		}
	}

	void MGURLConnection::start()
	{
		if (easy_request==NULL)
		{
			easy_request=new curlpp::Easy();
		}
		
		if(m_request!=0)
		{
			std::string http_field=m_request->allHTTPHeaderFields()->objectForKey("Range");
			if (http_field!="")
			{
				easy_request->setOpt(curlpp::options::Range(http_field));
			}
			std::string upload=m_request->allHTTPHeaderFields()->objectForKey("Upload");
			if (upload=="true")
			{
				easy_request->setOpt(curlpp::options::Upload(true));
				std::string upload_file=m_request->allHTTPHeaderFields()->objectForKey("UploadFileName");
				if (upload_file!="")
				{
					m_data_reader=new MGDataReader(upload_file);
					curlpp::types::ReadFunctionFunctor reader_functor(utilspp::BindFirst(utilspp::make_functor(&MGURLConnectionReadCallback), this));
					easy_request->setOpt(curlpp::options::ReadFunction(reader_functor));
					easy_request->setOpt(curlpp::options::InfileSize(m_data_reader->available()));
					easy_request->setOpt(curlpp::options::FtpCreateMissingDirs(true));

				}
			}

			std::string method=m_request->allHTTPHeaderFields()->objectForKey("RequestMethod");
			if (method=="PUT")
			{
				m_data_reader = new MGDataReader();
				m_data_reader->initWithData(m_request->HTTPBody());
				curlpp::types::ReadFunctionFunctor reader_functor(utilspp::BindFirst(utilspp::make_functor(&MGURLConnectionReadCallback), this));
				easy_request->setOpt(curlpp::options::ReadFunction(reader_functor));
				easy_request->setOpt(curlpp::options::InfileSize(m_data_reader->available()));
				easy_request->setOpt(curlpp::options::FtpCreateMissingDirs(true));
			}
			else if(method=="POST")
			{
				easy_request->setOpt(curlpp::options::PostFields(string((char*)m_request->HTTPBody()->bytes())));
				easy_request->setOpt(curlpp::options::PostFieldSize(m_request->HTTPBody()->length()));
			}
			//if (method.empty()||method=="GET")
			{
				//donothing
			}

			easy_request->setOpt(curlpp::options::ConnectTimeout(m_request->timeoutInterval()));

			std::string verbose=m_request->allHTTPHeaderFields()->objectForKey("Verbose");
			if (verbose=="true")
			{
				easy_request->setOpt(curlpp::options::Verbose(true));
				curlpp::types::DebugFunctionFunctor debug_functor(utilspp::BindFirst(utilspp::make_functor(&MGURLConnectionDebugCallback), this));
				easy_request->setOpt(curlpp::options::DebugFunction(debug_functor));
			}
		}

		curlpp::types::WriteFunctionFunctor header_functor(utilspp::BindFirst(utilspp::make_functor(&MGURLConnectionHeaderCallback), this));
		easy_request->setOpt(curlpp::options::HeaderFunction(header_functor));
		easy_request->setOpt(curlpp::options::Url(m_request->URL()->absoluteString()));

		m_state=MGURLConnectionStateInit;

		curlpp::types::WriteFunctionFunctor writer_functor(utilspp::BindFirst(utilspp::make_functor(&MGURLConnectionWriteCallback), this));
		easy_request->setOpt(curlpp::options::WriteFunction(writer_functor));
        easy_request->setOpt(curlpp::options::NoSignal(true));
		try
		{
			m_state=MGURLConnectionStateRunning;
			easy_request->perform();
		}
		catch(curlpp::LibcurlRuntimeError &e)
		{
			//cancelled
			if (e.whatCode()==CURLE_WRITE_ERROR)
			{
				if (this->m_delegate!=NULL)
				{
					this->m_delegate->connectionDidCancelLoading(this);
				}
			}
			else
			{
				if (this->m_delegate!=NULL)
				{
					this->m_delegate->connectionDidFailWithError(this,MGError(e.what()));
				}
				MGLogD("curlpp::LibcurlRuntimeError code=%d:%s",e.whatCode(),e.what());
			}

		}
		m_state=MGURLConnectionStateFinish;
		easy_request->reset();
	}

	void MGURLConnection::cancel()
	{
		AUTO_LOCK(&write_locker);
		m_state=MGURLConnectionStateCancel;
	}

	MGURLRequest * MGURLConnection::request()
	{
		return m_request;
	}

	void MGURLConnectionDelegate::connectionDidFailWithError( MGURLConnection *connection,const MGError&error )
	{

	}

}
