#include "./Pyramid.h"
#include "./PyramidTileIndex.h"
#include "./PyramidSettings.h"
#include "PyramidTileBuffer.h"
#include "ImagePyramidConfig.h"
#include "ImagePyramidFloorInfo.h"
#include "ImagePyramidUpload.h"
#include "ImagePyramid.h"
#include "System/Thread/MGSemaphore.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

using namespace z11;
namespace GIS_Embedded
{
int_32	Pyramid::m_thread_priority = THREAD_IDLE;

	using namespace std;
//	using namespace Buffer;
//	using namespace Index;
//	using namespace Constructor_BMP;

Pyramid::Pyramid(const TILE_PYRAMID& pyramid_info, ImageBuffer * buffer)
	:  m_buffer(buffer), m_ref_count(1)
{
	m_outter_buffer = true;
	m_exiting = false;
	m_can_exit = false;
	initialize(pyramid_info);
	
} // Pyramid constructor


Pyramid::~Pyramid()
{
	m_exiting = true;
	requestExit();
	while(!m_can_exit)
		MGSleep(5);
	delete m_index; m_index = NULL;
	delete [] m_floor_info; m_floor_info = NULL;

	if(!m_outter_buffer)
	{
		delete m_buffer;
		m_buffer = NULL;
	}

	semaphore_of_queue->Wait();
	for(unsigned int i = 0; i < PRIORITY_COUNT ; ++i)
	{
		while( ! m_requests[i].empty())
		{
			delete m_requests[i].front();
			m_requests[i].pop();
		}
	}
	semaphore_of_queue->Signal();
	delete semaphore_of_queue;
	semaphore_of_queue = NULL;

	delete semaphore_of_ref_cnt;
	semaphore_of_ref_cnt = NULL;
} // ~Pyramid()

int Pyramid::initialize(const TILE_PYRAMID& pyramid_info)
{
//	using namespace Index;
	int ret = -1;
	m_pyramid_info = pyramid_info;
	m_pyramid_info.samp_method = 0; // Not used in this edition.

	//ImagePyramidConfig* image_pyramid_config = new ImagePyramidConfig();;
	//ImagePyramidUpload::getImagePyramidConfig(m_map_name, image_pyramid_config);
	//ret = data_ex->getImagePyramidConfig(m_map_name, &image_pyramid_config);

	
	// read floors' information
	//Construct the Floors
	std::vector<Floor> floor_info;
	m_floor_info = new Floor[m_pyramid_info.level];
	MapScale last_level_width = m_pyramid_info.orig_width*2;
	MapScale last_level_height = m_pyramid_info.orig_height*2;
	for(int i = 0; i < m_pyramid_info.level; ++i)
	{
		m_floor_info[i].width = ((last_level_width + 1) >> 1);
		m_floor_info[i].height = ((last_level_height + 1) >> 1);
		last_level_width = m_floor_info[i].width;
		last_level_height = m_floor_info[i].height;
		// calculate other info of the floor
		m_floor_info[i].regular_per_row = m_floor_info[i].width / m_pyramid_info.tile_size;
		m_floor_info[i].non_regular_per_row = (0 < (m_floor_info[i].width % m_pyramid_info.tile_size) ? 1 : 0);
		m_floor_info[i].row_number = (m_floor_info[i].height / m_pyramid_info.tile_size) 
			+ ( (0 < m_floor_info[i].height % m_pyramid_info.tile_size) ?1:0) ;
		m_floor_info[i].resolution = m_pyramid_info.orig_resolution * max(ceil(1.0 * m_pyramid_info.orig_width / m_floor_info[i].width)
																, ceil(1.0 * m_pyramid_info.orig_height /m_floor_info[i].height));
		floor_info.push_back(m_floor_info[i]);
	}

	// Construct the buffer
	if(NULL == m_buffer)
	{
		const int image_buffer_entry = MAX_IMAGE_BUFFER_SIZE * 1.0 /( m_pyramid_info.tile_size * m_pyramid_info.tile_size * 3 ) ;
		m_buffer = new ImageBuffer(m_pyramid_info.raster_name + "_buffer", image_buffer_entry, m_pyramid_info.raster_type, m_pyramid_info.repo_type, m_pyramid_info.repo_name, floor_info);
		m_outter_buffer = false;
	}

	// Construct the index tree
	m_index = new IndexTree(m_floor_info,m_pyramid_info.region,m_pyramid_info.level,m_pyramid_info.tile_size);
	m_index->newBuffer(m_buffer);

	// Construct the semaphores
	semaphore_of_queue = new z11::MGSemaphore(1);
	semaphore_of_ref_cnt = new z11::MGSemaphore(1);

	run();	
	return 0;

} // initialize()

unsigned int  __cdecl Pyramid::deal_with_request(Pyramid * this_ptr)
{
	setThreadPriority(THREAD_IDLE);

	queue<Tile_Request *> * request_queue = this_ptr->m_requests;
	bool has_task = false;

	while(!this_ptr->m_exiting)
	{	
		has_task = false;
		for(unsigned int i = 0; i < PRIORITY_COUNT ; ++i)
		{
			if( ! request_queue[i].empty() )
			{	
				if( (i > SPECIAL) // Buffer is limited for 'low' priority requests
					&& 	(  (this_ptr->m_buffer->getEntryFreeRate() < BUFFER_ENTRY_RESERVE_RATE) 
					|| (this_ptr->m_buffer->getBufferFreeRate() < BUFFER_SIZE_RESERVE_RATE) 
					))	
				{
					break; 
				}

				this_ptr->semaphore_of_queue->Wait();
				Tile_Request *request = request_queue[i].front();
				request_queue[i].pop();
				this_ptr->semaphore_of_queue->Signal();

				bool do_load =false;
				request->tile->node_semaphore->Wait();
				if( NULL == request->tile->buffer )
				{	
					// not in memory now , load it in
					this_ptr->m_buffer->buffer_tile_image(request->raster_name, request->floor, request->row, request->col, request->tile, false);
					do_load = true;
				}
				else
				{
					request->tile->node_semaphore->Signal();
				}

				if(do_load)
					MGSleep(5);

				delete request;
				has_task = true;
			}
		} // for

		if(!has_task)
			MGSleep(100);
	} // while ! m_exiting

	this_ptr->m_can_exit = true;

	return 0;
} 

Tiles Pyramid::indexTiles(const Request & request)
{
	return m_index->getTiles(request.region,request.resolution);
}

Result * Pyramid::getMap(const Request & request)
{
	Result * result = new Result;
	reserveMap(request, SPECIAL);
	Tiles tiles = m_index->getTiles(request.region,request.resolution);

	result->block_count = static_cast<unsigned int>(tiles.size());
	result->blocks = new TileBlock[result->block_count];
	result->from = m_index;
	vector <unsigned int> not_ready_list;

	for(unsigned int i = 0; i < tiles.size(); ++i)
	{
		result->blocks[i].tile = tiles[i];
		result->blocks[i].ready = false;

		while (NULL == tiles[i]->buffer)
		{
			tiles[i]->node_semaphore->Wait();
			tiles[i]->node_semaphore->Signal();
		}
		//tiles[i]->node_semaphore->Wait();
		//if( NULL == tiles[i]->buffer )
		//{	
		//	// the requested tile is not in memory now , so load it in
		//	//Floor  current_floor = m_floor_info[tiles[i]->floor_number];

		//	unsigned int sleep_count = 0;
		//	while((tiles.size() - i) > m_buffer->getFreeEntryCount() * (1 - BUFFER_ERROR_RATE) )
		//	{
		//		MGSleep(10);
		//		if(++sleep_count >= MAX_SLEEP_COUNT)break ;
		//	}

		//	//m_buffer->buffer_tile_image(m_pyramid_root_path + L"/" + file_name,tiles[i],0,false,true);
		//	m_buffer->buffer_tile_image(m_pyramid_info.raster_name, tiles[i]->floor_number, tiles[i]->block_row_number, tiles[i]->block_col_number, tiles[i], true);
		//	m_buffer->use_entry(tiles[i]->buffer);
		//}
		//else
		//{	
		//	// Already in memory , just use it.
		//	m_buffer->use_entry(tiles[i]->buffer);
		//	tiles[i]->node_semaphore->Signal();
		//}
		m_index->refer(tiles[i]);
		result->blocks[i].width = tiles[i]->buffer->image_width;
		result->blocks[i].height = tiles[i]->buffer->image_height;
		result->blocks[i].color = (tiles[i]->buffer->color);
		result->blocks[i].ready = tiles[i]->buffer->ready_for_read;
		if(/*(true == single_thread) && */(false == result->blocks[i].ready) )
			not_ready_list.push_back(i);

	} // for

	while(!not_ready_list.empty())
	{
		MGSleep(10);
		for(int i = 0; i < static_cast<int>(not_ready_list.size()) ;i++)
		{
			unsigned int index = not_ready_list[i];
			ImageBufferEntry * ff = tiles[index]->buffer;
			bool dfd = tiles[index]->buffer->ready_for_read;
			if(false == (result->blocks[index].ready = tiles[index]->buffer->ready_for_read) ) // it is an assignment
			{
				result->blocks[index].width = tiles[index]->buffer->image_width;
				result->blocks[index].height = tiles[index]->buffer->image_height;
				not_ready_list.erase(not_ready_list.begin() + i);
				break;
			}
		}
	}
	return result;
}


Result * Pyramid::getTile(z11::uint_32 floor_number, z11::uint_32 row_number, z11::uint_32 col_number)
{
	Result * result = new Result;
	result->block_count = 1;
	result->from = m_index;
	result->blocks = new TileBlock[result->block_count]; 

	unsigned int sleep_count = 0;
	Index_Tree_Node * tile = m_index->getTile(floor_number,row_number,col_number);

	result->blocks[0].tile = tile;
	result->blocks[0].ready = false;

	tile->node_semaphore->Wait();
	if(NULL == tile->buffer)
	{
		// Not in memory, load it.
		while( 1 > m_buffer->getFreeEntryCount() * (1 - BUFFER_ERROR_RATE) )
		{
			MGSleep(10);
			if(++sleep_count >= MAX_SLEEP_COUNT)break ;
		}

		m_buffer->buffer_tile_image(m_pyramid_info.raster_name, floor_number,row_number,col_number, tile, true);
		m_buffer->use_entry(tile->buffer);
	}
	else
	{	
		// Already in memory , just use it.
		m_buffer->use_entry(tile->buffer);
		tile->node_semaphore->Signal();
	}

	m_index->refer(tile);
	while(false == (result->blocks[0].ready = tile->buffer->ready_for_read) ) // It is an assignment.
		MGSleep(10);

	result->blocks[0].width = tile->buffer->image_width;
	result->blocks[0].height = tile->buffer->image_height;
	result->blocks[0].color = (tile->buffer->color); 

	return result;
}


void Pyramid::reserveMap(const Request & request, Priority priority)
{
	Tiles tiles = m_index->getTiles(request.region,request.resolution);

	for(unsigned int i = 0 ; i < tiles.size() ; ++i)
	{
		Tile_Request * tile_req = new Tile_Request;

		tile_req->tile = m_index->getTile(tiles[i]->floor_number, tiles[i]->block_row_number, tiles[i]->block_col_number);					
		tile_req->raster_name = m_pyramid_info.raster_name;
		tile_req->floor = tiles[i]->floor_number;
		tile_req->row = tiles[i]->block_row_number;
		tile_req->col = tiles[i]->block_col_number;
		tile_req->prio = getThreadPriority();

		semaphore_of_queue->Wait();
		m_requests[priority].push(tile_req);
		semaphore_of_queue->Signal();
	} // for
}

void Pyramid::reserveTile(z11::uint_32 floor_number, z11::uint_32 row_number, z11::uint_32 col_number, Priority priority)
{
	Tile_Request * tile_req = new Tile_Request;

	tile_req->tile = m_index->getTile(floor_number,row_number,col_number);
	tile_req->raster_name = m_pyramid_info.raster_name;
	tile_req->floor = floor_number;
	tile_req->row = row_number;
	tile_req->col = col_number;
	tile_req->prio = getThreadPriority();

	semaphore_of_queue->Wait();
	m_requests[priority].push(tile_req);
	semaphore_of_queue->Signal();
}

void Pyramid::refer()
{
	semaphore_of_ref_cnt->Wait();
	m_ref_count ++;
	if(m_ref_count < 0) m_ref_count --;
	semaphore_of_ref_cnt->Signal();
} // refer()

int Pyramid::leave()
{
	semaphore_of_ref_cnt->Wait();
	m_ref_count --;
	if(m_ref_count < 0 )m_ref_count = 0;
	semaphore_of_ref_cnt->Signal();
	return m_ref_count;
} // leave()

int Pyramid::getImageWidth( )const
{
	return m_pyramid_info.orig_width;
}
int Pyramid::getImageHeight()const
{
	return m_pyramid_info.orig_height;
}

void Pyramid::setThreadPriority(int_32 priority)
{
	m_thread_priority = priority;
}
int_32 Pyramid::getThreadPriority()
{
	return m_thread_priority;
}

bool Pyramid::threadLoop()
{
	deal_with_request(this);
	return true;
}

} // end of namespace GIS_Embedded
