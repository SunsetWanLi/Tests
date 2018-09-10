#pragma once
#ifndef _MG_DEVICE_H_
#define _MG_DEVICE_H_
#include "../../CommonBase/MGObject.h"
#include "../Shared/Syncable.h"
#include <string>

using namespace std;

namespace z11
{
    typedef enum {
        MIDeviceOrientationUnknown = 1,
        MIDeviceOrientationPortrait = 1<<1,            // Device oriented vertically, home button on the bottom
        MIDeviceOrientationPortraitUpsideDown = 1<<2,  // Device oriented vertically, home button on the top
        MIDeviceOrientationLandscapeLeft = 1<<3,       // Device oriented horizontally, home button on the right
        MIDeviceOrientationLandscapeRight = 1<<4,      // Device oriented horizontally, home button on the left
        MIDeviceOrientationFaceUp = 1<<5,              // Device oriented flat, face up
        MIDeviceOrientationFaceDown = 1<<6             // Device oriented flat, face down
    } MIDeviceOrientation;
    
	class MGDevice
	{
	public:
		static void setScreenWidth(uint_32 width);
		static uint_32 getScreenWidth();
		static void setScreenHeight(uint_32 height);
		static uint_32 getScreenHeight();
		static void setScreenWidthAndHeight(uint_32 width,uint_32 height);
        static string getDeviceID();
        //设置 ID 的函数，只有在 MGDevice 类的第一次方法调用时才有效果，否则设置都不成功，确保ID不能被轻易修改
        static void setDeviceID(const string& d_id);
        static string getDeviceType();
        //设置 Type 的函数，只有在 MGDevice 类的第一次方法调用时才有效果，否则设置都不成功，确保Type不能被轻易修改
        static void setDeviceType(const string& d_type);
        static string getDeviceOSVersion();
        //设置 OS Version 的函数，只有在 MGDevice 类的第一次方法调用时才有效果，否则设置都不成功，确保OS Version不能被轻易修改
        static void setDeviceOSVersion(const string& d_os_version);
        static string getDeviceOSType();
        static void setDeviceOSType(const string& os);
        static void setScreenPPI(float ppi);
        static float getScreenPPI();
        
        static void setStatusBarHeight(uint_32 height);
        static uint_32 getStatusBarHeight();
	private:
		static MGDevice *getInstance();
		uint_32 m_screen_width;
		uint_32 m_screen_height;
        uint_32 m_status_bar_height;
        static bool is_ppi_setted;
        float m_screen_ppi;     //Pixel per inch
        static bool is_id_setted;
        string m_device_id;
        static bool is_os_type_setted;
        string m_os_type;
        static bool is_os_version_setted;
        string m_os_version;
        static bool is_type_setted;
        string m_device_type;
		MGDevice();
		static MGDevice *_instance;
        static Syncable device_lock;
	};

}
#endif
