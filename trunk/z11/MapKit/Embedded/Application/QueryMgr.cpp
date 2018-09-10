#include "QueryMgr.h"
#include "LayerMgr.h"
#include "../../../System/Shared/Utility.h"
#include "../Tools/StringTools.h"
#include "System/IO/MGFileManager.h"
#include <limits.h>
#include <string.h>
namespace Embedded
{
	uint_32 QueryInfo::getByteArraySize()
	{
		return getKeySize()
			+ sizeof(uint_32) +sizeof(uint_32);

	}
	void QueryInfo::storeToBuffer(uint_8 *buf)
	{
		storeKeyToBuffer(buf);
		buf += getKeySize();
		memcpy(buf, &m_nodeID, sizeof(uint_32));
		buf += sizeof(uint_32);
		memcpy(buf, &m_offset, sizeof(uint_32));
		//buf += sizeof(uint_32);
	}

	uint_32 QueryInfo::getKeySize()
	{
		assert(0); //zj should never be called.
		return 1 + m_name.size() * 16 /*sizeof(wchar_t)*/;
	}

	void QueryInfo::storeKeyToBuffer(uint_8 *buf)
	{
		assert(0); //zj should never be called.
		unsigned char size = m_name.size();
		memcpy(buf, &size, 1);
		buf += 1;
		memcpy(buf, &m_name[0], size * 16/*sizeof(wchar_t)*/);
	}
	void QueryInfo::loadFromBuffer(const uint_8 *&buf)
	{
		buf += loadKeyFromBuffer(buf, m_name);
		memcpy(&m_nodeID, buf, sizeof(uint_32));
		buf += sizeof(uint_32);
		memcpy(&m_offset, buf, sizeof(uint_32));
		buf += sizeof(uint_32);
	}

	uint_32 QueryInfo::loadKeyFromBuffer(const uint_8 *buf, z11::MGWString &key)
	{
		/*unsigned char size = *buf++;
		key.resize(size);
		memcpy(&key[0], buf, size * sizeof(wchar_t));
		return 1 + size * sizeof(wchar_t);*/
		return StringTools::readWString(buf, key);
	}

	QueryMgr::QueryMgr(const z11::MGWString &fileDir, LayerMgr* layerMgr)
	{
		m_layerMgr = layerMgr;
		m_queryEngines.resize(layerMgr->layersSize());
		m_fileDir = fileDir;
		if(m_fileDir[m_fileDir.size()-1] != (L'/') && m_fileDir[m_fileDir.size()-1] != (L'/'))
			m_fileDir.push_back((L'/'));
	}

	void QueryMgr::prefixQuery(const z11::MGWString &key, uint_32 layIndex, std::vector<std :: pair<uint_32,QueryInfo > > & results)
	{
		assert(layIndex < m_layerMgr->layersSize());
		SingleFileQueryEngine &engine = m_queryEngines[layIndex];
		if(!engine.isInitialized())
		{
			z11::MGWString layerName = m_layerMgr->getLayerName(layIndex);
			layerName = layerName.substr(0,layerName.find_last_of( (L'_') ));
			engine.init(m_fileDir + layerName + WS(L"_QUERY.dat") );
		}

		if(engine.isFileExist())
		{
			engine.prefixQuery(key, layIndex, results);
		}
	}

	void QueryMgr::prefixQuery(const z11::MGWString &key, const std::vector<uint_32> layIndexes, std::vector<std::pair<uint_32, QueryInfo> > &results)
	{
		for(uint_32 i = 0; i < layIndexes.size(); ++i)
		{
			prefixQuery(key, layIndexes[i], results);
		}
	}

	void QueryMgr::getObject(const std::pair<uint_32, QueryInfo> &info, IShape **shape)
	{
		m_layerMgr->getObjectByAddress(info.first, info.second.m_nodeID, info.second.m_offset, shape);
		assert(info.second.m_name.compare((*shape)->getName()) == 0);
	}
	void QueryMgr::Index::loadFromBuffer(const uint_8 *&buf)
	{
		buf += StringTools::readWString(buf, m_key);
		memcpy(&m_id, buf, 2);
		buf += 2;
	}

	QueryMgr::SingleFileQueryEngine::~SingleFileQueryEngine()
	{
		delete m_file;
	}

