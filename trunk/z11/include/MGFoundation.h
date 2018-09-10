//
//  MGFoundation.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-7-12.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "../CommonBase/Algorithm/Base64/Base64.h"
#include "../CommonBase/CommonDef.h"
#include "../CommonBase/MGDictionary.h"
#include "../CommonBase/MGWString.h"
#include "../CommonBase/MGString.h"
#include "../CommonBase/MGStructure.h"
#include "../CommonBase/MGDataReader.h"

#include "../ExternalLib/json/json/json.h"
#include "../ExternalLib/xml/rapidxml.hpp"
#include "../ExternalLib/xml/rapidxml_utils.hpp"
#include "../ExternalLib/xml/rapidxml_print.hpp"
#include "../ExternalLib/cryptopp/MD5.h"
#include "../ExternalLib/cryptopp/randpool.h"
#include "../ExternalLib/cryptopp/rsa.h"
#include "../ExternalLib/cryptopp/hex.h"
#include "../ExternalLib/cryptopp/files.h"

#include "../GraphicCore/MGImage.h"
#include "../GraphicCore/BitmapStructDefination.h"
#include "../GUI/GestureRecognizer/MIPanGestureRecognizer.h"
#include "../GUI/GestureRecognizer/MISwipeGestureRecognizer.h"
#include "../GUI/Drama/MGActor.h"
#include "../Platform/MGApplication_platform.h"
#include "../Platform/NetworkStatus/MGNetworkReachability.h"
#include "../Platform/MGFileUtils.h"

#include "../System/Device/MGDevice.h"
#include "../System/IO/MGBundle.h"
#include "../System/LocationService/MGHeading.h"
#include "../System/LocationService/MGLocation.h"
#include "../System/LocationService/MGLocationRegion.h"
#include "../System/LocationService/MGLocationManager.h"
#include "../System/MotionService/MGAcceleration.h"
#include "../System/MotionService/MGRotation.h"
#include "../System/MotionService/MGMotionManager.h"
#include "../System/MotionService/MGAttitude.h"
#include "../System/Media/Audio/MGAudioPlayer.h"
#include "../System/Media/Audio/MGAudioPlayerDelegate.h"
#include "../System/Media/Capture/MGCameraCaptureManager.h"
#include "../System/Media/Recorder/MGMediaRecorder.h"
#include "../System/Media/Video/MGVideoPlayer.h"
#include "../System/MediaPicker/MGMediaPicker.h"
#include "../System/MGSocket/MGSocket.h"
#include "../System/Network/FTP/FtpParseInfo.h"
#include "../System/Network/FTP/FtpUtil.h"
#include "../System/Network/HttpPost/MGHttpPost.h"
#include "../System/Network/URLFetcher/MGURLFetcher.h"
#include "../System/Network/URL/MGURLDownload.h"
#include "../System/Network/URL/MGURLConnection.h"
#include "../System/Network/URLFetcher/MGURLFetcher.h"
#include "../System/Shared/MGLog.h"
#include "../System/Shared/MGPath.h"
#include "../System/Shared/MGTime.h"
#include "../System/Shared/Utility.h"
#include "../System/SQLite/sqlite3.h"
#include "../System/Thread/MGThread.h"
#include "../System/Timer/MGTimer.h"
#include "../System/WebService/MGWebService.h"
#include "../System/WebService/MGWebServiceThread.h"
#include "../System/WebView/MGWebViewManager.h"
#include "../System/ClipBoard/MGClipBoard.h"

using namespace z11;

