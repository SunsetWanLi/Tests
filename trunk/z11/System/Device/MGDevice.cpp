#include "MGDevice.h"
#include "../../CommonBase/CommonDef.h"

using namespace std;

namespace z11
{
	MGDevice *MGDevice::_instance = NULL;
    bool MGDevice::is_id_setted = false;
    bool MGDevice::is_os_version_setted = false;
    bool MGDevice::is_type_setted = false;
    bool MGDevice::is_ppi_setted = false;
    bool MGDevice::is_os_type_setted = false;
    Syncable MGDevice::device_lock;
    
	void z11::MGDevice::setScreenWidth( uint_32 width )
	{
        //Syncable::AutoLock lock(&device_lock);
		getInstance()->m_screen_width=width;
	}

	uint_32 z11::MGDevice::getScreenWidth()
	{
        //Syncable::AutoLock lock(&device_lock);
		return getInstance()->m_screen_width;
	}

	void z11::MGDevice::setScreenHeight( uint_32 height )
	{
        //Syncable::AutoLock lock(&device_lock);
		getInstance()->m_screen_height=height;
	}

	uint_32 z11::MGDevice::getScreenHeight()
	{
        //Syncable::AutoLock lock(&device_lock);
		return getInstance()->m_screen_height;
	}
    
    void z11::MGDevice::setStatusBarHeight(uint_32 height)
    {
        getInstance()->m_status_bar_height = height;
    }
    
    uint_32 z11::MGDevice::getStatusBarHeight()
    {
        return getInstance()->m_status_bar_height;
    }

	MGDevice * z11::MGDevice::getInstance()
	{
        //Syncable::AutoLock lock(&device_lock);
		if (_instance==NULL)
		{
			_instance=new MGDevice();

		}
		return _instance;
	}

	z11::MGDevice::MGDevice()
	{
        //Syncable::AutoLock lock(&device_lock);
		m_screen_height=1;
		m_screen_width=1;
        m_status_bar_height = 0;
	}

	void MGDevice::setScreenWidthAndHeight( uint_32 width,uint_32 height )
	{
        //Syncable::AutoLock lock(&device_lock);
		setScreenWidth(width);
		setScreenHeight(height);
	}
    
    float MGDevice::getScreenPPI()
    {
        //Syncable::AutoLock lock(&device_lock);
        return getInstance()->m_screen_ppi;
    }
    
    void MGDevice::setScreenPPI(float ppi)
    {
        //Syncable::AutoLock lock(&device_lock);
        if(!is_ppi_setted)
        {
            getInstance()->m_screen_ppi = ppi;
            is_ppi_setted = true;
        }
    }
    
    string MGDevice::getDeviceID()
    {
        //Syncable::AutoLock lock(&device_lock);
        return getInstance()->m_device_id;
    }
    
    void MGDevice::setDeviceID(const string &d_id)
    {
        //Syncable::AutoLock lock(&device_lock);
        if(!is_id_setted)
        {
            getInstance()->m_device_id = d_id;
            is_id_setted = true;
        }
    }
    string MGDevice::getDeviceType()
    {
        //Syncable::AutoLock lock(&device_lock);
        return getInstance()->m_device_type;
    }
    
    void MGDevice::setDeviceType(const string &d_type)
    {
        //Syncable::AutoLock lock(&device_lock);
        if(!is_type_setted)
        {
            getInstance()->m_device_type = d_type;
            is_type_setted = true;
        }
    }
    string MGDevice::getDeviceOSVersion()
    {
        //Syncable::AutoLock lock(&device_lock);
        return getInstance()->m_os_version;
    }
    
    void MGDevice::setDeviceOSVersion(const string &d_os_version)
    {
        //Syncable::AutoLock lock(&device_lock);
        if(!is_os_version_setted)
        {
            getInstance()->m_os_version = d_os_version;
            is_os_version_setted = true;
        }
    }
    string MGDevice::getDeviceOSType()
    {
        //Syncable::AutoLock lock(&device_lock);
        return getInstance()->m_os_type;
    }
    
    void MGDevice::setDeviceOSType(const string &d_os_type)
    {
        //Syncable::AutoLock lock(&device_lock);
        if(!is_os_type_setted)
        {
            getInstance()->m_os_type = d_os_type;
            is_os_type_setted = true;
        }
    }
}

