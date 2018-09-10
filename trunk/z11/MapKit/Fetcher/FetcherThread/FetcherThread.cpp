/*
* =====================================================================================
*
*       Filename:  FetcherThread.cpp
*
*    Description:  The Thread that Fetch data from network , local standard map data,
*    or local statlite map data
*
*        Version:  1.0
*        Created:  04/13/2012 09:30:08 AM
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Lucifer (GRF), gurongfang@gmail.com
*        Company:  ZJU
*
* =====================================================================================
*/

#include "FetcherThread.h"
#include "CommonBase/MGData.h"
#include "System/Network/URLFetcher/MGURLFetcher.h"
#include "System/Shared/MGLog.h"
#include "System/Shared/Utility.h"
#include "System/Thread/MGSemaphore.h"
#include "MapKit/Fetcher/Pyramid/MKGlobalMapTiles.h"
#include "MapKit/Fetcher/WebDataFetcher/MKWebDataFetcher.h"
#include "MapKit/Views/MapViews/MKTileLayerOverlay.h"
#include "MapKit/Views/MapViews/MKTileMapView.h"
#include "MapKit/Views/MapViews/MKMapLayerView.h"
#include "MapKit/Views/MapViews/MKMapView.h"
#include "GUI/GuiContainer/GuiContainer.h"
#include <stdio.h>

namespace z11
{
    extern int z_base;
    
	MKFetcherThreadManager MKFetcherThread::manager;

	void MKFetcherThread::clear()
	{
		MGLogD("MKFetcherThread::clear task.");
		AUTO_LOCK_THIS;
		if (!m_task.empty())
		{
			while (!m_task->tiles.empty())
				m_task->tiles.pop();
		}
	}


	MKFetcherThread::MKFetcherThread() :request(NULL)
	{
		m_fetcher_semaphore=new MGSemaphore(0);//empty
		tile_layer_view=NULL;
		map_view=NULL;
		network_failed=0;
		blocked=true;
		request=new MGURLFetcher();
		request->setTimeOut(10);
	}


	void MKFetcherThread::addTask(const Ref<MKFetcherTask> &task )
	{
		AUTO_LOCK_THIS;
		m_task=task;
		m_fetcher_semaphore->Signal();
	}


	MKFetcherThread::~MKFetcherThread()
	{
		MGLogD("~MKFetcherThread");
		delete_and_set_null(request);
        delete_and_set_null(m_fetcher_semaphore);
		MGLogD("out ~MKFetcherThread");
	}

