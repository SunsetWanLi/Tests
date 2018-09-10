//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
#include "MGMediaRecorder.h"
#include "../../../Platform/MGPlatformConfig.h"
#include "../../../System/Shared/MGLog.h"
#include "../../../GUI/Events/MIEvent.h"
#include "../../../GUI/GuiContainer/GuiContainer.h"

namespace z11
{
    Syncable MGMediaRecorder::recoder_locker;
    bool MGMediaRecorder::is_recording = false;
	float MGMediaRecorder::m_channel_value = 0.0;
	MGMediaRecorder::MGMediaRecorder()
	{
	}

	MGMediaRecorder::~MGMediaRecorder()
	{
	}

    MGMediaRecorder* MGMediaRecorder::initWithContentsOfURL(const string &_url)
    {
        MGMediaRecorder *ret = new MGMediaRecorder();
        ret->setUrl(_url);
        return ret;
    }
    bool MGMediaRecorder::isRecording()
    {
        bool ret = false;
        recoder_locker.lock();
        ret = is_recording;
        recoder_locker.unlock();
        return ret;
    }
    
    bool MGMediaRecorder::prepareToRecord()
    {
        return true;
    }
    bool MGMediaRecorder::record()
    {
        bool ret = false;
        recoder_locker.lock();
        if (!is_recording) {
            MIEvent* event = new MIEvent;
            event->_event_type = MEDIA_RECORDER_START_EVENT;
            MGTextContent* content = new MGTextContent();
            content->text_content = url;
            event->_content = content;
            GuiContainer::container->preserveEvent(event);
            ret = true;
        }
        is_recording = true;
        recoder_locker.unlock();
        return ret;
    }
    bool MGMediaRecorder::pause()
    {
        return false;
    }
    bool MGMediaRecorder::stop()
    {
        bool ret = false;
        recoder_locker.lock();
        if (is_recording) {
            MIEvent* event = new MIEvent;
            event->_event_type = MEDIA_RECORDER_STOP_EVENT;
            GuiContainer::container->preserveEvent(event);
            ret = true;
        }
        is_recording = false;
        recoder_locker.unlock();
        return ret;
    }
    
    void MGMediaRecorder::setUrl(const std::string &_url)
    {
#if (MG_TARGET_PLATFORM == MG_PLATFORM_IOS)
        url = _url + ".wav";
#elif (MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID)
        url = _url + ".mp3";
#else
        url = _url + ".wav";
#endif
    }
    
    string MGMediaRecorder::getFinalPath()
    {
        return url;
    }


	void MGMediaRecorder::updateMeters()
	{
		recoder_locker.lock();
		if (is_recording) {
			MIEvent* event = new MIEvent;
			event->_event_type = MEDIA_RECORDER_UPDATE_METERS_EVENT;
			GuiContainer::container->preserveEvent(event);
		}
		recoder_locker.unlock();
	}
	void MGMediaRecorder::setChannelValue(float &v)
	{
		m_channel_value=v;
	}
	float MGMediaRecorder::getChannelValue()
	{
		return m_channel_value;
	}
}


