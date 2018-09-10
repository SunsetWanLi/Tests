#ifndef _EMBEDDED_FEATURES_CONDENSE_H_
#define _EMBEDDED_FEATURES_CONDENSE_H_
#include "../Tools/SpatialDataBase.h"

namespace Embedded
{
	namespace Features
	{
		class NoOffsetCondenseStrategy : public ICondenseStrategy
		{
		public:
			virtual uint_32 getType() const { return static_cast<uint_32>(CS_NO_OFFSET); }
			virtual uint_32 allShapeDataSize(const INode* n) const;
			virtual uint_32 dataSize(const IShape* s) const;
			virtual void loadFromByteArray(IShape** s, const uint_8* data, uint_32 length) const;
			virtual void storeToByteArray(const IShape* s, uint_8** data, uint_32& length) const;
			virtual void setShapeType(ShapeType st) { m_shapeType = st; };

		public:
			static const uint_32 TYPE_SIZE			= 1;
			static const uint_32 ATTR_ADDR_SIZE		= sizeof(uint_32);
			static const uint_32 POINT_COUNT_SIZE	= 2;
			//static const uint_32 POLY_COUNT_SIZE		= 1;
			static const uint_32 COORDS_SIZE			= 4;

			static const uint_32 MAX_POINT_COUNT		= 0xffff;
			static const uint_32 MAX_POLY_COUNT		= 0xff;
		private:
			ShapeType m_shapeType;

		}; // NoOffsetCondenseStrategy

//			class CertaintyOffsetCondenseStrategy : public ICondenseStrategy
//			{
//			public:
//				virtual uint_32 getType() const { return static_cast<uint_32>(CS_CERTAINTY_OFFSET); }
//				virtual uint_32 allShapeDataSize(const INode* n) const;
//				virtual uint_32 dataSize(const IShape* s) const;
//				virtual void loadFromByteArray(IShape** s, const uint_8* data, uint_32 length) const;
//				virtual void storeToByteArray(const IShape* s, uint_8** data, uint_32& length) const;
//			}; // CertaintyOffsetCondenseStrategy
//
//			class UncertaintyOffsetCondenseStrategy : public ICondenseStrategy
//			{
//			public:
//				virtual uint_32 getType() const { return static_cast<uint_32>(CS_UNCERTAINTY_OFFSET); }
//				virtual uint_32 allShapeDataSize(const INode* n) const;
//				virtual uint_32 dataSize(const IShape* s) const;
//				virtual void loadFromByteArray(IShape** s, const uint_8* data, uint_32 length) const;
//				virtual void storeToByteArray(const IShape* s, uint_8** data, uint_32& length) const;
//			}; // UncertaintyOffsetCondenseStrategy
	}
}

#endif // _EMBEDDED_FEATURES_CONDENSE_H_
