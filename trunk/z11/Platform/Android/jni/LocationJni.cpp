#include "LocationJni.h"
#include "JniHelper.h"

#include "../../../GUI/Events/MIEvent.h"
#include "../../../GUI/GuiContainer/GuiContainer.h"

#include "../../../System/LocationService/MGLocationManager.h"
#include "../../../System/LocationService/MGLocation.h"
//#include "../../../System/LocationService/MGHeading.h"
#include "../../../System/Shared/MGLog.h"
#include "../../../System/Shared/Utility.h"

#include <android/log.h>
#include <string.h>
#include <jni.h>
#include <assert.h>
#include <stdio.h>

#if 0
#define  LOG_TAG    "LocationJni"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...)
#endif

using namespace z11;
using namespace std;
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    // handle location message
    //////////////////////////////////////////////////////////////////////////
	void buildLocationFromString(MGLocation& to_location, std::string str_location);
	
    void startLocationService()
    {
		JniMethodInfo t;

		if (JniHelper::getStaticMethodInfo(t,
			"com/z11/mobile/framework/MGActivity",
			"startLocationService",
			"()V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
		}
    }

	void stopLocationService()
	{
		JniMethodInfo t;

		if (JniHelper::getStaticMethodInfo(t,
			"com/z11/mobile/framework/MGActivity",
			"stopLocationService",
			"()V"))
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
		}
	}
	

    void locationNativeUpdateWithNewLocationJNI(JNIEnv* env, jobject thiz, jstring to_location, jstring from_location)
    {
        jboolean isCopyTo = 0;
		jboolean isCopyFrom = 0;
        const char* str_to_location = env->GetStringUTFChars(to_location, &isCopyTo);
		const char* str_from_location = env->GetStringUTFChars(from_location, &isCopyFrom);

        if (isCopyTo && isCopyFrom)
        {
			MGLocation toLocation, fromLocation;
			buildLocationFromString(toLocation, str_to_location);
			buildLocationFromString(fromLocation, str_from_location);
			MGLocationManager::didUpdateLocation(toLocation, fromLocation);

            env->ReleaseStringUTFChars(to_location, str_to_location);
			env->ReleaseStringUTFChars(from_location, str_from_location);
        }
    }
    
    void locationNativeUpdateFailWithErrorJNI(JNIEnv* env, jobject thiz, jstring error)
    {
        jboolean isCopy = 0;
        const char* str_error = env->GetStringUTFChars(error, &isCopy);
        if (isCopy)
        {
			MGLocationManager::didUpdateLocationFailWithError(str_error);
            env->ReleaseStringUTFChars(error, str_error);
        }
    }

	void buildLocationFromString(MGLocation& to_location, std::string str_location)
	{
		std::vector<std::string> location;
		std::vector<std::string>::const_iterator location_it;
		location.clear();
		location = Utility::SplitString(str_location, ',');
		int index = 0;
		for (location_it=location.begin(); location_it!=location.end(); location_it++,index++)
		{
			switch(index)
			{
				case 0:
					sscanf((*location_it).c_str(), "%lf", &to_location.longitude);
					//to_location.longitude = Utility::BuiltInTypeFromString<double>(*location_it);
					break;
				case 1:
					sscanf((*location_it).c_str(), "%lf", &to_location.latitude);
					//to_location.latitude = Utility::BuiltInTypeFromString<double>(*location_it);
					break;
				case 2:
					sscanf((*location_it).c_str(), "%lf", &to_location.altitude);
					//to_location.altitude = Utility::BuiltInTypeFromString<double>(*location_it);
					break;
				case 3:
					sscanf((*location_it).c_str(), "%lf", &to_location.speed);
					//to_location.speed = Utility::BuiltInTypeFromString<double>(*location_it);
					break;
				case 4:
					sscanf((*location_it).c_str(), "%lf", &to_location.course);
					//to_location.course = Utility::BuiltInTypeFromString<double>(*location_it);
					break;
				case 5:
					sscanf((*location_it).c_str(), "%lf", &to_location.accuracy);
					//to_location.accuracy = Utility::BuiltInTypeFromString<double>(*location_it);
					break;
                case 6:
                {
                    int_64 timestamp = 0;
					sscanf((*location_it).c_str(), "%ll", &timestamp);
                    to_location.timestamp = MGTime::FromInternalValue(timestamp * MGTime::kMicrosecondsPerMillisecond);
					//to_location.accuracy = Utility::BuiltInTypeFromString<double>(*location_it);
                }
					break;
				default:
					break;
			}
			//MGLogD("location: %s", (*location_it).c_str());
		}
	}
	
}
