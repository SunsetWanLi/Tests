#pragma once
#include <string>
namespace z11
{
	class MKKMLSound
	{
	public:
		MKKMLSound(const std::string &sound_url);
		std::string link_href;
	};

}