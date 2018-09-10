#include "../../Thread/MGThread.h"
#include "MGURLFetcher.h"
#include "../../Shared/MGLog.h"
#include "../../Shared/Utility.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <sstream>

using namespace std;
using namespace z11;

namespace z11
{

	MGURLFetcher::MGURLFetcher(const std::string &url)
		:m_url(url)
	{
		request=new curlpp::Easy;
		m_connect_time_out = 10;
        m_time_out = 999999;
        m_verify_host = 0L;
        is_follow_location = false;
	}

	void MGURLFetcher::setURL( const std::string &url )
	{
		m_url=url;
	}

	const std::string & MGURLFetcher::getURL()
	{
		return m_url;
	}
    
	curlpp::Easy* MGURLFetcher::getRequest() const
	{
		return request;
	}

    void MGURLFetcher::setHttpHeader(const std::list<std::string> &header)
    {
        request->setOpt(curlpp::options::HttpHeader(header));
    }
    
    void MGURLFetcher::setVerifyHost(long v)
    {
        m_verify_host = v;
    }
    
    void MGURLFetcher::setUserPwd(const std::string &user, const std::string &pwd)
    {
        request->setOpt(curlpp::options::UserPwd(user+":"+pwd));
    }

	bool MGURLFetcher::fetch( std::string &fetch_result )
	{
		std::ostringstream os;
		curlpp::options::WriteStream ws(&os);
		request->setOpt(ws);
		request->setOpt(curlpp::options::Url(m_url));
		request->setOpt(curlpp::options::ConnectTimeout(m_connect_time_out));
		request->setOpt(curlpp::options::Timeout(m_time_out));
        // If not set this attr, it will crash when dns failed.
        request->setOpt(curlpp::options::FollowLocation(is_follow_location));
        request->setOpt(curlpp::options::NoSignal(true));
		request->setOpt(curlpp::options::SslVerifyPeer(false));
		request->setOpt(curlpp::options::SslVerifyHost(m_verify_host));
		//request->setOpt(curlpp::options::Proxy("192.168.115.16"));
		//request->setOpt(curlpp::options::ProxyPort(50021));
		bool ret = perform();
		if (ret)
		{
			fetch_result=os.str();
		}
		request->reset();
		return ret;

	}

	bool MGURLFetcher::fetch( std::string &fetch_result, int& code )
	{
		std::ostringstream os;
		curlpp::options::WriteStream ws(&os);
		request->setOpt(ws);
		request->setOpt(curlpp::options::Url(m_url));
		request->setOpt(curlpp::options::ConnectTimeout(m_connect_time_out));
		request->setOpt(curlpp::options::Timeout(m_time_out));
        // If not set this attr, it will crash when dns failed.
        request->setOpt(curlpp::options::FollowLocation(is_follow_location));
        request->setOpt(curlpp::options::NoSignal(true));
		request->setOpt(curlpp::options::SslVerifyPeer(false));
		request->setOpt(curlpp::options::SslVerifyHost(m_verify_host));
		//request->setOpt(curlpp::options::Proxy("192.168.115.16"));
		//request->setOpt(curlpp::options::ProxyPort(50021));
		bool ret = perform(code);
		if (ret)
		{
			fetch_result=os.str();
		}
		request->reset();
		return ret;
	}

	bool MGURLFetcher::downloadFile(const std::string &file_path )
	{
		FILE *file=fopen(file_path.c_str(),"wb");
		assert(file!=NULL);
		curlpp::options::WriteFile wf(file);
		request->setOpt(wf);
		request->setOpt(curlpp::options::Url(m_url));
        request->setOpt(curlpp::options::NoSignal(true));
        
		bool ret = perform();
		fclose(file);
		if(!ret)
		{
			Utility::delAFile(file_path);
		}
		request->reset();
		return ret;
	}

	bool MGURLFetcher::downloadFile(const std::string &file_path, int& code )
	{
		FILE *file=fopen(file_path.c_str(),"wb");
		assert(file!=NULL);
		curlpp::options::WriteFile wf(file);
		request->setOpt(wf);
		request->setOpt(curlpp::options::Url(m_url));
        request->setOpt(curlpp::options::NoSignal(true));
        
		bool ret = perform(code);
		fclose(file);
		if(!ret)
		{
			Utility::delAFile(file_path);
		}
		request->reset();
		return ret;
	}

	MGURLFetcher::~MGURLFetcher()
	{
		delete_and_set_null(request);
	}

	bool MGURLFetcher::perform()
	{
		bool ret=true;

		try
		{
			request->perform();
		}
		catch(curlpp::LibcurlRuntimeError &e)
		{
			MGLogD("curlpp::LibcurlRuntimeError code=%d:%s",e.whatCode(),e.what());
			ret= false;
		}
		catch( curlpp::RuntimeError &e )
		{
			MGLogD(e.what());
			ret= false;
		}
		catch( curlpp::LogicError &e )
		{
			MGLogD(e.what());
			ret= false;
		}
        catch(exception &e)
        {
            MGLogD(e.what());
			ret= false;
        }
		return ret;
	}

	bool MGURLFetcher::perform(int& code)
	{
		bool ret=true;

		try
		{
			request->perform();
		}
		catch(curlpp::LibcurlRuntimeError &e)
		{
			code = e.whatCode();
			ret= false;
		}
		catch( curlpp::RuntimeError &e )
		{
			MGLogD(e.what());
			ret= false;
		}
		catch( curlpp::LogicError &e )
		{
			MGLogD(e.what());
			ret= false;
		}
        catch(exception &e)
        {
            MGLogD(e.what());
			ret= false;
        }
		return ret;
	}

	void MGURLFetcher::setConnectTimeOut(long connect_time_out)
	{
		m_connect_time_out = connect_time_out;
	}
    
    void MGURLFetcher::setTimeOut(long time_out)
    {
        m_time_out = time_out;
    }
}
