#include "MGTimer_android.h"
#include "jni/JniHelper.h"
#include <jni.h>

#ifndef JAVAVM
#define JAVAVM	z11::JniHelper::getJavaVM()
#endif

namespace z11 {
    
    
    MGTimerAndroid::MGTimerAndroid()
    {
        
    }
    MGTimerAndroid * MGTimerAndroid::getInstance()
    {
        static MGTimerAndroid timer;
        /* ◊¢≤·≤∂◊Ω SIGALRM –≈∫≈¥¶¿Ì∫Ø ˝ */
        /* »Áπ˚≤ª∫ˆ¬‘ªÚ≤∂◊Ω SIGALRM –≈∫≈£¨À¸µƒƒ¨»œ≤Ÿ◊˜ «÷’÷πµ±«∞Ω¯≥Ã */
        //signal( SIGALRM,MGTimerAndroidProc );
        return &timer;
    }
    void MGTimerAndroid::handleEvent(MGTimerTag id)
    {
        
        //z11::MGLogD("MGTimerAndroid handleEvent(%d)",id);
        
        if(!MGTimer::fireTimerWithTag(id))
        {
            stopTimer(id);
        }
        
    }
    void MGTimerAndroid::stopTimer(MGTimerTag id)
    {
        //MGLogD("MGTimerAndroid stopTimer(%d)",id);
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in MGTimerAndroid::stopTimer");
                return;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_TIMER];
        
        t.methodID = t.env->GetStaticMethodID(t.classID,"stopTimer"  ,"(I)V");
        
        if(t.methodID!=NULL)
        {
            t.env->CallStaticVoidMethod(t.classID,t.methodID,id);
        }
        
        
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ;
    }
    void MGTimerAndroid::startTimer(MGTimerTag id,int delay,int period)
    {
        //MGLogD("MGTimerAndroid startTimer(%d)delay=%d,period=%d",id,delay,period);
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in MGTimerAndroid::startTimer");
                return;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_TIMER];
                
        t.methodID = t.env->GetStaticMethodID(t.classID,"startTimer" ,"(III)V");
        
        if(t.methodID!=NULL)
        {
            t.env->CallStaticVoidMethod(t.classID,t.methodID,id,delay,period);
        }
        
        //MGLogD("startTimer (%d)",delay);
        
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ;
    }
    
    void MGTimerAndroid::add(MGTimerObj *timer)
    {
        remove(timer);
        startTimer(timer->getTag(),timer->getInterval().InMilliseconds(),timer->getInterval().InMilliseconds());
        
    }
    void MGTimerAndroid::remove(MGTimerObj *timer)
    {
        stopTimer(timer->getTag());
    }
}

extern "C"
{
	void Java_com_z11_mobile_framework_MGTimer_nativeHandleEvent(JNIEnv* env, jobject thiz,jint i)
	{
		z11::MGTimerAndroid::getInstance()->handleEvent(i);
	}
}