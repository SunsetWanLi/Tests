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
#include "MessageJni.h"
#include "JniHelper.h"
#include "../MGApplication_android.h"
#include "../../../System/Device/MGDevice.h"
#include "../../../System/IO/MGBundle.h"
#include "../../../System/IO/MGFileManager.h"
#include "../../../System/Shared/Utility.h"
#include "../../../GUI/LightGraphic/LightGraphic.h"
#include "../../../GUI/GuiContainer/GuiContainer.h"
#include "../../MGFileUtils.h"
#include "SystemInfoJni.h"
#include "../../../System/Shared/MGLog.h"
#include "../../../GraphicCore/MGImage.h"
#include <android/log.h>
#include <jni.h>


#if 1
#define  LOG_TAG    "MessageJni"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...)
#endif

#ifndef JAVAVM
#define JAVAVM	z11::JniHelper::getJavaVM()
#endif

using namespace z11;

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    // native renderer
    //////////////////////////////////////////////////////////////////////////
#define JAVA_USER_CUSTOMER_EVENT 0xe0000000
    static bool bFirst=true;
    static bool need_recovery=true;
    void messageNativeRender(JNIEnv* env)
    //void Java_com_z11_mobile_framework_MGRenderer_nativeRender(JNIEnv* env)
    {
        if(bFirst)
        {
            //MGLogD("messageNativeRender");
            bFirst=false;
            //MGApplication::sharedApplication().run();
            MGGraphicEngine::sharedGraphicEngine()->initContext();
            GuiContainer::container->light_graphic->readyToRun();
        }
        if (need_recovery) {
            MGGraphicEngine::sharedGraphicEngine()->initContext();
            z11::MGImage::recoveryContext();
            need_recovery = false;
        }
        GuiContainer::container->light_graphic->threadLoop();
    }
    
    void messageNativeResize( JNIEnv*  env , jobject  thiz,jint w, jint h)
    //void Java_com_z11_mobile_framework_MGRenderer_nativeResize( JNIEnv*  env , jobject  thiz,jint w, jint h)
    {
        //MGLogD("Keyboard Height is : %d.", h);
        if (GuiContainer::container != NULL) {
            GuiContainer::container->moveEntirViewForKeyboardWithHeight(h);
        }
    }
    // handle onPause and onResume
    
    void messageNativeOnPause()
    //void Java_com_z11_mobile_framework_MGRenderer_nativeOnPause()
    {
        return;
    }
    
    void messageNativeOnResume()
    //void Java_com_z11_mobile_framework_MGRenderer_nativeOnResume()
    {
        if (!need_recovery) {
            need_recovery=true;
        }
        if(z11::GuiContainer::container!=NULL&&z11::GuiContainer::container->light_graphic!=NULL)
        {
            
            //z11::MGImage::recoveryContext();
            MGLogD("messageNativeOnResume");
            //MGLogD("ready to setProjectionMode container=%p,light_graphic=%p",,);
            //z11::GuiContainer::container->setProjectionMode();
            z11::GuiContainer::container->light_graphic->needSettingProjection();
        }
        return;
    }
    bool preserveEventJNI()
    {
        bool ret = false;
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in preserveEventJNI");
                return ret;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_ACTIVITY];
        
        t.methodID = t.env->GetStaticMethodID(t.classID, "preserveEvent","()V");
        if(t.methodID)
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            ret = true;
        }
        
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ret;
        
    }
    void showMessageBoxJNI(const char * pszMsg, const char * pszTitle)
    {
        if (! pszMsg)
        {
            return;
        }
        
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in showMessageBoxJNI");
                return;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_ACTIVITY];
        
        t.methodID = t.env->GetStaticMethodID(t.classID, "showMessageBox","(Ljava/lang/String;Ljava/lang/String;)V");
        if (t.methodID)
        {
            jstring StringArg1;
            
            if (! pszTitle)
            {
                StringArg1 = t.env->NewStringUTF("");
            }
            else
            {
                StringArg1 = t.env->NewStringUTF(pszTitle);
            }
            
            jstring StringArg2 = t.env->NewStringUTF(pszMsg);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, StringArg1, StringArg2);
        }
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ;
    }
    
    
    //////////////////////////////////////////////////////////////////////////
    // terminate the process
    //////////////////////////////////////////////////////////////////////////
    void terminateProcessJNI()
    {
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in terminateProcessJNI");
                return;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_ACTIVITY];
        
        t.methodID = t.env->GetStaticMethodID(t.classID, "terminateProcess","()V");
        if (t.methodID)
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        }
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ;
    }
    
    void appLockScreenWhenIdle()
    {
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in appLockScreenWhenIdle");
                return;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_ACTIVITY];
        
        t.methodID = t.env->GetStaticMethodID(t.classID, "appLockScreenWhenIdle","()V");
        if (t.methodID)
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        }
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ;
    }
    
    void appUnlockScreenWhenIdle()
    {
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in appUnlockScreenWhenIdle");
                return;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_ACTIVITY];
        
        t.methodID = t.env->GetStaticMethodID(t.classID, "appUnlockScreenWhenIdle","()V");
        if (t.methodID)
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        }
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ;
    }
    
    void appShowSystemStatusBar()
    {
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in appShowSystemStatusBar");
                return;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_ACTIVITY];
        
        t.methodID = t.env->GetStaticMethodID(t.classID, "appShowSystemStatusBar","()V");
        if (t.methodID)
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        }
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ;
    }
    
    void appHideSystemStatusBar()
    {
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in appHideSystemStatusBar");
                return;
            }
            isAttached = true;
        }
        t.classID=JniHelper::m_class_reference[JNI_CLASS_ACTIVITY];
        
        t.methodID = t.env->GetStaticMethodID(t.classID, "appHideSystemStatusBar","()V");
        if (t.methodID)
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        }
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ;
    }
    
    void showSoftInputJNI(const char * keyboard_type)
    {
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in showSoftInputJNI");
                return;
            }
            isAttached = true;
        }
        {
            jclass java_class = t.env->FindClass("com/z11/mobile/framework/MGGLSurfaceView");
            jclass java_class_ref = NULL;
            if (java_class == NULL)
            {
                MGLogD("Failed to find class :%s in showSoftInputJNI ", "com/z11/mobile/framework/MGGLSurfaceView");
                return;
            } else {
                java_class_ref = (jclass) t.env->NewGlobalRef(java_class);
            }
            t.classID=java_class_ref;
        }
        
        t.methodID = t.env->GetStaticMethodID(t.classID, "ShowSoftInput","(Ljava/lang/String;)V");
        if (t.methodID)
        {
            jstring StringArg1;
            if (! keyboard_type)
            {
                StringArg1 = t.env->NewStringUTF("");
            }
            else
            {
                StringArg1 = t.env->NewStringUTF(keyboard_type);
            }
            
            t.env->CallStaticVoidMethod(t.classID, t.methodID, StringArg1);
        }
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ;
    }
    
    void hideSoftInputJNI()
    {
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in hideSoftInputJNI");
                return;
            }
            isAttached = true;
        }
        {
            jclass java_class = t.env->FindClass("com/z11/mobile/framework/MGGLSurfaceView");
            jclass java_class_ref = NULL;
            if (java_class == NULL)
            {
                MGLogD("Failed to find class :%s in hideSoftInputJNI ", "com/z11/mobile/framework/MGGLSurfaceView");
                return;
            } else {
                java_class_ref = (jclass) t.env->NewGlobalRef(java_class);
            }
            t.classID=java_class_ref;
        }
        
        t.methodID = t.env->GetStaticMethodID(t.classID, "HideSoftInput","()V");
        if (t.methodID)
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        }
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ;
    }
    
    void foreignerSDKApiCallJni(const char * api)
    {
        JniMethodInfo t;
        bool isAttached = false;
        int status = JniHelper::getJavaVM()->GetEnv((void **) &t.env, JNI_VERSION_1_4);
        if(status != JNI_OK) {
            status = JniHelper::getJavaVM()->AttachCurrentThread(&t.env, NULL);
            if(status < 0) {
                MGLogD("Failed to getJavaVM()->AttachCurrentThread in foreignerSDKApiCallJni");
                return;
            }
            isAttached = true;
        }
        {
            jclass java_class = t.env->FindClass("com/z11/mobile/framework/MGActivity");
            jclass java_class_ref = NULL;
            if (java_class == NULL)
            {
                MGLogD("Failed to find class :%s in foreignerSDKApiCallJni ", "com/z11/mobile/framework/MGActivity");
                return;
            } else {
                java_class_ref = (jclass) t.env->NewGlobalRef(java_class);
            }
            t.classID=java_class_ref;
        }
        
        t.methodID = t.env->GetStaticMethodID(t.classID, "foreignerSDKApiCallJni","(Ljava/lang/String;)V");
        if (t.methodID)
        {
            jstring StringArg1;
            
            if (! api)
            {
                StringArg1 = t.env->NewStringUTF("");
            }
            else
            {
                StringArg1 = t.env->NewStringUTF(api);
            }
            t.env->CallStaticVoidMethod(t.classID, t.methodID, StringArg1);
        }
        if(isAttached)
            JniHelper::getJavaVM()->DetachCurrentThread();
        return ;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // set apk path
    //////////////////////////////////////////////////////////////////////////
    
    void messageNativeSetPaths(JNIEnv*  env, jobject thiz, jstring apkPath, jstring sdcardPath)
    {
        const char* str;
        jboolean isCopy;
        str = env->GetStringUTFChars(apkPath, &isCopy);
        const char* sdStr;
        jboolean isSDCopy;
        sdStr = env->GetStringUTFChars(sdcardPath, &isSDCopy);
        string package_name;
        string resource_path;
        {
            std::string path=str;
            size_t offset_path=path.find_first_of(",");
			size_t offset_bundle = path.find_last_of(",");
            package_name=path.substr(0,offset_path);
            resource_path=path.substr(offset_path+1,offset_bundle-offset_path-1);
			z11::MGBundle::main_bundle_path=path.substr(offset_bundle+1)+"/files/Android.App";//"/data/data/"+package_name+"/files/Android.App";
            MGLogD("package_name=%s",package_name.c_str());
            MGLogD("resource_path=%s",resource_path.c_str());
        }
        //z11::MGBundle::main_bundle_path="/data/data/com.z11.mobile.framework/files/Android.App";
        string storage_card_bundle_path = "";
        string ext_storage_card_bundle_path = "";
        vector<string> strings = Utility::SplitString(sdStr, ';');
        if (strings.size() > 0) {
            storage_card_bundle_path = strings.at(0);
        }
        if (strings.size() > 1) {
            ext_storage_card_bundle_path = strings.at(1);
        }
        string shared_path = storage_card_bundle_path;
        MGFileManager *manager=MGFileManager::defaultManager();
        if(storage_card_bundle_path != "" && manager->fileExistsAtPath(storage_card_bundle_path))
        {
            shared_path+="/Shared/z11/";
            if(!manager->fileExistsAtPath(shared_path))
            {
                manager->createDirectoryAtPath(shared_path,true);
            }
            storage_card_bundle_path+="/Android/data/"+package_name+"/";
            if(!manager->fileExistsAtPath(storage_card_bundle_path))
            {
                manager->createDirectoryAtPath(storage_card_bundle_path,true);
            }
        }
        else
        {
            //没有存储卡使用/data/data/ //
            storage_card_bundle_path = z11::MGBundle::main_bundle_path + "/";
            shared_path="/Shared/z11/";
        }
        LOGD("z11::MGBundle::storage_card_bundle_path=%s",storage_card_bundle_path.c_str());
        if(ext_storage_card_bundle_path != "" && manager->fileExistsAtPath(ext_storage_card_bundle_path))
        {
            ext_storage_card_bundle_path+="/Android/data/"+package_name+"/";
            if(!manager->fileExistsAtPath(ext_storage_card_bundle_path))
            {
                manager->createDirectoryAtPath(ext_storage_card_bundle_path,true);
            }
        }
        else
        {
            //没有外部存储卡使用和存储卡一样的地址 //
            ext_storage_card_bundle_path = storage_card_bundle_path;
        }
        LOGD("z11::MGBundle::ext_storage_card_bundle_path=%s",ext_storage_card_bundle_path.c_str());
        z11::MGBundle::storage_card_bundle_path=storage_card_bundle_path;
        z11::MGBundle::shared_bundle_path = shared_path;
        z11::MGBundle::ext_storage_card_bundle_path = ext_storage_card_bundle_path;
        LOGD("z11::MGBundle::main_bundle_path=%s",z11::MGBundle::main_bundle_path.c_str());
        MGFileUtils::sharedFileUtils()->setResourcePath(resource_path);
        if (isSDCopy) {
            env->ReleaseStringUTFChars(sdcardPath, sdStr);
        }
        if (isCopy) {
            env->ReleaseStringUTFChars(apkPath, str);
        }
    }
}
