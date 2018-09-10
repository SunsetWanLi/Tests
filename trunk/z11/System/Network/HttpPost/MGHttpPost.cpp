//
//  MGHttpPost.cpp
//  System
//
//  Created by slj on 13-3-8.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//
#include "../../Shared/MGLog.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <sstream>
#include "MGHttpPost.h"
#include "../../../CommonBase/MGData.h"
#include <cstdio>

using namespace std;

namespace z11
{
	char* file_data = NULL;
	size_t readData(char* buffer, size_t size, size_t nitems)
	{
		MGLogD("buffer=%d, size=%d, nitems=%d", strlen(buffer), size, nitems);
		strncpy(buffer, file_data, size*nitems);
		return size*nitems;
	}

	MGNameValuePair::MGNameValuePair()
	{

	}

	MGNameValuePair::MGNameValuePair(const std::string &name, const std::string &value)
	{
		this->name = name;
		this->value = value;
	}

	MGHttpPost::MGHttpPost()
	{
		request = new curlpp::Easy();
		m_connect_time_out = 10;
		m_time_out = 999999;
        m_verify_host = 0L;
		posted_json = "";
	}

	MGHttpPost::~MGHttpPost()
	{
		m_file_pair.clear();
		request->reset();
		delete request;
		request = NULL;
	}

	void MGHttpPost::setConnectTimeOut(long connect_time_out)
	{
		m_connect_time_out = connect_time_out;
	}

	void MGHttpPost::setTimeOut(long time_out)
	{
		m_time_out = time_out;
	}
    
    void MGHttpPost::setVerifyHost(long v)
    {
        m_verify_host = v;
    }

	void MGHttpPost::setHttpHeader(const std::list<std::string> &headers)
    {
        m_headers = headers;
    }

	//void MGHttpPost::setProxy(const std::string &host)
	//{
	//	curl_easy_setopt(request , CURLOPT_PROXY , host);
	//}

	curlpp::Easy* MGHttpPost::getRequest() const
	{
		return request;
	}

	void MGHttpPost::setURL(const string &url)
	{
		m_url = url;
	}

	const string & MGHttpPost::getURL()
	{
		return m_url;
	}

	string MGHttpPost::sendPost(const std::list<MGNameValuePair> &params)
	{
		string ret = "";
		try
		{
			std::list<std::string> headers;

			if(!posted_json.empty()){
				headers.push_back("Accept: application/json");
				headers.push_back("Content-Type: application/json");
				headers.push_back("charsets: utf-8");
				char buf[64];
				int size = posted_json.size();
				sprintf(buf, "Content-Length: %d", size);
				headers.push_back(buf);

				request->setOpt(curlpp::options::PostFields(posted_json));
				request->setOpt(curlpp::options::PostFieldSize(posted_json.size()));
			}
            else{
                headers.push_back("Content-Type: multipart/form-data");
            }
            
			for (auto it = m_headers.begin(); it != m_headers.end(); ++it)
			{
				headers.push_back(*it);
			}
			if (headers.size() > 0)
			{
				request->setOpt(curlpp::options::HttpHeader(headers));
			}

			initCurlRequest(params);
            
			ostringstream os;
			curlpp::options::WriteStream ws(&os);
			request->setOpt(ws);

			request->perform();
			ret = os.str();
			request->reset();
		}
		catch(curlpp::LibcurlRuntimeError &e)
		{
			ret.clear();
		}

		return ret;
	}

	string MGHttpPost::sendPost(const std::list<MGNameValuePair> &params, int& code)
	{
		string ret = "";
		try
		{
			std::list<std::string> headers;

			if(!posted_json.empty()){
				headers.push_back("Accept: application/json");
				headers.push_back("Content-Type: application/json");
				headers.push_back("charsets: utf-8");
				char buf[64];
				int size = posted_json.size();
				sprintf(buf, "Content-Length: %d", size);
				headers.push_back(buf);

				request->setOpt(curlpp::options::PostFields(posted_json));
				request->setOpt(curlpp::options::PostFieldSize(posted_json.size()));
			}
            else{
                headers.push_back("Content-Type: multipart/form-data");
            }
            
			for (auto it = m_headers.begin(); it != m_headers.end(); ++it)
			{
				headers.push_back(*it);
			}
			if (headers.size() > 0)
			{
				request->setOpt(curlpp::options::HttpHeader(headers));
			}

			//curl_easy_setopt(request , CURLOPT_PROXY , "192.168.115.16:50021");

			initCurlRequest(params);

			ostringstream os;
			curlpp::options::WriteStream ws(&os);
			request->setOpt(ws);

			request->perform();
			ret = os.str();
			request->reset();
		}
		catch(curlpp::LibcurlRuntimeError &e)
		{
			code = e.whatCode();
			ret.clear();
		}

		return ret;
	}

