#include "DiskStorageManager.h"
#include "../../../System/Shared/Utility.h"
#include "../../../System/IO/MGFileManager.h"
#include "../../../CommonBase/MGData.h"
#include "../../../System/Shared/MGLog.h"
#include <string.h>
using namespace z11;
using namespace Embedded;
using namespace Embedded::StorageManager;


IStorageManager* Embedded::StorageManager::returnDiskStorageManager(PropertySet& ps)
{
	IStorageManager* sm = new DiskStorageManager(ps);
	return sm;
}

IStorageManager* Embedded::StorageManager::createNewDiskStorageManager(const z11::MGWString& baseName, uint_32 pageSize)
{
	Variant var;
	PropertySet ps;

	var.m_varType = VT_BOOL;
	var.m_val.blVal = true;
	ps.setProperty("Overwrite", var);
	// overwrite the file if it exists.

	var.m_varType = VT_PWCHAR;
	var.m_val.pwcVal = const_cast<wchar_t*>(reinterpret_cast<const wchar_t*>(baseName.c_str()));
	ps.setProperty("FileName", var);
	// .idx and .dat extensions will be added.

	var.m_varType = VT_ULONG;
	var.m_val.ulVal = pageSize;
	ps.setProperty("PageSize", var);
	// specify the page size. Since the index may also contain user defined data
	// there is no way to know how big a single node may become. The storage manager
	// will use multiple pages per node if needed. Off course this will slow down performance.

	return returnDiskStorageManager(ps);
}

IStorageManager* StorageManager::loadDiskStorageManager(const z11::MGWString& baseName)
{
	Variant var;
	PropertySet ps;

	var.m_varType = VT_PWCHAR;
	var.m_val.pwcVal = const_cast<wchar_t*>(reinterpret_cast<const wchar_t*>(baseName.c_str()));
	ps.setProperty("FileName", var);
	// .idx and .dat extensions will be added.

	return returnDiskStorageManager(ps);
}

