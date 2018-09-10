//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGApplication_linux.h)
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
#ifndef _MGAPPLICATION_LINUX_H_
#define _MGAPPLICATION_LINUX_H_

#include "../../CommonBase/MGStructure.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
namespace z11
{
	class  MGApplication:public GuiContainer
	{
	public:
		MGApplication();
		virtual ~MGApplication();

		/**
		@brief	Implement for initialize OpenGL instance, set source path, etc...
		*/
		virtual bool initInstance() ;

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

		/**
		@brief Get current language config
		@return Current language config
		*/
		//static ccLanguageType getCurrentLanguage();

	protected:

		static MGApplication * sm_pSharedApplication;

	public:
		//void initialise(WndHandle);
		virtual void preserveEvent(z11::MIEvent *event);
	public:
		GUICONTAINER_DECLARE;
	};

}

#endif //_MGAPPLICATION_LINUX_H_
