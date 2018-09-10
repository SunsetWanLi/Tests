#ifndef __ANDROID_MEDIA_JNI_H__
#define __ANDROID_MEDIA_JNI_H__

#include <string>
namespace z11
{
	class MGMediaPickerType;
}
extern "C"

{
    extern void openMedia();
	extern void closeMedia();
}
extern z11::MGMediaPickerType *g_picker_type;
#endif // __ANDROID_MEDIA_JNI_H__