DiskStorageManager::DiskStorageManager(PropertySet& ps)
	: m_pageSize(0), m_nextPage(-1), m_buffer(NULL)
{
	Variant var;

	// Open/Create flag.
	bool bOverwrite = false;
	var = ps.getProperty("Overwrite");

	if (var.m_varType != VT_EMPTY)
	{
		if (var.m_varType != VT_BOOL)
			throw IllegalArgumentException("SpatialDataBase::DiskStorageManager: Property Overwrite must be VT_BOOL");
		bOverwrite = var.m_val.blVal;
	}

	// storage filename.
	var = ps.getProperty("FileName");

	if (var.m_varType != VT_EMPTY)
	{
		if (var.m_varType != VT_PWCHAR)
			throw IllegalArgumentException("SpatialDataBase::DiskStorageManager: Property FileName must be VT_PWCHAR");

		uint_32 cLen = strlen(var.m_val.pcVal);

		//std::string sIndexFile = std::string(var.m_val.pcVal) + ".idx";
		//std::string sDataFile = std::string(var.m_val.pcVal) + ".dat";
		z11::MGWString sIndexFile =  z11::MGWString(reinterpret_cast<z11::widechar*>(var.m_val.pwcVal)) + WS(L".idx");
		z11::MGWString sDataFile =  z11::MGWString(reinterpret_cast<z11::widechar*>(var.m_val.pwcVal)) + WS(L".dat");

		// check if file exists.
		z11::MGFileManager *file_manager=z11::MGFileManager::defaultManager();
		bool bFileExists=file_manager->fileExistsAtPath(sIndexFile) &&file_manager->fileExistsAtPath(sDataFile);
		//bFileExists=true;

		// check if file can be read/written.
		if (bFileExists == true && bOverwrite == false)
		{
			//MGLogD("DiskStorageManager %s,%s exists",Utility::wstr2astr(sIndexFile).c_str(),Utility::wstr2astr(sDataFile).c_str());
			MGData *data=NULL;
			data=MGData::dataWithContentsOfFile(Utility::wstr2astr(sIndexFile));
			if (data==NULL)
			{
				throw IllegalArgumentException("SpatialDataBase::DiskStorageManager: Index file cannot be read.");
			}
			m_indexFile.rdbuf()->sputn ((char *)data->bytes(),data->length());
			delete_and_set_null(data);


			data=MGData::dataWithContentsOfFile(Utility::wstr2astr(sDataFile));
			if (data==NULL)
			{
				throw IllegalArgumentException("SpatialDataBase::DiskStorageManager: Data file cannot be read.");
			}
			m_dataFile.rdbuf()->sputn ((char *)data->bytes(),data->length());
			delete_and_set_null(data);
			
			//m_indexFile.open(FILEPATH(sIndexFile), std::ios::in | std::ios::binary);
			//m_dataFile.open(FILEPATH(sDataFile), std::ios::in  | std::ios::binary);

			//if (m_indexFile.fail() || m_dataFile.fail())
			//	throw IllegalArgumentException("SpatialDataBase::DiskStorageManager: Index/Data file cannot be read/writen.");
		}
		else
		{
			MGLogD("DiskStorageManager %s,%s not exists",Utility::wstr2astr(sIndexFile).c_str(),Utility::wstr2astr(sDataFile).c_str());
			assert(0);
			//m_indexFile.open(FILEPATH(sIndexFile), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
			//m_dataFile.open(FILEPATH(sDataFile), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

			//if (m_indexFile.fail() || m_dataFile.fail())
			//	throw IllegalArgumentException("SpatialDataBase::DiskStorageManager: Index/Data file cannot be created.");

		}
	}
	else
	{
		throw IllegalArgumentException("SpatialDataBase::DiskStorageManager: Property FileName was not specified.");
	}

	// find page size.
	// todo : if file not exist ?
	if (bOverwrite == true)
	{
		var = ps.getProperty("PageSize");

		if (var.m_varType != VT_EMPTY)
		{
			if (var.m_varType != VT_ULONG)
				throw IllegalArgumentException("SpatialDataBase::DiskStorageManager: Property PageSize must be VT_ULONG");
			m_pageSize = var.m_val.ulVal;
			m_nextPage = 0;
		}
		else
		{
			throw IllegalArgumentException("SpatialDataBase::DiskStorageManager: A new storage manager is created and property PageSize was not specified.");
		}
	}
	else
	{
        uint_32 size = sizeof(uint_32);
        MGLogD("sizeof(uint_32) = %d.", size);
		m_indexFile.read(reinterpret_cast<char*>(&m_pageSize), sizeof(uint_32));
		if (m_indexFile.fail())
			throw IllegalStateException("SpatialDataBase::DiskStorageManager: Failed reading pageSize.");

		m_indexFile.read(reinterpret_cast<char*>(&m_nextPage), sizeof(id_type));
		if (m_indexFile.fail())
			throw IllegalStateException("SpatialDataBase::DiskStorageManager: Failed reading nextPage.");
	}

	// create buffer.
	m_buffer = new uint_8[m_pageSize];
	memset(m_buffer,0,m_pageSize);
	//bzero(m_buffer, m_pageSize);

	if (bOverwrite == false)
	{
		uint_32 count;
		id_type id, page;

		// load empty pages in memory.
		m_indexFile.read(reinterpret_cast<char*>(&count), sizeof(uint_32));
		if (m_indexFile.fail())
			throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

		for (uint_32 cCount = 0; cCount < count; ++cCount)
		{
			m_indexFile.read(reinterpret_cast<char*>(&page), sizeof(id_type));
			if (m_indexFile.fail())
				throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");
			m_emptyPages.push(page);
		}

		// load index table in memory.
		m_indexFile.read(reinterpret_cast<char*>(&count), sizeof(uint_32));
		if (m_indexFile.fail())
			throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

		for (uint_32 cCount = 0; cCount < count; ++cCount)
		{
			Entry* e = new Entry();

			m_indexFile.read(reinterpret_cast<char*>(&id), sizeof(id_type));
			if (m_indexFile.fail())
				throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

			m_indexFile.read(reinterpret_cast<char*>(&(e->m_length)), sizeof(uint_32));
			if (m_indexFile.fail())
				throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

			uint_32 count2;
			m_indexFile.read(reinterpret_cast<char*>(&count2), sizeof(uint_32));
			if (m_indexFile.fail())
				throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

			for (uint_32 cCount2 = 0; cCount2 < count2; ++cCount2)
			{
				m_indexFile.read(reinterpret_cast<char*>(&page), sizeof(id_type));
				if (m_indexFile.fail())
					throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");
				e->m_pages.push_back(page);
			}
			m_pageIndex.insert(std::pair<id_type, Entry* >(id, e));
		}
	}
}

