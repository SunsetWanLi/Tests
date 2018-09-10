#include "FclsInfoAdapter.h"
#include "../Common/Share/Scope.h"
#include "DataReader.h"
#include "MapKit/Embedded/Tools/SpatialDataBase.h"
#include <assert.h>

namespace GIS_Embedded
{

	int FclsInfoAdapter::translate(byte_8 *data, int size, int grid_side_divider, FeatureClassInfo **fcls_info)
	{
		FeatureClassInfo *ret = new FeatureClassInfo();
		//if(size == 0)
		//{
		//	ret->extend = new Scope();
		//	ret->extend->m_bottom = ret->extend->m_left = ret->extend->m_right = ret->extend->m_top = INT_MAX;
		//	ret->global_index = 0;
		//	ret->spatial_index = NULL;
		//	ret->type = IMAGE_TYPE;
		//	*fcls_info = ret;
		//	return 0;
		//}
		if(size == 0)
		{
			*fcls_info = NULL;
			return 0;
		}
		DataReader read;
		read.setStartPointer(data);

		if(size == 4 * sizeof(double) + 2 * sizeof(int))	//tmp
		{
			SpatialIndexGrid *spatial_index = new SpatialIndexGrid();
			ret->spatial_index = spatial_index;
			ret->type = IMAGE_TYPE;

			spatial_index->_left = read.getDouble();
			spatial_index->_top = read.getDouble();
			spatial_index->_right = read.getDouble();
			spatial_index->_bottom = read.getDouble();
			spatial_index->_rows = read.getInt();
			spatial_index->_cols = read.getInt();

			spatial_index->_width_per_unit = (spatial_index->_right - spatial_index->_left) / (double)spatial_index->_cols;
			spatial_index->_height_per_unit = (spatial_index->_top - spatial_index->_bottom)/ (double)spatial_index->_rows;

			ret->extend = new Scope(spatial_index->_left,spatial_index->_top,spatial_index->_right,spatial_index->_bottom);
		}
		else
		{
			SpatialIndexMultiLevel *spatial_index = new SpatialIndexMultiLevel();
			ret->spatial_index = spatial_index;
			ret->type = FILE_TYPE;

			spatial_index->_left = read.getDouble();
			spatial_index->_bottom = read.getDouble();

			double side_length = read.getDouble();
			spatial_index->side_length = side_length;
			if(grid_side_divider == 0)
				grid_side_divider = PUBLISH_PAGE_SIZE;
			spatial_index->offset_length = side_length / grid_side_divider/*4096*/;

			spatial_index->_right = spatial_index->_left + spatial_index->side_length;
			spatial_index->_top = spatial_index->_bottom + spatial_index->side_length;

			Scope *extend = ret->extend = new Scope();
			extend->m_bottom = spatial_index->_bottom;
			extend->m_left = spatial_index->_left;
			extend->m_right = spatial_index->_right;
			extend->m_top = spatial_index->_top;

			spatial_index->all_nodes_count = read.getShort();
			spatial_index->non_leaf_nodes_count = read.getShort();

			if(grid_side_divider == 4096)
			{
				int node_data_size = spatial_index->all_nodes_count * sizeof(GridNode4096);
				//assert(size - 3 * sizeof(double) - 2 * sizeof(short) == node_data_size);
				spatial_index->grid_nodes4096 = new GridNode4096[spatial_index->all_nodes_count];
				memcpy((void *)spatial_index->grid_nodes4096, (void *)read.getStartPointer(), node_data_size);
			}
			else if(grid_side_divider == 65536)
			{
				int node_data_size = spatial_index->all_nodes_count * sizeof(GridNode65536);
				//assert(size - 3 * sizeof(double) - 2 * sizeof(short) == node_data_size);
				spatial_index->grid_nodes65536 = new GridNode65536[spatial_index->all_nodes_count];
				memcpy((void *)spatial_index->grid_nodes65536, (void *)read.getStartPointer(), node_data_size);
				int a = 1;
			}

		}

		*fcls_info = ret;
		return 0;

		//FeatureClassInfo *ret = new FeatureClassInfo();

		//DataReader read;
		//read.setStartPointer(data);

		//Scope* scope=new Scope();

		//scope->setLeft(read.getDouble());
		//scope->setTop(read.getDouble());
		//scope->setRight(read.getDouble());
		//scope->setBottom(read.getDouble());

		//int rows = read.getInt();
		//int cols = read.getInt();

		//int nwLayer = 0;
		//if (size > 42)
		//	nwLayer = read.getInt();

		//ret->NWLAYER = false;
		//if (nwLayer ==1)
		//	ret->NWLAYER = true;

		//ret->type = type;
		//ret->extend = scope;
		//ret->ROWS = rows;
		//ret->COLS = cols;

		//*fcls_info = ret;

		return 0;
	}


}