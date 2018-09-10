#pragma once
#include <vector>
#include "MGLocation.h"
#include <string>
#include "../Thread/MGThread.h"
namespace z11
{
	class MGTimer;
	class MKSimulateGPSReader
	{
	public:
		MKSimulateGPSReader(const std::string &path);
		void parseData(std::vector<z11::MGLocation> &coords);
		std::string m_simulate_gps_data;
		
	};

	class MKSimulateGPSReaderThreadController;
	class MKGPXParser
	{
	public:
		void parseData( const std::string &path,std::vector<MGLocation> &coords );
	};
	class MKSimulateGPSReaderThread:public z11::MGThread
	{
	public:
		MKSimulateGPSReaderThread(const std::string &simulate_gps_data);
		~MKSimulateGPSReaderThread();
		virtual bool   threadLoop();
		int simulate_speed;
	protected:
		virtual int    readyToRun();
		bool bFirst;
		std::string m_simulate_gps_data;
		z11::MGTimer *reader_timer;
		MKSimulateGPSReaderThreadController* controller;
	};
}