DiskStorageManager::~DiskStorageManager()
{
	flush();
//	m_indexFile.close();
//	m_dataFile.close();
	if (m_buffer != 0)
		delete[] m_buffer;

	std::map<id_type, Entry*>::iterator it;
	for(it = m_pageIndex.begin(); it != m_pageIndex.end(); ++it)
		delete (*it).second;
}

void DiskStorageManager::flush()
{
	/*m_indexFile.seekp(0, std::ios_base::beg);
	if (m_indexFile.fail())
		throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

	m_indexFile.write(reinterpret_cast<const char*>(&m_pageSize), sizeof(uint_32));
	if (m_indexFile.fail())
		throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

	m_indexFile.write(reinterpret_cast<const char*>(&m_nextPage), sizeof(id_type));
	if (m_indexFile.fail())
		throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

	uint_32 count = m_emptyPages.size();
	id_type id, page;

	m_indexFile.write(reinterpret_cast<const char*>(&count), sizeof(uint_32));
	if (m_indexFile.fail())
		throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

	while (!m_emptyPages.empty())
	{
		page = m_emptyPages.top(); m_emptyPages.pop();
		m_indexFile.write(reinterpret_cast<const char*>(&page), sizeof(id_type));
		if (m_indexFile.fail())
			throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");
	}

	count = m_pageIndex.size();

	m_indexFile.write(reinterpret_cast<const char*>(&count), sizeof(uint_32));
	if (m_indexFile.fail())
		throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

	std::map<id_type, Entry*>::iterator it;

	for (it = m_pageIndex.begin(); it != m_pageIndex.end(); ++it)
	{
		id = (*it).first;
		m_indexFile.write(reinterpret_cast<const char*>(&id), sizeof(id_type));
		if (m_indexFile.fail())
			throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

		uint_32 length = (*it).second->m_length;
		m_indexFile.write(reinterpret_cast<const char*>(&length), sizeof(uint_32));
		if (m_indexFile.fail())
			throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

		count = (*it).second->m_pages.size();
		m_indexFile.write(reinterpret_cast<const char*>(&count), sizeof(uint_32));
		if (m_indexFile.fail())
			throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");

		for (uint_32 cIndex = 0; cIndex < count; ++cIndex)
		{
			page = (*it).second->m_pages[cIndex];
			m_indexFile.write(reinterpret_cast<const char*>(&page), sizeof(id_type));
			if (m_indexFile.fail())
				throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted storage manager index file.");
		}
	}

	m_indexFile.flush();
	m_dataFile.flush();*/
}

void DiskStorageManager::loadByteArray(const id_type id, uint_32& len, uint_8** data)
{
	std::map<id_type, Entry*>::iterator it = m_pageIndex.find(id);

	if (it == m_pageIndex.end())
		throw InvalidPageException(id);

	std::vector<id_type>& pages = (*it).second->m_pages;
	uint_32 cNext = 0;
	uint_32 cTotal = pages.size();

	len = (*it).second->m_length;
	*data = new uint_8[len];

	uint_8* ptr = *data;
	uint_32 cLen;
	uint_32 cRem = len;

	do
	{
		m_dataFile.seekg(pages[cNext] * m_pageSize, std::ios_base::beg);
		if (m_dataFile.fail())
			throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted data file.");

		m_dataFile.read(reinterpret_cast<char*>(m_buffer), m_pageSize);
		if (m_dataFile.fail())
			throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted data file.");

		cLen = (cRem > m_pageSize) ? m_pageSize : cRem;
		memcpy(ptr, m_buffer, cLen);

		ptr += cLen;
		cRem -= cLen;
		++cNext;
	} while (cNext < cTotal);
}

