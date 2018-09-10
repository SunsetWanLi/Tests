/*
 * =====================================================================================
 *
 *       Filename:  FetcherThread.h
 *
 *    Description:  The Thread that Fetch data from network , local standard map data,
 *    or local statlite map data
 *
 *        Version:  1.0
 *        Created:  04/13/2012 09:32:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include <queue>
#include <stack>
#include "System/Thread/MGThread.h"
#include "System/Thread/MGMutex.h"
#include "GUI/Base/MIResponder.h"
#include <vector>
#include "Platform/MGPlatformConfig.h"
#include "MapKit/DataTypes/Base/MKGeometry.h"
#include "MapKit/DataTypes/Base/MKTypes.h"
#include "CommonBase/Memory/MGRefObject.h"
#include "MapKit/Views/MapViews/MGReusableQueue.h"
#include "CommonBase/MGError.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

namespace z11
{
	class MKTileLayerOverlay;
	class MKFetcherThread;
	class MGURLFetcher;

	class MKFetcherTask:public MGRefObject
	{
	public:
		MKFetcherTask()
		{
			type=MKMapTypeStandard;
            isWebData = false;
		}
		MKMapType type;
        bool isWebData;
		//std::queue<MKTileLayerOverlay*> tiles;
		std::queue<MKGlobalTileIndex> tiles;
	};
	class MKMapView;
	class MKMapLayerView;

	//contain MKFetcherThread common resource;
	class MKFetcherThreadManager:public MGObject,public Syncable
	{
	public:
		void collectNTiles(unsigned long n);
		void checkReuseQueueLimited();
		void freeMemory();
		void freeActuallyMemory();
		void addThread(MKFetcherThread *thread);
		void removeThread(MKFetcherThread *thread);
		void enqueue(MKSateliteMapViewCell *cell);
		void update(MKSateliteMapViewCell *cell);
		MKSateliteMapViewCell *contain(const MKGlobalTileIndex &identify);
	private:
		MGReusableQueueBasedAccessTime reuse_queue;
		std::set<MKFetcherThread *> m_thread_pool;
		std::set<MKTileLayerOverlay *> need_to_free;
	};

	class MKFetcherThread:public MGThread,public Syncable
	{
	public:
		//retain
		void addTask(const Ref<MKFetcherTask> &task);
		void clear();
		MKFetcherThread();
		virtual ~MKFetcherThread();
		virtual void requestExit();
		void clearCache();
	public :
		void setParentView(MKMapView *_map_view , MKMapLayerView * _tile_layer_view);
		void setStartResponder(MIResponderSEL& threadWillStartLoadingMap);
		void setFinshResponder(MIResponderSEL& threadDidFinishLoadingMap);
	protected:
		MKMapView *map_view;
		MKMapLayerView *tile_layer_view;
		MIResponderSEL m_threadWillStartLoadingMap;
		MIResponderSEL m_threadDidFinishLoadingMap;
		MGMutex m_parent_pointer_lock;

	protected:
		virtual int readyToRun();
		virtual int readyToStop();
		Ref<MKFetcherTask> m_task;
	protected:
		MGSemaphore *m_fetcher_semaphore;
		Ref<MGError> m_error;
	private:
		int network_failed;
		MGURLFetcher* request;
		virtual bool threadLoop();
		void download(const MKGlobalTileIndex &index,MKMapType  type);
		bool blocked;
		static MKFetcherThreadManager manager;
	};

}