	string MGHttpPost::sendPost(const std::list<MGNameValuePair> &params, const std::string &file_full_path)
	{
		string ret = "";
		MGData* data = MGData::dataWithContentsOfFile(file_full_path);
		if(data == NULL)
			return "";

		try
		{
			// init headers
			string sss = "1234567890";
			file_data = (char*)sss.c_str();


			int_32 size = strlen(file_data);
			std::list<std::string> headers;

			headers.push_back("Content-Type: text/*");
			char buf[64];
			sprintf(buf, "Content-Length: %d", size);
			headers.push_back(buf);
			if (headers.size() > 0)
			{
				request->setOpt(curlpp::options::HttpHeader(headers));
			}

			//curl_easy_setopt(request , CURLOPT_PROXY , "192.168.115.16:50021");
			// init curl
			initCurlRequest(params);
			ostringstream os;
			curlpp::options::WriteStream ws(&os);
			request->setOpt(ws);

			// curlpp::FormParts::File

			//  request->setOpt(new curlpp::options::);
			//  request->setOpt(new curlpp::options::Verbose(true));
			request->setOpt(new curlpp::options::InfileSize(size));
			// request->setOpt(new curlpp::options::re;
			request->setOpt(new curlpp::options::Upload(true));
			request->setOpt(new curlpp::options::HttpHeader(headers));
			request->setOpt(new curlpp::options::ReadFunction(curlpp::types::ReadFunctionFunctor(readData)));
			//request->setOpt(new curlpp::options::PostFields("abcd"));
			//request->setOpt(new curlpp::options::PostFieldSize(5));

			request->perform();
			ret = os.str();
			request->reset();
		}
		catch(curlpp::LibcurlRuntimeError &e)
		{
			MGLogD(e.what());
		}
		catch(curlpp::LogicError &e)
		{
			MGLogD(e.what());
		}
		catch(cURLpp::CallbackExceptionBase &e)
		{
			MGLogD(e.what());
		}
		catch(exception &e)
		{
			MGLogD(e.what());
		}

		return ret;
	}

	void MGHttpPost::initCurlRequest(const std::list<MGNameValuePair> &params)
	{
		curlpp::Forms formParts;
		std::list<MGNameValuePair>::const_iterator param_it = params.begin();
		for(; param_it != params.end(); ++param_it)
		{
			formParts.push_back(new curlpp::FormParts::Content((*param_it).name, (*param_it).value));
		}
		param_it = m_file_pair.begin();
		for(;param_it!=m_file_pair.end();param_it++){
			formParts.push_back(new curlpp::FormParts::File((*param_it).name, (*param_it).value));
		}

		request->setOpt(curlpp::options::Url(m_url));
		if(posted_json.empty())
			request->setOpt(curlpp::options::HttpPost(formParts));
		request->setOpt(curlpp::options::ConnectTimeout(m_connect_time_out));
		request->setOpt(curlpp::options::Timeout(m_time_out));
		request->setOpt(curlpp::options::NoSignal(true));
        request->setOpt(curlpp::options::SslVerifyPeer(false));
        request->setOpt(curlpp::options::SslVerifyHost(m_verify_host));
		//curl_easy_setopt(request , CURLOPT_PROXY , "192.168.115.16:50021");
	}
	void MGHttpPost::pushFilePair(const MGNameValuePair &param)
	{
		m_file_pair.push_back(param);
	}
    void MGHttpPost::clearFilePair()
    {
        m_file_pair.clear();
    }
	void MGHttpPost::setPostJson(const std::string &json)
	{
		posted_json  = json;
	}
	    //FILE *f;
	    //const char * remotepath = path.c_str();
	    //const char * localpath = local_path.c_str();
	    //long uploaded_len = 0;
	    //CURLcode r = CURLE_GOT_NOTHING;
	    //int c;
	    //f = fopen(localpath, "rb");
	    //if (f == NULL) {
	    //    perror(NULL);
	    //    return 0;
	    //}
	    //curl_easy_setopt(curlhandle, CURLOPT_UPLOAD, 1L);
	    //curl_easy_setopt(curlhandle, CURLOPT_URL, remotepath);
	    //if (timeout)
	    //    curl_easy_setopt(curlhandle, CURLOPT_FTP_RESPONSE_TIMEOUT, timeout);
	    //    curl_easy_setopt(curlhandle, CURLOPT_HEADERFUNCTION, getContentLength);
	    //    curl_easy_setopt(curlhandle, CURLOPT_HEADERDATA, &uploaded_len);
	    //    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, discardfunc);
	    //    curl_easy_setopt(curlhandle, CURLOPT_READFUNCTION, readfunc);
	    //    curl_easy_setopt(curlhandle, CURLOPT_READDATA, f);
	    //    curl_easy_setopt(curlhandle, CURLOPT_FTPPORT, "-"); /* disable passive mode */
	    //    curl_easy_setopt(curlhandle, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
	    //    curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L);
	    //    for (c = 0; (r != CURLE_OK) && (c < tries); c++) {
	    //        /* are we resuming? */
	    //        /* determine the length of the file already written */
	    //        /*
	    //         * With NOBODY and NOHEADER, libcurl will issue a SIZE
	    //         * command, but the only way to retrieve the result is
	    //         * to parse the returned Content-Length header. Thus,
	    //         * getContentLength(). We need discardfunc() above
	    //         * because HEADER will dump the headers to stdout
	    //         * without it. */
	    //        if (c) { /* yes */
	    //            curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 1L);
	    //            curl_easy_setopt(curlhandle, CURLOPT_HEADER, 1L);
	    //            r = curl_easy_perform(curlhandle);
	    //            if (r != CURLE_OK)
	    //                continue;
	    //            curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 0L);
	    //            curl_easy_setopt(curlhandle, CURLOPT_HEADER, 0L);
	    //            fseek(f, uploaded_len, SEEK_SET);
	    //            curl_easy_setopt(curlhandle, CURLOPT_APPEND, 1L);
	    //            
	    //        }
	    //        else { /* no */
	    //            curl_easy_setopt(curlhandle, CURLOPT_APPEND, 0L);
	    //        }
	    //        
	    //        r = curl_easy_perform(curlhandle);
	    //        
	    //    }
	    //
	    //fclose(f);
	    //if (r == CURLE_OK)
	    //    return 1;
	    //
	    //else {
	    //    fprintf(stderr, "%s/n", curl_easy_strerror(r));
	    //    return 0;
	    //}
}

