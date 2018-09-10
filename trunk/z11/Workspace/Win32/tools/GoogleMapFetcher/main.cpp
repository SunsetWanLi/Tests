#include "MGFoundation.h"
#include "MIKit.h"
#include "MapKit.h"
using namespace std;
std::string path_prefix="D:/E/ftp/upload/";
std::string input_path="standard/";
std::string satellite_output_path="satellite/";
std::string hybrid_output_path="hybrid/";
std::string standard_output_path="standard/";
std::string google_satelite_format="http://mt1.google.cn/vt/lyrs=s@113&hl=zh-CN&gl=cn&src=app&x=%d&y=%d&z=%d&s=Ga";
std::string google_hybrid_format="http://mt0.google.cn/vt/imgtp=png32&lyrs=h@177000000&hl=zh-CN&gl=cn&src=app&x=%d&y=%d&z=%d&s=Galile";
std::string google_standard_format="http://mt2.google.cn/vt/lyrs=m@177000000&hl=zh-CN&gl=cn&src=app&x=%d&y=%d&z=%d&s=Galileo";
std::string output_satelite_path_format="%s%d/%d/%d.jpg";
std::string output_hybrid_path_format="%s%d/%d/%d.png";
std::string output_standard_path_format="%s%d/%d/%d.png";
//root D:/E/ftp/upload/satellite/
//url_format "http://mt1.google.cn/vt/lyrs=s@113&hl=zh-CN&gl=cn&src=app&x=%d&y=%d&z=%d&s=Ga"
FILE *file=NULL;
void download(const std::string& root,const std::string &url_format,const std::string &output_format,int x,int y,int z)
{
	if (z<1)
	{
		return ;
	}
	char url[1024];
	{
		sprintf(url,output_format.c_str(),root.c_str(),z,x,y);
		if (Utility::isFileORDirExist(url))
		{
			return ;
		}
	}
	sprintf(url,url_format.c_str(),x,y,z);
	MGURLFetcher fetcher(url);
	string data;
	if(fetcher.fetch(data))
	{
		if (data.substr(0,15)=="<!DOCTYPE html>")
		{
			fprintf(file,"URL aborted %s \n",url);
			fflush(file);
			printf("URL aborted %s \n",url);
			return ;
		}

		fprintf(file,"Successfully Download %s \n",url);
		fflush(file);
		printf("Successfully Download %s \n",url);

		MGData *write=MGData::dataWithBytesNoCopy((void *)data.c_str(),data.size(),false);
		sprintf(url,"%s%d/%d/",root.c_str(),z,x);
		Utility::createDirOnPath(url);
		sprintf(url,output_format.c_str(),root.c_str(),z,x,y);
		write->writeToFile(url);
		delete_and_set_null(write);


	}
	else
	{
		fprintf(file,"Failed Download %s \n",url);
		fflush(file);
		printf("Failed Download %s \n",url);
	}
}
void visit(const std::string& dir)
{
	vector<string> subdirs;
	if(Utility::getSubdirsInDir(dir,subdirs))
	{
		for (size_t i=0;i<subdirs.size();i++)
		{
			visit(dir+subdirs[i]+"/");
		}
	}
	else
	{
		vector<string> files;
		if (Utility::getFilesInDir(dir,files))
		{
			for (size_t i=0;i<files.size();i++)
			{
				int x,y,z;
				std::string full_path=dir+files[i];
				size_t offset=full_path.find(input_path);
				if (offset!=std::string::npos)
				{
					offset+=input_path.size();
					if (offset<full_path.size())
					{
						sscanf(full_path.substr(offset).c_str(),"%d/%d/%d.png",&z,&x,&y);
						download(path_prefix+satellite_output_path,google_satelite_format,output_satelite_path_format,x,y,z);
						download(path_prefix+hybrid_output_path,google_hybrid_format,output_hybrid_path_format,x,y,z);
					}
				}
			}
		}
	}
}
void downloadByCoordinate(const MKLocationCoordinate2D& lefttop,const MKLocationCoordinate2D& rightbottom,int zoomMin,int zoomMax)
{
	{
		int minv=MINVALUE(zoomMin,zoomMax);
		int maxv=MAXVALUE(zoomMax,zoomMin);
		zoomMin=minv;
		zoomMax=maxv;
	}
	GlobalMercator mecator;
	for (int z=zoomMin;z<=zoomMax;z++)
	{
		MGPoint ltm=mecator.LatLonToMeters(lefttop.latitude,lefttop.longitude);
		MGIntPoint ltt=mecator.MetersToTile(ltm.x,ltm.y,z);
		ltt=mecator.GoogleTile(ltt.x,ltt.y,z);
		MGPoint rbm=mecator.LatLonToMeters(rightbottom.latitude,rightbottom.longitude);
		MGIntPoint rbt=mecator.MetersToTile(rbm.x,rbm.y,z);
		rbt=mecator.GoogleTile(rbt.x,rbt.y,z);

		int minx=MINVALUE(ltt.x,rbt.x);
		int miny=MINVALUE(ltt.y,rbt.y);
		int maxx=MAXVALUE(ltt.x,rbt.x);
		int maxy=MAXVALUE(ltt.y,rbt.y);
		for (int x=minx;x<=maxx;x++)
		{
			for (int y=miny;y<=maxy;y++)
			{
				download(path_prefix+"new/"+satellite_output_path,google_satelite_format,output_satelite_path_format,x,y,z);
				download(path_prefix+"new/"+hybrid_output_path,google_hybrid_format,output_hybrid_path_format,x,y,z);
				download(path_prefix+"new/"+standard_output_path,google_standard_format,output_standard_path_format,x,y,z);
			}
		}
	}
}
void showHelp()
{
	printf("==========================命令列表================================\n");
	printf("(h)elp 帮助\n");
	printf("(s)tart 开始\n");
	printf("(d)ownload 下载\n");
	printf("(r)epair 修复（未实现）\n");
	printf("(q)uit 退出\n");
	printf("==================================================================\n");
}
int main()
{
	if (file==NULL)
	{
		Utility::createDirOnPath("D:/Log/");
		char str[1024];
		sprintf(str,"D:/Log/%d.txt",GetTickCount());
		file=fopen(str,"w");
	}

	char cmd_str[1024];
	showHelp();
	while (gets(cmd_str))
	{
		string cmd=cmd_str;
		if (cmd=="quit" || cmd=="q")
		{
			break;
		}
		else if(cmd=="start"||cmd=="s")
		{
			visit(path_prefix+input_path);
		}
		else if(cmd[0]=='d')
		{
			MKLocationCoordinate2D lefttop;
			MKLocationCoordinate2D rightbottom;
			int minz=0;
			int maxz=0;
			if(sscanf(cmd.c_str(),"download %lf,%lf %lf,%lf %d,%d",&lefttop.longitude,&lefttop.latitude,&rightbottom.longitude,&rightbottom.latitude,&minz,&maxz)==6)
				downloadByCoordinate(lefttop,rightbottom,minz,maxz);
			//download 120.03490447998047,30.29612858227995 120.18991470336914,30.173624550358536 11,11
			
		}
		else if(cmd=="help"||cmd=="h")
		{
			showHelp();
		}
	}

	if (file!=NULL)
	{
		fclose(file);
	}
	return 0;
}