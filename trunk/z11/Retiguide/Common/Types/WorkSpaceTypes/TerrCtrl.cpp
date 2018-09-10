
#include "TerrCtrl.h"
#include <math.h>

namespace GIS_Embedded
{
	TerrLevel::TerrLevel()
	{
	
	}

	void TerrLevel::getTerrByIndex(int index,Terr **terr)
	{
		*terr = m_terrs[index];
	}
	
	int TerrLevel::getTerrSum()
	{
		return m_terrs.size();
	}
	
	void TerrLevel::addTerr(Terr &terr)
	{
		m_terrs.push_back(&terr);
	}

	TerrCtrl::TerrCtrl()
	{
	}

	void TerrCtrl::getWSUnderTerrCtrl(const Scope &scope, vector<z11::MGWString> *ws_names)
	{

		int i,j,sum;
		
		sum=m_terr_levels.size();

		double view_len= sqrt(scope.getArear());
		for (i=0;i<sum;i++)
		{
			TerrLevel *level = m_terr_levels[i];
			//if (view_len<level->m_max&&view_len>level->m_min)
			{
				for (j=0;j<level->getTerrSum();j++)
				{
					Terr *terr;
					level->getTerrByIndex(j,&terr);
					if (terr->m_ws_extension.isIntersect(scope)==true)
						ws_names->push_back(terr->m_ws_name);
				}
			}
			
		}

	}

	void TerrCtrl::getDefauleScope(GIS_Embedded::Scope *scope)
	{
		TerrLevel *level = m_terr_levels[0];
		Terr *terr;
		level->getTerrByIndex(0,&terr);
		
		scope->setScope(terr->m_ws_extension);

	}

	void TerrCtrl::initialScopeCenter(double conf_prj_x,double conf_prj_y,double* prj_x,double* prj_y)
	{
		int i,j,sum;
		
		sum=m_terr_levels.size();

		for (i=0;i<sum;i++)
		{
			TerrLevel *level = m_terr_levels[i];

			for (j=0;j<level->getTerrSum();j++)
			{
				Terr *terr;
				level->getTerrByIndex(j,&terr);
				if (terr->m_ws_extension.isContain(conf_prj_x,conf_prj_y))
				{
					*prj_x = conf_prj_x;
					*prj_y = conf_prj_y;
					return;
				}
			}
			
		}

		Terr *default_terr;
		m_terr_levels[0]->getTerrByIndex(0,&default_terr);
		*prj_x = (default_terr->m_ws_extension.getLeft() + default_terr->m_ws_extension.getRight())/2;
		*prj_y = (default_terr->m_ws_extension.getTop() + default_terr->m_ws_extension.getBottom())/2;

	}
	
}