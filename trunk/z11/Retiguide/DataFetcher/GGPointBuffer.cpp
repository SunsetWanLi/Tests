#include "GGPointBuffer.h"
namespace GIS_Embedded{

	int GGPointBuffer::getBlock(z11::MGWString &type_key,GGPointBlock** block)
	{
		if (m_hash_blocks.find(type_key) != m_hash_blocks.end())	// 判断是否已经读过
		{
			*block = m_hash_blocks[type_key];
			(*block)->chance_left = 1;
			(*block)->ref_count ++;
			return 0;
		}
		return -1;
	}
	int GGPointBuffer::getGGPoint(z11::MGWString &name, GuiderGeoPoint** p)
	{
		map<z11::MGWString,GuiderGeoPoint*>::iterator it;
		it = m_hash_points.find(name);
		if (it != m_hash_points.end())
		{
			*p = it->second;
			return 0;
		}
		return -1;
	}
	int GGPointBuffer::saveRam(z11::MGWString &key, std::vector<GuiderGeoPoint*> *gg_points,int size)
	{
		GGPointBlock *new_block = new GGPointBlock;
		new_block->ggp_objs = gg_points;
		new_block->size = size;
		new_block->chance_left = 0;
		new_block->ref_count = 0;
		//curr_used_space += size;
		m_hash_blocks[key] = new_block;	

		GuiderGeoPoint* temp;

		for (unsigned int i = 0; i < gg_points->size(); i++)
		{	
			temp = (*gg_points)[i];
			temp->setKey(key); 
			m_hash_points[temp->getName()]=temp;
		}
		return 0;
	}
}