	bool MKFetcherThread::threadLoop()
	{
		m_fetcher_semaphore->Wait();
		MKTileLayerOverlay* task=NULL;
		do
		{
			{
                AUTO_LOCK_THIS;
                if (network_failed>=2)
                {
                    MGLogD("Failed twice,give up to load map");
                    //clear();
                    task = NULL;
                    network_failed = 0;
                    break;
                }
                
                if (m_task->tiles.empty())
                {
                    blocked = true;
                    break;
                }
            }

			while (1)
			{
                MKGlobalTileIndex key;
                MKMapType map_type;
                {
                    AUTO_LOCK_THIS;
                    if (m_task->tiles.empty() || task!=NULL) {
                        break;
                    }
                    
                    key = m_task->tiles.front();
                    m_task->tiles.pop();
                    map_type = m_task->type;
                }
                
                {
                    manager.lock();
                    MKSateliteMapViewCell *cell=manager.contain(key);
                    GlobalMercator mercator;
                    MKGlobalTileIndex index=key;//mercator.QuadKeyToTileIndexInt64(key);
                    uint_32 x=index.x;
                    uint_32 y=index.y;
                    uint_32 z=index.z;
                    MGIntPoint g=mercator.GoogleTile(x,y,z);
                    if (cell==NULL)
                    {
                        cell=new MKSateliteMapViewCell();
                        cell->manualClean();
                        cell->m_tms_code=index;
                        double start_zoom_value=1.0*(1<<z)/(1<<z_base);
                        cell->initWithFrame(MGRectMake((g.x)*MAP_CELL_SIZE/start_zoom_value,
                                                       (g.y)*MAP_CELL_SIZE/start_zoom_value,
                                                       MAP_CELL_SIZE/start_zoom_value,
                                                       MAP_CELL_SIZE/start_zoom_value));
                        cell->isWebData = m_task->isWebData;
                        cell->setMapType(m_task->type);
                        manager.enqueue(cell);
                    }
                    else if(cell->fetchState()!=MKFetchStateProcessing)
                    {
                        manager.update(cell);
                        if (cell->getMapType()!=map_type)
                        {
                            cell->setBackgroundColor(NULL);
                            cell->setFetchState(MKFetchStateFailed);
                            cell->setMapType(map_type);
                        }
                        else
                        {
                            m_parent_pointer_lock.lock();
                            if(tile_layer_view!=NULL)
                            {
                                if (cell->parent() != tile_layer_view) {
                                    cell->detachFromSuperview();
                                    tile_layer_view->addSubview(cell);
                                } else {
                                    tile_layer_view->moveSubviewToBack(cell);
                                }
                            }
                            m_parent_pointer_lock.unlock();
                        }
                    }
                    
                    if (cell!=NULL && cell->fetchState()!=MKFetchStateProcessing)
                    {
                        if( (cell->fetchState()!=MKFetchStateSuccess) || cell->getBackgroundColor()==NULL)
                        {
                            cell->setFetchState(MKFetchStateProcessing);
                            task=cell;
                        }
                    }
                    manager.unlock();
                }
			}
		}while(false);//End of Lock this 

		MKMapType type= MKMapTypeStandard;
		MKGlobalTileIndex index;

		do 
		{
			if (task==NULL || exitPending())
			{
				break;
			}

			//need call m_threadWillStartLoadingMap while exitPending?
			if (blocked)
			{
				blocked=false;
				m_parent_pointer_lock.lock();
				if (!m_threadWillStartLoadingMap.isNull())
				{
					MIResponder *target_response=m_threadWillStartLoadingMap.responder;
					SELHandler target_action=m_threadWillStartLoadingMap.action;
					SELFunc handler=target_action.sel_func;
					(target_response->*handler)(target_action.sender);
				}
				m_parent_pointer_lock.unlock();
			}

			MKSateliteMapViewCell *cell=dynamic_cast<MKSateliteMapViewCell*>(task);
			if (cell!=NULL)
			{
				index=cell->m_tms_code;
				type=cell->getMapType();
                switch (type)
                {
                    case MKMapTypeStandard:
                        MKWebDataFetcher::getInstance()->setMapProvider(MKMapProviderGoogle);
                        download(index,MKMapTypeStandard);
                        break;
                    case MKMapTypeHybrid:
                        MKWebDataFetcher::getInstance()->setMapProvider(MKMapProviderGoogle);
                        download(index,MKMapTypeHybrid);//故意不加break的  //
                    case MKMapTypeSatellite:
                        MKWebDataFetcher::getInstance()->setMapProvider(MKMapProviderGoogle);
                        download(index,MKMapTypeSatellite);
                        break;
                    case MKMapTypeTopographic:
                        download(index, MKMapTypeTopographic);
                        break;
                    case MKMapTypeCustomWebTile:
                    case MKMapTypeCustomTile:
                        if(cell->isWebData){
                            MKWebDataFetcher::getInstance()->setMapProvider(MKMapProviderCustom);
                            download(index, type);
                        }
                        break;
					case MKMapTypeYDJWTile:
						MKWebDataFetcher::getInstance()->setMapProvider(MKMapProviderCustom);
						download(index, type);
						break;
					case MKMapTypeTiandiVec:
						MKWebDataFetcher::getInstance()->setMapProvider(MKMapProviderTiandi);
						download(index,MKMapTypeTiandiVec);
						break;
					case MKMapTypeTiandiImg:
						MKWebDataFetcher::getInstance()->setMapProvider(MKMapProviderTiandi);
						download(index,MKMapTypeTiandiImg);
						break;
					case MKMapTypeTiandiTer:
						MKWebDataFetcher::getInstance()->setMapProvider(MKMapProviderTiandi);
						download(index,MKMapTypeTiandiTer);
						break;
                    default:
                        break;
                }
			}

		} while (false);

		do 
		{
			if (task==NULL )
			{
				break;
			}

			m_parent_pointer_lock.lock();
			if(tile_layer_view!=NULL)
			{
				tile_layer_view->addSubview(task);
				task->render();
			}
			m_parent_pointer_lock.unlock();
            
            if (task->getBackgroundColor()!=NULL)
            {
                manager.lock();
                task->setFetchState(MKFetchStateSuccess);
                manager.unlock();
                network_failed=0;
            }
            else
            {
                manager.lock();
                task->setFetchState(MKFetchStateFailed);
                manager.unlock();
                network_failed++;
            }
		} while (false);
		
		manager.checkReuseQueueLimited();
		
		if (blocked || exitPending())
		{
			if (network_failed>0)
			{
				MGLogD("MKFetcherThread::threadLoop network_failed>0 3 %p",this);
				network_failed=0;
				m_parent_pointer_lock.lock();
				if (map_view!=NULL && map_view->delegate != NULL)
				{
					map_view->delegate->mapViewDidFailLoadingMapWithError(map_view,m_error);
				}
				m_parent_pointer_lock.unlock();
			}
			else 
			{
				m_parent_pointer_lock.lock();
				if (!m_threadDidFinishLoadingMap.isNull())
				{
					MIResponder *target_response=m_threadDidFinishLoadingMap.responder;
					SELHandler target_action=m_threadDidFinishLoadingMap.action;
					SELFunc handler=target_action.sel_func;
					(target_response->*handler)(target_action.sender);
				}
				m_parent_pointer_lock.unlock();
			}
			
		}
		else
		{
			m_fetcher_semaphore->Signal();
		}
		return true;
	}