void DiskStorageManager::storeByteArray(id_type& id, const uint_32 len, const uint_8* const data)
{
	assert(0);
	if (id == NewPage)
	{
		Entry* e = new Entry();
		e->m_length = len;

		const uint_8* ptr = data;
		id_type cPage;
		uint_32 cRem = len;
		uint_32 cLen;

		while (cRem > 0)
		{
			if (!m_emptyPages.empty())
			{
				cPage = m_emptyPages.top();
				m_emptyPages.pop();
			}
			else
			{
				cPage = m_nextPage;
				++m_nextPage;
			}

			cLen = (cRem > m_pageSize) ? m_pageSize : cRem;
			memcpy(m_buffer, ptr, cLen);

			m_dataFile.seekp(cPage * m_pageSize, std::ios_base::beg);
			if (m_dataFile.fail())
				throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted data file.");

			m_dataFile.write(reinterpret_cast<const char*>(m_buffer), m_pageSize);
			if (m_dataFile.fail())
				throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted data file.");

			ptr += cLen;
			cRem -= cLen;
			e->m_pages.push_back(cPage);
		}

		id = e->m_pages[0];
		m_pageIndex.insert(std::pair<id_type, Entry*>(id, e));
	}
	else
	{
		// find the entry.
		std::map<id_type, Entry*>::iterator it = m_pageIndex.find(id);

		// check if it exists.
		if (it == m_pageIndex.end())
			throw IndexOutOfBoundsException(id);

		Entry* oldEntry = (*it).second;

		m_pageIndex.erase(it);

		Entry* e = new Entry();
		e->m_length = len;

		const uint_8* ptr = data;
		id_type cPage;
		uint_32 cRem = len;
		uint_32 cLen, cNext = 0;

		while (cRem > 0)
		{
			if (cNext < oldEntry->m_pages.size())
			{
				cPage = oldEntry->m_pages[cNext];
				cNext++;
			}
			else if (!m_emptyPages.empty())
			{
				cPage = m_emptyPages.top();
				m_emptyPages.pop();
			}
			else
			{
				cPage = m_nextPage;
				m_nextPage++;
			}

			cLen = (cRem > m_pageSize) ? m_pageSize : cRem;
			memcpy(m_buffer, ptr, cLen);

			m_dataFile.seekp(cPage * m_pageSize, std::ios_base::beg);
			if (m_dataFile.fail())
				throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted data file.");

			m_dataFile.write(reinterpret_cast<const char*>(m_buffer), m_pageSize);
			if (m_dataFile.fail())
				throw IllegalStateException("SpatialDataBase::DiskStorageManager: Corrupted data file.");

			ptr += cLen;
			cRem -= cLen;
			e->m_pages.push_back(cPage);
		}

		while (cNext < oldEntry->m_pages.size())
		{
			m_emptyPages.push(oldEntry->m_pages[cNext]);
			++cNext;
		}

		m_pageIndex.insert(std::pair<id_type, Entry*>(id, e));
		delete oldEntry;
	}
}

void DiskStorageManager::deleteByteArray(const id_type id)
{
	std::map<id_type, Entry*>::iterator it = m_pageIndex.find(id);

	if (it == m_pageIndex.end())
		throw InvalidPageException(id);

	for (uint_32 cIndex = 0; cIndex < (*it).second->m_pages.size(); ++cIndex)
		m_emptyPages.push((*it).second->m_pages[cIndex]);

	delete (*it).second;
	m_pageIndex.erase(it);
}

//void DiskStorageManager::getNodePageInfo(id_type id, uint_32& length, uint_32& pageSize) const
//{
//	std::map<id_type, Entry*>::const_iterator it = m_pageIndex.find(id);
//
//	if (it == m_pageIndex.end())
//		throw InvalidPageException(id);
//
//	length = it->second->m_length;
//	pageSize = it->second->m_pages.size();
//}
