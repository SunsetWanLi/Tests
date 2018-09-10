//
//  MGVideoPlayer.h
//  System
//
//  Created by King Asdy on 12-10-17.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../../CommonBase/MGObject.h"
#include "../../../System/Shared/Syncable.h"
#include "../../../GraphicCore/MGImage.h"
#include <string>
#include <map>
#include "../../Device/MGDevice.h"

using namespace std;

namespace z11 {
    
    enum {
        MGMovieScalingModeNone,       // No scaling
        MGMovieScalingModeAspectFit,  // Uniform scale until one dimension fits
        MGMovieScalingModeAspectFill, // Uniform scale until the movie fills the visible bounds. One dimension may have clipped contents
        MGMovieScalingModeFill        // Non-uniform scale. Both render dimensions will exactly match the visible bounds
    };
    typedef uint_8 MGMovieScalingMode;
    enum {
        MGMovieControlModeNone,
        MGMovieControlModeEmbedded,
        MGMovieControlModeFullscreen,
        MGMovieControlModeDefault = MGMovieControlModeEmbedded
    };
    typedef uint_8 MGMovieControlMode;
    
    class MGVideoPlayerDelegate;
    class MGVideoPlayer : public MGObject
    {
    protected:
        MGVideoPlayer();
    public:
        virtual ~MGVideoPlayer();
    protected:
        static map<string, MGVideoPlayer*> video_manager;
        static Syncable manager_locker;
    public:
        static MGVideoPlayer* initWithContentsOfURL(const string& url);
        static void videoPlayerDidFinishPlaying(const string& file_path);
        static void videoPlayerDidFinishPlayingByUser(const string& file_path);
    public:
		string video_file_path;
    protected:
        bool playing;   //A Boolean Value that indicates whether the Video player is playing.
		static MGImage *m_preview_image;
    public:
        bool isPlaying(){return playing;}
        void play();
        void stop();
	public:
		static int preview_width;
		static int preview_height;
		static MGImage *getPreviewImage(const string &url,int imageWidth=MGDevice::getScreenWidth(),int imageHeight=MGDevice::getScreenHeight());
		static void setPreviewImage(MGImage *img);
    public:
        MGMovieScalingMode scaling_mode;
        MGMovieControlMode control_mode;
        MGVideoPlayerDelegate* delegate;
    };
    typedef map<string, MGVideoPlayer*>::iterator VideoIter;
} //namespace z11
