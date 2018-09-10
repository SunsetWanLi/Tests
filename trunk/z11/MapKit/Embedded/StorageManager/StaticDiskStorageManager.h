#ifndef _EMBEDDED_STORAGEMANAGER_STATIC_DISKSTORAGEMANAGER_H_
#define _EMBEDDED_STORAGEMANAGER_STATIC_DISKSTORAGEMANAGER_H_
#include "../Tools/SpatialDataBase.h"

namespace Embedded
{
	namespace StorageManager
	{
		class StaticDiskStorageManager : public IStorageManager
		{
		public:
			StaticDiskStorageManager(PropertySet&);
			// String       Value		Description
			// ----------------------------------------------------------------
			// Overwrite	VT_BOOL		overwrite the file if it exists.
			// FileName		VT_PWCHAR	.idx and .dat extensions will be added.
			// PageSize		VT_ULONG	specify the page size.

			virtual ~StaticDiskStorageManager();

			void flush();

			virtual void loadByteArray(const id_type id, uint_32& len, uint_8** data);
			virtual void storeByteArray(id_type& id, const uint_32 len, const uint_8* const data);
			virtual void deleteByteArray(const id_type id);
			virtual uint_32 getPage0Capacity() { return m_pageSize - sizeof(uint_32) - sizeof(id_type);}


		private:
			std::stringstream m_dataFile;
			//std::fstream m_indexFile;
			uint_32 m_pageSize;
			id_type m_nextPage;
			std::queue<id_type> m_emptyPages;

			uint_8* m_buffer;
		}; // StaticDiskStorageManager
	}
}

#endif // _EMBEDDED_STORAGEMANAGER_STATIC_DISKSTORAGEMANAGER_H_
