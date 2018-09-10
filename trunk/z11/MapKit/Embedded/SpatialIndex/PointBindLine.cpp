#include "PointBindLine.h"
#include "StaticRTree.h"
#include "../BasicType/Point.h"
#include "../BasicType/Line.h"
#include "../Features/Data.h"
#include "../Tools/SpatialMath.h"
#include "System/Shared/MGLog.h"
#include "Retiguide/Common/CRS/GlobalCrt.h"
using namespace Embedded;
using namespace Embedded::SpatialIndex;
using namespace z11;
using namespace GIS_Embedded;
PointBindLine::PointBindLine(const std::vector<ISpatialIndex*>& layers, const ICondenseStrategy* cs) 
	: m_condenseStrategy(cs)
{
	uint_32 size = layers.size();
	m_layers.reserve(size);
	for (uint_32 i = 0; i < size; ++i)
	{
		StaticRTree* tree = dynamic_cast<StaticRTree*>(layers.at(i));
		assert(tree);
		if (tree == NULL)
			throw IllegalStateException("PointBindLine: Layer type is not StaticRTree");
		m_layers.push_back(tree);
	}
}

void PointBindLine::bind(const MapPos2D& p, Line** line, uint_64* id) const
{
	Point query(p);
	NNComparator nnc;

	bool find = false;
	double nearest = DOUBLE_MAX;

	for (uint_32 i = 0; i < m_layers.size(); ++i)
	{
		std::priority_queue<NNEntry*, std::vector<NNEntry*>, NNEntry::ascending> queue;
		queue.push(new NNEntry(m_layers.at(i)->m_rootID, 0, 0.0));

		while (!queue.empty())
		{
			NNEntry* pFirst = queue.top();

			if (find && pFirst->m_minDist > nearest)
				break;

			queue.pop();

			if (pFirst->m_pEntry == 0)      //中间节点
			{
				NodePtr n = m_layers.at(i)->readNode(pFirst->m_id);

				for (uint_32 cChild = 0; cChild < n->m_children; cChild++)
				{
					if (n->m_bLeaf)
					{
						Features::Data* e = new Features::Data(n->m_pDataLength[cChild], n->m_pData[cChild], *(n->m_ptrMBR[cChild]), n->m_pIdentifier[cChild], m_condenseStrategy);
						queue.push(new NNEntry(n->m_pIdentifier[cChild], e, nnc.getMinimumDistance(query, *e)));
					}
					else
					{
						queue.push(new NNEntry(n->m_pIdentifier[cChild], 0, nnc.getMinimumDistance(query, *(n->m_ptrMBR[cChild]))));
					}
				}
			}
			else				//叶子节点
			{
				if (find && pFirst->m_minDist > nearest)
				{
					delete pFirst->m_pEntry;
					break;
				}
				else
				{
					if (find)
						delete *line;
					else
						find = true;

					IShape* pS;
					static_cast<IData*>(pFirst->m_pEntry)->getShape(&pS);
					*line = dynamic_cast<Line*>(pS);
					assert(*line);

					*id = pFirst->m_pEntry->getIdentifier();

					nearest = pFirst->m_minDist;
					delete pFirst->m_pEntry;
				}
			}

			delete pFirst;
		}

		while (!queue.empty())
		{
			NNEntry* e = queue.top(); queue.pop();
			if (e->m_pEntry != 0) delete e->m_pEntry;
			delete e;
		}
	}
}

