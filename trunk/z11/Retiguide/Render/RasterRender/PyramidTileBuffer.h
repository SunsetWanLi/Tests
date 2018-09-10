#ifndef _RASTERIMAGE_PYRAMID_TILE_BUFFER_H_
#define _RASTERIMAGE_PYRAMID_TILE_BUFFER_H_	
#pragma once

#include "./PyramidScaler.h"
#include "Retiguide/Common/crs/basic_gm_type.h"
#include "System/Shared/Syncable.h"
#include "GraphicCore/MGColor.h"
#include <string>
#include <fstream>

namespace z11
{
	class MGColor;
	class MGSemaphore;
}
namespace GIS_Embedded
{

//enum PYRAMID_IMAGE_TYPE;

struct Index_Tree_Node;  //it's only a decaleration,not definition
class TileLoader;
struct Floor;

enum BufferEntryState
{
	ENTRY_FREE		=	0,	
	ENTRY_RESERVED	=	1,
	ENTRY_USING		=	2,
	ENTRY_IDLE		=	3
};

enum BufferEntryPriority
{
	ENTRY_DROP_FIRST				=	0,
	ENTRY_NORMAL					=	1,
	ENTYR_IN_MEM_IF_POSSIBLE		=	3,
	ENTRY_ALWAYSE_IN_MEM			=	4
};

struct ImageBufferEntry
{
	z11::MGColor *		color;
	Index_Tree_Node *	tree_node;
	unsigned long		ready_bytes;
	unsigned long		buffer_size;
	BufferEntryState	state;
	BufferEntryPriority	priority;
	unsigned int		dirty_count;
	bool				ready_for_read;
	MapScale			image_width , image_height;
	unsigned int		next;
public:
	//ImageBufferEntry(const ImageBufferEntry& img_buffer_entry);	
	//ImageBufferEntry& operator = (const ImageBufferEntry& img_buffer_entry);

	ImageBufferEntry()
	{
		color			=	NULL;
		tree_node		=	NULL;
		ready_bytes		=	0;
		buffer_size		=	0;
		state			=	ENTRY_FREE;
		priority		=	ENTRY_NORMAL;
		dirty_count		=	0;
		ready_for_read	=	false;
		image_width		=	0;
		image_height	=	0;
		next			=	0;
	}
	~ImageBufferEntry()
	{
		delete [] color, color = NULL;
		tree_node		=	NULL;
		ready_bytes		=	0;
		buffer_size		=	0;
		state			=	ENTRY_FREE;
		priority		=	ENTRY_NORMAL;
		dirty_count		=	0;
		ready_for_read	=	false;
		image_width		=	0;
		image_height	=	0;
		next			=	0;
	}
//private:
//	void assign(const ImageBufferEntry& img_buffer_entry);
}; 


class ImageBuffer : public Syncable
{
protected:
	ImageBufferEntry *		m_entry;
	int						m_free_entry_count;
	long					m_free_buffer_size;
	std::string				m_buffer_name;
	z11::MGSemaphore		*m_semaphore_of_dirty_bit,
							*m_semaphore_of_buffer_size_using;
	const int				m_image_buffer_entry;
	TileLoader *			m_tile_loader;

	void initialize();

	void dirty_dec(ImageBufferEntry * entry); 
	void change_buffer_size_using(long delta); 
	ImageBufferEntry * reserve_a_free_entry(); 
	void drop(unsigned int index); 
	bool release_free_entry(); 
public:
	ImageBuffer(const std::string& buffer_name, const int image_buffer_entry, 
				enum RasterType raster_type, enum RepositoryType repo_type, const string& repo, std::vector<Floor> floor_info);
	virtual ~ImageBuffer(); 			

	void buffer_tile_image(const std::string& image_name, z11::uint_32 floor, z11::uint_32 row, z11::uint_32 col, Index_Tree_Node* tile, bool keeping = false);
	void setBufferPriority(ImageBufferEntry * buffer , BufferEntryPriority prio); 
	void dirty_inc(ImageBufferEntry * entry);
	void use_entry(ImageBufferEntry * entry);
	void release_entry(ImageBufferEntry * entry);
	std::string getBufferName();
	double getEntryFreeRate();
	double getBufferFreeRate();
	unsigned getFreeEntryCount();
}; 

}
#endif