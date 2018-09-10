#include "FeatureClassInfo.h"

namespace GIS_Embedded
{
	void SpatialIndexMultiLevel::calculateIndex( unsigned short grid_id, Scope &scope, vector<int> &ret_index_units )
	{

		Scope grid_scope;
		if(grid_nodes65536 == NULL)
		{
			GridNode4096 *grid_node = grid_nodes4096 + grid_id;
			calculateScope(grid_id, grid_scope);

			if(!scope.isIntersect(grid_scope))
				return;
			if(grid_node->child_grid_ids[0] == 0)	//is a leaf node
			{
				ret_index_units.push_back(grid_id);
				return;
			}
			for(int i = 0; i < 4; i ++)
			{
				calculateIndex(grid_node->child_grid_ids[i], scope, ret_index_units);
			}
		}
		else
		{
			GridNode65536 *grid_node = grid_nodes65536 + grid_id;
			calculateScope(grid_id, grid_scope);

			if(!scope.isIntersect(grid_scope))
				return;
			if(grid_node->child_grid_ids[0] == 0)	//is a leaf node
			{
				ret_index_units.push_back(grid_id);
				return;
			}
			for(int i = 0; i < 4; i ++)
			{
				calculateIndex(grid_node->child_grid_ids[i], scope, ret_index_units);
			}
		}
		return;
	}

	//void SpatialIndexMultiLevel::calculateScope(GridNode *grid_node, Scope &grid_scope)
	//{
	//	int left_offset = (grid_node->offset[0] << 4) | (grid_node->offset[1] >> 4);
	//	int bottom_offset = ((grid_node->offset[1] & 0xf) << 8) | grid_node->offset[2];
	//	grid_scope.m_bottom = (int)(bottom_offset * offset_length + _bottom);
	//	grid_scope.m_left = (int)(left_offset * offset_length + _left);
	//	//int length = side_length >> grid_node->grid_level;
	//	double length = side_length / (1 << grid_node->grid_level);
	//	grid_scope.m_top = (int)(bottom_offset * offset_length + _bottom + length);
	//	grid_scope.m_right = (int)(left_offset * offset_length + _left + length);
	//}

	void SpatialIndexMultiLevel::calculateScope4096(GridNode4096 *grid_node, Scope &grid_scope)
	{
		int left_offset = (grid_node->offset[0] << 4) | (grid_node->offset[1] >> 4);
		int bottom_offset = ((grid_node->offset[1] & 0xf) << 8) | grid_node->offset[2];
		grid_scope.m_bottom = (int)(bottom_offset * offset_length + _bottom);
		grid_scope.m_left = (int)(left_offset * offset_length + _left);
		//int length = side_length >> grid_node->grid_level;
		double length = side_length / (1 << grid_node->grid_level);
		grid_scope.m_top = (int)(bottom_offset * offset_length + _bottom + length);
		grid_scope.m_right = (int)(left_offset * offset_length + _left + length);
	}

	void SpatialIndexMultiLevel::calculateScope65536(GridNode65536 *grid_node, Scope &grid_scope)
	{
		int left_offset = (grid_node->offset[1] << 8) | grid_node->offset[0];
		int bottom_offset = (grid_node->offset[3] << 8) | grid_node->offset[2];
		grid_scope.m_bottom = (int)(bottom_offset * offset_length + _bottom);
		grid_scope.m_left = (int)(left_offset * offset_length + _left);
		//int length = side_length >> grid_node->grid_level;
		double length = side_length / (1 << grid_node->grid_level);
		grid_scope.m_top = (int)(bottom_offset * offset_length + _bottom + length);
		grid_scope.m_right = (int)(left_offset * offset_length + _left + length);
	}

	void SpatialIndexGrid::transformScopeToIndexUnits( Scope& screen_scope,vector<int>& ret_index_units )
	{
		int lt_unit_col = (int)((screen_scope.getLeft() - _left) / _width_per_unit);
		int lt_unit_row = (int)((_top - screen_scope.getTop()) / _height_per_unit);
		int rb_unit_col = (int)((screen_scope.getRight() - _left) / _width_per_unit);
		int rb_unit_row = (int)((_top - screen_scope.getBottom()) / _height_per_unit);
		if(lt_unit_col < 0)
			lt_unit_col = 0;
		if(lt_unit_col >= _cols)
			lt_unit_col = _cols - 1;
		if(lt_unit_row < 0)
			lt_unit_row = 0;
		if(lt_unit_row >= _rows)
			lt_unit_row = _rows - 1;
		if(rb_unit_col < 0)
			rb_unit_col = 0;
		if(rb_unit_col >= _cols)
			rb_unit_col = _cols - 1;
		if(rb_unit_row < 0)
			rb_unit_row = 0;
		if(rb_unit_row >= _rows)
			rb_unit_row = _rows - 1;
		for(int i = lt_unit_row; i <= rb_unit_row; i++)
		{
			for(int j = lt_unit_col; j <= rb_unit_col; j++)
				ret_index_units.push_back(i * _cols + j);

		}

	}

	void SpatialIndexGrid::calculateScope( int grid_id, Scope &grid_scope )
	{
		int lt_unit_row = grid_id / _cols;
		int lt_unit_col = grid_id % _cols;
		int rb_unit_row = grid_id / _cols;
		int rb_unit_col = grid_id % _cols;
		double left = _left + (double)lt_unit_col * _width_per_unit;
		double top = _top - (double)lt_unit_row * _height_per_unit;
		double right = _left + (double)(rb_unit_col + 1) * _width_per_unit;
		double bottom = _top - (double)(rb_unit_row + 1) * _height_per_unit;
		grid_scope = Scope(left,top,right,bottom);
	}
}