	void QueryMgr::SingleFileQueryEngine::init(const z11::MGWString &filePath)
	{
		setInitialized(true);
		if(z11::MGFileManager::defaultManager()->fileExistsAtPath(z11::Utility::wstr2ustr (filePath) ))
		{
			//in.close();
			m_file = StorageManager::loadStaticDiskStorageManager(filePath.substr(0, filePath.find_last_of( (L'.') )));
			setFileExist(true);
		}
		else
		{
			setFileExist(false);
		}

		if(isFileExist())
		{
			uint_32 len;
			uint_8* data;
			m_file->loadByteArray(0, len, &data);

			//uint_32 pageSize;
			//id_type nextPage;
			uint_32 indexPageNum;
			uint_32 totalInfos;
			uint_32 currentNum;
			uint_32 offset = 0;
			//memcpy(&pageSize, data+offset, sizeof(uint_32));
			//offset += sizeof(uint_32);
			//memcpy(&nextPage,  data+offset, sizeof(id_type));
			//offset += sizeof(id_type);
			memcpy(&indexPageNum, data+offset, sizeof(uint_32));
			m_indexPageNum = indexPageNum;
			offset += sizeof(uint_32);
			memcpy(&totalInfos, data+offset, sizeof(uint_32));
			offset += sizeof(uint_32);
			memcpy(&currentNum, data+offset, sizeof(uint_32));
			offset += sizeof(uint_32);

			const uint_8* ptr = data + offset;

			if(indexPageNum == 0)
			{
				assert(totalInfos == currentNum);
				setHasIndex(false);
				m_bufferedInfoes.resize(totalInfos);
				for(uint_32 i = 0; i < totalInfos; ++i)
				{
					m_bufferedInfoes[i].loadFromBuffer(ptr);
					assert(ptr - data < len);
				}
			}
			else
			{
				setHasIndex(true);
				uint_32 totalIndex = currentNum;
				uint_32 index = 0;
				m_indexes.resize(totalIndex);
				for(uint_32 i = 0; i < indexPageNum; ++i)
				{
					if(i > 0)
					{
						delete []data;
						m_file->loadByteArray((id_type)i, len, &data);
						memcpy(&currentNum, data, sizeof(uint_32));
						ptr = data + sizeof(uint_32);
						totalIndex += currentNum;
						m_indexes.resize(totalIndex);
					}

					for(uint_32 j = 0; j < currentNum; ++j)
					{
						m_indexes[index++].loadFromBuffer(ptr);
						assert(ptr - data < len);
					}

				}
				assert(totalIndex == index);
				setIndexLoaded(true);
			}

			delete[] data;
		}

	}
	void QueryMgr::SingleFileQueryEngine::prefixQuery(const z11::MGWString &key, uint_32 layIndex, std::vector<std::pair<uint_32,QueryInfo> > &results)
	{
		assert(isInitialized());

		if(m_bufferedKey.size() > 0 && m_bufferedKey.compare(0, m_bufferedKey.size(), key, 0, m_bufferedKey.size()) == 0)
		{
			QueryInfo keyQuery;
			keyQuery.m_name = key;
			std::vector<QueryInfo> candidates = m_bufferedInfoes;
			std::vector<QueryInfo>::iterator bIt = std::lower_bound(candidates.begin(), candidates.end(), keyQuery, QueryInfoComp);
			std::vector<QueryInfo>::iterator eIt = std::upper_bound(bIt, candidates.end(), keyQuery, QueryInfoComp);

			std::pair<uint_32, QueryInfo> temp;
			temp.first = layIndex;
			m_bufferedInfoes.reserve(eIt - bIt);
			m_bufferedKey = key;
			while(bIt < eIt)
			{
				temp.second = *bIt;
				results.push_back(temp);
				m_bufferedInfoes.push_back(*bIt);
				++bIt;
			}

		}
		else if(!hasIndex())
		{
			if(key.compare(0, key.size(), m_bufferedInfoes.back().m_name, 0, key.size()) > 0)
				return;
			if(key.compare(0, key.size(), m_bufferedInfoes.front().m_name, 0, key.size()) < 0)
				return;

			QueryInfo keyQuery;
			keyQuery.m_name = key;

			std::vector<QueryInfo>::iterator bIt = std::lower_bound(m_bufferedInfoes.begin(), m_bufferedInfoes.end(), keyQuery, QueryInfoComp);
			std::vector<QueryInfo>::iterator eIt = std::upper_bound(bIt, m_bufferedInfoes.end(), keyQuery, QueryInfoComp);
			//std::pair<std::vector<QueryInfo>::iterator, std::vector<QueryInfo>::iterator> range = std::equal_range(m_bufferedInfoes.begin(), m_bufferedInfoes.end(), keyQuery, QueryInfoComp);

			//std::vector<QueryInfo>::iterator bIt = range.first;
			//std::vector<QueryInfo>::iterator eIt = range.second;

			std::pair<uint_32, QueryInfo> temp;
			temp.first = layIndex;
			while(bIt < eIt)
			{
				temp.second = *bIt;
				results.push_back(temp);
				++bIt;
			}
		}
		else
		{
			uint_32 index = (uint_32)-1;
			std::vector<uint_32> pages;
			for(uint_32 i = 0; i < m_indexes.size(); ++i)
			{
				int sign = key.compare(0, key.size(), m_indexes[i].m_key);

				if(m_indexes.size() == 1 && sign >= 0 )
					pages.push_back(0);

				if( sign <= 0 && i > 0)
				{
					pages.push_back( i-1 );
					if(sign < 0)
						break;
				}
			}

			QueryInfo info;
			std::vector<QueryInfo> candidates;

			for(uint_32 i = 0; i < pages.size(); ++i)
			{
				uint_32 len, num;
				uint_8* data;
				m_file->loadByteArray((id_type)i, len, &data);
				memcpy(&num, data, sizeof(data));
				const uint_8* ptr = data + sizeof(data);

				candidates.reserve(candidates.size() + num);
				for(uint_32 j = 0; j < num; ++j)
				{
					info.loadFromBuffer(ptr);
					assert(ptr - data <= len);
					candidates.push_back(info);
				}
			}

			QueryInfo keyQuery;
			keyQuery.m_name = key;
			std::vector<QueryInfo>::iterator bIt = std::lower_bound(candidates.begin(), candidates.end(), keyQuery, QueryInfoComp);
			std::vector<QueryInfo>::iterator eIt = std::upper_bound(bIt, candidates.end(), keyQuery, QueryInfoComp);

			std::pair<uint_32, QueryInfo> temp;
			temp.first = layIndex;
			m_bufferedInfoes.reserve(eIt - bIt);
			m_bufferedKey = key;
			while(bIt < eIt)
			{
				temp.second = *bIt;
				results.push_back(temp);
				m_bufferedInfoes.push_back(*bIt);
				++bIt;
			}

		}
	}

	bool QueryMgr::SingleFileQueryEngine::QueryInfoComp(const QueryInfo &left, const QueryInfo &right)
	{
		if(&left == &right)
			return false;
		int size = std::min(left.m_name.size(), right.m_name.size());
		if( left.m_name.compare(0, size, right.m_name, 0, size) <0 )
			return true;
		else
			return false;
	}

}
