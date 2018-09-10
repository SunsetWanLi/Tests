//
//  MGHttpPost.cpp
//  System
//
//  Created by slj on 13-3-8.
//  Copyright (c) 2013年 Zhejiang University. All rights reserved.
//
#include "../../Shared/MGLog.h"
#include "../../Shared/Utility.h"
#include "FtpUtil.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/infos.hpp>
#include <sstream>

using namespace std;

namespace z11
{
	static size_t write_file_callback(void *stream, void *buffer, size_t size, size_t nmemb)
	{
		FILE *file = (FILE *)stream;
		if(file != NULL)
		{
			return fwrite(buffer, size, nmemb, file);
		}
		else
		{
			return size*nmemb;
		}
	}

	size_t read_file_callback(void *stream, void *ptr, size_t size, size_t nmemb)
	{
		FILE *f = (FILE *)stream;
		size_t n;

		if (ferror(f))
		{
			return CURL_READFUNC_ABORT;
		}

		n = fread(ptr, size, nmemb, f) * size;

		return n;
	}

	static size_t discard_callback(void *ptr, size_t size, size_t nmemb)
	{
		return size*nmemb;
	}

	FtpUtil::FtpUtil()
	{
		request = new curlpp::Easy();
		m_port = 21;

		m_time_out = 999999;
		m_curl_code = CURLE_COULDNT_CONNECT;
	}

	FtpUtil::~FtpUtil()
	{
		disconnect();
		delete_and_set_null(request);
	}

	// connect
	bool FtpUtil::connect()
	{
        initCommonParams();
		request->setOpt(curlpp::options::Url(m_server));
        
		return perform();
	}
    
    void FtpUtil::initCommonParams()
    {
        request->setOpt(curlpp::options::NoSignal(true));
		request->setOpt(curlpp::options::Timeout(m_time_out));
		request->setOpt(curlpp::options::UserPwd(m_user+":"+m_password));
        request->setOpt(curlpp::options::FileTime(true));
		request->setOpt(curlpp::options::Upload(false));
		request->setOpt(curlpp::options::WriteFunction(discard_callback));
    }

	bool FtpUtil::perform()
	{
		CURLcode tmp_curl_code = CURLE_OK;

		try
		{
			request->perform();
		}
		catch(curlpp::LibcurlRuntimeError &e)
		{
			tmp_curl_code = e.whatCode();
			MGLogD("curlpp::LibcurlRuntimeError code=[%d:%s]", tmp_curl_code, e.what());
		}
		catch(curlpp::LibcurlLogicError &e)
		{
			tmp_curl_code = e.whatCode();
			MGLogD("curlpp::LibcurlLogicError code=[%d:%s]", tmp_curl_code, e.what());
		}
		catch(curlpp::CallbackExceptionBase &e)
		{
			tmp_curl_code = CURLE_COULDNT_CONNECT;
			MGLogD("curlpp::CallbackExceptionBase code=[%d:%s]", tmp_curl_code, e.what());
		}
		catch(exception &e)
		{
			tmp_curl_code = CURLE_COULDNT_CONNECT;
			MGLogD("curlpp::exception code=[%d:%s]", tmp_curl_code, e.what());
		}
		
		if(CURLE_OK != tmp_curl_code)
		{
			disconnect();
		}
		m_curl_code = tmp_curl_code;
        
        if(CURLE_OK == m_curl_code)
        {
            return true;
        }
        else
        {
			MGLogD("curl error: [%s]", curl_easy_strerror(tmp_curl_code));
            return false;
        }
	}

	void FtpUtil::disconnect()
	{
		request->reset();
		m_curl_code = CURLE_COULDNT_CONNECT;
	}
	
	// properties
	void FtpUtil::setServer(const string &server, int port/* = 21*/)
	{
		m_server = server;
		m_port = port;
	}
	
	void FtpUtil::setUserAndPassword(const string &user, const string &password)
	{
		m_user = user;
		m_password = password;
	}
    
    list<FtpParseInfo> FtpUtil::getList(const string &remote_dir)
    {
        initCommonParams();
 		std::ostringstream os;
 		curlpp::options::WriteStream ws(&os);
 		request->setOpt(ws);
        request->setOpt(curlpp::options::Url(remote_dir));
		request->setOpt(curlpp::options::CustomRequest("LIST"));
		perform();
        
        list<FtpParseInfo> ret;
		
		if(CURLE_OK == m_curl_code)
		{
			string result = os.str();
            
#ifdef WIN32
			string seperator_key = "\r\n";
#else
            string seperator_key = "\n";
#endif
            
			string::size_type begin_pos = 0;
			string::size_type end_pos = result.find(seperator_key);
            
			while(string::npos != end_pos)
			{
                FtpParseInfo info;
                string original = result.substr(begin_pos, end_pos-begin_pos);
                if(FtpParseInfo::ftpparse(info, (char*)original.c_str(), original.size()) > 0)
                {
                    ret.push_back(info);
                }
				begin_pos = end_pos+seperator_key.size();
				end_pos = result.find(seperator_key, begin_pos);
			}
		}
        
		return ret;
    }
    
