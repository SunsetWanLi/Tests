#ifndef _CORE_RENDERENGINE_GEOLABELLINERENDER_H
#define _CORE_RENDERENGINE_GEOLABELLINERENDER_H

#include "GeoLabelRender.h"
#include "Retiguide/Common/Share/ScreenPos2D.h"
#include "../StyleRender.h"
#include "../../Map/ScreenPixMap.h"
#include <vector>
using namespace std;

namespace GIS_Embedded{
	class GeoLabelLineRender :public GeoLabelRender
{

private:
	static vector<int> _distance;

    static vector<ScreenPos2D> _label_ctrl_points;

	static int _longest_sub_segment_index;
	static double _longest_sub_segment_length;

	static vector<UserScreenRect> _rect_queue;
	static vector<ScreenPos2D> _pos_queue;
	static vector<std::string> _label_char_queue;
	static vector<int> _label_escape_queue;

    bool _reverse;

	bool _upright;
	
	void getLabelCtrlPoints();

    void computeCtrlPointsDistance();

	//计算标记文字起始点（左上角位置）
    void computeScreenPos(ScreenPos2D& p1, ScreenPos2D& p2, int distance, int label_height, double radian, ScreenPos2D *pos);

	void drawStringByCharacter(StyleRender& mg, ScreenPixMap& screen_pix_map);

	void renderChineseLabel(StyleRender& mg, ScreenPixMap& screen_pix_map);

	void renderEnLabel(StyleRender &mg,ScreenPixMap &screen_pix_map);

public:

	GeoLabelLineRender(){}
    
    void render(StyleRender& mg, ScreenPixMap& screen_pix_map);

	//static int checkLabelToRender(wstring& label,ScreenPixMap& screen_pix_map);


};
}
#endif