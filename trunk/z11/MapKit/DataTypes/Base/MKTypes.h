/*
 * =====================================================================================
 *
 *       Filename:  MKTypes.h
 *
 *    Description:  define Map Kit Data Types
 *
 *        Version:  1.0
 *        Created:  04/11/2012 03:15:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma  once
namespace z11
{

	enum MKMapType
	{
		MKMapTypeStandard,
		MKMapTypeSatellite,
		MKMapTypeHybrid,
        MKMapTypeTopographic,
		MKMapTypeCustom,
		MKMapTypeCustomTraffic,
        MKMapTypeCustomTile,
        MKMapTypeCustomWebTile,
		MKMapTypeYDJWTile,		
		MKMapTypeOsm,
		MKMapTypeTiandiVec,
		MKMapTypeTiandiImg,
		MKMapTypeTiandiTer,
		MKMapTypeCount
	};
	enum MKMapProvider
	{
		MKMapProviderGoogle,
		MKMapProviderBing,
		MKMapProviderOSM,
		MKMapProviderTiandi,
        MKMapProviderCustom
	};
	enum{
		MKMapShiftNone=0,
		MKMapShiftGoogle = 1,
		MKMapShiftBaidu = 1<<1,
		MKMapShiftBing = 1<<2,
		MKMapShiftOSM = 1<<3
	};
	typedef unsigned int  MKMapShiftType;
#define MAP_CELL_SIZE 256
}
