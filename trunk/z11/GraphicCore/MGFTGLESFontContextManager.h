#pragma once


#include <map>
#include "../System/Thread/MGSemaphore.h"
namespace z11
{
	class MGFTGLESFont;
	class MGFTGLESFontContextManager
	{
	public:
		static MGFTGLESFontContextManager *getInstance();
		void clear();
		void push(MGFTGLESFont *_font);
		void pop(MGFTGLESFont *_font);
	private:
		MGFTGLESFontContextManager();
		std::map<MGFTGLESFont *,int> m_fonts;
		MGSemaphore semphore;
	};

}