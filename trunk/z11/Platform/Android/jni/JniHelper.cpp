#include "JniHelper.h"
#include <android/log.h>
#include <string.h>
#include "../../../System/Shared/MGLog.h"
#if 1
#define  LOG_TAG    "JniHelper"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...)
#endif

#define JAVAVM	z11::JniHelper::getJavaVM()

using namespace std;

extern "C"
{

//////////////////////////////////////////////////////////////////////////
// java vm helper function
//////////////////////////////////////////////////////////////////////////
const char *class_path[] =
{ "com/z11/mobile/framework/MGActivity",
		"com/z11/mobile/framework/MGMediaPlayer",
		"com/z11/mobile/framework/MGTimer",
		"com/z11/mobile/framework/MGNetworkReachability",
		};
int z11::JniHelper::init()
{
	MGLogD("================================JniHelper::init======================================");
	JNIEnv *env;
	if (JAVAVM->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
	{
		LOGD("Failed to get the environment using GetEnv() in JniHelper::init");
		return -1;
	}

	m_class_reference.clear();

	for (int i = 0; i < JNI_CLASS_COUNT; i++)
	{
		jclass java_class = env->FindClass(class_path[i]);
		jclass java_class_ref = NULL;
		if (java_class == NULL)
		{
			MGLogD("Failed to find class :%s in JniHelper::init!! ",
					class_path[i]);
			//return -1;
		}
		else
		{
			MGLogD("Success to find class :%s", class_path[i]);
			java_class_ref = (jclass) env->NewGlobalRef(java_class);
		}
		m_class_reference.push_back(java_class_ref);
	}

	MGLogD("================================JniHelper::init OK======================================");
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	z11::JniHelper::setJavaVM(vm);
	z11::JniHelper::init();
	//init();
	return JNI_VERSION_1_4;
}
int attached = 0;
static bool getEnv(JNIEnv **env)
{
	bool bRet = false;

	do
	{
		if (JAVAVM->GetEnv((void**)env, JNI_VERSION_1_4 ) <0)
		{
			LOGD("Failed to get the environment using GetEnv()");
			if (JAVAVM->AttachCurrentThread(env, 0) < 0)
			{
				LOGD("Failed to get the environment using AttachCurrentThread()");
				break;
			}
			else
			{
				attached++;
			}
			//break;
		}

		bRet = true;
	} while (0);

	return bRet;
}

static jclass getClassID_(const char *className, JNIEnv *env)
{
	JNIEnv *pEnv = env;
	jclass ret = 0;

	do
	{
		if (!pEnv)
		{
			if (!getEnv(&pEnv))
			{
				break;
			}
		}

		ret = pEnv->FindClass(className);
		if (!ret)
		{
			LOGD("Failed to find class of %s", className);
			break;
		}
	} while (0);

	return ret;
}

static bool getStaticMethodInfo_(z11::JniMethodInfo &methodinfo,
		const char *className, const char *methodName, const char *paramCode)
{
	jmethodID methodID = 0;
	JNIEnv *pEnv = 0;
	bool bRet = false;

	do
	{
		if (!getEnv(&pEnv))
		{
			break;
		}

		jclass classID = getClassID_(className, pEnv);
		if (classID == NULL)
		{
			break;
		}

		//LOGD("start getStaticMethodInfo_ classID=%p methodName=%s paramCode=%s pEnv=%p ",classID,methodName,paramCode,pEnv);
		methodID = pEnv->GetStaticMethodID(classID, methodName, paramCode);
		if (!methodID)
		{
			LOGD("Failed to find static method id of %s", methodName);
			break;
		}
		//LOGD("Succes to find static method id of %s", methodName);

		methodinfo.classID = classID;
		methodinfo.env = pEnv;
		methodinfo.methodID = methodID;

		bRet = true;
	} while (0);
	if (attached)
	{
		attached--;
		//JAVAVM->DetachCurrentThread();
	}

	return bRet;
}

static bool getMethodInfo_(z11::JniMethodInfo &methodinfo,
		const char *className, const char *methodName, const char *paramCode)
{
	jmethodID methodID = 0;
	JNIEnv *pEnv = 0;
	bool bRet = false;

	do
	{
		if (!getEnv(&pEnv))
		{
			break;
		}

		jclass classID = getClassID_(className, pEnv);

		methodID = pEnv->GetMethodID(classID, methodName, paramCode);
		if (!methodID)
		{
			LOGD("Failed to find method id of %s", methodName);
			break;
		}

		methodinfo.classID = classID;
		methodinfo.env = pEnv;
		methodinfo.methodID = methodID;

		bRet = true;
	} while (0);

	return bRet;
}

static string jstring2string_(jstring jstr)
{
	JNIEnv *env = 0;

	jboolean isCopy;
	if (!getEnv(&env))
	{
		return 0;
	}

	const char* chars = env->GetStringUTFChars(jstr, &isCopy);
	string ret(chars);
	if (isCopy)
	{
		env->ReleaseStringUTFChars(jstr, chars);
	}

	return ret;
}
}

namespace z11
{

JavaVM* JniHelper::m_psJavaVM = NULL;

std::vector<jclass> JniHelper::m_class_reference;
JavaVM* JniHelper::getJavaVM()
{
	return m_psJavaVM;
}

void JniHelper::setJavaVM(JavaVM *javaVM)
{
	m_psJavaVM = javaVM;
}

jclass JniHelper::getClassID(const char *className, JNIEnv *env)
{
	return getClassID_(className, env);
}

bool JniHelper::getStaticMethodInfo(JniMethodInfo &methodinfo,
		const char *className, const char *methodName, const char *paramCode)
{
	return getStaticMethodInfo_(methodinfo, className, methodName, paramCode);
}

bool JniHelper::getMethodInfo(JniMethodInfo &methodinfo, const char *className,
		const char *methodName, const char *paramCode)
{
	return getMethodInfo_(methodinfo, className, methodName, paramCode);
}

string JniHelper::jstring2string(jstring str)
{
	return jstring2string_(str);
}

AutoJni::AutoJni()
{
	env = NULL;
	isAttached = false;
	int status = JniHelper::getJavaVM()->GetEnv((void **) &env,
			JNI_VERSION_1_4);
	if (status < 0)
	{
		status = JniHelper::getJavaVM()->AttachCurrentThread(&env, NULL);
		if (status < 0)
		{
			return;
		}
		isAttached = true;
	}
}
AutoJni::~AutoJni()
{
	if (isAttached)
		JniHelper::getJavaVM()->DetachCurrentThread();
}
}