    double FtpUtil::getFileSize(const string &remote_file_path)
    {
        initCommonParams();
        request->setOpt(curlpp::options::Url(remote_file_path));
        perform();
        
        double file_size = -1;
        curlpp::InfoGetter info;
        info.get(*request, CURLINFO_CONTENT_LENGTH_DOWNLOAD, file_size);
        
        return file_size;
    }
    
    bool FtpUtil::initParentDirAndName(std::string &parent, std::string &name, const string &full_dir)
    {
        string tmp_dir = full_dir;
        string::iterator it = tmp_dir.end()-1;
        while((*it) == '/')
        {
            tmp_dir.erase(it);
            it = tmp_dir.end()-1;
        }
        
        parent = "";
        name = "";
        
        string::size_type last_pos = tmp_dir.find_last_of('/');
        string::size_type first_pos = tmp_dir.find_first_of("//")+1;
        if(first_pos == string::npos)
        {
            return false;
        }
        
        if(string::npos != last_pos && first_pos < last_pos)
        {
            parent = tmp_dir.substr(0, last_pos+1);
            name = tmp_dir.substr(last_pos+1);
            return true;
        }
        else
        {
            MGLogD("wrong ftp address: [%s]", full_dir.c_str());
            return false;
        }
    }
    
    bool FtpUtil::postQuote(std::string &remote_dir, const std::string &value)
    {
        initCommonParams();
 		std::ostringstream os;
 		curlpp::options::WriteStream ws(&os);
 		request->setOpt(ws);
        request->setOpt(curlpp::options::Url(remote_dir));
        
        list<string> slist;
        slist.push_back(value);
        request->setOpt(curlpp::options::PostQuote(slist));
        
		return perform();
    }

	bool FtpUtil::createDir(const std::string &remote_dir)
	{
		string parent, name;
        if(initParentDirAndName(parent, name, remote_dir))
        {
            string value = "MKD "+name;
            return postQuote(parent, value);
        }
        else
        {
            return false;
        }
	}

	bool FtpUtil::removeDir(const std::string &remote_dir)
	{
        string parent, name;
        if(initParentDirAndName(parent, name, remote_dir))
        {
            string value = "RMD "+name;
            return postQuote(parent, value);
        }
        else
        {
            return false;
        }
    }
    
    bool FtpUtil::removeFile(const std::string &remote_file_path)
    {
        string parent, name;
        if(initParentDirAndName(parent, name, remote_file_path))
        {
            string value = "DELE "+name;
            return postQuote(parent, value);
        }
        else
        {
            return false;
        }
    }

	bool FtpUtil::upload(const std::string &remote_file_path, const std::string &local_file_path)
	{
		FILE* local_file = fopen(local_file_path.c_str(), "ab+");
		if(local_file == NULL)
		{
			MGLogD("fopen failed: [%s]", local_file_path.c_str());
			return false;
		}
		
		disconnect();

		initCommonParams();
		request->setOpt(curlpp::options::Upload(true));
		request->setOpt(curlpp::options::FileTime(false));
		request->setOpt(curlpp::options::Url(remote_file_path));
		curlpp::types::WriteFunctionFunctor read_func = utilspp::BindFirst(utilspp::make_functor(&read_file_callback), local_file);
		request->setOpt(curlpp::options::ReadFunction(read_func));
		request->setOpt(curlpp::options::ReadFile(local_file));
		request->setOpt(curlpp::options::FtpPort("-")); /* disable passive mode */
		request->setOpt(curlpp::options::FtpCreateMissingDirs(true));
		
		bool ret = perform();
		fclose(local_file);

		disconnect();
		return ret;
	}

	bool FtpUtil::download(const std::string &remote_file_path, const std::string &local_file_path)
	{
        FILE* local_file = fopen(local_file_path.c_str(), "ab+");
        if(local_file == NULL)
        {
            MGLogD("fopen failed: [%s]", local_file_path.c_str());
            return false;
        }

		disconnect();
        
        initCommonParams();
        request->setOpt(curlpp::options::Url(remote_file_path));
		curlpp::types::WriteFunctionFunctor write_func = utilspp::BindFirst(utilspp::make_functor(&write_file_callback), local_file);
        request->setOpt(curlpp::options::WriteFunction(write_func));
        fseek(local_file, 0, SEEK_END);
        curl_off_t off_set = ftell(local_file);
		request->setOpt(curlpp::options::ResumeFromLarge(off_set));
        bool ret = perform();
        
        fclose(local_file);

		disconnect();
        
        return ret;
	}
}