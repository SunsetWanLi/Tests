#ifndef _COMMON_TYPES_WORKSPACETYPES_TERRCONF_H
#define _COMMON_TYPES_WORKSPACETYPES_TERRCONF_H

#include "../../Share/Scope.h"
#include "CommonBase/MGWString.h"
#include <string>
#include <vector>
using namespace std;
using namespace z11;

namespace GIS_Embedded
{
	class Terr
	{
	public:

		z11::MGWString m_ws_name;
		Scope m_ws_extension;
	};

	class TerrLevel
	{

	public:
		
		void getTerrByIndex(int index,Terr **terr);
	
		int getTerrSum();
	
		void addTerr(Terr &terr);
	
		TerrLevel();

	public:
		double m_min;
		double m_max;
	
	private:
		vector<Terr*> m_terrs;
	
	};

	class TerrCtrl
	{
	public:
		TerrCtrl();

		void getWSUnderTerrCtrl(const Scope &scope,vector<z11::MGWString> *ws_names);
	
		void getDefauleScope(Scope *scope);

		void initialScopeCenter(double conf_prj_x,double conf_prj_y,double* prj_x,double* prj_y);

	public:
		vector<TerrLevel*> m_terr_levels;

	};


}

#endif