#ifndef _COMMON_TYPES_FEATURECLASSINFO_H
#define _COMMON_TYPES_FEATURECLASSINFO_H

#include "../Share/Scope.h"
#include "GridNode.h"

#include <vector>

using namespace std;

namespace GIS_Embedded
{

	class SpatialIndex
	{
	public:
		virtual void transformScopeToIndexUnits(Scope& screen_scope,vector<int>& ret_index_units) = 0;
		virtual void calculateScope(int grid_id, Scope &grid_scope) = 0;
	};

	class SpatialIndexMultiLevel: public SpatialIndex
	{
	public:
		SpatialIndexMultiLevel()
		{
			_left = _right = _bottom = _top = side_length = offset_length = 0;
			all_nodes_count = non_leaf_nodes_count = 0;
			grid_nodes4096 = NULL;
			grid_nodes65536 = NULL;
		}
		double _left;
		double _right;
		double _bottom;
		double _top;

		double side_length;
		double offset_length;	// = side_length / 4096 or side_length / 65536;
		unsigned short all_nodes_count;
		unsigned short non_leaf_nodes_count;
		GridNode4096 *grid_nodes4096;
		GridNode65536 *grid_nodes65536;

		void transformScopeToIndexUnits(Scope& screen_scope,vector<int>& ret_index_units)
		{
			calculateIndex(0, screen_scope, ret_index_units);
		}

		void calculateScope(int grid_id, Scope &grid_scope)
		{
			if(grid_nodes65536 == NULL)
			{
				GridNode4096 *grid_node = grid_nodes4096 + grid_id;
				calculateScope4096(grid_node, grid_scope);
			}
			else
			{
				GridNode65536 *grid_node = grid_nodes65536 + grid_id;
				calculateScope65536(grid_node, grid_scope);
			}
		}
	private:

		void calculateIndex(unsigned short grid_id, Scope &scope, vector<int> &ret_index_units);
		void calculateScope4096(GridNode4096 *grid_node, Scope &grid_scope);
		void calculateScope65536(GridNode65536 *grid_node, Scope &grid_scope);
	};

	class SpatialIndexGrid: public SpatialIndex
	{
	public:
		SpatialIndexGrid(double left,double top,double right,double bottom,int row,int col)
		{
			_left = left;
			_top = top;
			_right = right;
			_bottom = bottom;
			_rows = row;
			_cols = col;

			_width_per_unit = (right-left)/col;
			_height_per_unit = (top-bottom)/row;
		}

		SpatialIndexGrid()
		{
		}

	public:
		double _left;
		double _right;
		double _bottom;
		double _top;

		int _rows;
		int _cols;

		double _width_per_unit;
		double _height_per_unit;

		void transformScopeToIndexUnits(Scope& screen_scope,vector<int>& ret_index_units);
		void calculateScope(int grid_id, Scope &grid_scope);

	};

	enum FeatureClassType
	{
		FILE_TYPE,
		IMAGE_TYPE
	};

	class FeatureClassInfo
	{
	public:
		FeatureClassType type;
		int global_index;
		Scope* extend;
		SpatialIndex *spatial_index;
	};
}

#endif