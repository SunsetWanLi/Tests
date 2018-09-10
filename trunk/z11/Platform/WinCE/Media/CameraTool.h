#ifndef _CAMERA_TOOL_H_
#define _CAMERA_TOOL_H_
#pragma once
#include <aygshell.h>
#include <string>
using namespace std;

namespace z11
{
	class MGMediaPickerType;
	class CameraTool
	{
	public :
		static CameraTool* getInstance();
		static void releaseInstance();
		void openMedia(MGMediaPickerType* pickerType);
		void closeMedia();

	private:
		CAMERACAPTURE_STILLQUALITY translateQuality(int _quality);
		CAMERACAPTURE_MODE translateMode(int _mode);

		CameraTool();
		~CameraTool();

	private:
		HWND	hwndOwner; 
		CAMERACAPTURE_STILLQUALITY	stillQuality;    
		CAMERACAPTURE_VIDEOTYPES	videoTypes;    
		DWORD width; 
		DWORD height; 
		DWORD timeLimit; 
		CAMERACAPTURE_MODE	mode;  
		int		count;

		LPCTSTR title;
		TCHAR	*infile;
		TCHAR	*path;
		string curInfile;

		static CameraTool* m_instance;
	};
}

#endif
