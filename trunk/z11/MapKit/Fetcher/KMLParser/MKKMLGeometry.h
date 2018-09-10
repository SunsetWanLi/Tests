#pragma once
#include <string>
namespace z11
{
	class MKKMLGeometry
	{
	public:
		MKKMLGeometry(void);
		virtual ~MKKMLGeometry(void);
	};

	class MKKMLPoint:public MKKMLGeometry
	{
	public:
		MKKMLPoint();
		void read(const std::string &str);
		double longitude;
		double latitude;
		double altitude;
	};

}