void Embedded::SpatialIndex::PointBindLine::bind( const MapPos2D& p, double course,Line** line, uint_64* id ) const
{
	Point query(p);
	NNComparator nnc;

	bool find = false;
	double nearest = DOUBLE_MAX;

	for (uint_32 i = 0; i < m_layers.size(); ++i)
	{
		std::priority_queue<NNEntry*, std::vector<NNEntry*>, NNEntry::ascending> queue;
		queue.push(new NNEntry(m_layers.at(i)->m_rootID, 0, 0.0));

		while (!queue.empty())
		{
			NNEntry* pFirst = queue.top();

			if (find && pFirst->m_minDist > nearest)
				break;

			queue.pop();

			if (pFirst->m_pEntry == 0)      //中间节点
			{
				NodePtr n = m_layers.at(i)->readNode(pFirst->m_id);

				for (uint_32 cChild = 0; cChild < n->m_children; cChild++)
				{
					if (n->m_bLeaf)
					{
						Features::Data* e = new Features::Data(n->m_pDataLength[cChild], n->m_pData[cChild], *(n->m_ptrMBR[cChild]), n->m_pIdentifier[cChild], m_condenseStrategy);

						double weighted_dis=0.0;
						if(true)
						{
							//if (n->m_pIdentifier[cChild]==786434)
							//{
							//	static bool flag=true;
							//	if (flag)
							//	{
							//		flag=false;
							//		IShape* pS=NULL;
							//		e->getShape(&pS);
							//		Line *l=dynamic_cast<Line *>(pS);
							//		assert(l);
							//		std::vector<MapPos2D> ctlpos;
							//		l->getPoints(&ctlpos);
							//		FILE *file=fopen("D:/1.txt","wb");
							//		GlobalCrt crt;
							//		for (int i=0;i<ctlpos.size();i++)
							//		{
							//			double lon,lat;
							//			crt.prjToGeo(ctlpos[i].d0,ctlpos[i].d1,&lon,&lat);
							//			fprintf(file,"%d %lf,%lf\n",i,lon,lat);
							//		}
							//		fclose(file);
							//	}
							//}

							IShape* pS=NULL;
							e->getShape(&pS);
							Line *l=dynamic_cast<Line *>(pS);
							assert(l);
							std::vector<MapPos2D> ctlpos;
							l->getPoints(&ctlpos);
							double line_course =0;
							if(ctlpos.size()>=2)
							{
								uint_32 iNearestPos = 0;
								double nearestDis = SpatialMath::getDistance(p, ctlpos.at(iNearestPos));
								for (uint_32 i = 1; i < ctlpos.size(); ++i)
								{
									double dis = SpatialMath::getDistance(p, ctlpos.at(i));
									if (dis < nearestDis)
									{
										nearestDis = dis;
										iNearestPos = i;
									}
								}

								{
									if (iNearestPos==0)
									{
										Pos2D geo_s,geo_e;
										GlobalCrt crt;
										crt.prjToGeo(ctlpos[iNearestPos],geo_s);
										crt.prjToGeo(ctlpos[iNearestPos+1],geo_e);
										line_course= SpatialMath::calculateCourse(geo_s,geo_e);
//                                        MGLogD("start lat %lf, lon %lf, course %lf",geo_s.d0, geo_s.d1, line_course);
//                                        MGLogD("end lat %lf, lon %lf, course %lf",geo_e.d0, geo_e.d1, line_course);
									}
									else
									{
										Pos2D geo_s,geo_e;
										GlobalCrt crt;
										crt.prjToGeo(ctlpos[iNearestPos-1],geo_s);
										crt.prjToGeo(ctlpos[iNearestPos],geo_e);
										line_course= SpatialMath::calculateCourse(geo_s,geo_e);
//                                        MGLogD("start lat %lf, lon %lf, course %lf",geo_s.d0, geo_s.d1, line_course);
//                                        MGLogD("end lat %lf, lon %lf, course %lf",geo_e.d0, geo_e.d1, line_course);

									}
									//if (iNearestPos != ctlpos.size() - 1)
									//{
									//	MapPos2D foot;
									//	if (SpatialMath::getFootOfPerpendicular(ctlpos.at(iNearestPos), ctlpos.at(iNearestPos + 1), p, &foot) == 0)
									//	{
									//		double dis = SpatialMath::getDistance(foot, p);
									//		if (dis < nearestDis)
									//		{
									//			nearestDis = dis;
									//			//*nearest = foot;
									//			line_course= SpatialMath::calculateCourse(ctlpos[iNearestPos],foot);
									//		}
									//	}

									//}

									//if (iNearestPos != 0)
									//{
									//	MapPos2D foot;
									//	if (SpatialMath::getFootOfPerpendicular(ctlpos.at(iNearestPos), ctlpos.at(iNearestPos - 1), p, &foot) == 0)
									//	{
									//		double dis = SpatialMath::getDistance(foot, p);
									//		if (dis < nearestDis)
									//		{

									//			//*nearest = foot;
									//			line_course= SpatialMath::calculateCourse(foot,ctlpos[iNearestPos]);
									//		}
									//	}
									//}
								}
								//if (n->m_pIdentifier[cChild]==786434)
								//{
								//	GlobalCrt crt;
								//	double lon,lat;
								//	crt.prjToGeo(p.d0,p.d1,&lon,&lat);
								//	double lon2,lat2;
								//	crt.prjToGeo(ctlpos[iNearestPos].d0,ctlpos[iNearestPos].d1,&lon2,&lat2);
								//	static int jj=0;
								//	MGLogD("(%d),(%lf,%lf) = bind point(%d) (%lf,%lf)",jj++,lon,lat,iNearestPos,lon2,lat2);
								//	MGLogD("line_course=%lf,course=%lf",line_course,course);
								//}
								if (line_course-course<30&&line_course-course>-30)
								{
									weighted_dis=-50;
									//MGLogD("bind id=%lld couse %lf,weight  -50",n->m_pIdentifier[cChild],line_course-course);
								}
								else
								{
									weighted_dis=0;
									//MGLogD("bind id=%lld  couse %lf,weight 0",n->m_pIdentifier[cChild],line_course-course);
								}
							}
							delete pS;
						}
						queue.push(new NNEntry(n->m_pIdentifier[cChild], e, nnc.getMinimumDistance(query, *e)+weighted_dis));
					}
					else
					{
						queue.push(new NNEntry(n->m_pIdentifier[cChild], 0, nnc.getMinimumDistance(query, *(n->m_ptrMBR[cChild]))));
					}
				}
			}
			else				//叶子节点
			{
				if (find && pFirst->m_minDist > nearest)
				{
					delete pFirst->m_pEntry;
					break;
				}
				else
				{
					if (find)
						delete *line;
					else
						find = true;

					IShape* pS;
					static_cast<IData*>(pFirst->m_pEntry)->getShape(&pS);
					*line = dynamic_cast<Line*>(pS);
					assert(*line);

					*id = pFirst->m_pEntry->getIdentifier();

					nearest = pFirst->m_minDist;
					delete pFirst->m_pEntry;
				}
			}

			delete pFirst;
		}

		while (!queue.empty())
		{
			NNEntry* e = queue.top(); queue.pop();
			if (e->m_pEntry != 0) delete e->m_pEntry;
			delete e;
		}
	}
}

