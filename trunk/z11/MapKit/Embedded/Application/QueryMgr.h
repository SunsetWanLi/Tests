#ifndef _EMBEDDED_APP_QUERY_MANAGER_H_
#define _EMBEDDED_APP_QUERY_MANAGER_H_
#include <string>
#include "../Tools/SpatialDataBase.h"

namespace Embedded
{
	class LayerMgr;
	class QueryInfo
	{
	public:
		uint_32 getByteArraySize();
		void storeToBuffer(uint_8 *buf);
		uint_32 getKeySize();
		void storeKeyToBuffer(uint_8* buf);
		void loadFromBuffer(const uint_8* &buf);
		static uint_32 loadKeyFromBuffer(const uint_8* buf,z11::MGWString &key);

	public:
		uint_32 m_nodeID;
		uint_32 m_offset;
		z11::MGWString m_name;
	};

	class QueryMgr
	{
	private:
		/*class QueryFileState
		{
		public:
			QueryFileState(): m_state(0) {}
			bool isInitialized(){ return m_state& INITIALIZE_BIT;}
			bool isFileExist(){ return m_state& FILE_EXIST_BIT;}
			bool hasIndex(){ return m_state& HAS_INDEX_BIT;}
			bool isIndexLoaded(){ return m_state& LOAD_INDEX_BIT;}
			void setInitialized(bool b = true) { if(b) m_state |= INITIALIZE_BIT; else m_state &= ~INITIALIZE_BIT; }
			void setFileExist(bool b = true) { if(b) m_state |= FILE_EXIST_BIT; else m_state &= ~FILE_EXIST_BIT; }
			void setHasIndex(bool b = true) { if(b) m_state |= HAS_INDEX_BIT; else m_state &= ~HAS_INDEX_BIT; }
			void setIndexLoaded(bool b = true) { if(b) m_state |= HAS_INDEX_BIT; else m_state &= ~HAS_INDEX_BIT; }
		private:
			char m_state;
			static const char INITIALIZE_BIT = 0x01;
			static const char FILE_EXIST_BIT = 0x01 << 1;
			static const char HAS_INDEX_BIT = 0x01 << 2;
			static const char LOAD_INDEX_BIT = 0x01 << 2;
		};*/

		class Index
		{
		public:
			void loadFromBuffer(const uint_8*& buf);
		public:
			z11::MGWString m_key;
			unsigned short m_id;
		};

		class SingleFileQueryEngine
		{
		public:
			SingleFileQueryEngine(): m_state(0),m_file(NULL) {}
			~SingleFileQueryEngine();
			void prefixQuery(const z11::MGWString & key, uint_32 layIndex, std::vector<std::pair<uint_32, QueryInfo> > & results);
			void init(const z11::MGWString &filePath);
		public:
			bool isInitialized(){ return m_state& INITIALIZE_BIT;}
			bool isFileExist(){ return m_state& FILE_EXIST_BIT;}
			bool hasIndex(){ return m_state& HAS_INDEX_BIT;}
			bool isIndexLoaded(){ return m_state& LOAD_INDEX_BIT;}
			void setInitialized(bool b = true) { if(b) m_state |= INITIALIZE_BIT; else m_state &= ~INITIALIZE_BIT; }
			void setFileExist(bool b = true) { if(b) m_state |= FILE_EXIST_BIT; else m_state &= ~FILE_EXIST_BIT; }
			void setHasIndex(bool b = true) { if(b) m_state |= HAS_INDEX_BIT; else m_state &= ~HAS_INDEX_BIT; }
			void setIndexLoaded(bool b = true) { if(b) m_state |= HAS_INDEX_BIT; else m_state &= ~HAS_INDEX_BIT; }
		private:
			static bool QueryInfoComp(const QueryInfo &left, const QueryInfo& right);
		private:
			char m_state;
			static const char INITIALIZE_BIT = 0x01;
			static const char FILE_EXIST_BIT = 0x01 << 1;
			static const char HAS_INDEX_BIT = 0x01 << 2;
			static const char LOAD_INDEX_BIT = 0x01 << 2;
		private:
			IStorageManager* m_file;
			std::vector<Index> m_indexes;
			std::vector<QueryInfo> m_bufferedInfoes;
			uint_32 m_indexPageNum;
			z11::MGWString m_bufferedKey;
		};
		

	public:
		QueryMgr(const z11::MGWString &fileDir, LayerMgr* layerMgr);
		void prefixQuery(const z11::MGWString &key, uint_32 layIndex, std::vector<std::pair<uint_32, QueryInfo> > &results);
		void prefixQuery(const z11::MGWString &key, const std::vector<uint_32> layIndexes, std::vector<std::pair<uint_32, QueryInfo> > &results);
		void getObject(const std::pair<uint_32, QueryInfo> &info, IShape **shape);
	private:
		LayerMgr *m_layerMgr;
		std::vector<SingleFileQueryEngine> m_queryEngines;
		z11::MGWString m_fileDir;
		
	};
}

#endif