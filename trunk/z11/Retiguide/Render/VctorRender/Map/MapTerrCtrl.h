#ifndef _RENDER_MAPTERRCTRL_H
#define _RENDER_MAPTERRCTRL_H

#include "MapFrame.h"
#include "Retiguide/Common/Types/WorkSpaceTypes/TerrCtrl.h"
#include "Retiguide/Common/Types/Settings.h"
#include "CommonBase/MGWString.h"
#include <map>

using namespace z11;

namespace GIS_Embedded
{
	class MapTerrCtrl
	{
	public:
		MapTerrCtrl(Settings* &setConf,TerrCtrl &terr_ctrl);

		~MapTerrCtrl();

		void getDefaultScope(Scope *scope);

		void change(const Scope &scope,MapFrame *map_frame);

		void setSpecialSubject(const z11::MGWString& subject);

	private:
		TerrCtrl *m_terr_ctrl;
		Settings* m_setting_config;

		std::map<z11::MGWString, MapFrame*> m_map_frames;
		z11::MGWString mSpecialSubject;

		DataFetcher *m_data_fetcher;

	};

}
#endif