	void MKFetcherThread::download( const MKGlobalTileIndex &index,MKMapType type )
	{
		MKWebDataFetcher*web_fetcher=MKWebDataFetcher::getInstance();

		std::string path;

		path=web_fetcher->getLocalDownloadPath(index,type);
		if (Utility::isFileORDirExist(path))
			return ;

		path=web_fetcher->getDownloadPath(index,type);
		if (Utility::isFileORDirExist(path))
			return ;

		std::string url=web_fetcher->getURL(index,type);
		if (url.empty())
		{
			return ;
		}
		
		std::string output_path=web_fetcher->getDownloadPath(index,type);
		request->setURL(url);
		request->setTimeOut(10);
        // add default headers
        list<string> header;
        header.push_back("User-Agent: Chrome");
        request->setHttpHeader(header);

		std::string buffer;
		std::string host=web_fetcher->getProxyHost();
		int port=web_fetcher->getProxyPort();
		if (!host.empty()&&port!=-1)
		{
			curlpp::Easy* req=request->getRequest();
			req->setOpt(curlpp::options::Proxy(host));
			req->setOpt(curlpp::options::ProxyPort(port));
		}	
		//默认从google上获取 //
		if (!request->fetch(buffer))
		{
			buffer.clear();
			return ;
		}

		MGData *data=MGData::dataWithBytesNoCopy((void *)buffer.c_str(),buffer.size(),false);
		data->writeToFile(output_path);
		delete_and_set_null(data);
		MGLogD(url.c_str());
	}

	void MKFetcherThread::requestExit()
	{
		MGLogD("in  MKFetcherThread requestExit");
		MGThread::requestExit();

		//clear parent view pointer
		m_parent_pointer_lock.lock();
		map_view = NULL;
		tile_layer_view = NULL;
		m_threadWillStartLoadingMap.responder=NULL;
		m_threadDidFinishLoadingMap.responder=NULL;
		m_parent_pointer_lock.unlock();

		//clear fetcher queue
		clear();
		manager.removeThread(this);
		//resume blocked thread
		m_fetcher_semaphore->Signal();


		MGLogD("out  MKFetcherThread requestExit");
	}


	void MKFetcherThread::setParentView( MKMapView *_map_view , MKMapLayerView * _tile_layer_view )
	{
		AUTO_LOCK(&m_parent_pointer_lock);
		map_view = _map_view;
		tile_layer_view = _tile_layer_view;
	}
	
	class MKFetcherThreadManagerController: public MIViewController
	{
	public:
		virtual void clean(MGObject *object)
		{
			MKFetcherThread *thread=dynamic_cast<MKFetcherThread *>(object);
			thread->join();
			delete_and_set_null(thread);
		}

		virtual void freeMemory(MGObject *object)
		{
			MKFetcherThreadManager *manager=dynamic_cast<MKFetcherThreadManager *>(object);
			manager->freeActuallyMemory();
		}
	};
	static MKFetcherThreadManagerController clean_controller;

