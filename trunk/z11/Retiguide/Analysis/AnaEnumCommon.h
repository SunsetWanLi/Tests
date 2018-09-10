#ifndef _GIS_ANA_ENUM_COMMON_H_
#define _GIS_ANA_ENUM_COMMON_H_

namespace GIS_Embedded
{

enum AnaPosVerdict
{
	APV_NOT_BOUND, // 未绑定到地图网络中
	APV_MAY_BOUND, // 可能绑定到地图网络中，目前未用
	//APV_BOUND,
	APV_DETOUR, // 绑定到地图网络中，但不在规划路径上
	//APV_ON_PATH,
	APV_DIRECTION_FORWARD, //在规划路径上，并且是向前行驶
	APV_DIRECTION_BACKWARD, //在规划路径上，并且是向前行驶
	APV_DIRECTION_UNSURE //在规划路径上，但是方向不确定
};

typedef enum
{
	DIRT_UNKNOWN = 0x0,// unkonwn
	DIRT_FORTH = 0x1, // the 1st bit for "forth"
	DIRT_RIGHT = 0x2, // the 2nd  bit for "right"
	DIRT_BACK = 0x4, // the 3rdbit for "back"
	DIRT_LEFT = 0x8, // the 4th bit for "left"
	DIRT_FORTH_RIGHT = DIRT_FORTH | DIRT_RIGHT,
	DIRT_RIGHT_BACK = DIRT_RIGHT | DIRT_BACK,
	DIRT_BACK_LEFT = DIRT_BACK | DIRT_LEFT,
	DIRT_LEFT_FORTH = DIRT_LEFT | DIRT_FORTH,
	DIRT_STOP = 0xf,  // stop

	CMD_START = 0x10, //start navigation
	CMD_REACHFINAL = 0x11,//reach final
	CMD_REACHTARGET = 0x12,//reach target
	CMD_GPSSIGNAL = 0x13, //gps signal info
	CMD_CALCOM = 0x14, //path caculation completed
	CMD_BUTTON = 0x15, //BUTTON 
	CMD_EXIT = 0x16,	//exit navigation
	CMD_CURVOL = 0x17,//current volume   lw
	CMD_KEEPGOSTRIGHT = 0x18, // keep go stright   lw
	CMD_FIXED_RADAR = 0X19,
	CMD_UNFIXED_RADAR = 0X1a,
	CMD_LIMITED_SPEED = 0x1b,
	CMD_CURRENT_SPEED = 0x1c,
	CMD_SPEEDING = 0x1d//have speeding
} DirectionType ;

}

#endif