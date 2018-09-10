#include "AttributeMgr.h"
namespace Embedded
{
	AttributeMgr* AttributeMgr::_instance = NULL;
	AttributeMgr* AttributeMgr::getInstance()
	{
		if(!_instance)
			_instance = new AttributeMgr();
		return _instance;
	}
	void AttributeMgr::loadAttributes(const z11::MGWString &dir)
	{
		if(m_dir.size() == 0)
		{
			m_dir = dir;
			if(m_lineSource) 
				delete m_lineSource;
			z11::MGWString attrPath = dir;
			attrPath+= (L'/');
			attrPath+= WS(L"AllLineAttributes");
			IStorageManager* attrFile = StorageManager::loadStaticDiskStorageManager(attrPath);
			m_lineSource = StorageManager::createNewFIFOBuffer(*attrFile, 100, false);
		}
		else
			assert(m_dir == dir);
	}

	void AttributeMgr::getLineAttribute(uint_32 address, Attribute** attr)
	{
		assert(*attr == 0);
		*attr = new Attribute();
		if(m_lineSource)
		{
			id_type page = address >> PUBLISH_PAGE_SIZE_SHIFT; // 65536 //
			uint_32 leng, offset = address & PUBLISH_PAGE_SIZE_MASK;
			uint_8* data;
			m_lineSource->loadByteArray(page, leng, &data);
			(*attr)->loadFromBuf(data+offset);
			delete[] data;
		}
	}
}