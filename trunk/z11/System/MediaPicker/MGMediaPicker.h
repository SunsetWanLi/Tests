//
//  MGMediaPicker.h
//  System
//
//  Created by DanYuan Hu on 11-12-29.
//  Copyright (c) 2011Äê Zhejiang University. All rights reserved.
//

#pragma once

#include "../../CommonBase/MGObject.h"
#include "../../CommonBase/CommonDef.h"
#include "../Shared/Syncable.h"
#include <set>
#include <string>

using namespace std;

namespace z11 {
	
	class MGDictionary;
	class MGImage;
	class MGData;
	enum
	{
		MGMediaPickerSourceTypeImage = 0,		//capture image
		MGMediaPickerSourceTypePhotoLibrary,	//lookup photo library
		MGMediaPickerSourceTypeSavedPhotoAlbum,	//lookup photo album
		MGMediaPickerSourceTypeSound,			//capture sound
		MGMediaPickerSourceTypeSoundLibrary,	//lookup sound library
		MGMediaPickerSourceTypeVideo,			//capture video
		MGMediaPickerSourceTypeVideoLibrary,	//lookup video library
	};
	typedef int_32 MGMediaPickerSourceType;
	enum
	{
		MGMediaPickerCameraQualityHigh = 0,
		MGMediaPickerCameraQualityMedium,
		MGMediaPickerCameraQualityLow,
	};
	typedef int_32 MGMediaPickerCameraQuality;
	class MGMediaPickerType : public MGObject
	{
	public:
		MGMediaPickerType()
		{
			sourceType = MGMediaPickerSourceTypeImage;
			mediaQuality = MGMediaPickerCameraQualityHigh;
            select_limit = 1;
            allows_edit = false;
		}
		MGMediaPickerSourceType sourceType;
		MGMediaPickerCameraQuality mediaQuality;
        unsigned int select_limit;
        bool allows_edit;
    };
	

	class MGMediaPickerDelegate;
	class MGMediaPicker : public MGObject
	{
	public:
		MGMediaPicker();
		virtual ~MGMediaPicker();
	protected:
		static bool m_is_media_opened;
        static Syncable media_locker;
		static set<MGMediaPicker*> image_pool;
		static set<MGMediaPicker*> sound_pool;
		static set<MGMediaPicker*> video_pool;
		static Syncable image_pool_locker;
		static Syncable sound_pool_locker;
		static Syncable video_pool_locker;
	public:
		static void openMedia();
		static void closeMedia();
		void startPickingImage();
		void startPickingSound();
		void startPickingVideo();
		void stopPickingImage();
		void stopPickingSound();
		void stopPickingVideo();
		void setDelegate(MGMediaPickerDelegate* dlg);
	public:
		static void didFinishPickingImageWithInfo(MGDictionary *info);
		static void didFinishPickingSoundWithInfo(MGDictionary *info);
		static void didFinishPickingVideoWithInfo(MGDictionary *info);
		static void didCancelPickingImageWithInfo(MGDictionary *info);
		static void didCancelPickingSoundWithInfo(MGDictionary *info);
		static void didCancelPickingVideoWithInfo(MGDictionary *info);
	public:
		static MGMediaPickerType mediaPickerType;
	private:
		bool m_is_image_picking;
		bool m_is_sound_picking;
		bool m_is_video_picking;
		MGMediaPickerDelegate* delegate;
	};
	typedef set<MGMediaPicker*> ManagerPool;

	//******************************************************************************************//
	// info dictionary keys
	//				keys								objects
	// MGMediaPickerOriginalImageData					a MGData(with compressing)
	// MGMediaPickerOriginalImageFile					a Image File(without compressing)
	// MGMediaPickerOriginalVideoFile					a Video File(without compressing)
	// MGMediaPickerOriginalSoundFile					a Sound File(without compressing)
	//******************************************************************************************//

	MIINTERFACE MGMediaPickerDelegate
	{
	public:
		virtual void didFinishPickingMediaWithInfo(MGDictionary *info) = 0;
		virtual void didCancelPickingMediaWithInfo(MGDictionary *info) = 0;
		//virtual void didFinishPickingVideoWithInfo() = 0;
	};
#define MGMEDIAPICKER_DELEGAGE_DECLARE \
	virtual void didFinishPickingMediaWithInfo(MGDictionary *info); \
	virtual void didCancelPickingMediaWithInfo(MGDictionary *info); \
	//virtual void didFinishPickingVideoWithInfo();
} //namespace z11
