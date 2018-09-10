#include "TerrCtrlAdapter.h"

namespace GIS_Embedded
{

	DataReader * TerrCtrlAdapter::read;

	int TerrCtrlAdapter::translate(unsigned char *data, int size,TerrCtrl **terrCtrl)
	{
		read = new DataReader();
		read->setStartPointer(data);

		TerrCtrl *ret = new TerrCtrl();

		int i,sum;
	    sum= read->getInt();
	    for (i=0;i<sum;i++)
	    {
			TerrLevel *level;
	    	getTerrLevel(*read,&level);
			ret->m_terr_levels.push_back(level);
	    }
 
		*terrCtrl = ret;

		return 0;

	}

	void TerrCtrlAdapter::getTerrLevel(DataReader &read,TerrLevel **terr_level)
	{

		int i,sum;
		TerrLevel *ret = new TerrLevel();
		ret->m_max=read.getDouble();
		ret->m_min=read.getDouble();

		sum=read.getInt();
		for (i=0;i<sum;i++)
		{
			Terr *terr;
			getTerr(read,&terr);
			ret->addTerr(*terr);
		}

		*terr_level = ret;
	}

	void TerrCtrlAdapter::getTerr(DataReader &read,Terr **terr)
	{
		Terr *ret = new Terr();

		ret->m_ws_name = read.getString();
        ret->m_ws_extension.m_left = read.getDouble();
 		ret->m_ws_extension.m_top = read.getDouble();
		ret->m_ws_extension.m_right = read.getDouble();
		ret->m_ws_extension.m_bottom = read.getDouble();

		*terr = ret;

	}


}