#pragma once
#include <string>
namespace z11
{

	class MKKMLStyle
	{
	public:
		MKKMLStyle(void);
		virtual ~MKKMLStyle(void);
		std::string style_id;
	};

	class MKKMLIconStyle:public MKKMLStyle
	{
	public:
		std::string icon_href;
	};
}