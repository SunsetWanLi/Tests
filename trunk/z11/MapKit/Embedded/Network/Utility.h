#ifndef _EMBEDDED_NETWORK_UTILITY_H_
#define _EMBEDDED_NETWORK_UTILITY_H_
#include "../Tools/SpatialMath.h"


#if defined ( _WIN32_WINNT_WIN8 )
#include "stdint.h"
#endif

namespace Embedded
{
	namespace Network
	{
		typedef uint_32 NODE_ID;
		typedef uint_32 NEAREST_DIS;

		class BindPoint;
		class LowNetwork;
		class VisitedNode;
        
        typedef enum {
            BOTH_SEARCHING,
            SINGLE_SEARCHING
        } SearchingType;
        
        class NetworkSearchingType {
        public:
            static SearchingType searching_type;
        };

		struct BetweenTwoNodeSearchingInfo
		{
			uint_32 m_start, m_end;
			std::vector<uint_64> m_roadIDs;
			double m_knownDis;

			BetweenTwoNodeSearchingInfo() {}
			BetweenTwoNodeSearchingInfo(uint_32 s, uint_32 e, double dis = DOUBLE_MAX) 
				: m_start(s), m_end(e), m_knownDis(dis) {}

		}; // BetweenTwoNodeSearchingInfo

		class DestInfo
		{
		public:
			DestInfo();
			~DestInfo();

			void initByNodePos(const BindPoint& p, const ILookUp* table, bool bPreLoadTable);
			void initByNodeID(const LowNetwork* nw, uint_32 nodeID, const ILookUp* table, bool bPreLoadTable);

			bool beHasLookUpTableData() const {
				return m_bGridID != UINT32_MAX_1
					|| m_eGridID != UINT32_MAX_1
					|| m_bTable != NULL || m_eTable != NULL;
			}

			double getMinDisToHere(const ILookUp* lookUp, const MapPos2D& srcPos) const ;
			double getMinDisToHere(const MapPos2D& p) const { return SpatialMath::getDistance(p, m_pos); }

			bool beSameNodeID(uint_32 _id) const {
                bool ret = false;
                switch (NetworkSearchingType::searching_type) {
                    case BOTH_SEARCHING:
                        ret = _id == m_beginID || _id == m_endID;
                        break;
                    case SINGLE_SEARCHING:
                    default:
                        ret =  _id == m_beginID;
                        break;
                }
                return ret;
            }
			double getEndingDis(uint_32 nodeID) const { return nodeID == m_beginID ? m_beginLen : m_endLen; }

		private:
			MapPos2D m_pos;
			uint_32 m_beginID, m_endID;
			double m_beginLen, m_endLen;
			uint_64 m_roadID;

			uint_32 m_bGridID, m_eGridID;
			uint_8 *m_bTable, *m_eTable;

		}; // DestInfo

		class VisitedNode
		{
		public:
			uint_32 m_nodeID, m_highID;
			double m_from, m_to;
			VisitedNode* m_prev;

		public:
			VisitedNode(uint_32 id, double from, const LowNetwork* network, const ILookUp* table, 
				const DestInfo& dest, VisitedNode* prev);
			VisitedNode(uint_32 lowID, uint_32 highID, double from, const LowNetwork* network, const ILookUp* table, 
				const DestInfo& dest, VisitedNode* prev);
			VisitedNode(uint_32 id, double from, VisitedNode* prev) 
				: m_nodeID(id), m_highID(0), m_from(from), m_to(0), m_prev(prev) {}
			VisitedNode(uint_32 id) 
				: m_nodeID(id), m_highID(0) {}

			void getShortestPath(std::vector<uint_32>* pathNodeIDs) const;
			void getShortestPath(std::vector<std::pair<NODE_ID, NEAREST_DIS> >* path) const;

			struct LengthAscending : public std::binary_function<VisitedNode*, VisitedNode*, bool>
			{
				bool operator() (const VisitedNode* lhs, const VisitedNode* rhs) const {
					return lhs->m_from + lhs->m_to < rhs->m_from + rhs->m_to
						|| (lhs->m_from + lhs->m_to == rhs->m_from + rhs->m_to && lhs->m_nodeID < rhs->m_nodeID);
				}
			};

			struct IDAscending : public std::binary_function<VisitedNode*, VisitedNode*, bool>
			{
				bool operator() (const VisitedNode* lhs, const VisitedNode* rhs) const {
					return lhs->m_nodeID < rhs->m_nodeID;
				}
			};

		private:
			void initToDis(const LowNetwork* network, const ILookUp* table,
				const DestInfo& dest);

		}; // VisitedNode

		typedef std::set<VisitedNode*, VisitedNode::IDAscending>::iterator PTR_VISITED;
		class VisitedList
		{
		public:
			~VisitedList() { clear(); }

			void push(VisitedNode* n) { m_data.insert(n); }

			PTR_VISITED find(uint_32 _id) {
				VisitedNode n(_id);
				return m_data.find(&n);
			}

			bool exist(PTR_VISITED ptr) const { return ptr != m_data.end(); }

			void clear() {
				PTR_VISITED itr = m_data.begin();
				for ( ; itr != m_data.end(); ++itr)
					delete *itr;
				m_data.clear();
			}

			// debug
			uint_32 size() const { return m_data.size(); }
			//

		private:
			std::set<VisitedNode*, VisitedNode::IDAscending> m_data;

		}; // VisitedList

		class CandidateList
		{
		public:
			bool empty() const { return m_data.empty(); }

			void push(VisitedNode* n) { m_data.insert(n); }

			VisitedNode* top() const { return *m_data.begin(); }

			void pop() { m_data.erase(m_data.begin()); }

			void update(VisitedNode* v, double len, VisitedNode* src) {
				std::set<VisitedNode*, VisitedNode::LengthAscending>::iterator itr
					= m_data.find(v);

				if (itr != m_data.end())
					m_data.erase(itr);

				v->m_from = len;
				v->m_prev = src;
				m_data.insert(v);
			}

			void clear() { m_data.clear(); }

		private:
			std::set<VisitedNode*, VisitedNode::LengthAscending> m_data;

		}; // CandidateList

		void fetchLayersName(IStorageManager* sm, std::vector<z11::MGWString>& names);
		void noSearchPath(const BindPoint& from, const BindPoint& to, std::vector<uint_32>* routeNodeID);
	}
}

#endif // _EMBEDDED_NETWORK_UTILITY_H_