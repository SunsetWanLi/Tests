#include "MGFTGLESFontContextManager.h"
#include "MGFTGLESFont.h"
#include "../System/Shared/MGLog.h"
namespace z11
{
	MGFTGLESFontContextManager * MGFTGLESFontContextManager::getInstance()
	{
		static MGFTGLESFontContextManager manager;
		return &manager;
	}

	void MGFTGLESFontContextManager::clear()
	{
		semphore.Wait();
		MGFTGLESFont::recoveryContext();
		MGLogD("MGFTGLESFontContextManager::clear");
		semphore.Signal();
	}

	void MGFTGLESFontContextManager::push( MGFTGLESFont *_font )
	{
		semphore.Wait();
		m_fonts[_font]=1;
		semphore.Signal();
	}

	void MGFTGLESFontContextManager::pop( MGFTGLESFont *_font )
	{
		semphore.Wait();
		m_fonts[_font]=0;
		semphore.Signal();
	}

	MGFTGLESFontContextManager::MGFTGLESFontContextManager()
		:semphore(1)
	{

	}
}
