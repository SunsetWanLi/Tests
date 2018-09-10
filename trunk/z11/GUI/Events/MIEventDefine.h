//
//  MIEventDefine.h
//  Gui
//
//  Created by Wang ZhiPeng on 11-6-13.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//
#pragma once

namespace z11 {
    // 由系统及z11底层框架触发的事件 //
    //==============================================//
#define SYS_EVENT                               0x01000000
#define KEYBOARD_INPUT                          SYS_EVENT+1
#define KEYBOARD_PRE_INPUT                      SYS_EVENT+2
#define KEYBOARD_DELETE_BACKWARD                SYS_EVENT+3
#define WILL_START_ANIMATION_SELECTOR_EVENT     SYS_EVENT+4
#define DID_STOP_ANIMATION_SELECTOR_EVENT       SYS_EVENT+5
#define THREAD_MANAGER_EVENT                    0x0dffffff
#define MAP_FETCHER_THREAD_READY_STOP_EVENT     THREAD_MANAGER_EVENT+1
#define HARDWARE_SIGNAL_EVENT                   0x0effffff
#define KEY_DOWN_EVENT                          HARDWARE_SIGNAL_EVENT+1
#define SYS_EVENT_END                           0x0fffffff
    //==============================================//
    
    // 由控件触发的事件 //
    //==============================================//
#define CONTROL_EVENT                           0x10000000
#define SCROLL_VIEW_LONG_PRESS_EVENT            CONTROL_EVENT+1
#define SCROLL_VIEW_LIGHT_PRESS_EVENT           CONTROL_EVENT+2
#define CONTROL_RETRANS_PURE_TOUCHES_EVENT      CONTROL_EVENT+3
#define SCROLL_VIEW_EVENT_DIRECTPASS            CONTROL_RETRANS_PURE_TOUCHES_EVENT+1
#define CONTROL_PURE_TOUCHES_EVENT              CONTROL_RETRANS_PURE_TOUCHES_EVENT+2
#define CONTROL_REPLACE_TOUCHES_EVENT              CONTROL_RETRANS_PURE_TOUCHES_EVENT+3
#define CONTROL_PURE_TOUCHES_DELAY_EVENT        CONTROL_RETRANS_PURE_TOUCHES_EVENT+4
    
#define MAP_VIEW_EVENT                          0x11000000
#define MAP_VIEW_START_SIMU_EVENT               MAP_VIEW_EVENT+1
    
#define CONTROL_TOUCH_EVENT                     0x12000000
    
#define CONTROL_TEXT_EVENT                      0x13000000
#define TEXTFIELD_TEXT_CHANGE_EVENT             CONTROL_TEXT_EVENT+1
    
#define GESTURE_RECOGNIZER_EVENT                0x14000000
#define LONG_PRESS_GESTURE_EVENT                GESTURE_RECOGNIZER_EVENT+1
#define TAP_GESTURE_LONG_INTERVAL_EVENT         GESTURE_RECOGNIZER_EVENT+2
#define TAP_GESTURE_LONG_STAY_EVENT             GESTURE_RECOGNIZER_EVENT+3
#define CONTROL_EVENT_END                       0x1fffffff

    //==============================================//
    
