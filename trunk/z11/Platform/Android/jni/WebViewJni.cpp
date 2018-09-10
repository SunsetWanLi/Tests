#include "JniHelper.h"
#include "WebViewJni.h"
#include "../../../System/Shared/MGLog.h"
#include "../../../System/WebView/MGWebViewManager.h"

using namespace z11;

void jniWebViewOpenURL(const std::string &url ,z11::MGRect rect, const std::string &post_body)
{
    JniMethodInfo methodInfo;
    if(JniHelper::getStaticMethodInfo(methodInfo, "com/z11/mobile/framework/MGWebViewManager", "webViewOpenURL", "(Ljava/lang/String;Ljava/lang/String;FFFF)V"))
    {													
        MGLogD("jniWebViewOpenURL success");
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, methodInfo.env->NewStringUTF(url.c_str()), methodInfo.env->NewStringUTF(post_body.c_str()),
				rect.x, rect.y, rect.width, rect.height);
    }
    else
        MGLogD("jniWebViewOpenURL Failed");
}

void jniWebViewShow()
{
	JniMethodInfo methodInfo;
    if(JniHelper::getStaticMethodInfo(methodInfo, "com/z11/mobile/framework/MGWebViewManager", "webViewShow", "()V"))
    {													
        MGLogD("jniWebViewShow success");
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    }
    else
        MGLogD("jniWebViewShow Failed");
}

void jniWebViewClose()
{
	JniMethodInfo methodInfo;
    if(JniHelper::getStaticMethodInfo(methodInfo, "com/z11/mobile/framework/MGWebViewManager", "webViewClose", "()V"))
    {													
        MGLogD("jniWebViewClose success");
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    }
    else
        MGLogD("jniWebViewClose Failed");
}

extern "C"
{
    static string webview_jstring2string_(JNIEnv *env, jstring jstr)
    {
        
        jboolean isCopy;
        
        const char* chars = env->GetStringUTFChars(jstr, &isCopy);
        string ret(chars);
        if (isCopy)
        {
            env->ReleaseStringUTFChars(jstr, chars);
        }
        
        return ret;
    }
    
    void Java_com_z11_mobile_framework_MGWebViewManager_didStartLoad()
    {
        MGWebViewManager::didStartLoad();
    }
    
    jboolean Java_com_z11_mobile_framework_MGWebViewManager_shouldStartLoadWithRequest(JNIEnv *env, jobject thiz, jstring request)
    {
        string url = webview_jstring2string_(env, request);
        jboolean ret = MGWebViewManager::shouldStartLoadWithRequest(url);
        return ret;
    }
    
    void Java_com_z11_mobile_framework_MGWebViewManager_didFinishLoad(JNIEnv *env, jobject thiz)
    {
        MGWebViewManager::didFinishLoad();
    }
    
    void Java_com_z11_mobile_framework_MGWebViewManager_didFailLoadWithError(JNIEnv *env, jobject thiz, jint err_code, jstring description)
    {
        std::string description_str = webview_jstring2string_(env, description);
        MGWebViewManager::didFailLoadWithError(err_code, description_str);
    }

	void Java_com_z11_mobile_framework_MGWebViewManager_didRecievedTitle(JNIEnv *env, jobject thiz , jstring title)
	{
		std::string title_str = webview_jstring2string_(env, title);
		MGWebViewManager::didReceivedTitle(title_str);
	}
}