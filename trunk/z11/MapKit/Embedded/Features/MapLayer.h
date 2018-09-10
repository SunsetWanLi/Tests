#ifndef _EMBEDDED_FEATURES_MAP_LAYER_H_
#define _EMBEDDED_FEATURES_MAP_LAYER_H_
#include "../Tools/SpatialDataBase.h"
#include "../SpatialIndex/Utility.h"

namespace Embedded
{
	namespace Features
	{
		class MapLayer
		{
		public:
			MapLayer(const z11::MGWString& filePath, const z11::MGWString layerName);
			~MapLayer();

			ISpatialIndex* getSpatialIndex();

			const z11::MGWString& getLayerName() const { return m_layerName; }
			z11::MGWString getFilePath() const { return m_filePath; }

			static void getLineByAddress(const std::vector<STATIC_LAYER*>* layers, uint_64 id, Line** l);
			void getObjectByAddress(id_type nodeID, uint_32 offset, IShape** shape);

		private:
			void loadSpatialIndex();
			void loadStaticRTree(bool bCrossPage);

		private:
			IStorageManager*			m_file;
			StorageManager::IBuffer*	m_buffer;
			ISpatialIndex*				m_spatialIndex;

			z11::MGWString				m_layerName;
			z11::MGWString				m_filePath;

		}; // MapLayer
	}
}

#endif // _EMBEDDED_FEATURES_MAP_LAYER_H_