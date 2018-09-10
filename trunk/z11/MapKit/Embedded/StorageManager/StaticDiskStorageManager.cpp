#include "StaticDiskStorageManager.h"
#include "../../../System/Shared/Utility.h"
#include "../../../System/Shared/MGLog.h"
#include "../../../System/IO/MGFileManager.h"
#include "string.h"
#include "../../../CommonBase/MGData.h"
//#include <clocale>

using namespace Embedded;
using namespace Embedded::StorageManager;

IStorageManager* StorageManager::returnStaticDiskStorageManager(PropertySet& ps)
{
	IStorageManager* sm = new StaticDiskStorageManager(ps);
	return sm;
}

IStorageManager* StorageManager::createNewStaticDiskStorageManager(const z11::MGWString& baseName, uint_32 pageSize)
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

	return returnStaticDiskStorageManager(ps);
}

IStorageManager* StorageManager::loadStaticDiskStorageManager(const z11::MGWString& baseName)
{
	Variant var;
	PropertySet ps;

	var.m_varType = VT_PWCHAR;
	var.m_val.pwcVal = const_cast<wchar_t*>(reinterpret_cast<const wchar_t*>(baseName.c_str()));
	ps.setProperty("FileName", var);
	// .idx and .dat extensions will be added.

	return returnStaticDiskStorageManager(ps);
}

StaticDiskStorageManager::StaticDiskStorageManager(PropertySet& ps)
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
			throw IllegalArgumentException(
			"StorageManager: Property FilePath must be VT_PWCHAR"
			);

		//z11::MGWString sIndexFile = z11::MGWString(reinterpret_cast<z11::widechar*>(var.m_val.pwcVal)) + WS(L".idx");
		z11::MGWString sDataFile = z11::MGWString(reinterpret_cast<z11::widechar*>(var.m_val.pwcVal)) + WS(L".dat");

		//locale loc = std::locale::global(locale(""));  //by zj
		//setlocale(LC_ALL,"Chinese-simplified");
		// check if file exists.
		bool bFileExists = z11::MGFileManager::defaultManager()->fileExistsAtPath(sDataFile);
		string datafile = z11::Utility::wstr2astr(sDataFile);
	    z11::MGLogD("StaticDiskStorageManager File Name:%s",datafile.c_str());


		//std::locale::global(loc);//恢复全局locale


		// check if file can be read/written.
		if (bFileExists == true && bOverwrite == false)
		{
			//m_indexFile.open(z11::Utility::unicode_to_utf8(buf, reinterpret_cast<const wchar_t*>(sIndexFile.c_str()) ), std::ios::in | std::ios::out | std::ios::binary);
			z11::MGData *data=z11::MGData::dataWithContentsOfFile(z11::Utility::wstr2astr(sDataFile));
			if (data!=NULL)
			{
				m_dataFile.rdbuf()->sputn((char *)data->bytes(),data->length());
				delete_and_set_null(data);
			} else {
                throw IllegalArgumentException("StorageManager: Index/Data file are empty.");
            }
			//m_dataFile.open(FILEPATH(sDataFile), std::ios::in | std::ios::binary);
//
//			if (/*m_indexFile.fail() || */m_dataFile.fail())
//				throw IllegalArgumentException(
//				"StorageManager: Index/Data file cannot be read/writen."
//				);

		}
		else
		{
			//m_indexFile.open(z11::Utility::unicode_to_utf8(buf, reinterpret_cast<const wchar_t*>(sIndexFile.c_str()) ), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
			//m_dataFile.open(FILEPATH(sDataFile), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

//			if (/*m_indexFile.fail() ||*/ m_dataFile.fail())
//				throw IllegalArgumentException(
//				"StorageManager: Index/Data file cannot be created."
//				);
            if (!bFileExists) {
                throw IllegalArgumentException("StorageManager: Index/Data file are not exist.");
            }
		}
	}
	else
	{
		throw IllegalArgumentException(
			"StorageManager: Property FileName was not specified."
			);
	}

	if (bOverwrite == true)
	{
		var = ps.getProperty("PageSize");

		if (var.m_varType != VT_EMPTY)
		{
			if (var.m_varType != VT_ULONG)
				throw IllegalArgumentException(
				"StorageManager: Property PageSize must be VT_LONG"
				);
			m_pageSize = var.m_val.ulVal;
			//m_nextPage = 0;
			m_nextPage = 1;
		}
		else
		{
			throw IllegalArgumentException(
				"StorageManager: A new storage manager is created and property PageSize was not specified."
				);
		}
	}
	else
	{
		m_dataFile.read(reinterpret_cast<char*>(&m_pageSize), sizeof(uint_32));
		if (m_dataFile.fail())
			throw IllegalStateException(
			"StorageManager: Failed reading pageSize."
			);

		m_dataFile.read(reinterpret_cast<char*>(&m_nextPage), sizeof(id_type));
		if (m_dataFile.fail())
			throw IllegalStateException(
			"StorageManager: Failed reading nextPage."
			);
	}

	// create buffer.
	m_buffer = new uint_8[m_pageSize];
	memset(m_buffer,0,m_pageSize);
	//bzero(m_buffer, m_pageSize);
}

