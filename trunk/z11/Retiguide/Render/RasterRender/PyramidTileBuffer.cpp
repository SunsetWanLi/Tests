#include "PyramidTileBuffer.h"
#include "PyramidTileIndex.h"
#include "PyramidSettings.h"
#include "TileLoader.h"
#include "System/Thread/MGSemaphore.h"

using namespace z11;
namespace GIS_Embedded
{

//using namespace PyramidSpace;
///////////   ImageBuffer //////////
void ImageBuffer::initialize()
{	
	m_free_entry_count = m_image_buffer_entry - 1;
	m_free_buffer_size = MAX_IMAGE_BUFFER_SIZE;
	m_entry = new ImageBufferEntry[m_image_buffer_entry];
	for(unsigned int i = 0; i < m_image_buffer_entry; ++i)
	{
		m_entry[i].next = i + 1;
	}
	m_entry[m_image_buffer_entry - 1].next = 0;
	m_semaphore_of_dirty_bit = new z11::MGSemaphore(1);
	m_semaphore_of_buffer_size_using = new z11::MGSemaphore(1);
} 


ImageBuffer::ImageBuffer(const string& buffer_name, const int image_buffer_entry, enum RasterType raster_type,
						enum RepositoryType repo_type, const string& repo, std::vector<Floor> floor_info)
	:m_image_buffer_entry(image_buffer_entry), m_buffer_name(buffer_name)
{
	initialize();
	//m_tile_loader = TileLoaderFactory::crtTileLoader(raster_type, repo_type, repo);
	m_tile_loader = new TileLoader(raster_type, repo_type, repo, floor_info);
} 


ImageBuffer::~ImageBuffer()
{
	delete [] m_entry;
	m_entry = NULL;
	m_free_entry_count = 0;
	m_free_buffer_size = MAX_IMAGE_BUFFER_SIZE;
	delete m_semaphore_of_dirty_bit;
	delete m_semaphore_of_buffer_size_using;
	m_semaphore_of_dirty_bit = m_semaphore_of_buffer_size_using = NULL;
	return ;
}

void ImageBuffer::dirty_inc(ImageBufferEntry * entry)
{
	m_semaphore_of_dirty_bit->Wait();
	entry->dirty_count ++;
	if(entry->dirty_count <= 0)entry->dirty_count --;
	m_semaphore_of_dirty_bit->Signal();
} 

void ImageBuffer::dirty_dec(ImageBufferEntry * entry)
{
	m_semaphore_of_dirty_bit->Wait();
	if(entry->dirty_count > 0)
		entry->dirty_count --;
	m_semaphore_of_dirty_bit->Signal();
} 

void ImageBuffer::buffer_tile_image(const std::string& image_name, z11::uint_32 floor, z11::uint_32 row, z11::uint_32 col, Index_Tree_Node* tile, bool keeping)
{			
	ImageBufferEntry * free_buffer = reserve_a_free_entry();
	tile->buffer = free_buffer;
	free_buffer->tree_node = tile;
	//tile->node_semaphore->Signal();
	free_buffer->state = ENTRY_USING;
	free_buffer->ready_bytes = 0;

	m_tile_loader->loadTile(image_name, floor, row, col, free_buffer);
	if(! keeping)
		free_buffer->state = ENTRY_IDLE;

	tile->node_semaphore->Signal();
	return ;
}

void ImageBuffer::change_buffer_size_using(long delta)
{
	m_semaphore_of_buffer_size_using->Wait();
	m_free_buffer_size -= delta;
	m_semaphore_of_buffer_size_using->Signal();
} 

ImageBufferEntry * ImageBuffer::reserve_a_free_entry()
{
	lock();
	long old_free_buffer = m_free_buffer_size;
	if( (m_free_entry_count <= 0) )
	{ // no free buffer entry , release some.
		if(false == release_free_entry())
		{
			unlock();
			return NULL;
			//throw no_enough_memory(L"@ ImageBuffer::reserve_a_free_entry : no entry");
		}
	}
	if(true == CRITICAL_BUFFER_SIZE)
	{
		if((old_free_buffer < 0) && (m_free_buffer_size < old_free_buffer))
		{
			if(false == release_free_entry())
			{
				unlock();
				return NULL;
				//throw no_enough_memory(L"@ ImageBuffer::reserve_a_free_entry : no_memory");
			}
		}
	}
	unsigned int result = m_entry[0].next;
	m_entry[0].next = m_entry[result].next;
	m_entry[result].next = result;
	m_entry[result].state = ENTRY_RESERVED;
	m_entry[result].priority = ENTRY_NORMAL;
	m_entry[result].ready_for_read = false;
	m_entry[result].dirty_count = 0;
	m_entry[result].tree_node = NULL;
	m_free_entry_count --;
	unlock();
	if(0 == result)
		//throw no_enough_memory(L"@ ImageBuffer::reserve_a_free_entry : using entry[0]");
		return NULL;
	return &m_entry[result];
} 

void ImageBuffer::drop(unsigned int index)
{
	if(m_entry[index].state ==	ENTRY_FREE)
		return ;
	m_entry[index].tree_node->buffer = NULL;
	m_entry[index].next = m_entry[0].next;
	m_entry[0].next = index;
	m_entry[index].state = ENTRY_FREE;
	m_entry[index].ready_bytes = 0;
	m_entry[index].ready_for_read = false;
	m_semaphore_of_dirty_bit->Wait();
	m_entry[index].dirty_count = 0;
	m_semaphore_of_dirty_bit->Signal();
	m_free_entry_count ++;
} 

bool ImageBuffer::release_free_entry()
{
	unsigned int released_count = 0;
	// Release entris whose priority is 'ENTRY_DROP_FIRST'	
	for(unsigned int i = 1;	i < m_image_buffer_entry; ++i)
	{
		if( (ENTRY_FREE == m_entry[i].state) || (ENTRY_RESERVED == m_entry[i].state) )
			continue;
		m_entry[i].tree_node->node_semaphore->Wait();		
		if( (ENTRY_DROP_FIRST == m_entry[i].priority)
			&& (ENTRY_IDLE == m_entry[i].state))
		{
			drop(i);
			released_count++;
		}
		m_entry[i++].tree_node->node_semaphore->Signal();
	}
	if(released_count > 0)
	{
		return true;
	}
	// Release the Least Frequently Used.
	unsigned int can_not_free = 0;
	static unsigned index = 1;
	do
	{
		can_not_free = 0;
		for(unsigned count = 0; count < m_image_buffer_entry ; ++count)
		{
			can_not_free++;
			if((ENTRY_FREE == m_entry[index].state) || (ENTRY_RESERVED == m_entry[index].state))
				continue;
			m_entry[index].tree_node->node_semaphore->Wait();
			dirty_dec(&m_entry[index]);
			if((ENTRY_NORMAL == m_entry[index].priority) && (ENTRY_IDLE == m_entry[index].state))
			{
				can_not_free--;
				if(0 == m_entry[index].dirty_count)
				{
					drop(index);
					released_count++;
					if(this->getEntryFreeRate() > BUFFER_ENTRY_RELEASE_RATE)
					{
						m_entry[index].tree_node->node_semaphore->Signal();
						break;
					}
				}// if dirty_count
			}
			m_entry[index].tree_node->node_semaphore->Signal();
			index = index % (m_image_buffer_entry - 1) + 1;
		}
		
	}while( (0 == released_count) && (can_not_free < m_image_buffer_entry - 1) );
	if(released_count > 0 )
		return true;
	else
		return false;
} 

void ImageBuffer::setBufferPriority(ImageBufferEntry * buffer, BufferEntryPriority prio)
{
	lock();
	buffer->priority = prio;
	unlock();
}

string ImageBuffer::getBufferName()
{
	return m_buffer_name;
} 

void ImageBuffer::use_entry(ImageBufferEntry * entry)
{
	//lock();
	entry->state = ENTRY_USING;
	//unlock();
} 

void ImageBuffer::release_entry(ImageBufferEntry * entry)
{
	////lock();
	entry->state = ENTRY_IDLE;
	////unlock();
} 

double ImageBuffer::getEntryFreeRate()
{
	return static_cast<double>(m_free_entry_count) / (m_image_buffer_entry - 1);
} 

double ImageBuffer::getBufferFreeRate()
{
	return static_cast<double>(m_free_buffer_size) / MAX_IMAGE_BUFFER_SIZE;
} 

unsigned int ImageBuffer::getFreeEntryCount()
{
	return m_free_entry_count;
}

//ImageBufferEntry::ImageBufferEntry(const ImageBufferEntry& img_buffer_entry)
//{
//	if(this == &img_buffer_entry)
//		return;
//	assign(img_buffer_entry);
//	//delete image, image = 0;			
//	image = new MGColor[buffer_size];
//	memcpy(image, img_buffer_entry.image, sizeof(MGColor)*buffer_size);		
//}

//ImageBufferEntry& ImageBufferEntry::operator = (const ImageBufferEntry& img_buffer_entry)
//{
//	if(this == &img_buffer_entry)
//		return *this;
//	assign(img_buffer_entry);
//	delete image, image = 0;			
//	image = new MGColor[buffer_size];
//	memcpy(image, img_buffer_entry.image, sizeof(MGColor)*buffer_size);
//	return *this;
//}

//void ImageBufferEntry::assign(const ImageBufferEntry& img_buffer_entry)
//{
//	tree_node = 0;	//not copy	
//	ready_bytes = img_buffer_entry.ready_bytes;
//	buffer_size = img_buffer_entry.buffer_size;
//	//delete image, image = 0;			
//	//image = new MGColor[buffer_size];
//	//memcpy(image, img_buffer_entry.image, sizeof(MGColor)*buffer_size);
//	ready_bytes = img_buffer_entry.ready_bytes;			
//	state= img_buffer_entry.state;
//	priority = img_buffer_entry.priority;
//	dirty_count = img_buffer_entry.dirty_count;
//	ready_for_read = img_buffer_entry.ready_for_read;
//	image_width = img_buffer_entry.image_width;
//	image_height = img_buffer_entry.image_height;
//	next = img_buffer_entry.next;				
//}


}