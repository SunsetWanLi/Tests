//
//  MGAudioPlayer.h
//  System
//
//  Created by King Asdy on 12-3-31.
//  Copyright (c) 2012年 Zhejiang University. All rights reserved.
//

#pragma once

#include "../../../CommonBase/MGObject.h"
#include "../../../System/Shared/Syncable.h"
#include <string>
#include <map>

using namespace std;

namespace z11 {
    class MGAudioPlayerDelegate;
    class MGAudioPlayer : public MGObject
    {
    protected:
        MGAudioPlayer();
    public:
        virtual ~MGAudioPlayer();
	public:
		static map<string, MGAudioPlayer*> audio_manager;
    protected:
        static Syncable manager_locker;
    public:
        static MGAudioPlayer* initWithContentsOfURL(const string& url);
        static MGAudioPlayer* initWithData();
        static void audioPlayerDidFinishPlaying(const string& player_id);
		void setDuration(int len){duration = len;};
    public:
        string getAudioFilePath();
        string getAudioPlayerID();
        float rate; //The audio player's playback rate. This property’s default value of 1.0 provides normal playback rate. The available range is from 0.5 for half-speed playback through 2.0 for double-speed playback.
        float volume;   //The playback gain for the audio player, ranging from 0.0 throuth 1.0.
        float pan;  //The audio player's stereo pan position. A value of –1.0 is full left, 0.0 is center, and 1.0 is full right.
        int number_of_loops;    //The number of times a sound will return to the beginning, upon reaching the end, to repeat playback. A value of 0, which is the default, means to play the sound once. Set a positive integer value to specify the number of times to return to the start and play again. For example, specifying a value of 1 results in a total of two plays of the sound. Set any negative integer value to loop the sound indefinitely until you call the stop method.
	protected:
        bool playing;   //A Boolean Value that indicates whether the audio player is playing.
        string audio_file_path;
        string audio_player_id;
		int duration; //The length of the audio file.  In millisecond//
    public:
        MGAudioPlayerDelegate* delegate;
    public:
		int getDuration();
        bool isPlaying(){return playing;}
        void play();
		void pause();
        void stop();
        void prepareToPlay();
        void uninitPlayer();
		void rewind();
    };
    typedef map<string, MGAudioPlayer*>::iterator AudioIter;
} //namespace z11
