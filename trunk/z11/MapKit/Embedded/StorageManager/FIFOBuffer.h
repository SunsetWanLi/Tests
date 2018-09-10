#ifndef _EMBEDDED_STORAGEMANAGER_FIFO_BUFFER_H_
#define _EMBEDDED_STORAGEMANAGER_FIFO_BUFFER_H_
#include "Buffer.h"

namespace Embedded
{
	namespace StorageManager
	{
		class FIFOBuffer : public Buffer
		{
		public:
			FIFOBuffer(IStorageManager&, PropertySet& ps);
			virtual ~FIFOBuffer();

			void addEntry(id_type id, Entry* pEntry);
			void removeEntry();

		private:
			std::queue<id_type> m_queue;
		}; // FIFOBuffer
	}
}

#endif // _EMBEDDED_STORAGEMANAGER_FIFO_BUFFER_H_
