#include "../../../CommonBase/MGData.h"
#include "../../../GraphicCore/MGImage.h"
#include "../../../System/Shared/MGLog.h"
#include "JniHelper.h"
#include <jni.h>
#include <stdio.h>
#include "../../../System/Media/Capture/MGCameraCaptureManager.h"
using namespace z11;
extern bool decode_image(MGImage* image,std::string &str);
extern "C"
{
	static vector<int> g_v_table;
	static vector<int> g_u_table;
	static vector<int> y_table;
	static vector< vector<unsigned char> >r_yv_table;
	static vector< vector<unsigned char> >b_yu_table;
    static MGImage * capture_image = NULL;
	int inited = 0;
	void releaseTable()
	{
		inited=0;
		g_v_table.clear();
		g_u_table.clear();
		y_table.clear();
		r_yv_table.clear();
		b_yu_table.clear();
	}
	void initTable()
	{
		if (inited == 0)
		{
			inited = 1;
			
			g_v_table.resize(256);
			g_u_table.resize(256);
			y_table.resize(256);
			r_yv_table.resize(256);
			b_yu_table.resize(256);
			for(size_t i=0;i<256;i++)
			{
				r_yv_table[i].resize(256);
				b_yu_table[i].resize(256);
			}
			
			for (int mm=0; mm < 256; mm++)
			{
				unsigned char m=mm;
				g_v_table[m] = 833 * (m - 128);
				g_u_table[m] = 400 * (m - 128);
				y_table[m] = 1192 * (m - 16);
			}
			int temp = 0;
			for (int mm = 0; mm < 256; mm++)
				for (int nn = 0; nn < 256; nn++)
				{
					unsigned char m=mm;
					unsigned char n=nn;
					temp = y_table[m] + 1634 * (n - 128);
					if (temp < 0) temp = 0; else if (temp > 262143) temp = 262143;
					r_yv_table[m][n] = (((temp << 6) & 0xff0000)>>16);
	 
					temp = y_table[m] + 2066 * (n - 128);
					if (temp < 0) temp = 0; else if (temp > 262143) temp = 262143;
					b_yu_table[m][n] = ((temp >> 10) & 0xff);
					
				}
		}
	}
	
	
	
	static bool cameraOpened=false;
	
	void yuv2rgb(unsigned char* rgb,unsigned char* yuv420sp,int& width,int& height)
	{
		int frameSize = width * height;
		unsigned char *p=rgb;
		initTable();
			 
		for (int j = 0, yp = 0; j < height; j++) 
		{
			int uvp = frameSize + (j >> 1) * width;
			unsigned char u = 0;
			unsigned char v = 0;
					
					
			for (int i = 0; i < width; i++, yp++) 
			{
				unsigned char  y = yuv420sp[yp];
				if (y < 16) y = 16;
				if ((i & 1) == 0) {
					v = yuv420sp[uvp++];
					u =yuv420sp[uvp++] ;
				}

		 
				int y1192 = y_table[y];
				int r =r_yv_table[y][v];
				int g = (y_table[y] - g_v_table[v] - g_u_table[u]);
				int b = b_yu_table[y][u];
		 
				if (g < 0) g = 0; else if (g > 262143) g = 262143;
				*p++=r;
				*p++=( ((g >> 2) & 0xff00)>>8);
				*p++=b;
					
				//    rgba[yp] = 0xff000000 | (r) | ((g >> 2) & 0xff00) | (b<<16);
			}
		}
	}
	void yuv2rgbAndRotate(unsigned char* rgb,unsigned char* yuv420sp,int& width,int& height)
	{
		initTable();
		int frameSize = width * height;
		unsigned char *p=rgb;
			 
		for (int j = 0, yp = 0; j < height; j++) 
		{
			int uvp = frameSize + (j >> 1) * width;
			unsigned char u = 0;
			unsigned char v = 0;
					
			p=rgb+(height-j)*3;
			int rowBytes=height*3;
					
			for (int i = 0; i < width; i++, yp++) 
			{
				unsigned char  y = yuv420sp[yp];
				if (y < 16) y = 16;
				if ((i & 1) == 0) {
					v = yuv420sp[uvp++];
					u =yuv420sp[uvp++] ;
				}

		 
				int y1192 = y_table[y];
				int r =r_yv_table[y][v];
				int g = (y_table[y] - g_v_table[v] - g_u_table[u]);
				int b = b_yu_table[y][u];
		 
				if (g < 0) g = 0; else if (g > 262143) g = 262143;
				p[0]=r;
				p[1]=( ((g >> 2) & 0xff00)>>8);
				p[2]=b;
					
				p+=rowBytes;
				//    rgba[yp] = 0xff000000 | (r) | ((g >> 2) & 0xff00) | (b<<16);
			}
		}
		
		int temp=width;
		width=height;
		height=temp;
	}
//void Java_com_z11_mobile_framework_CameraPreview_nativeOnPreviewFrame(JNIEnv* env, jobject thiz, jbyteArray bytes, jint length)
	void cameraNativeOnPreviewFrame(JNIEnv* env, jobject thiz, jbyteArray bytes, jint length,jint width,jint height,jint rotate)
	{
		jboolean isCopy = 0;
		MGData* data = NULL;
		unsigned char* _data = (unsigned char*)env->GetByteArrayElements(bytes, &isCopy);
		if (_data != NULL)
		{
			//MGLogD("=========MGData::dataWithBytes=============");
			data = MGData::dataWithBytesNoCopy(_data, length, false);
			//env->ReleaseByteArrayElements(bytes, (jbyte*)_data, 0);
		}
		
		if(data!=NULL)
		{
				
			
			unsigned char rgb[width*height*3]; // –¬ÕºœÒœÒÀÿ÷µ
			switch(rotate)
			{
			case 90:
				yuv2rgbAndRotate(rgb,_data,width,height);
				break;
			case 0:
				yuv2rgb(rgb,_data,width,height);
				break;
			default:
				memset(rgb,0,sizeof(rgb));
				MGLogD("unsupported format rotate=%d",rotate);
				break;
			}
			
            if (capture_image == NULL)
                capture_image = MGImage::fromMem(24, width, height, (unsigned char*)rgb);
            else {
                capture_image->resetMem((unsigned char*)rgb, 24, width, height);
            }
            if(capture_image!=NULL)
            {
                // //MGLogD("cameraNativeOnPreviewFrame image decompress successed %dX%d bitcount=%d",image->getWidth(),image->getHeight(),(int)image->content_info.bit_count);
                MGCameraCaptureManager::didCameraCaptureOutput(capture_image);
            }
            else
            {
                MGLogD("cameraNativeOnPreviewFrame image decompress failed");
            }
		}

		if (_data != NULL)
		{
			//MGLogD("=========ReleaseByteArrayElements=============");
			env->ReleaseByteArrayElements(bytes, (jbyte*)_data, 0);
		}
		delete_and_set_null(data);
	}
	
	void cameraOpenCamera(int position)
	{
		if(cameraOpened)
			return ;
		cameraOpened=true;
		initTable();
		
		JniMethodInfo t;

		if (JniHelper::getStaticMethodInfo(t,
			"com/z11/mobile/framework/CameraPreview",
			"openCamera",
			"(I)V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID, position);
		}
	}
	
	void cameraReleaseCamera()
	{
		if(!cameraOpened)
			return ;
		cameraOpened=false;
		releaseTable();
		
		JniMethodInfo t;

		if (JniHelper::getStaticMethodInfo(t,
			"com/z11/mobile/framework/CameraPreview",
			"releaseCamera",
			"()V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
		}
        
        if (capture_image != NULL)
        {
            delete capture_image;
            capture_image = NULL;
        }
	}
}