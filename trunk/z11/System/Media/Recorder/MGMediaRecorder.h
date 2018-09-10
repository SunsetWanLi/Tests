//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
#pragma once

#include <string>
#include "../../../CommonBase/MGObject.h"
#include "../../Shared/Syncable.h"


namespace z11
{
    class MGMediaRecorderDelegate;
	class MGMediaRecorder : public MGObject
	{
    public:
		virtual ~MGMediaRecorder();
	protected:
		MGMediaRecorder();

	public:
        //*********************************//
        //_url without the file extension
        //    Platform        File Type
        //      IOS              .caf
        //    Android            .3gp
        //     Other             .wav
        //*********************************//
        static MGMediaRecorder* initWithContentsOfURL(const std::string &_url);
        static bool isRecording();
        
        void setUrl(const std::string &_url);
        string getFinalPath();
        
        bool prepareToRecord();
        bool record();
        bool pause();
		bool stop();
        
		void updateMeters();        //开启音量检测，需要应用定时调用//
		float getChannelValue();	//获取当前录音音量，从0到1//
		static void setChannelValue(float &v);
    private:
        std::string url;
		static float m_channel_value;
	protected:
        static Syncable recoder_locker;
        static bool is_recording;
	};
}
