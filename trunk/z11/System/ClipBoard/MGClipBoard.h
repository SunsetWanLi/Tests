#pragma once
#include "../../CommonBase/MGObject.h"
#include "../Shared/Syncable.h"
#include <string>

using namespace std;

namespace z11
{
	class MGClipBoard
	{
	public:
		static void setString(const string& str);
        static string getString();
        static bool hasData();
	private:
		static MGClipBoard *getInstance();
		MGClipBoard();
		static MGClipBoard *_instance;
        static Syncable clip_lock;
    private:
        string clip_str;
	};
}
