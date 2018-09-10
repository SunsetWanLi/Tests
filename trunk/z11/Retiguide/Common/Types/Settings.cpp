#include "Settings.h"
#include "CommonBase/MGWString.h"
#include "System/Shared/Utility.h"
#include <cstring>
using namespace z11;
namespace GIS_Embedded
{
	int Settings::addHistoryPos(const z11::MGWString& label)
	{
		int size = 40;

		int last_char = label.find_last_of(WS(L"]"));

		int empty_index = 0,same_index = 0;
		while(z11::Utility::wcscmp(my_points.history[empty_index],L"") !=0 && empty_index<size)
			empty_index++;

		while(::wcsncmp(my_points.history[same_index],RWS(label.c_str()),last_char) !=0 && same_index<size)
			same_index++;

		if(same_index >=size)//no the same
		{
			if(empty_index>=size) //full record
			{
				::memcpy(my_points.history,my_points.history+1,360*(size-1));
				z11::Utility::WStringCpy(my_points.history[size-1],RWS(label.c_str()));
			}
			else
			{
				z11::Utility::WStringCpy(my_points.history[empty_index],RWS(label.c_str()));
			}
		}
		else //the same
		{
			::memcpy(my_points.history + same_index,my_points.history+same_index+1,360*(empty_index-same_index-1));
			z11::Utility::WStringCpy(my_points.history[empty_index-1],RWS(label.c_str()));
		}

		//for(int i=0;i<size;i++)
		//{
		//	if(::wcscmp(my_points.history[i],WS(L"")) != 0)
		//	{
		//		::OutputDebugString(my_points.history[i]);
		//		::OutputDebugString(L"\n");
		//	}
		//}
		//::OutputDebugString(L"\n\n");
		//int i=0;
		//while(::wcscmp(my_points.history[i],WS(L"")) !=0 && i<40)
		//	i++;

		//if(i>=40) //full record
		//{
		//	::memcpy(my_points.history,my_points.history+1,7800);
		//	::wcscpy(my_points.history[39],label.c_str());
		//}
		//else
		//{
		//	::wcscpy(my_points.history[i],label.c_str());
		//}

		return 0;
	}

	Settings::Settings()
	{
		gerneral.automatic_night_colors = false;
		gerneral.off_route_recalculation = AUTOMATIC;
		gerneral.safty_mode = false;
		map.host_pos_x = -1;
		map.host_pos_y = -1;
		map.alternative_road_names = false;
		map.show_point_labels = true;
		map.show_street_labels = true;
		map.auto_show_cross = true;
		map.textured_polygons = true;
		sound.master.master_open = true;
		sound.master.master_sacle = 0.8;
		sound.voice.voice_open = true;
		sound.voice.voice_sacle = 0.6;
		sound.keys.keys_open = false;
		sound.keys.keys_sacle = 0.0;
		sound.dynamic_volume = false;
		sound.tone = Double_Tone;
		route_parameters.Method = 0.5;
		route_parameters.route = SHORT;
		route_parameters.vehicle = CAR;
		route_parameters.unPavedRoad = false;
		route_parameters.highWays = true;
		route_parameters.ferries = false;
		route_parameters.uTurn = false;
		route_parameters.permitNeeded = true;
		route_parameters.tollRoad = true;
		language_units.program_language = TXT_ENGLISH_US;
		language_units.units = KILOMETERS_METERS;
		language_units.voice_language = VOC_ENGLISH_US;
		advance_settings.backlight_always_on = false;
		advance_settings.keep_position_on_road = true;
		advance_settings.zoom_in_after_find = true;
		advance_settings.keep_position_in_screen = false;

		for(int i=0;i<40;i++)
		{
			z11::Utility::WStringCpy(my_points.history[i],L"");
			z11::Utility::WStringCpy(my_points.point_str[i],L"");
		}
	}
}
