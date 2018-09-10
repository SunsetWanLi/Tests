#ifndef _PROGRAMLANGUAGE_ADAPTER_H_
#define _PROGRAMLANGUAGE_ADAPTER_H_

#include "../Common/Types/ProgramLanguage.h"
#include "DataReader.h"

namespace GIS_Embedded
{
	class ProgramLanguageAdapter
	{
	public:

		static int translate(byte_8 *data, int size, ProgramLanguage ** lang);

	private:

		static void readProgramLanguage(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readOtherNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readPopMessageNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readGPSDialogNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readAddressNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readFindAddressNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readFindPointNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readFindCoordinateNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readFindHistoryNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readSettingNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readSettingGeneralNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readSettingMapNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readSettingSoundNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readSettingLanguageNode(DataReader& read,PROGRAM_LANGUAGE &lang);

		static void readSettingAdvancedNode(DataReader& read,PROGRAM_LANGUAGE &lang);

	private:
		static DataReader *read;
	};
}

#endif