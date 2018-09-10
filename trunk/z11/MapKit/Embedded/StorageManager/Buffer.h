#ifndef _EMBEDDED_STORAGEMANAGER_BUFFER_H_
#define _EMBEDDED_STORAGEMANAGER_BUFFER_H_
#include "../Tools/SpatialDataBase.h"
#include <string.h>

namespace Embedded
{
	namespace StorageManager
	{
		class Buffer : public IBuffer
		{
		public:
			Buffer(IStorageManager& sm, PropertySet& ps);
				// String       Value		Description
				// ----------------------------------------------------------------
				// Capacity		VT_ULONG	Buffer maximum capacity.
				// WriteThrough	VT_BOOL		Enable or disable write through policy.

			virtual ~Buffer();

			//
			// IStorageManager interface
			//
			virtual void loadByteArray(const id_type id, uint_32& len, uint_8** data);
			virtual void storeByteArray(id_type& id, const uint_32 len, const uint_8* const data);
			virtual void deleteByteArray(const id_type id);
			virtual uint_32 getPage0Capacity() { return m_pStorageManager->getPage0Capacity(); }


			//virtual void getNodePageInfo(id_type id, uint_32& length, uint_32& pageSize) const;

			//
			// IBuffer interface
			//
			virtual void clear();
			virtual uint_32 getHits();

		protected:
			class Entry
			{
			public:
				Entry(uint_32 l, const uint_8* const d) : m_pData(0), m_length(l), m_bDirty(false)
				{
					m_pData = new uint_8[m_length];
					memcpy(m_pData, d, m_length);
				}

				~Entry() { delete[] m_pData; }

				uint_8* m_pData;
				uint_32 m_length;
				bool m_bDirty;
			}; // Entry

			virtual void addEntry(id_type id, Entry* pEntry) = 0;
			virtual void removeEntry() = 0;

			uint_32 m_capacity;
			bool m_bWriteThrough;
			IStorageManager* m_pStorageManager;
			std::map<id_type, Entry*> m_buffer;
			uint_32 m_hits;

		};	// Buffer
	}
}

#endif // _EMBEDDED_STORAGEMANAGER_BUFFER_H_
