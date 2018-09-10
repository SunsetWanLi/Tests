#include "CameraTool.h"
#include "../../../CommonBase/MGData.h"
//#include "../../../GraphicCore/MGImage.h"
#include "../../../CommonBase/MGString.h"
#include "../../../CommonBase/MGDictionary.h"
#include "System/MediaPicker/MGMediaPicker.h"
#include "System/Shared/Utility.h"

namespace z11 
{
	CameraTool* CameraTool::m_instance = NULL;
	CameraTool::CameraTool()
	{
		stillQuality=	CAMERACAPTURE_STILLQUALITY_DEFAULT;
		videoTypes	=	CAMERACAPTURE_VIDEOTYPE_ALL;
		mode		=	CAMERACAPTURE_MODE_STILL;
		count		=	0;
		hwndOwner	=	NULL;
		title		=	NULL;
		width		=	640;
		height		=	480;
		timeLimit	=	0;
		path		=   TEXT("\\Storage Card");
	}
	CameraTool::~CameraTool()
	{

	}

	CameraTool* CameraTool::getInstance()
	{
		if(m_instance ==NULL)
		{
			m_instance = new CameraTool();
		}
		return m_instance;
	}
	void CameraTool::releaseInstance()
	{
		if (m_instance)
		{
			delete m_instance;
			m_instance = NULL;
		}
	}

	void CameraTool::openMedia(MGMediaPickerType* pickerType) 
	{ 
		HRESULT  hReturn;
		SHCAMERACAPTURE shcc;

		stillQuality = translateQuality(pickerType->mediaQuality);
		mode = translateMode(pickerType->sourceType);
		width = pickerType->width;
		height = pickerType->height;

		//MGTime time;
		//curInfile	=	time.CurTimeToString("yyyy-mm-dd HH:MM:SS");
		//curInfile.append(".jpg");
		//Utility::c2wc(infile, curInfile.c_str());
		count++;

		ZeroMemory( &shcc,  sizeof (shcc) );
		shcc.cbSize				= sizeof ( shcc );
		shcc.hwndOwner			= hwndOwner;
		shcc.pszDefaultFileName = infile;
		shcc.pszTitle			= title;
		shcc.pszInitialDir		= path;
		shcc.StillQuality		= stillQuality;
		shcc.VideoTypes			= videoTypes;
		shcc.Mode				= mode;
		shcc.nResolutionWidth   = width;
		shcc.nResolutionHeight  = height;
		shcc.nVideoTimeLimit    = timeLimit;
		
		hReturn = SHCameraCapture( &shcc );
		switch  (hReturn)
		{
		case  S_OK:
			{
				//AfxMessageBox( L"成功拍照或摄像"  ); 
				wstring file = shcc.pszInitialDir;
				file.append(L"\\");
				file.append(shcc.pszDefaultFileName);

				MGData* data = MGData::dataWithContentsOfFile(Utility::wstr2astr(file));
				std::vector<MGObject*> value;
				value.push_back(data);
				std::vector<MGObject*> key;
				key.push_back(MGString::initWithSTLString("MGMediaPickerOriginalImageData"));
				MGDictionary *dictionary = MGDictionary::dictionaryWithObjectsForKeys(value, key);
				MGMediaPicker::didFinishPickingMediaWithInfo(dictionary);

				delete_and_set_null(data);
				//delete_and_set_null(image);
				delete_and_set_null(dictionary);
				break;
			}
		case  S_FALSE:
			{
				//The   user   canceled   the   Camera   Capture   dialog   box.    
				//AfxMessageBox( L"没有拍照"  );
				MGMediaPicker::didFinishPickingMediaWithInfo(NULL);
				break;
			}
		case  E_INVALIDARG:
			{
				MGMediaPicker::didFinishPickingMediaWithInfo(NULL);
				break;
			}
		case  E_OUTOFMEMORY:
			{
				//AfxMessageBox( L"Out of Memory"  ); 
				MGMediaPicker::didFinishPickingMediaWithInfo(NULL);
				break;
			}
		default :
			break;
		}
	}

	void CameraTool::closeMedia()
	{

	}

	CAMERACAPTURE_STILLQUALITY CameraTool::translateQuality(int _quality)
	{
		CAMERACAPTURE_STILLQUALITY ret = CAMERACAPTURE_STILLQUALITY_DEFAULT;
		switch (_quality)
		{
		case MGMediaPickerCameraQualityHigh:
			ret = CAMERACAPTURE_STILLQUALITY_HIGH;
			break;
		case MGMediaPickerCameraQualityMedium:
			ret = CAMERACAPTURE_STILLQUALITY_NORMAL;
			break;
		case MGMediaPickerCameraQualityLow:
			ret = CAMERACAPTURE_STILLQUALITY_LOW;
			break;
		default:
			break;
		}
		return ret;
	}

	CAMERACAPTURE_MODE CameraTool::translateMode(int _mode)
	{
		CAMERACAPTURE_MODE ret = CAMERACAPTURE_MODE_STILL;
		switch (_mode)
		{
		case MGMediaPickerSourceTypeImage:
			ret = CAMERACAPTURE_MODE_STILL;
			infile = TEXT("image.jpg");
			break;
		case MGMediaPickerSourceTypeVideo:
			ret = CAMERACAPTURE_MODE_VIDEOWITHAUDIO;
			infile = TEXT("video.mp4");
			break;
		case MGMediaPickerSourceTypeSound:
			break;
		case MGMediaPickerSourceTypePhotoLibrary:
			break;
		case MGMediaPickerSourceTypeSavedPhotoAlbum:
			break;
		case MGMediaPickerSourceTypeVideoLibrary:
			break;
		default:
			break;
		}
		return ret;
	}//CameraTool
}//z11