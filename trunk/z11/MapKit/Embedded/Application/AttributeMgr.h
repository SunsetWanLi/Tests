#ifndef _EMBEDDED_ATTRIBUTE_MGR_H
#define _EMBEDDED_ATTRIBUTE_MGR_H
#include "../Tools/SpatialDataBase.h"
#include "../BasicType/Attribute.h"
namespace Embedded
{
	class AttributeMgr
	{
	public:
		static AttributeMgr* getInstance() ;
	public:		
		void loadAttributes(const z11::MGWString& dir);
		void getLineAttribute(uint_32 address, Attribute** attr);
	private:
		AttributeMgr(){ m_lineSource = 0;}
	private:
		IStorageManager *m_lineSource;
		z11::MGWString m_dir;
		static AttributeMgr* _instance;
	};
}
#endif