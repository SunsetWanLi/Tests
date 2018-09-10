#include "ProgramLanguageAdapter.h"
#include "DataReader.h"
#include "DataWriter.h"
#include "System/Shared/MGLog.h"
#include "Retiguide/Common/String_Tools.h"
namespace GIS_Embedded
{
	DataReader * ProgramLanguageAdapter::read;

	int ProgramLanguageAdapter::translate(byte_8 *data, int size, ProgramLanguage ** lang)
	{
		*lang = new ProgramLanguage();

		read = new DataReader();
		read->setStartPointer(data);

		int sum = read->getInt();
		(*lang)->m_languages.resize(sum);
		for(int i=0;i<sum;i++)
			readProgramLanguage(*read,(*lang)->m_languages[i]);

		if(sum > 0)
			(*lang)->m_current_language = &(*lang)->m_languages[0];

		return 0;
	}

	void ProgramLanguageAdapter::readProgramLanguage(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		lang.LANGUAGE_NAME = read.getString();
		lang.INDEX = read.getInt();

		readSettingNode(read,lang);
		readAddressNode(read,lang);
		readGPSDialogNode(read,lang);
		readPopMessageNode(read,lang);
		readOtherNode(read,lang);
	}

	void ProgramLanguageAdapter::readOtherNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.EXIT.EXIT_TEXT);
		read.getString(lang.TEXT_INPUT.TITLE);
		read.getString(lang.DLL_TEXT.MAP);
	}

	void ProgramLanguageAdapter::readPopMessageNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.POP_MESSAGE.AUTHORITE);
		read.getString(lang.POP_MESSAGE.LOADING);
		read.getString(lang.POP_MESSAGE.LOCATING);
		read.getString(lang.POP_MESSAGE.SEARCH_PATH);
		read.getString(lang.POP_MESSAGE.RECAL_PATH);
		read.getString(lang.POP_MESSAGE.START_NAVIGATION);
		read.getString(lang.POP_MESSAGE.END_NAVIGATION);
		read.getString(lang.POP_MESSAGE.START_SIMULATE);
		read.getString(lang.POP_MESSAGE.END_SIMULATE);
	}

	void ProgramLanguageAdapter::readGPSDialogNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.GPS_DATA.TITLE);
		read.getString(lang.GPS_DATA.SATELLITE_SUM);
		read.getString(lang.GPS_DATA.ACCURACY);
		read.getString(lang.GPS_DATA.ELEVATION);
		read.getString(lang.GPS_DATA.SPEED);
		read.getString(lang.GPS_DATA.POSITION);
		read.getString(lang.GPS_DATA.DATE_TIME);
	}

	void ProgramLanguageAdapter::readAddressNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.FIND_ADDRESS.TITLE);
		read.getString(lang.FIND_ADDRESS.ADDRESS);
		read.getString(lang.FIND_ADDRESS.MY_POINT);
		read.getString(lang.FIND_ADDRESS.COORDINATE);
		read.getString(lang.FIND_ADDRESS.HISTORY);


		readFindAddressNode(read,lang);
		readFindPointNode(read,lang);
		readFindCoordinateNode(read,lang);
		readFindHistoryNode(read,lang);
	}

	void ProgramLanguageAdapter::readFindAddressNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.NAME_QUERY.SELECT_TYPE_TXT);
		read.getString(lang.NAME_QUERY.TYPE_ALL);
		lang.NAME_QUERY.SELECT_TYPE_SWITCH.push_back(lang.NAME_QUERY.TYPE_ALL);
	}

	void ProgramLanguageAdapter::readFindPointNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.FIND_ADDR_MY_PI.TITLE);
		read.getString(lang.FIND_ADDR_MY_PI.SET_DEST);
		read.getString(lang.FIND_ADDR_MY_PI.EDIT);
		read.getString(lang.FIND_ADDR_MY_PI.DELET);
		read.getString(lang.FIND_ADDR_MY_PI.ADD_SCREEN_POS);
		read.getString(lang.FIND_ADDR_MY_PI.NAME_QUERY);
		read.getString(lang.FIND_ADDR_MY_PI.PAGE_UP);
		read.getString(lang.FIND_ADDR_MY_PI.PAGE_DOWN);
		read.getString(lang.FIND_ADDR_MY_PI.POP_MSG);
	}

	void ProgramLanguageAdapter::readFindCoordinateNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.FIND_ADDR_COORDINATE.TITLE);
		read.getString(lang.FIND_ADDR_COORDINATE.LONGITUDE);
		read.getString(lang.FIND_ADDR_COORDINATE.LATITUDE);
		read.getString(lang.FIND_ADDR_COORDINATE.EAST);
		read.getString(lang.FIND_ADDR_COORDINATE.WEST);
		read.getString(lang.FIND_ADDR_COORDINATE.SOUTH);
		read.getString(lang.FIND_ADDR_COORDINATE.NORTH);
		read.getString(lang.FIND_ADDR_COORDINATE.OK);
	}

	void ProgramLanguageAdapter::readFindHistoryNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.FIND_ADDR_HISTORY.TITLE);
		read.getString(lang.FIND_ADDR_HISTORY.SET_DEST);
		read.getString(lang.FIND_ADDR_HISTORY.DELET);
		read.getString(lang.FIND_ADDR_HISTORY.CLEAR_ALL);
		read.getString(lang.FIND_ADDR_HISTORY.PAGE_UP);
		read.getString(lang.FIND_ADDR_HISTORY.PAGE_DOWN);
	}

	void ProgramLanguageAdapter::readSettingNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.SETTINGS.TITLE);
		read.getString(lang.SETTINGS.GENERAL);
		read.getString(lang.SETTINGS.MAP);
		read.getString(lang.SETTINGS.SOUND);
		read.getString(lang.SETTINGS.LANGUAGE_UNITS);
		read.getString(lang.SETTINGS.ADVANCED);

		readSettingGeneralNode(read,lang);
		readSettingMapNode(read,lang);
		readSettingSoundNode(read,lang);
		readSettingLanguageNode(read,lang);
		readSettingAdvancedNode(read,lang);
	}

	void ProgramLanguageAdapter::readSettingGeneralNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.SETTING_GENERAL.TITLE);
		read.getString(lang.SETTING_GENERAL.AUTOMATIC_NIGHT_COLOR);
	}

	void ProgramLanguageAdapter::readSettingMapNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.SETTING_MAP.TITLE);
		read.getString(lang.SETTING_MAP.SHOW_POINT_LABEL);
		read.getString(lang.SETTING_MAP.SHOW_STREET_LABEL);
		read.getString(lang.SETTING_MAP.AUTO_SHOW_CROSS);
	}

	void ProgramLanguageAdapter::readSettingSoundNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.SETTING_SOUND.TITLE);
		read.getString(lang.SETTING_SOUND.MASTER);
		read.getString(lang.SETTING_SOUND.VOICE);
		read.getString(lang.SETTING_SOUND.KEYS);
	}

	void ProgramLanguageAdapter::readSettingLanguageNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.SETTING_LANGUAGE.TITLE);

		read.getString(lang.SETTING_LANGUAGE.PROGRAM_LANGUAGE_TEXT);

		int size = read.getInt();
		lang.SETTING_LANGUAGE.PROGRAM_LANGUAGE_SWITCH.resize(size);
		for (int i = 0; i < size; i++)
			read.getString(lang.SETTING_LANGUAGE.PROGRAM_LANGUAGE_SWITCH[i]);

		read.getString(lang.SETTING_LANGUAGE.VOICE_LANGUAGE_TEXT);

		size = read.getInt();
		lang.SETTING_LANGUAGE.VOICE_LANGUAGE_SWITCH.resize(size);
		for (int i = 0; i < size; i++)
			read.getString(lang.SETTING_LANGUAGE.VOICE_LANGUAGE_SWITCH[i]);
	}

	void ProgramLanguageAdapter::readSettingAdvancedNode(DataReader& read,PROGRAM_LANGUAGE &lang)
	{
		read.getString(lang.SETTING_ADVANCED.TITLE);
		read.getString(lang.SETTING_ADVANCED.ZOOM_IN_AFTER_FIND);
		read.getString(lang.SETTING_ADVANCED.BACKLIGHT_ALWAYS_ON);
		read.getString(lang.SETTING_ADVANCED.KEEP_POSITION_IN_SCREEN);
	}


}