	int MKFetcherThread::readyToStop()
	{

		//delete the thread
		MIEvent *event=new MIEvent();
		event->_event_type = MAP_FETCHER_THREAD_READY_STOP_EVENT;
		event->_responder = &clean_controller;
		event->_content=NULL;

		SELHandler selector;
		selector.sender = this;
		selector.sel_func = (SELFunc)&MKFetcherThreadManagerController::clean;
		event->_handler = selector;

		GuiContainer::container->preserveEvent(event);
		return 0;
	}

	int MKFetcherThread::readyToRun()
	{
		manager.addThread(this);
		return MGThread::readyToRun();
	}

	void MKFetcherThread::setStartResponder( MIResponderSEL& threadWillStartLoadingMap )
	{
		AUTO_LOCK(&m_parent_pointer_lock);
		m_threadWillStartLoadingMap = threadWillStartLoadingMap;
	}

	void MKFetcherThread::setFinshResponder( MIResponderSEL& threadDidFinishLoadingMap )
	{
		AUTO_LOCK(&m_parent_pointer_lock);
		m_threadDidFinishLoadingMap = threadDidFinishLoadingMap;
	}

	void MKFetcherThread::clearCache()
	{
		AUTO_LOCK_THIS;
		manager.collectNTiles(1<<31);
		manager.freeMemory();
	}





	/************************************************************************/
	/*                           MKFetcherThreadManager                     */
	/************************************************************************/


	static const unsigned long LIMIT = 72;

	void MKFetcherThreadManager::collectNTiles( unsigned long n )
	{
		AUTO_LOCK_THIS;
		std::set<MKTileLayerOverlay*> ret=reuse_queue.collectNTiles(n);
		need_to_free.insert(ret.begin(),ret.end());
        freeMemory();
	}

	void MKFetcherThreadManager::checkReuseQueueLimited()
	{
		AUTO_LOCK_THIS;
		if (reuse_queue.size()>LIMIT)
		{
			collectNTiles(reuse_queue.size()-LIMIT);
		}
	}

	void MKFetcherThreadManager::freeMemory()
	{
		AUTO_LOCK_THIS;

		//delete the thread
		MIEvent *event=new MIEvent();
		event->_event_type = MAP_FETCHER_THREAD_READY_STOP_EVENT;
		event->_responder = &clean_controller;
		event->_content=NULL;

		SELHandler selector;
		selector.sender = this;
		selector.sel_func = (SELFunc)&MKFetcherThreadManagerController::freeMemory;
		event->_handler = selector;

		GuiContainer::container->preserveEvent(event);
	}

	void MKFetcherThreadManager::addThread( MKFetcherThread *thread )
	{
		AUTO_LOCK_THIS;
		m_thread_pool.insert(thread);
	}

	void MKFetcherThreadManager::removeThread( MKFetcherThread *thread )
	{
		AUTO_LOCK_THIS;
		MGLogD("============MKFetcherThreadManager::removeThread %p==================",thread);
		m_thread_pool.erase(thread);
		if (m_thread_pool.empty())
		{
			collectNTiles(1<<31);
			freeMemory();
			MGLogD("============MKFetcherThreadManager::removeThread freeMemory==================");
		}
		else
		{
			MGLogD("============MKFetcherThreadManager::removeThread not freeMemory==================");
		}
	}

	void MKFetcherThreadManager::enqueue( MKSateliteMapViewCell *cell )
	{
		AUTO_LOCK_THIS;
		reuse_queue.enqueue(cell);
	}

	void MKFetcherThreadManager::update( MKSateliteMapViewCell *cell )
	{
		AUTO_LOCK_THIS;
		reuse_queue.update(cell);
        need_to_free.erase(cell);
	}

	MKSateliteMapViewCell * MKFetcherThreadManager::contain( const MKGlobalTileIndex &identify )
	{
		AUTO_LOCK_THIS;
		return reuse_queue.contain(identify);
	}

	void MKFetcherThreadManager::freeActuallyMemory()
	{
		AUTO_LOCK_THIS;
		//not for this thread
		for (std::set<MKTileLayerOverlay *>::iterator it=need_to_free.begin();
			it!=need_to_free.end();it++)
		{
            //MGLogD("freeActuallyMemory %p",*it);
			delete (*it);
		}

		need_to_free.clear();
	}


}