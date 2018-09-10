#ifndef _EMBEDDED_FEATURES_DATA_H_
#define _EMBEDDED_FEATURES_DATA_H_
#include "../BasicType/Rect.h"

namespace Embedded
{
	namespace Features
	{
		class Data : public IData, public ISerializable
		{
		public:
			Data(uint_32 len, uint_8* pData, Rect& r, id_type id, const ICondenseStrategy* strategy);
			Data(const IShape* s, id_type id, const ICondenseStrategy* strategy);
			virtual ~Data();

			//
			// IObject interface
			//
			virtual Data* clone();

			//
			// IEntry interface
			//
			virtual id_type getIdentifier() const;
			virtual void getShape(IShape** out) const;

			//
			// IData interface
			//
			virtual void getData(uint_32& len, uint_8** data) const;

			//
			// ISpatialIndex interface
			//
			virtual uint_32 getByteArraySize() const;
			virtual void loadFromByteArray(const uint_8* data);
			virtual void storeToByteArray(uint_8** data, uint_32& len) const;

		private:
			void getCertainShape(IShape** s) const;

		public:
			id_type m_id;
			Rect m_rect;
			uint_8* m_pData;
			uint_32 m_dataLength;

			const ICondenseStrategy* m_strategy;

		}; // Data
	}
}

#endif // _EMBEDDED_FEATURES_DATA_H_