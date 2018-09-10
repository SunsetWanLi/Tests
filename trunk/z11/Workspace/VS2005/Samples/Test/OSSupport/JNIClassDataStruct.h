#ifndef _JNI_CLASS_H_
#define _JNI_CLASS_H_
#include <string>
#include <queue>
class JNIClass
{
	public :
	int code;
	virtual void call()=0;
	JNIClass ()
	{
		code=0;
	}
};

extern "C"
{
	void CallReloadMapMethod(double x,double y,double scale);
	void CallConnectVideoServer(const char *puid);
	void CallSetTrafficInfoAndStart(std::string plate, std::string sTime, std::string eTime);
	void CallSetWeatherInfoAndStart(std::string city_name, std::string data_time);
}
#ifndef ANDROID_NDK
 void CallReloadMapMethod(double x,double y,double scale)
{

}

 void CallConnectVideoServer(const char *puid)
{

}

 void CallSetTrafficInfoAndStart(string plate, string sTime, string eTime)
{

}

 void CallSetWeatherInfoAndStart(string city_name, string data_time)
{

}
#endif
class ReloadMapStruct:public JNIClass
{
	public :
	ReloadMapStruct(double _x,double _y,double _scale)
	{
		x=_x;
		y=_y;
		scale=_scale;
		code=1;
	}
	virtual void call()
	{
		CallReloadMapMethod(x,y,scale);
	}
	private:
		double x,y,scale;
};
class ConnectVideoServerStruct:public JNIClass
{
public:
	ConnectVideoServerStruct(const std::string &code)
	{
		puid=code;
	}
	virtual void call()
	{
		CallConnectVideoServer(puid.c_str());
	}
private:
	std::string puid;
};
class SetTrafficInfoAndStartStruct:public JNIClass
{
public:
	SetTrafficInfoAndStartStruct(const std::string &plate, const std::string &sTime, const std::string &eTime)
	{
		m_plate = plate;
		m_sTime = sTime;
		m_eTime = eTime;
	}
	virtual void call()
	{
		CallSetTrafficInfoAndStart(m_plate, m_sTime, m_eTime);
	}
private:
	std::string m_plate;
	std::string m_sTime;
	std::string m_eTime;
};
class SetWeatherInfoAndStartStruct:public JNIClass
{
public:
	SetWeatherInfoAndStartStruct(const std::string &city_name, const std::string &data_time)
	{
		m_city_name = city_name;
		m_data_time = data_time;
	}
	virtual void call()
	{
		CallSetWeatherInfoAndStart(m_city_name, m_data_time);
	}
private:
	std::string m_city_name;
	std::string m_data_time;
};
class JNIClassContainer
{
	public :
	static JNIClassContainer *getInstance()
	{
		if(_instance==NULL)
		{
			_instance=new JNIClassContainer();
		}
		return _instance;
	}
	void PushClass(JNIClass *classobject)
	{
		m_subclass.push(classobject);
	}
	void PopClass()
	{
		if(!m_subclass.empty())
		{
			m_subclass.front()->call();
			m_subclass.pop();
		}
		
	}
private:
	std::queue<JNIClass *> m_subclass;
	static JNIClassContainer *_instance;
};
//#ifndef ANDROID_NDK
//JNIClassContainer *JNIClassContainer::_instance=NULL;
//#endif
#endif