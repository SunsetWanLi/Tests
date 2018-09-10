//yzy,qzy
//040823
//No name space, compatible with ATL idl

#ifndef _BASIC_GEO_TYPE_CORE_
#define _BASIC_GEO_TYPE_CORE_

typedef unsigned int GeoLayerType;

const GeoLayerType GLT_UNKNOWN = 0x00000000 ;
const GeoLayerType GLT_SIMPLE = 0x00000001 ;
const GeoLayerType GLT_NETWORK = 0x00000010 ;
const GeoLayerType GLT_TOPOLOGY = 0x00000100 ;
const GeoLayerType GLT_OBJECT = GLT_SIMPLE | GLT_NETWORK | GLT_TOPOLOGY ;


const GeoLayerType GLT_T = 0x00001000 ;

typedef unsigned int GeoFclsType;

const GeoFclsType GFT_UNKNOWN =		0x00000000 ;
//for general
const GeoFclsType GFT_DEFAULT =		0x80000000 ;
const GeoFclsType GFT_TEXT    =		0x00000001;//Set to "1" for compatibility!
//...
//for network
const GeoFclsType GFT_NETWORK =		0x40000000 ;
const GeoFclsType GFT_NW_NODE =		0x40000001 ;
const GeoFclsType GFT_NW_EDGE =		0x40000002 ;
//for topology
const GeoFclsType GFT_TOPOLOGY =	0x20000000 ;
//...
//for gis_t
const GeoFclsType GFT_T	=			0x10000000 ;
//...

const GeoFclsType GFT_ROAD =			0x00000010 ;
const GeoFclsType GFT_ROADPART =		0x00000011 ;

const GeoFclsType GFT_ROADCOMPONENT =	0X00000020 ;
const GeoFclsType GFT_LANE =			0x00000020 ;
const GeoFclsType GFT_SCRIBELINE =		0x00000021 ;
const GeoFclsType GFT_VIRESCENCEAREA =	0x00000022 ;
const GeoFclsType GFT_ROADSHOULDER =	0x00000023 ;
const GeoFclsType GFT_FOOTPATH =		0x00000024 ;
const GeoFclsType GFT_HANDRAIL =		0x00000025 ;
const GeoFclsType GFT_ROADCOMPONENTCUSTOM = 0x00000026 ;
//const GeoFclsType GFT_MILESTONE	 =			0x00000027 ;

const GeoFclsType GFT_ROADOBJ	 =		0x00000040 ;
const GeoFclsType GFT_STATICPOINT =		0x00000040 ;
const GeoFclsType GFT_ROADEVENT =		0x00000041 ;
const GeoFclsType GFT_ROADFOG =			0x00000042 ;
const GeoFclsType GFT_ROADREPAIR =		0x00000043 ;
const GeoFclsType GFT_ROADACCIDENT =	0x00000044 ;
const GeoFclsType GFT_ROADCUSTOM =		0x00000045 ;
const GeoFclsType GFT_ROADFACILITY =	0x00000046 ;
const GeoFclsType GFT_BRIDGE =			0x00000047 ;
const GeoFclsType GFT_TUNNEL =			0x00000048 ;
const GeoFclsType GFT_BLINKPOINT =		0x00000049 ;
const GeoFclsType GFT_ROADHOUSE =		0x0000004a ;
const GeoFclsType GFT_INSTRUMENT =		0x0000004b ;
const GeoFclsType GFT_RAMP =			0x0000004c ;
const GeoFclsType GFT_ROADCENTEREXIT =	0x0000004d ;
const GeoFclsType GFT_CUSTOMFACILITY =	0x0000004e ;

const GeoFclsType GFT_RCOBJ =			0x00000080 ;
const GeoFclsType GFT_RCPOINT =			0x00000080 ;
const GeoFclsType GFT_RCSEGMENT =		0x00000081 ;
const GeoFclsType GFT_RCAGGREGATE =		0x00000082 ;

#endif //#ifndef _BASIC_GEO_TYPE_CORE_


