#ifndef GPS_DATA_RECORD_H_
#define GPS_DATA_RECORD_H_

// #include <fstream>
// using namespace std;
namespace IS_GIS
{
	struct GPS_Info;
	struct Pos2D;

	class GPSDataRecord
	{
	public:
		static void record(const GPS_Info & gps_info);
// 	private:
// 		static std::ofstream data_out;
	};
}
#endif