    // 需要回调系统的事件 //
    //==============================================//
#define SYS_CALLBACK_EVENT                      0x20000000
#define BECOME_FIRST_RESPONDER_EVENT            SYS_CALLBACK_EVENT+1
#define RESIGN_FIRST_RESPONDER_EVENT            SYS_CALLBACK_EVENT+2
#define SCAN_WIFI_STATUS_EVENT                  SYS_CALLBACK_EVENT+3
#define LIGHT_GRAPHIC_FIRST_FRAME_DRAWED_EVENT  SYS_CALLBACK_EVENT+4
#define APP_LOCK_SCREEN_WHEN_IDLE_EVENT         SYS_CALLBACK_EVENT+5
#define APP_UNLOCK_SCREEN_WHEN_IDLE_EVENT       SYS_CALLBACK_EVENT+6
#define APP_SHOW_SYSTEM_STATUS_BAR_EVENT        SYS_CALLBACK_EVENT+7
#define APP_HIDE_SYSTEM_STATUS_BAR_EVENT        SYS_CALLBACK_EVENT+8


#define LOCATION_EVENT_BEGIN                    0x20000200
#define START_LOCATION_SERVICE_EVENT            LOCATION_EVENT_BEGIN+1
#define STOP_LOCATION_SERVICE_EVENT             LOCATION_EVENT_BEGIN+2
#define START_HEADING_SERVICE_EVENT             LOCATION_EVENT_BEGIN+3
#define STOP_HEADING_SERVICE_EVENT              LOCATION_EVENT_BEGIN+4
#define START_MONITORING_FOR_REGION_EVENT       LOCATION_EVENT_BEGIN+5
#define STOP_MONITORING_FOR_REGION_EVENT        LOCATION_EVENT_BEGIN+6
#define START_RANGING_FOR_REGION_EVENT          LOCATION_EVENT_BEGIN+7
#define STOP_RANGING_FOR_REGION_EVENT           LOCATION_EVENT_BEGIN+8
#define RELEASE_LOCATION_SERVICE_EVENT          LOCATION_EVENT_BEGIN+100
    
#define MOTION_EVENT_BEGIN                      0x20000300
#define START_ACCELERATION_SERVICE_EVENT        MOTION_EVENT_BEGIN+1
#define STOP_ACCELERATION_SERVICE_EVENT         MOTION_EVENT_BEGIN+2
#define START_ROTATION_SERVICE_EVENT            MOTION_EVENT_BEGIN+3
#define STOP_ROTATION_SERVICE_EVENT             MOTION_EVENT_BEGIN+4
#define RELEASE_MOTION_SERVICE_EVENT            MOTION_EVENT_BEGIN+5
#define START_ATTITUDE_SERVICE_EVENT            MOTION_EVENT_BEGIN+6
#define STOP_ATTITUDE_SERVICE_EVENT             MOTION_EVENT_BEGIN+7
    
#define WEBSERVICE_EVENT_BEGIN                  0x20000400
#define WEB_SERVICE_BEGIN_EVENT                 WEBSERVICE_EVENT_BEGIN+1
#define WEB_SERVICE_END_EVENT                   WEBSERVICE_EVENT_BEGIN+2

#define APP_EVENT_BEGIN                         0x20000500
#define LAUNCH_APP_WITH_URL_EVENT               APP_EVENT_BEGIN+1
#define EXIT_APPLICATION_EVENT                  APP_EVENT_BEGIN+2
    
#define TIMER_EVENT_BEGIN                       0x20000600
#define VALIDATE_A_TIMER_EVENT                  TIMER_EVENT_BEGIN+1
#define INVALIDATE_A_TIMER_EVENT                TIMER_EVENT_BEGIN+2

#define MEDIA_EVENT_BEGIN                       0x20000700
#define MEDIA_PICK_EVENT                        MEDIA_EVENT_BEGIN+1
#define MEDIA_DROP_EVENT                        MEDIA_EVENT_BEGIN+2
#define INIT_AUDIO_PLAY_EVENT                   MEDIA_EVENT_BEGIN+3
#define PLAY_AUDIO_PLAY_EVENT                   MEDIA_EVENT_BEGIN+4
#define PAUSE_AUDIO_PLAY_EVENT                  MEDIA_EVENT_BEGIN+5
#define STOP_AUDIO_PLAY_EVENT                   MEDIA_EVENT_BEGIN+6
#define PREPARE_AUDIO_PLAY_EVENT                MEDIA_EVENT_BEGIN+7
#define REWIND_AUDIO_PLAY_EVENT                 MEDIA_EVENT_BEGIN+8
#define UNINIT_AUDIO_PLAY_EVENT                 MEDIA_EVENT_BEGIN+9
#define MEDIA_RECORDER_START_EVENT              MEDIA_EVENT_BEGIN+10
#define MEDIA_RECORDER_STOP_EVENT               MEDIA_EVENT_BEGIN+11
#define INIT_VIDEO_PLAY_EVENT                   MEDIA_EVENT_BEGIN+12
#define PLAY_VIDEO_PLAY_EVENT                   MEDIA_EVENT_BEGIN+13
#define STOP_VIDEO_PLAY_EVENT                   MEDIA_EVENT_BEGIN+14
#define MEDIA_RECORDER_UPDATE_METERS_EVENT      MEDIA_EVENT_BEGIN+15
#define GET_VIDEO_PREVIEW_EVENT                 MEDIA_EVENT_BEGIN+16
#define GET_AUDIO_DURATION_EVENT                 MEDIA_EVENT_BEGIN+17

#define CAPTURE_EVENT_BEGIN                     0x20000800
#define START_CAMERA_CAPTURE_SERVICE_EVENT      CAPTURE_EVENT_BEGIN+1
#define STOP_CAMERA_CAPTURE_SERVICE_EVENT       CAPTURE_EVENT_BEGIN+2
#define ADJUST_CAMERA_CAPTURE_FOCUS_EVENT       CAPTURE_EVENT_BEGIN+3
    
#define FILE_EVENT_BEGIN                        0x21000000
#define OPEN_FILE_WITH_URL_EVENT				FILE_EVENT_BEGIN+1
    
#define MAP_EVENT_BEGIN                         0x21000100
#define GOOGLEMAP_RESPONDER_EVENT               MAP_EVENT_BEGIN+1
    
#define UIWEBVIEW_EVENT_BEGIN                   0x22000000
#define UIWEBVIEW_OPEN_URL_EVENT                UIWEBVIEW_EVENT_BEGIN+1
#define UIWEBVIEW_SHOW_VIEW_EVENT               UIWEBVIEW_EVENT_BEGIN+2
#define UIWEBVIEW_CLOSE_VIEW_EVENT              UIWEBVIEW_EVENT_BEGIN+3
#define UIWEBVIEW_CAN_BACK_EVENT                UIWEBVIEW_EVENT_BEGIN+4
#define UIWEBVIEW_CAN_FORWARD_EVENT             UIWEBVIEW_EVENT_BEGIN+5
#define UIWEBVIEW_GO_BACK_EVENT                 UIWEBVIEW_EVENT_BEGIN+6
#define UIWEBVIEW_GO_FORWARD_EVENT              UIWEBVIEW_EVENT_BEGIN+7
    
    // 外部SDK调用事件定义 //
#define FOREIGNER_SDK_API_IMMEDIATELY_CALL_EVENT    0x2ffffffd
#define FOREIGNER_SDK_API_CALL_EVENT                0x2ffffffe
#define SYS_CALLBACK_EVENT_END                      0x2fffffff
    //==============================================//

    // 用户事件定义 //
#define USER_CUSTOMER_EVENT                     0xe0000000
#define USER_CUSTOMER_EVENT_END                 0xffffffff
    
} //namespace z11
