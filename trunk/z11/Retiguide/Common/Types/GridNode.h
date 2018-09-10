#ifndef _GRID_NODE_H_
#define _GRID_NODE_H_

#pragma pack(push, 1)
namespace GIS_Embedded
{
	//class GridNode
	//{
	//public:
	//	unsigned char offset[3];
	//	unsigned char grid_level;
	//	unsigned short child_grid_ids[4];

	//};

	class GridNode4096
	{
	public:
		unsigned char offset[3];
		unsigned char grid_level;
		unsigned short child_grid_ids[4];
	};

	class GridNode65536
	{
	public:
		unsigned char offset[4];
		unsigned char grid_level;
		unsigned short child_grid_ids[4];
	};

}
#pragma pack(pop)

#endif