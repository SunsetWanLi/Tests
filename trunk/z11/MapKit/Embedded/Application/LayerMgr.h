#ifndef _EMBEDDED_APP_LAYER_MANAGER_H_
#define _EMBEDDED_APP_LAYER_MANAGER_H_
#include "../BasicType/Scope.h"

namespace Embedded
{
	namespace Features
	{
		class MapLayer;
	}

	class LayerMgr
	{
	public:
		static LayerMgr* getInstance();
		
		~LayerMgr();

		bool loadLayer(const z11::MGWString& filePath);
		bool isLayerExist(const z11::MGWString& layerName) const;
		void flipVisible(uint_32 index);
		MapScope getTotDisplayScope() const;
		uint_32 layersSize() const { return m_layerWithVisible.size(); }

		Features::MapLayer* getFirstVisibleMapLayer();
		void getVisibleLayers(std::vector<Features::MapLayer*>* ret);
		void getNames(std::vector< z11::MGWString > & names, bool considerNonVisible = false) const;

		bool isNoneVisible() const { return m_countVisibleLayer == 0; }

		void storeDisplayRegion(const Rect& scope);

		void queryShapeByRect(const Rect& r, std::vector< IShape* >& shapes, bool considerNonVisible = false) const;
		void queryShapeByRect(const Rect& r, std::vector<IShape*>& shapes, const z11::MGWString &layerName, bool considerNonVisible = false) const;
		void queryShapeByRect(const Rect& r, std::vector<IShape*>& shapes, uint_32 layerIndex, bool considerNonVisible = false) const;
		void queryLineByRect(const Rect&r, std::vector<Line*>& lines, bool considerNonVisible = false ) const;
		uint_32 getLayerIndex(z11::MGWString &layerName) const;
		const z11::MGWString& getLayerName(uint_32 index) const;
		void getObjectByAddress(uint_32 layerIndex,id_type nodeID, uint_32 offset, IShape** shape);

	private:

		class LayerRegionQueryStrategy : public IQueryStrategy
		{
		public:
			Rect m_indexedSpace;

		public:
			void getNextEntry(const IEntry& entry, id_type& nextEntry, bool& hasNext);
			bool shouldStoreEntry() { return false; }
		}; // LayerRegionQueryStrategy

		class FetchDataVisitor : public IVisitor
		{
		public:
			FetchDataVisitor(std::vector<IShape*>& shapes) 
				: m_shapes(shapes) {}

			bool shouldCheckEachData() const { return true; }
			void visitNode(const INode& n) {}
			void visitData(const IData& d, uint_32 dataIndex);
			void visitData(std::vector<const IData*>& v) {}
			void visitData(const INode& n) {}

		private:
			std::vector<IShape*> &m_shapes;

		}; // FetchDataVisitor

	private:
		LayerMgr();
		bool loadLayer(const z11::MGWString& path, const z11::MGWString& name);

	private:
		std::vector<std::pair<Features::MapLayer*, bool> > m_layerWithVisible;
		std::set<z11::MGWString> m_layersName;

		uint_32 m_countVisibleLayer;
		static LayerMgr* _instance;

	}; // LayerMgr
}

#endif // _EMBEDDED_APP_LAYER_MANAGER_H_