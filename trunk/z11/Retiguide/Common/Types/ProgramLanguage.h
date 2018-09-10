#ifndef _Z_APPNOMFC_PROGRAMLANGUAGE
#define _Z_APPNOMFC_PROGRAMLANGUAGE

#include "../../Common/Types/Settings.h"
#include <vector>
#include "CommonBase/MGWString.h"

using namespace std;
using namespace z11;


namespace GIS_Embedded
{
	struct PROGRAM_LANGUAGE
	{
		struct setting 
		{
			z11::MGWString TITLE;
			z11::MGWString GENERAL;
			z11::MGWString MAP;
			z11::MGWString SOUND;
			z11::MGWString ROUTE_PARAMTERS;
			z11::MGWString LANGUAGE_UNITS;
			z11::MGWString ADVANCED;
		};

		struct setting_general
		{
			z11::MGWString TITLE;
			z11::MGWString SAFETY_MODE;
			z11::MGWString SET_FAVORITE_DESTINATIONS;
			z11::MGWString AUTOMATIC_NIGHT_COLOR;
			z11::MGWString ALERTS;
			z11::MGWString OFF_ROUTE_RECAL_TEXT;
			vector<z11::MGWString> OFF_ROUTE_RECAL_SWITCH;
		};

		struct setting_map
		{
			z11::MGWString TITLE;
			z11::MGWString ALTERNATIVE_ROAD_NAME;
			z11::MGWString SHOW_POINT_LABEL;
			z11::MGWString SHOW_STREET_LABEL;
			z11::MGWString AUTO_SHOW_CROSS;
			z11::MGWString TEXTURED_ROLYGONS;
		};

		struct setting_sound
		{
			z11::MGWString TITLE;
			z11::MGWString MASTER;
			z11::MGWString VOICE;
			z11::MGWString KEYS;
			z11::MGWString DYNAMIC_VOLUME;
			z11::MGWString TONE_TEXT;
			vector<z11::MGWString> TONE_SWITCH;
		};

		struct setting_route
		{
			z11::MGWString TITLE;
			z11::MGWString METHOD;
			z11::MGWString METHOD_FAST;
			z11::MGWString METHOD_OPTIMAL;
			z11::MGWString ROUTE_TEXT;
			vector<z11::MGWString> ROUTE_SWITCH;
			z11::MGWString VEHICLE_TEXT;
			vector<z11::MGWString> VEHICLE_SWITCH;
			z11::MGWString UNPAVED_ROAD;
			z11::MGWString HIGHTWAYS;
			z11::MGWString FERRIES;
			z11::MGWString U_TURN;
			z11::MGWString PERMIT_NEEDED;
			z11::MGWString TOLL_ROAD;
		};

		struct setting_language
		{
			z11::MGWString TITLE;
			z11::MGWString PROGRAM_LANGUAGE_TEXT;
			vector<z11::MGWString> PROGRAM_LANGUAGE_SWITCH;
			z11::MGWString VOICE_LANGUAGE_TEXT;
			vector<z11::MGWString> VOICE_LANGUAGE_SWITCH;
			z11::MGWString UNITS_TEXT;
			vector<z11::MGWString> UNITS_SWITCH;
		};

		struct setting_advanced
		{
			z11::MGWString TITLE;
			z11::MGWString ZOOM_IN_AFTER_FIND;
			z11::MGWString BACKLIGHT_ALWAYS_ON;
			z11::MGWString KEEP_POSITION_ON_ROAD;
			z11::MGWString KEEP_POSITION_IN_SCREEN;
		};

		struct setting_favorite
		{
			z11::MGWString TITLE;
			z11::MGWString PRIMARY_DESTINATION_TEXT;
			z11::MGWString SECONDARY_DESTINATION_TEXT;
			z11::MGWString CHANGE_ADDRESS;

		};

		struct gps_data
		{
			z11::MGWString TITLE;
			z11::MGWString SATELLITE_SUM;
			z11::MGWString ACCURACY;
			z11::MGWString ELEVATION;
			z11::MGWString SPEED;
			z11::MGWString POSITION;
			z11::MGWString DATE_TIME;
		};


		struct name_query
		{
			z11::MGWString ADDRESS;
			z11::MGWString SELECT_TYPE_TXT;
			z11::MGWString TYPE_ALL;
			vector<z11::MGWString> SELECT_TYPE_SWITCH;
		};

		struct find_address
		{
			z11::MGWString TITLE;
			z11::MGWString ADDRESS;
			z11::MGWString MY_POINT;
			z11::MGWString COORDINATE;
			z11::MGWString HISTORY;
		};

		struct find_addr_my_pi
		{
			z11::MGWString TITLE;
			z11::MGWString SET_DEST;
			z11::MGWString EDIT;
			z11::MGWString DELET;
			z11::MGWString ADD_SCREEN_POS;
			z11::MGWString NAME_QUERY;
			z11::MGWString PAGE_UP;
			z11::MGWString PAGE_DOWN;
			z11::MGWString POP_MSG;
		};

		struct find_addr_coordinate
		{
			z11::MGWString TITLE;
			z11::MGWString LONGITUDE;
			z11::MGWString LATITUDE;
			z11::MGWString EAST;
			z11::MGWString WEST;
			z11::MGWString SOUTH;
			z11::MGWString NORTH;
			z11::MGWString OK;
		};

		struct find_addr_history
		{
			z11::MGWString TITLE;
			z11::MGWString SET_DEST;
			z11::MGWString DELET;
			z11::MGWString CLEAR_ALL;
			z11::MGWString PAGE_UP;
			z11::MGWString PAGE_DOWN;
		};

		struct pop_message
		{
			z11::MGWString AUTHORITE;
			z11::MGWString LOADING;
			z11::MGWString LOCATING;
			z11::MGWString SEARCH_PATH;
			z11::MGWString RECAL_PATH;
			z11::MGWString START_NAVIGATION;
			z11::MGWString END_NAVIGATION;
			z11::MGWString START_SIMULATE;
			z11::MGWString END_SIMULATE;
		};

		struct exit
		{
			z11::MGWString EXIT_TEXT;
		};

		struct text_input
		{
			z11::MGWString TITLE;
		};

		struct dll_text
		{
			z11::MGWString MAP;
		};

		setting SETTINGS;
		setting_general SETTING_GENERAL;
		setting_map SETTING_MAP;
		setting_sound SETTING_SOUND;
		setting_route SETTING_ROUTE;
		setting_language SETTING_LANGUAGE;
		setting_advanced SETTING_ADVANCED;
		setting_favorite SETTING_FAVORITE;
		gps_data GPS_DATA;
		name_query NAME_QUERY;
		find_address FIND_ADDRESS;
		find_addr_my_pi FIND_ADDR_MY_PI;
		find_addr_coordinate FIND_ADDR_COORDINATE;
		find_addr_history FIND_ADDR_HISTORY;
		pop_message POP_MESSAGE;
		exit EXIT;
		text_input TEXT_INPUT;
		dll_text DLL_TEXT;

		z11::MGWString LANGUAGE_NAME;
		int INDEX;
	};

	class ProgramLanguage
	{
	public:
		vector<PROGRAM_LANGUAGE> m_languages;
		PROGRAM_LANGUAGE* m_current_language;

	public:
		ProgramLanguage()
		{
			m_current_language = NULL;
		}

		int changeLanguage(Settings& setting);
		int getLanguageID(int index);
		int getLanguageIndex(int id);

	};

}
#endif