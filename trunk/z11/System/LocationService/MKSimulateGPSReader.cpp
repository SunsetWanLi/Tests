#include "MKSimulateGPSReader.h"
#include "../../CommonBase/MGData.h"
#include "../../ExternalLib/xml/rapidxml.hpp"
#include "../../ExternalLib/xml/rapidxml_utils.hpp"
#include "../../ExternalLib/xml/rapidxml_print.hpp"
#include "../IO/MGBundle.h"
#include "../Timer/MGTimer.h"
#include "../Shared/MGLog.h"
#include "../../MapKit/DataTypes/Base/MKGeometry.h"
#include "../../GUI/Controllers/MIViewController.h"
#include "MGLocationManager.h"
#include "../Shared/Utility.h"
#include "../../GUI/GuiContainer/GuiContainer.h"
using namespace rapidxml;
using namespace z11;
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 
#endif
namespace z11
{
	
void MKSimulateGPSReader::parseData( std::vector<MGLocation> &coords )
{
	std::string path_name=m_simulate_gps_data;
	file<> fdoc(path_name.c_str());
	xml_document<>  *doc=new xml_document<>;
	doc->parse<0>(fdoc.data());
	xml_node<> *node=doc->first_node();//<Configuration>
	node=node->first_node();//<Location>
	for (;node!=NULL;node=node->next_sibling())
	{
		MGLocation location;
		sscanf(node->first_node("Longitude")->value(),"%lf",&location.longitude);
		sscanf(node->first_node("Latitude")->value(),"%lf",&location.latitude);
		sscanf(node->first_node("Altitude")->value(),"%lf",&location.altitude);
		sscanf(node->first_node("Speed")->value(),"%lf",&location.speed);
		sscanf(node->first_node("Bearing")->value(),"%lf",&location.course);
		coords.push_back(location);
	}
}

MKSimulateGPSReader::MKSimulateGPSReader( const std::string &path )
{
	m_simulate_gps_data=path;
}


#define SIMULATED_GPS_EVENT USER_CUSTOMER_EVENT+123
static int offset;
class MKSimulateGPSReaderThreadController:public MIViewController
{
public:
	void init(const std::string &path)
	{
		if (Utility::hasExtName(path,"xml"))
		{
			MKSimulateGPSReader reader(path);
			reader.parseData(coords);
		}
		else if (Utility::hasExtName(path,"gpx"))
		{
			MKGPXParser reader;
			reader.parseData(path,coords);

		}
		index=0;
		offset=1;
	}

	void updateLocation(MGObject* object)
	{
		//MGLogD("MKSimulateGPSReaderThread::updateLocation");
		if (index-1>=0&&index<(int)coords.size())
		{
			if (offset>0)
			{
				MGLocationManager::didUpdateLocation(coords[index], coords[index-1]);
			}
			else if(offset<0)
			{
				MGLocationManager::didUpdateLocation(coords[index-1], coords[index]);
			}
		}
        else if(index>=(int)coords.size()){
			offset=-1;
        }
		else if(index<1)
		{
			offset=1;
		}

		index+=offset;
	}
private:
	int index;
	std::vector<z11::MGLocation> coords;
};

int MKSimulateGPSReaderThread::readyToRun()
{
	controller->init(m_simulate_gps_data);
	//{
	//	MIEvent _event;
	//	_event._event_type = SIMULATED_GPS_EVENT;
	//	_event._content = NULL;
	//	_event._responder = &controller;
	//	SELHandler selector;
	//	selector.sender = &controller;
	//	selector.sel_func = (SELFunc)&MKSimulateGPSReaderThreadController::updateLocation;
	//	_event._handler = selector;
	//	reader_timer = MGTimer::timerWithTimeInterval(MGTimeDelta::FromSecondsF(2), _event, true);
	//}

	return 0;
}



bool MKSimulateGPSReaderThread::threadLoop()
{
	MGSleep(simulate_speed);
	MIEvent *event = new MIEvent();
	event->_responder = controller;
	event->_handler.sel_func = (SELFunc)&MKSimulateGPSReaderThreadController::updateLocation;
	event->_handler.sender = controller;
	GuiContainer::container->preserveEvent(event);
	//controller.updateLocation(&controller);
	return true;
}
MKSimulateGPSReaderThread::MKSimulateGPSReaderThread(const std::string &simulate_gps_data)
{
	bFirst=true;
	m_simulate_gps_data=simulate_gps_data;
	simulate_speed = 1000;
	controller = new MKSimulateGPSReaderThreadController();
}

MKSimulateGPSReaderThread::~MKSimulateGPSReaderThread()
{
}
void MKGPXParser::parseData( const std::string &path,std::vector<MGLocation> &coords )
{
	std::string path_name=path;
	//file<> fdoc(path_name.c_str());
	MGData *data=MGData::dataWithContentsOfFile(path_name);
	xml_document<>  *doc=new xml_document<>;
	doc->parse<0>((char *)data->bytes());

	xml_node<> *node=doc->first_node();//<gpx>
	node=node->first_node();//<trk>
	//<trkseg>
	int i=0;
	for(node=node->first_node("trkseg");node!=NULL;node=node->next_sibling("trkseg"))
	{
		xml_node<> *trkpt=NULL;
		for (trkpt=node->first_node("trkpt");trkpt!=NULL;trkpt=trkpt->next_sibling("trkpt"))
		{
			MGLocation location;
			sscanf(trkpt->first_attribute("lat")->value(),"%lf",&location.latitude);
			sscanf(trkpt->first_attribute("lon")->value(),"%lf",&location.longitude);
			static double speed=0;
			if(i%6)
				speed=100+(40*(rand()%10000)/10000.0); 
			location.speed=115/3.6;
			if (i>0)
			{
				//location.course=120;
				location.course=90-atan2((location.latitude-coords[i-1].latitude),(location.longitude-coords[i-1].longitude))/M_PI*180;
				if (location.course<0)
				{
					location.course+=360;
				}
			}
			else
			{
				location.course=0;
			}
			
			coords.push_back(location);
			i++;
		}
	}
	delete_and_set_null(data);
}

}