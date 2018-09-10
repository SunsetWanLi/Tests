#include "IMEJni.h"
#include "JniHelper.h"

#include "../../../GUI/Events/MIEvent.h"
#include "../../../GUI/GuiContainer/GuiContainer.h"

#include <android/log.h>
#include <string.h>
#include <jni.h>

#if 0
#define  LOG_TAG    "IMEJni"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...)
#endif

using namespace z11;

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    // handle IME message
    //////////////////////////////////////////////////////////////////////////

    void setKeyboardStateJNI(int bOpen)
    {
		JniMethodInfo t;

		if (JniHelper::getStaticMethodInfo(t,
			"com/z11/mobile/framework/MGGLSurfaceView",
			(bOpen) ? "openIMEKeyboard" : "closeIMEKeyboard",
			"()V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
		}
    }

    void imeNativeInsertJNI(const char *pszText)
    {
        {
            MIEvent* event = new MIEvent();
            MGTextContent* content = new MGTextContent();
            content->text_content = string(pszText);
            event->_event_type = KEYBOARD_INPUT;
            event->_content = content;
            event->_responder = NULL;
            GuiContainer::container->handleSystemEvent(event);
        }



    }

    void imeNativeDeleteBackward()
    {
            MIEvent* event = new MIEvent();
            event->_event_type = KEYBOARD_DELETE_BACKWARD;
            event->_responder = NULL;
            event->_content = NULL;
            GuiContainer::container->handleSystemEvent(event);
    }

    const char *imeNativeGetContentText()
    {
        const char * pszText = "no defined";
		return pszText;
    }
}
