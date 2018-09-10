/****************************************************************************
Copyright (c) 2010 z11-x.org

http://www.z11-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include <set>
#include "../../../GUI/Events/MITouch.h"
#include "../../../GUI/Events/MIEvent.h"
#include "../../../GUI/GuiContainer/GuiContainer.h"

#include <android/log.h>
#include <jni.h>

#if 0
#define  LOG_TAG    "NativeTouchesJni"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...)
#endif

using namespace z11;

extern "C"
{


    // handle touch event
    void touchNativeTouchesBegin(JNIEnv*  env, jobject thiz, jint id, jfloat x, jfloat y)
    {
        std::set<MITouch*> touches;
        MITouch *mi_touch=new MITouch();
        mi_touch->point.x=x;
        mi_touch->point.y=y;
        touches.insert(mi_touch);
        GuiContainer::container->handleTouchesEvent(touches,MITouchEventBegan);
    }

    void touchNativeTouchesEnd(JNIEnv*  env, jobject thiz, jint id, jfloat x, jfloat y)
    {
        std::set<MITouch*> touches;
        MITouch *mi_touch=new MITouch();
        mi_touch->point.x=x;
        mi_touch->point.y=y;
        touches.insert(mi_touch);
        GuiContainer::container->handleTouchesEvent(touches,MITouchEventEnded);
    }

    void touchNativeTouchesMove(JNIEnv*  env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
    {

        int size = env->GetArrayLength(ids);
        jint id[size];
        jfloat x[size];
        jfloat y[size];

        env->GetIntArrayRegion(ids, 0, size, id);
        env->GetFloatArrayRegion(xs, 0, size, x);
        env->GetFloatArrayRegion(ys, 0, size, y);

        std::set<MITouch*> touches;
        for( int i = 0 ; i < size ; i++ )
        {
            MITouch *mi_touch=new MITouch();
            mi_touch->point.x=x[i];
            mi_touch->point.y=y[i];
            touches.insert(mi_touch);
        }
        GuiContainer::container->handleTouchesEvent(touches,MITouchEventMoved);
    }

    void touchNativeTouchesCancel(JNIEnv*  env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
    {
        int size = env->GetArrayLength(ids);
        jint id[size];
        jfloat x[size];
        jfloat y[size];

        env->GetIntArrayRegion(ids, 0, size, id);
        env->GetFloatArrayRegion(xs, 0, size, x);
        env->GetFloatArrayRegion(ys, 0, size, y);

        std::set<MITouch*> touches;
        for( int i = 0 ; i < size ; i++ )
        {
            MITouch *mi_touch=new MITouch();
            mi_touch->point.x=x[i];
            mi_touch->point.y=y[i];
            touches.insert(mi_touch);
        }
        GuiContainer::container->handleTouchesEvent(touches,MITouchEventCancelled);
    }

#define KEYCODE_BACK 0x04
#define KEYCODE_MENU 0x52

    // handle keydown event

    jboolean touchNativeKeyDown(JNIEnv*  env, jobject thiz, jint keyCode)
    {
        jboolean ret = JNI_FALSE;
        if(keyCode==67) // 删除键  //
        {
            MIEvent* event = new MIEvent();
            event->_event_type = KEYBOARD_DELETE_BACKWARD;
            event->_responder = NULL;
            event->_content = NULL;
            GuiContainer::container->handleSystemEvent(event);
            return JNI_TRUE;
        }
        switch (keyCode)
        {
        case KEYCODE_BACK:
			if (GuiContainer::container != NULL)
			{
                if (GuiContainer::container->needResetEntirViewForKeyboardWithView()) {
                    GuiContainer::container->resetEntirViewForKeyboardWithView();
                    ret = JNI_TRUE;
                }
                bool need_ret = GuiContainer::container->applicationNeedHandleBackKeyDown();
                ret = ret ? ret : need_ret;
			}
            //if (CCKeypadDispatcher::sharedDispatcher()->dispatchKeypadMSG(kTypeBackClicked))
            //return JNI_TRUE;
            break;
        case KEYCODE_MENU:
            //if (CCKeypadDispatcher::sharedDispatcher()->dispatchKeypadMSG(kTypeMenuClicked))
            //return JNI_TRUE;
            ret = JNI_TRUE;
            if (GuiContainer::container != NULL)
            {
                ret = GuiContainer::container->applicationNeedHandleMenuKeyDown();
            }
            break;
        default:
            {
                if ((keyCode >= 32 && keyCode <= 126) || keyCode == 0x0A || keyCode == 0x0D) {
                    MIEvent* event = new MIEvent();
                    event->_event_type = KEY_DOWN_EVENT;
                    event->_responder = NULL;
                    MGKeyDownContent* content = new MGKeyDownContent();
                    content->key_code = keyCode;
                    event->_content = content;
                    GuiContainer::container->handleSystemEvent(event);
                }
                ret = JNI_TRUE;
            }
        }
        return ret;
    }
}