StaticDiskStorageManager::~StaticDiskStorageManager()
{
	flush();
	//m_indexFile.close();
//	m_dataFile.close();
	if (m_buffer != 0)
		delete[] m_buffer;
}

void StaticDiskStorageManager::flush()
{
	//std::fstream::pos_type pos = m_dataFile.tellp();
	//m_dataFile.seekp(0, std::ios_base::beg);
	//if (m_dataFile.fail())
	//	throw IllegalStateException(
	//	"DiskStorageManager::flush: Corrupted storage manager index file."
	//	);

	//m_dataFile.write(reinterpret_cast<const char*>(&m_pageSize), sizeof(uint_32));
	//if (m_dataFile.fail())
	//	throw IllegalStateException(
	//	"DiskStorageManager::flush: Corrupted storage manager index file."
	//	);

	//m_dataFile.write(reinterpret_cast<const char*>(&m_nextPage), sizeof(id_type));
	//if (m_dataFile.fail())
	//	throw IllegalStateException(
	//	"DiskStorageManager::flush: Corrupted storage manager index file."
	//	);
	//m_dataFile.seekp(pos, std::ios_base::beg);

	////m_indexFile.flush();
	//m_dataFile.flush();
}

void StaticDiskStorageManager::loadByteArray(const id_type id, uint_32& len, uint_8** data)
{


	m_dataFile.seekg(id * m_pageSize, std::ios_base::beg);
	if (m_dataFile.fail())
		throw IllegalStateException(
		"StorageManager::loadByteArray: Corrupted data file."
		);

	m_dataFile.read(reinterpret_cast<char*>(m_buffer), m_pageSize);
	if (m_dataFile.fail())
		throw IllegalStateException(
		"StorageManager::loadByteArray: Corrupted data file."
		);

	if(id == HeaderPage)
	{
		len = m_pageSize - (sizeof(uint_32) + sizeof(id_type)) ;
		*data = new uint_8[len];
		memcpy(*data, m_buffer + sizeof(uint_32) + sizeof(id_type), len);
	}
	else
	{
		len = m_pageSize;
		*data = new uint_8[len];
		memcpy(*data, m_buffer, len);
	}

}

void StaticDiskStorageManager::storeByteArray(id_type& id, const uint_32 len, const uint_8* const data)
{
	assert(0);
	assert(len <= m_pageSize);

	if (id == NewPage)
	{
		if (m_emptyPages.empty())
			id = m_nextPage++;
		else
		{
			id = m_emptyPages.front(); m_emptyPages.pop();
		}
	}

	m_dataFile.seekp(id * m_pageSize, std::ios_base::beg);
	if (m_dataFile.fail())
		throw IllegalStateException(
		"StorageManager::storeByteArray: Corrupted data file."
		);


	if(id == HeaderPage)
	{
		assert(len <= m_pageSize - sizeof(uint_32) - sizeof(id_type));
		memcpy(m_buffer + sizeof(uint_32) + sizeof(id_type), data, len);
	}
	else
		memcpy(m_buffer, data, len);

	m_dataFile.write(reinterpret_cast<const char*>(m_buffer), m_pageSize);
	if (m_dataFile.fail())
		throw IllegalStateException(
		"StorageManager::storeByteArray: Corrupted data file."
		);
}

void StaticDiskStorageManager::deleteByteArray(const id_type id)
{
	m_emptyPages.push(id);
}
