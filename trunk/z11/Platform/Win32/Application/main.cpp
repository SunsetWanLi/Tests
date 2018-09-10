#include "GUIRootView/GUIRootViewController.h"
//#include "Soap/SoapDelegate.h"
#include "MGFoundation.h"
#include "MIKit.h"
#include <sstream>
//#pragma comment(lib, "Wldap32.lib")

#include "MapKit/Fetcher/Pyramid/MKGlobalMapTiles.h"
#include "MapKit/Fetcher/WebDataFetcher/MKWebDataFetcher.h"
#include "System/Network/URLFetcher/MGURLFetcher.h"
#include "Platform/MGFileUtils.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include "System/Network/URL/MGURLConnection.h"

void fetcher_main()
{
	string result;
	MGURLFetcher fetcher("http://automation.whatismyip.com/n09230945.asp");
	if(fetcher.fetch(result))
	{
		MGLogD("GetURLDataBycurl = %s",result.c_str());
	}
	result.clear();
	fetcher.setURL("http://mt2.google.cn/vt/lyrs=m@177000000&hl=zh-CN&gl=cn&src=app&x=104&y=56&z=7&s=Galileo");

	if(fetcher.fetch(result))
	{
		MGLogD("GetURLDataBycurl = %s",result.c_str());
	}
}

class  AppDelegate : public z11::MGApplication
{
public:
	virtual bool applicationDidFinishLaunching()
	{
		MGApplication::applicationDidFinishLaunching();
		//MGWebService::setSystemDelegate(new SoapDelegate());
		gui_root = GUIRootViewController::getInstance(false);
		gui_root->initManually();
		//GuiContainer::container->light_graphic->setFps(400);
		//GuiContainer::disableAnimation();
		return true;
	}
};

int ids=0;
void MutiLog()
{
	if (ids==0)
	{
		ids++;
		LogStart();
	}
}
void MultiLogEnd()
{
	if (--ids==0)
	{
		LogEnd("MultiLogEnd");
	}
}
class MKWebDataMultiFetcher:public MGThread
{
public:
	void setID(int _id)
	{
		id=_id;
	}
	int id;
	curlpp::Easy myRequest;
	void addTask( int x,int y,int z ,MKMapType map_type)
	{

	}
	vector<std::string> urls;
	void addTask( const std::string &url)
	{
		urls.push_back(url);
	}
	virtual bool threadLoop()
	{
		MutiLog();
		char path[256];
		try
		{
			for (int i=0;i<urls.size();i++)
			{
				myRequest.setOpt(curlpp::options::Url(std::string(urls[i])));
				std::ostringstream os;
				curlpp::options::WriteStream ws(&os);
				myRequest.setOpt(ws);
				sprintf(path,"D:/%d/%d.png",id,i);
				myRequest.perform();
				FILE *file=fopen(path,"wb");
				string buffer=os.str();
				fwrite(buffer.c_str(),buffer.size(),1,file);
				fclose(file);

			}
		}
		catch( curlpp::RuntimeError &e )
		{
			MGLogD(e.what());
		}

		catch( curlpp::LogicError &e )
		{
			MGLogD(e.what());
		}
		MultiLogEnd();
		return false;
	}
};
void fetcher_test()
{

	//#define M 10
	//	MKWebDataMultiFetcher fetcher[M];
	//	for (int i=0;i<M;i++)
	//	{
	//		fetcher[i].setID(i);
	//		for (int j=0;j<900/M;j++)
	//		{
	//			fetcher[i].addTask("http://mt1.google.cn/vt/lyrs=s@113&hl=zh-CN&gl=cn&src=app&x=103&y=55&z=7&s=Ga");
	//		}
	//	}
	//	for (int i=0;i<M;i++)
	//	{
	//		fetcher[i].run();
	//	}

}
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInstance,char * szCmd,int nCmdShow) 
{
	//MGFileUtils *utils=MGFileUtils::sharedFileUtils();
	//unsigned long zip_size=0;
	//unsigned char *text=utils->getFileDataFromZip("F:/GRF/Projects/z11/trunk/z11/Workspace/GNU/Sample/Demo/bin/com.example.demo.DemoActivity.apk","assets/test.txt",&zip_size);
	//{

	//}
	curlpp::Cleanup myCleanup;
	if(false)
	{
		//std::string str;
		//MGURLFetcher fetcher("http://115.236.166.215/bym/utype");
		//fetcher.fetch(str);
		//FILE *file=fopen("D:/1.html","wb");
		//fwrite(str.c_str(),str.size(),1,file);
		//fclose(file);	

		rapidxml::file<> fdoc("D:/1.html");
		rapidxml::xml_document<>  doc;
		doc.parse<0>(fdoc.data());
	}
	if(false)
	{
		char str[256];
		int t;
		double lat,lon;
		FILE *file2=fopen("D:/2.mif","w");
		FILE *file=fopen("D:/1.mif","r");
		while(fgets(str,256,file))
		{
			sscanf(str,"Pline %d",&t);
			MGLogD("Pline %d\n",t);
			fprintf(file2,"Pline %d\n",t);
			while(t--)
			{
				fgets(str,256,file);
				sscanf(str,"%lf %lf\n",&lat,&lon);
				fprintf(file2,"%.14lf %.14lf\n",120.10673274191483,30.416239756723652);
			}
			//break;
			t=0;
		}
		fclose(file);
		fclose(file2);
	}
	//fetcher_test
	//z11::MGDevice::setScreenWidthAndHeight(1200,750);
	z11::MGDevice::setScreenWidthAndHeight(480,800);
	AppDelegate m_app_main;
	if(false)
	{
		int a=120;
		int b=11;
		char str[256];
		sprintf(str,"%d %d",a+12,b-21);
		istringstream  stream;
		stream.str(str);
		stream>>a>>b;
		a++;
		b++;
	}
	if (false)
	{
		string m_url="http://a4.att.hudong.com/40/79/19300001306677131029792269727.jpg";
		
		//string read_data;
		//MGURLFetcher fetcher(m_url);
		//fetcher.fetch(read_data);
		FILE *file=fopen("D:/1.jpg","wb");
		curlpp::Easy *request=new curlpp::Easy;
		std::ostringstream os;
		curlpp::options::WriteStream ws(&os);
		request->setOpt(ws);
		request->setOpt(curlpp::options::Url(m_url));
		request->setOpt(curlpp::options::Range("0-100000"));


		try
		{

			request->perform();
			{
				string str=os.str();
				fwrite(str.c_str(),str.length(),1,file);
				os.str("");
			}

			request->setOpt(curlpp::options::Range("100000-481972"));
			request->perform();
			{
				string str=os.str();
				fwrite(str.c_str(),str.length(),1,file);
				os.str("");
			}

		}
		catch(curlpp::LibcurlRuntimeError &e)
		{
			MGLogD("curlpp::LibcurlRuntimeError code=%d:%s",e.whatCode(),e.what());
			//MGSleep(2000);
			//MGLogD("sleep");
		}
		catch( curlpp::RuntimeError &e )
		{
			MGLogD(e.what());
		}
		catch( curlpp::LogicError &e )
		{
			MGLogD(e.what());
		}
		fclose(file);
	}
	MGApplication::sharedApplication().run();
	return 0;
}