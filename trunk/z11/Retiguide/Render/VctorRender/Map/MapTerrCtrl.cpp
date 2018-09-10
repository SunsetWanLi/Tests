#include "MapTerrCtrl.h"
#include "GraphicCore/MGColor.h"
#include "CommonBase/MGWString.h"

using namespace z11;

namespace GIS_Embedded
{
	MapTerrCtrl::MapTerrCtrl(Settings* &setConf,TerrCtrl &terr_ctrl)
	{
		m_setting_config = setConf;
		m_terr_ctrl = &terr_ctrl;

		m_data_fetcher = DataFetcher::getInstance();
	}

	MapTerrCtrl::~MapTerrCtrl()
	{
		for(std::map<z11::MGWString,MapFrame*>::iterator i= m_map_frames.begin(); i != m_map_frames.end(); i++)
		{
			delete (*i).second;
		}
	}

	void MapTerrCtrl::getDefaultScope(Scope *scope)
	{
		m_terr_ctrl->getDefauleScope(scope);
	}

	void MapTerrCtrl::change(const Scope &scope,MapFrame *ret_map_frame)
	{
 		vector<z11::MGWString> ws_names;
 
 		if(mSpecialSubject.compare(WS(L"")) ==0)
 			m_terr_ctrl->getWSUnderTerrCtrl(scope,&ws_names);
 		else
 			ws_names.push_back(mSpecialSubject);
 
 		int i,sum=ws_names.size();
 		
 		ret_map_frame->clear();
 
 		for (i=0;i<sum;i++)
 		{
 			z11::MGWString &ws_name= ws_names[i];
 
//  			if(m_setting_config->gerneral.automatic_night_colors)
//  			{
//  				SYSTEMTIME time;
//  				::GetLocalTime(&time);
//  
//  				if(time.wHour <= 18 && time.wHour > 6)
//  					ws_name += L"_day";
//  				else
//  					ws_name += L"_night";
//  			}
//  			else
 				ws_name += WS(L"_day");
 
 		    if(m_map_frames.find(ws_name) == m_map_frames.end())
 		    {
 				WorkSpace work_space;
 				m_data_fetcher->getWorkSpaceConf(ws_name,&work_space);
 				ret_map_frame->m_bg_color = z11::RGBAColorRef::fromRGB(work_space.m_bg_color);
 				ret_map_frame->m_cross_bg_color = z11::RGBAColorRef::fromRGB(work_space.m_cross_type.m_bg_color);
 				ret_map_frame->m_arrow_width = work_space.m_cross_type.m_arrow_width;
 				ret_map_frame->m_arrow_color = z11::RGBAColorRef::fromRGB(work_space.m_cross_type.m_arrow_color);
 				ret_map_frame->m_cross_bg_color = z11::RGBAColorRef::fromRGB(work_space.m_cross_type.m_bg_color);
 
 				MapFrame *map_frame = new MapFrame();
 				map_frame->initialMapFrame(work_space);
 				m_map_frames[ws_name] = map_frame;
 
 				ret_map_frame->bindMapFrame(*map_frame);
 
 		    }
 		    else
 		    {
 				MapFrame *map_frame = m_map_frames[ws_name];
 		    	ret_map_frame->bindMapFrame(*map_frame);
 		    }
 		    	
 		 }

	}

	void MapTerrCtrl::setSpecialSubject(const z11::MGWString& subject)
	{
		mSpecialSubject = subject;
	}

}