#include "MapConfigAdapter.h"
#include "DataReader.h"
#include "DataWriter.h"
#include <sstream>

using namespace z11;
namespace GIS_Embedded
{
	char MapConfigAdapter::tmp[1000];

	int MapConfigAdapter::translate(istream &ifstrm, MapConfigType ** map_config)
	{
		MapConfigType *config;
		config = new MapConfigType();
		getWstring(ifstrm,&(config->gps_com_port));
		getInt(ifstrm,&(config->gps_baunrate));
		getInt(ifstrm,&(config->gps_simulate));
		getInt(ifstrm,&(config->gps_need_read));
		getInt(ifstrm, &(config->gps_need_record));

		getVectorInt(ifstrm, &(config->scale_ranks));
		getInt(ifstrm, &(config->scale_pixel_width));
		getInt(ifstrm, &(config->initial_scale));
		getInt(ifstrm, &(config->query_resize_scale));
		getInt(ifstrm, &(config->navi_need));
		getInt(ifstrm, &(config->navi_scale));


		getDouble(ifstrm, &(config->cross_scope_width));
		getInt(ifstrm,&(config->show_3d_map));
		getInt(ifstrm, &(config->grid_side_divider));
		getInt(ifstrm,&(config->vector_map));
		getInt(ifstrm,&(config->raster_map));
		getWstring(ifstrm,&(config->raster_file));

		*map_config = config;

		return 0;
	}

	int MapConfigAdapter::getVectorInt(istream &ifstrm, vector<int> *ret)
	{
		ifstrm.getline(tmp, 1000);
		while(tmp[0] == L'\0' && !ifstrm.eof())
			ifstrm.getline(tmp, 1000);
		if(ifstrm.eof())
		{
			return 0;
		}

		istringstream iss(tmp);
		string item_name;
		iss>>item_name;
		int value;
		while(iss>>value)
		{
			ret->push_back(value);
		}
		return 0;
	}

	//int MapConfigAdapter::getVectorWstr(ifstream &ifstrm, vector<z11::MGWString> *ret)
	//{
	//	ifstrm.getline(tmp, 1000);
	//	while(tmp[0] == L'\0' && !ifstrm.eof())
	//		ifstrm.getline(tmp, 1000);
	//	if(ifstrm.eof())
	//	{
	//		return 0;
	//	}

	//	wistringstream iss(tmp);
	//	z11::MGWString item_name;
	//	iss>>item_name;
	//	z11::MGWString value;
	//	while(iss>>value)
	//	{
	//		ret->push_back(value);
	//	}
	//	return 0;
	//}

	int MapConfigAdapter::getInt(istream &ifstrm, int *ret)
	{
		ifstrm.getline(tmp, 100);
		while(tmp[0] == L'\0' && !ifstrm.eof())
		{
			ifstrm.getline(tmp, 100);
		}
		if(ifstrm.eof())
		{
			*ret = 0;
			return 0;
		}

		istringstream iss(tmp);
		string item_name;
		iss>>item_name;

		int value;
		iss>>value;
		*ret = value;
		return 0;

	}
	int MapConfigAdapter::getDouble(istream &ifstrm, double *ret)
	{
		ifstrm.getline(tmp, 100);
		while(tmp[0] == L'\0' && !ifstrm.eof())
			ifstrm.getline(tmp, 100);
		if(ifstrm.eof())
		{
			*ret = 0;
			return 0;
		}

		istringstream iss(tmp);
		string item_name;
		iss>>item_name;

		double value;
		iss>>value;
		*ret = value;
		return 0;
	}

	int MapConfigAdapter::getWstring(istream &ifstrm,string *ret)
	{
		ifstrm.getline(tmp, 1000);
		while(tmp[0] == '\0' && !ifstrm.eof())
			ifstrm.getline(tmp, 1000);
		if(ifstrm.eof())
		{
			*ret = "";
			return 0;
		}

		stringstream iss(tmp);
		string item_name;
		iss>>item_name;
		
		string value;
		iss>>value;
		*ret = value;
		
		return 0;
	}

	int MapConfigAdapter::getPos2D(istream &ifstrm, Pos2D *ret)
	{
		ifstrm.getline(tmp, 100);
		while(tmp[0] == L'\0' && !ifstrm.eof())
			ifstrm.getline(tmp, 100);
		if(ifstrm.eof())
		{
			*ret = Pos2D();
			return 0;
		}

		istringstream iss(tmp);
		string item_name;
		iss>>item_name;

		Pos2D value;
		iss>>value.d0>>value.d1;
		*ret = value;
		return 0;
	}

}