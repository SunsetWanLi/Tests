#ifndef _EMBEDDED_STORAGEMANAGER_DISKSTORAGEMANAGER_H_
#define _EMBEDDED_STORAGEMANAGER_DISKSTORAGEMANAGER_H_
#include "../Tools/SpatialDataBase.h"

namespace Embedded
{
	namespace StorageManager
	{
		class DiskStorageManager : public IStorageManager
		{
		public:
			DiskStorageManager(PropertySet&);
				// String       Value		Description
				// ----------------------------------------------------------------
				// Overwrite	VT_BOOL		overwrite the file if it exists.
				// FileName		VT_PWCHAR	.idx and .dat extensions will be added.
				// PageSize		VT_ULONG	specify the page size.

			virtual ~DiskStorageManager();

			void flush();

			virtual void loadByteArray(const id_type id, uint_32& len, uint_8** data);
			virtual void storeByteArray(id_type& id, const uint_32 len, const uint_8* const data);
			virtual void deleteByteArray(const id_type id);
			virtual uint_32 getPage0Capacity() { return (uint_32)-1;} //zj

			//virtual void getNodePageInfo(id_type id, uint_32& length, uint_32& pageSize) const;

		private:
			class Entry
			{
			public:
				uint_32 m_length;
				std::vector<id_type> m_pages;
			};

			std::stringstream m_dataFile;
			std::stringstream m_indexFile;
			uint_32 m_pageSize;
			id_type m_nextPage;
			std::priority_queue<id_type, std::vector<id_type>, std::greater<id_type> > m_emptyPages;
			std::map<id_type, Entry*> m_pageIndex;

			uint_8* m_buffer;
		}; // DiskStorageManager
	}
}

#endif // _IS_SDE_STORAGEMANAGER_DISKSTORAGEMANAGER_H_