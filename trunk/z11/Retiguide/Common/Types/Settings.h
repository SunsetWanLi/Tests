#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "basic_types.h"
#include <string>
#include <vector>
#include "CommonBase/MGWString.h"

using namespace std;
using namespace z11;


namespace GIS_Embedded
{
	enum OffRouteRecalculation
	{
		AUTOMATIC = 0,
		ASK_FIRST = 1,
		DISABLED = 2
	};
	enum Route
	{
		SHORT = 0,
		FAST,
		ECONOMICAL
	};
	enum Vehicle
	{
		CAR = 0,
		BUS,
		TRUCK,
		PEDESTRIAN,
		BICYCLE
	};
	enum TextLanguage
	{
		TXT_ENGLISH_US = 0,
		TXT_CHINESE_SIMP = 1,
		TXT_FRENCH = 2,
		TXT_ARABIC
	};
	enum VoiceLanguage
	{
		VOC_ENGLISH_US = 0,
		VOC_CHINESE_SIMP = 1,
		VOC_FRENCH = 2,
		VOC_ARABIC
	};
	enum Units
	{
		KILOMETERS_METERS =0,
		MILES_FEET,
		MILES_YARDS
	};
	enum SoundTone
	{
		Single_Tone = 0,
		Double_Tone,
		Disabled
	};

	struct Settings
	{
		struct MY_POINTS
		{
			wchar_t point_str[40][180];
			wchar_t history[40][180];
		};

		struct Gerneral
		{
			OffRouteRecalculation off_route_recalculation;
			bool safty_mode;
			bool automatic_night_colors;
		};
		struct Map
		{
			bool alternative_road_names;
			bool show_point_labels;
			bool show_street_labels;
			bool auto_show_cross;
			bool textured_polygons;

			double host_pos_x;
			double host_pos_y;
		};
#pragma  pack(push,8)
		struct Sound_Master
		{
			bool master_open;
			double master_sacle; //<=1
		};
		struct Sound_Voice
		{
			bool voice_open;
			double voice_sacle; //<=1
		};
		struct Sound_Keys
		{
			bool keys_open;
			double keys_sacle; //<=1
		};
#pragma pack(pop)
		struct Sound
		{
			Sound_Master master;
			Sound_Voice voice;
			Sound_Keys keys;
			bool dynamic_volume;
			SoundTone tone;
		};
		struct RouteParameters
		{
			double Method; // <= 1
			Route route;
			Vehicle vehicle;
			bool unPavedRoad;
			bool highWays;
			bool ferries;
			bool uTurn;
			bool permitNeeded;
			bool tollRoad;
		};
		struct LanguageUnits
		{
			TextLanguage program_language;
			VoiceLanguage voice_language;
			Units units;
		};
		struct AdvanceSettings
		{
			bool zoom_in_after_find;
			bool backlight_always_on;
			bool keep_position_on_road;
			bool keep_position_in_screen;
		};

		Gerneral gerneral;
		Map map;
		Sound sound;
		RouteParameters route_parameters;
		LanguageUnits language_units;
		AdvanceSettings advance_settings;
		MY_POINTS my_points;


		Settings();

		int addHistoryPos(const z11::MGWString& label);

	};

}

#endif
