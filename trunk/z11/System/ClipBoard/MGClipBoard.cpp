#include "MGClipBoard.h"
#include "../../CommonBase/CommonDef.h"

using namespace std;

namespace z11
{
	MGClipBoard *MGClipBoard::_instance = NULL;
    Syncable MGClipBoard::clip_lock;
    
	MGClipBoard * z11::MGClipBoard::getInstance()
	{
        Syncable::AutoLock lock(&clip_lock);
		if (_instance==NULL)
		{
			_instance=new MGClipBoard();

		}
		return _instance;
	}

	z11::MGClipBoard::MGClipBoard()
	{
        Syncable::AutoLock lock(&clip_lock);
	}
    
    void z11::MGClipBoard::setString(const string &str)
    {
        Syncable::AutoLock lock(&clip_lock);
        getInstance()->clip_str = str;
    }
    
    string z11::MGClipBoard::getString()
    {
        Syncable::AutoLock lock(&clip_lock);
        return getInstance()->clip_str;
    }
    
    bool z11::MGClipBoard::hasData()
    {
        Syncable::AutoLock lock(&clip_lock);
        if (getInstance()->clip_str.length() > 0) {
            return true;
        }
        return false;
    }
}

