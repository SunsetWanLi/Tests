//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGApplication_win32.h)  
/// @brief (Application 的win32实现)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-08-29)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MGAPPLICATION_WIN32_H_
#define _MGAPPLICATION_WIN32_H_

#include <winsock2.h>
#include <windows.h>
#include "../../CommonBase/MGStructure.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
#include "../../System/Timer/MGTimer.h"
#include "../../System/Media/Capture/MGCameraCaptureManager.h"
namespace z11
{
	typedef enum {
		WINDOWS_NORMAL = 0,
		WINDOWS_MAX = 1,
		WINDOWS_FULL_SCREEN = 1 << 1,		// Full Screen //
		WINDOWS_FORCE_FULL_SCREEN = 1 << 2,		// Full Screen Without Task Bar //
		WINDOWS_POP_UP = 1<<3,		// Without Title And Bounds //
		WINDOWS_CLIENT_EDGE = 1 << 4
	} WindowsType;

	//必须确保MGTimerWin32的所有函数在同一个线程中执行，(目前都是主线程) //
	class MGTimerWin32
	{
	public:
		static MGTimerWin32 *getInstance()
		{
			static MGTimerWin32 timer;
			return &timer;
		}
		std::map<MGTimerTag, MGTimeTicks> timer_map;
		typedef std::map<MGTimerTag, MGTimeTicks>::iterator Iter;

	public:
		void handle()
		{
			if(timer_map.empty())
			{
				return;
			}
			MGTimeTicks now = MGTimeTicks::Now();
			size_t i = 0;
			const size_t map_size = 3; // 防止其他事件饥饿 //

			std::map<MGTimerTag, MGTimeTicks> tmp_timer_map = timer_map;
			for(Iter it = tmp_timer_map.begin(); it != tmp_timer_map.end() && i < map_size; ++it)
			{
				if(now > it->second)
				{
					MGTimerTag timer_tag = it->first;
					MGTimeTicks timer_start = it->second;
					Iter timer_it = timer_map.find(timer_tag);
					if(timer_it != timer_map.end())
					{
						timer_map.erase(timer_it);
					}
					if(MGTimer::fireTimerWithTag(timer_tag))
					{
						MGTimer::timer_locker->lock();
						MGTimerMapIt it2 = MGTimer::timer_map.find(timer_tag);
						if (it2 != MGTimer::timer_map.end())
						{
							timer_map[timer_tag] = MGTimeTicks::Now()+it2->second->getTimerInterval();
						}
						MGTimer::timer_locker->unlock();
					}

					++i;
					now = MGTimeTicks::Now();
				}
			}
		}

		void add(MGTimerObj *timer)
		{
			timer_map[timer->getTag()] = MGTimeTicks::Now()+timer->getInterval();
		}

		void remove(MGTimerObj *timer)
		{
			Iter it = timer_map.find(timer->getTag());
			if(it != timer_map.end())
			{
				timer_map.erase(it);
			}
		}
	};

	class  MGApplication:public GuiContainer
	{
	public:
		MGApplication();
		virtual ~MGApplication();

		/**
		@brief	Implement for initialize OpenGL instance, set source path, etc...
		*/
		virtual bool initInstance();

		/**
		@brief	Implement CCDirector and CCScene init code here.
		@return true    Initialize success, app continue.
		@return false   Initialize failed, app terminate.
		*/
		virtual bool applicationDidFinishLaunching();

		/**
		@brief	Callback by CCDirector for limit FPS.
		@interval       The time, which expressed in second in second, between current frame and next. 
		*/
		void setAnimationInterval(double interval);

		typedef enum
		{
			/// Device oriented vertically, home button on the bottom
			kOrientationPortrait = 0,
			/// Device oriented vertically, home button on the top
			kOrientationPortraitUpsideDown = 1,
			/// Device oriented horizontally, home button on the right
			kOrientationLandscapeLeft = 2,
			/// Device oriented horizontally, home button on the left
			kOrientationLandscapeRight = 3,
		} Orientation;

		/**
		@brief	Callback by CCDirector for change device orientation.
		@orientation    The defination of orientation which CCDirector want change to.
		@return         The actual orientation of the application.
		*/
		Orientation setOrientation(Orientation orientation);

		/**
		@brief	Get status bar rectangle in EGLView window.
		*/
		void    statusBarFrame(MGRect * rect);

		/**
		@brief	Run the message loop.
		*/
		int run();

		/**
		@brief	Get current applicaiton instance.
		@return Current application instance pointer.
		*/
		static MGApplication& sharedApplication();
        
        virtual void handleForeignerSDKApiCall(const string& cmd);

		virtual bool preHandleMessage(MSG& msg);

		virtual void startCameraCapture(MGCaptureDevicePosition pos = MGCaptureDevicePositionUnspecified);
		virtual void stopCameraCapture();

	protected:
		HINSTANCE				m_hInstance;
		LARGE_INTEGER       m_nAnimationInterval;
		HWND						m_hCapWnd;
		static MGApplication *sm_pSharedApplication;

	public:
		//void initialise(WndHandle);
		uint_8 wnd_type;
		bool isSimulateMultiTouch;
		LPCTSTR title;
		virtual void preserveEvent(z11::MIEvent *event);
	public:
		GUICONTAINER_DECLARE;
	};

}





#endif //_MGAPPLICATION_WIN32_H_
