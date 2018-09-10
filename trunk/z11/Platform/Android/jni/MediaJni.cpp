#include "MediaJni.h"
#include "JniHelper.h"

#include "../../../System/Shared/MGLog.h"
#include "../../../CommonBase/MGData.h"
//#include "../../../GraphicCore/MGImage.h"
#include "../../../CommonBase/MGString.h"
#include "../../../CommonBase/MGDictionary.h"
#include "../../../System/MediaPicker/MGMediaPicker.h"
#include <android/log.h>
#include <string.h>
#include <jni.h>
#include <assert.h>
#include <stdio.h>
#include <vector>

#if 0
#define  LOG_TAG    "LocationJni"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...)
#endif

using namespace z11;
using namespace std;
z11::MGMediaPickerType *g_picker_type = NULL;
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    // handle camera message
    //////////////////////////////////////////////////////////////////////////

    void openMedia()
    {
		try {
            JniMethodInfo methodInfo;
            if(JniHelper::getStaticMethodInfo(methodInfo,
				"com/z11/mobile/framework/MGMediaPicker",
				"openMedia",
				"()V"))
            {
                MGLogD("FindClass success");
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID,methodInfo.methodID);
            }
            else
                MGLogD("FindClass Failed");
        }
        catch (std::exception &ex) {
            string s=ex.what();
            __android_log_print(ANDROID_LOG_INFO, "z11","%s",s.c_str());
        }
    }

	void closeMedia()
	{
		try {
			JniMethodInfo methodInfo;
			if (JniHelper::getStaticMethodInfo(methodInfo,
				"com/z11/mobile/framework/MGMediaPicker",
				"closeMedia",
				"()V"))
			{
				methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
			}
		}
		catch (std::exception &ex) {
			string s=ex.what();
			__android_log_print(ANDROID_LOG_INFO, "z11","%s",s.c_str());
		}
	}

	//JNIEXPORT jobject JNICALL 
	//Java_com_z11_mobile_framework_MGMediaPicker_getPickerType( JNIEnv*  env , jobject  thiz )
	jobject mediaGetPickerType( JNIEnv*  env , jobject  thiz )
	{
        if (g_picker_type == NULL) {
            return NULL;
        }
		try {
			jclass MediaTypeClass = env->FindClass("com/z11/mobile/framework/MGMediaPickerType");
			jmethodID methodID = env->GetMethodID(MediaTypeClass, "<init>", "()V");
			jobject obj = env->NewObject(MediaTypeClass, methodID);
			//jclass MediaTypeClass = env->GetObjectClass(thiz);
			jfieldID sourceTypeID = env->GetFieldID(MediaTypeClass, "sourceType", "I");
			jfieldID mediaQualityID = env->GetFieldID(MediaTypeClass, "mediaQuality", "I");
            jfieldID mediaLimitID = env->GetFieldID(MediaTypeClass, "mediaLimit", "I");
            jfieldID allowEditID = env->GetFieldID(MediaTypeClass, "allows_edit", "Z");
//			jfieldID widthID = env->GetFieldID(MediaTypeClass, "width", "I");
//			jfieldID heightID = env->GetFieldID(MediaTypeClass, "height", "I");
//			jfieldID filePathID = env->GetFieldID(MediaTypeClass, "filePath", "Ljava/lang/String;");
//			jfieldID fileNameID = env->GetFieldID(MediaTypeClass, "fileName", "Ljava/lang/String;");
			env->SetIntField(obj, sourceTypeID, g_picker_type->sourceType);
			env->SetIntField(obj, mediaQualityID, g_picker_type->mediaQuality);
            env->SetIntField(obj, mediaLimitID, g_picker_type->select_limit);
            env->SetBooleanField(obj, allowEditID, g_picker_type->allows_edit);
//			env->SetIntField(obj, widthID, g_picker_type->width);
//			env->SetIntField(obj, heightID, g_picker_type->height);
//			env->SetObjectField(obj, filePathID, env->NewStringUTF(g_picker_type->filePath.c_str()));
//			env->SetObjectField(obj, fileNameID, env->NewStringUTF(g_picker_type->fileName.c_str()));
			return obj;
		}
		catch (std::exception &ex) {
			string s=ex.what();
			__android_log_print(ANDROID_LOG_INFO, "z11","%s",s.c_str());
		}
        delete_and_set_null(g_picker_type);
	}

	//void Java_com_z11_mobile_framework_MGActivity_didFinishPickingImageWithInfo(JNIEnv* env, jobject thiz, jstring filepath, jbyteArray bytes, jint length)
	void meidaDidFinishPickingImageWithInfoAndFile(JNIEnv* env, jobject thiz, jstring filepath, jbyteArray bytes, jint length)
	{
		jboolean isCopy = 0;
		MGData* data = NULL;
		unsigned char* _data = (unsigned char*)env->GetByteArrayElements(bytes, &isCopy);
		if (_data != NULL)
		{
			data = MGData::dataWithBytesNoCopy(_data, length, false);
		}

		const char* file = env->GetStringUTFChars(filepath, &isCopy);
		MGString* mg_file = NULL;
		if (file != NULL)
		{
			mg_file = MGString::initWithSTLString(file);
		}

		std::vector<MGObject*> value;
		value.push_back(data);
		value.push_back(mg_file);
		std::vector<MGObject*> key;
		MGString* object_key1 = MGString::initWithSTLString("MGMediaPickerOriginalImageData");
		key.push_back(object_key1);
		MGString* object_key2 = MGString::initWithSTLString("MGMediaPickerOriginalImageFile");
		key.push_back(object_key2);
		MGDictionary *dictionary = MGDictionary::dictionaryWithObjectsForKeys(value, key);
		MGMediaPicker::didFinishPickingImageWithInfo(dictionary);


		if (_data != NULL)
		{
			env->ReleaseByteArrayElements(bytes, (jbyte*)_data, 0);
		}
		if (file != NULL)
		{
			env->ReleaseStringUTFChars(filepath, file);
		}
		delete_and_set_null(data);
		delete_and_set_null(object_key1);
		delete_and_set_null(object_key2);
		delete_and_set_null(mg_file);
		delete_and_set_null(dictionary);
	}

    //void Java_com_z11_mobile_framework_MGActivity_didFinishPickingImageWithInfo(JNIEnv* env, jobject thiz, jbyteArray bytes, jint length)
	void meidaDidFinishPickingImageWithInfo(JNIEnv* env, jobject thiz, jbyteArray bytes, jint length)
    {
		jboolean isCopy = 0;
		MGData* data = NULL;
		unsigned char* _data = (unsigned char*)env->GetByteArrayElements(bytes, &isCopy);
		if (_data != NULL)
		{
			//MGLogD("=========MGData::dataWithBytes=============");
			data = MGData::dataWithBytesNoCopy(_data, length, false);
			//env->ReleaseByteArrayElements(bytes, (jbyte*)_data, 0);
		}
		//MGImage* image = MGImage::imageWithData(data);
		std::vector<MGObject*> value;
		value.push_back(data);
		std::vector<MGObject*> key;
		MGString* object_key = MGString::initWithSTLString("MGMediaPickerOriginalImageData");
		key.push_back(object_key);
		MGDictionary *dictionary = MGDictionary::dictionaryWithObjectsForKeys(value, key);
		MGMediaPicker::didFinishPickingImageWithInfo(dictionary);

		if (_data != NULL)
		{
			//MGLogD("=========ReleaseByteArrayElements=============");
			env->ReleaseByteArrayElements(bytes, (jbyte*)_data, 0);
		}
		delete_and_set_null(data);
		//delete_and_set_null(image);
		delete_and_set_null(object_key);
		delete_and_set_null(dictionary);
    }

	//void Java_com_z11_mobile_framework_MGActivity_didFinishPickingImageWithFile(JNIEnv* env, jobject thiz, jstring filepath)
	void mediaDidFinishPickingMediaWithFile(JNIEnv* env, jobject thiz, jint type, jstring filepath)
    {
		jboolean isCopy = 0;
		int _type = type;
        const char* file = env->GetStringUTFChars(filepath, &isCopy);
		MGString* mg_file = MGString::initWithSTLString(file);
		MGData* data = NULL;
        if (file != NULL)
		{
			env->ReleaseStringUTFChars(filepath, file);
		}
		std::vector<MGObject*> value;
		value.push_back(mg_file);
		std::vector<MGObject*> key;
		MGString* object_key = NULL;
		MGDictionary* dictionary = NULL;
		switch (_type)
		{
		case MGMediaPickerSourceTypeImage:
			;
		case MGMediaPickerSourceTypePhotoLibrary:
			;
		case MGMediaPickerSourceTypeSavedPhotoAlbum:
			MGLogD("============Image==========");
			object_key = MGString::initWithSTLString("MGMediaPickerOriginalImageFile");
			key.push_back(object_key);
			dictionary = MGDictionary::dictionaryWithObjectsForKeys(value, key);
			MGMediaPicker::didFinishPickingImageWithInfo(dictionary);
			break;
		case MGMediaPickerSourceTypeSound:
			;
		case MGMediaPickerSourceTypeSoundLibrary:
			MGLogD("============Sound==========");
			object_key = MGString::initWithSTLString("MGMediaPickerOriginalSoundFile");
			key.push_back(object_key);
			dictionary = MGDictionary::dictionaryWithObjectsForKeys(value, key);
			MGMediaPicker::didFinishPickingSoundWithInfo(dictionary);
			break;
		case MGMediaPickerSourceTypeVideo:
			;
		case MGMediaPickerSourceTypeVideoLibrary:
			MGLogD("============Video==========");
			object_key = MGString::initWithSTLString("MGMediaPickerOriginalVideoFile");
			key.push_back(object_key);
			dictionary = MGDictionary::dictionaryWithObjectsForKeys(value, key);
			MGMediaPicker::didFinishPickingVideoWithInfo(dictionary);
			break;
		default:
			MGLogD("========MGMediaPickerOriginalUnknowType=========");
			break;
		}
		

		delete_and_set_null(object_key);
		delete_and_set_null(mg_file);
		delete_and_set_null(dictionary);
    }

	void mediaDidCancelPickingMeida(JNIEnv* env, jobject thiz, jint type)
    {
		int _type = type;
		switch (_type)
		{
		case MGMediaPickerSourceTypeImage:
			;
		case MGMediaPickerSourceTypePhotoLibrary:
			;
		case MGMediaPickerSourceTypeSavedPhotoAlbum:
			MGMediaPicker::didCancelPickingImageWithInfo(NULL);
			break;
		case MGMediaPickerSourceTypeSound:
			;
		case MGMediaPickerSourceTypeSoundLibrary:
			MGMediaPicker::didCancelPickingSoundWithInfo(NULL);
			break;
		case MGMediaPickerSourceTypeVideo:
			;
		case MGMediaPickerSourceTypeVideoLibrary:
			MGMediaPicker::didCancelPickingVideoWithInfo(NULL);
			break;
		default:
			MGLogD("========MGMediaPickerOriginalUnknowType=========");
			break;
		}
		
	}
}
