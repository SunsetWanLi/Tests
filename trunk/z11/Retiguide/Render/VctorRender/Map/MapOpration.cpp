#include "MapOpration.h"
#include "../../../../System/Shared/MGLog.h"
using namespace z11;
namespace GIS_Embedded
{
	int MapOpration::SCALE_WIDTH;
	int MapOpration::SCALE_SIZE;
	vector<int> MapOpration::SCALE;
	MapTerrCtrl *MapOpration::TERR_CTRL;
	int MapOpration::SCALE_CURRENT;

	void MapOpration::init(int scale_width,vector<int> &scale_ranks,MapTerrCtrl &terr_ctrl)
	{
		SCALE_WIDTH = scale_width;
		SCALE_SIZE = scale_ranks.size();
		for(int i=0;i<SCALE_SIZE;i++)
			SCALE.push_back(scale_ranks[i]);
		TERR_CTRL = &terr_ctrl;
	}

	float MapOpration::ZoomIn(MapView *map_view)
	{
        float zoom_ret=SCALE_CURRENT;
		Scope& view_scope = map_view->m_view_scope;
		double view_width = view_scope.getWidth();
		int index = 0;
		int ret = 0;

		{
			while(index<SCALE_SIZE && view_width > SCALE[index]*map_view->getScreenPixWidth()/(double)SCALE_WIDTH-10)
				index++;
			index --;
			if(index>0&&(index<SCALE_SIZE))
			{
				double new_width = SCALE[index-1]*map_view->getScreenPixWidth()/(double)SCALE_WIDTH;
				double new_height = new_width*map_view->getScreenPixHeight()/map_view->getScreenPixWidth();
				double view_h_center = (view_scope.getTop()+view_scope.getBottom())/2;
				double view_w_center = (view_scope.getLeft() + view_scope.getRight())/2;
				view_scope.setBottom(view_h_center - new_height/2);
				view_scope.setTop(view_h_center + new_height/2);
				view_scope.setLeft(view_w_center - new_width/2);
				view_scope.setRight(view_w_center + new_width/2);
				ret = SCALE[index-1];
				MapFrame *map_frame = map_view->getMapFrame();
				TERR_CTRL->change(view_scope,map_frame);
				RenderFlagAdjuster::adjust(*map_frame,SCALE[index-1]);
				SCALE_CURRENT = ret;
				map_view->setScaleRank(SCALE_CURRENT);
				map_view->m_vector_map->setScaleRank(SCALE_CURRENT);
                
                zoom_ret = SCALE_CURRENT/zoom_ret;
			}
            else {
                return 0;
            }

		}

//		map_view->m_vector_map->setViewScope(view_scope);
//		map_view->m_vector_map->getLayerDataInScope();
//		map_view->m_vector_map->m_need_redraw=true;

		//return ret;
		if (zoom_ret<10e-4||SCALE_CURRENT<1)
		{
			return 0;
		}
		else
		{
			//zoom_ret=SCALE_CURRENT/zoom_ret;
			return zoom_ret;
		}
	}

	float MapOpration::ZoomOut(MapView *map_view)
	{
		float zoom_ret= SCALE_CURRENT;
		Scope& view_scope = map_view->m_view_scope;
		double view_width = view_scope.getWidth();

		int index = 0;
		int ret = 0;

		{
			while(index<SCALE_SIZE && view_width > SCALE[index]*map_view->getScreenPixWidth()/(double)SCALE_WIDTH-10)
				index++;
            index--;
			if(index< SCALE_SIZE-1)
			{
				double new_width = SCALE[index+1]*map_view->getScreenPixWidth()/(double)SCALE_WIDTH;
				double new_height = new_width*map_view->getScreenPixHeight()/map_view->getScreenPixWidth();
				double view_h_center = (view_scope.getTop()+view_scope.getBottom())/2;
				double view_w_center = (view_scope.getLeft() + view_scope.getRight())/2;
				view_scope.setBottom(view_h_center - new_height/2);
				view_scope.setTop(view_h_center + new_height/2);
				view_scope.setLeft(view_w_center - new_width/2);
				view_scope.setRight(view_w_center + new_width/2);
				ret = SCALE[index+1];
				MapFrame *map_frame = map_view->getMapFrame();
				TERR_CTRL->change(view_scope,map_frame);
				RenderFlagAdjuster::adjust(*map_frame,SCALE[index+1]);

				SCALE_CURRENT = ret;
				map_view->setScaleRank(SCALE_CURRENT);
				map_view->m_vector_map->setScaleRank(SCALE_CURRENT);
                MGLogD("SCALE %d = %d",index+1,SCALE[index+1]);
			}
            else {
                return 0;
            }

		}

//		map_view->m_vector_map->setViewScope(view_scope);
//		map_view->m_vector_map->getLayerDataInScope();
//		map_view->m_vector_map->m_need_redraw=true;
		//return ret;
		if (zoom_ret<10e-4||SCALE_CURRENT<1)
		{
			return 0;
		}
		else
		{
			zoom_ret=SCALE_CURRENT/zoom_ret;
			return zoom_ret;
		}
	}

//	int MapOpration::Resizer(MapView* map_view,int width, int height)
//	{	
//		Scope &view_scope = map_view->m_view_scope;
//
//		double ratio = (double)width/(double)height;
//
//		double view_width;
//		if((view_scope.getWidth()/view_scope.getHeight())>ratio)
//		{
//			view_width = view_scope.getWidth();
//		}
//		else if((view_scope.getWidth()/view_scope.getHeight())<ratio)
//		{
//			double view_height = view_scope.getHeight();
//			view_width = view_height*ratio;
//
//		}
//
//		int index = 0;
//		while(index<SCALE_SIZE && view_width > SCALE[index]*map_view->getScreenPixWidth()/(double)SCALE_WIDTH-10)
//			index++;
//
//		if(index>0)
//			index --;
//
//		double new_width = SCALE[index]*map_view->getScreenPixWidth()/(double)SCALE_WIDTH;
//		double new_height = new_width*map_view->getScreenPixHeight()/map_view->getScreenPixWidth();
//		double view_h_center = (view_scope.getTop()+view_scope.getBottom())/2;
//		double view_w_center = (view_scope.getLeft() + view_scope.getRight())/2;
//		view_scope.setBottom((int)(view_h_center - new_height/2));
//		view_scope.setTop((int)(view_h_center + new_height/2));
//		view_scope.setLeft((int)(view_w_center - new_width/2));
//		view_scope.setRight((int)(view_w_center + new_width/2));
//
//		MapFrame *map_frame = map_view->getMapFrame();
//
//		TERR_CTRL->change(view_scope,map_frame);
//
//		RenderFlagAdjuster::adjust(*map_frame,SCALE[index]);
//
//		SCALE_CURRENT = SCALE[index];
//		map_view->setScaleRank(SCALE_CURRENT);
//// 		map_view->getLayerDataInScope();
//
//		return SCALE[index];
//
//	}

	void MapOpration::Locater(MapView *map_view,double x,double y)
	{
		Scope& view_scope = map_view->m_view_scope;

		int x_dis = x-(view_scope.getLeft() + view_scope.getRight())/2;
		int y_dis = y - (view_scope.getBottom() + view_scope.getTop())/2;

		moveXY(map_view,-x_dis,y_dis);
	}
    void MapOpration::fetchData(MapView *map_view)
    {
        map_view->m_vector_map->setViewScope(map_view->m_view_scope);
		map_view->m_vector_map->getLayerDataInScope();
		map_view->m_vector_map->m_need_redraw=true;
    }

	void MapOpration::moveXY(MapView* map_view,double x, double y)
	{

		Scope& view_scope = map_view->m_view_scope;

		{
			view_scope.setLeft(view_scope.getLeft()-x);
			view_scope.setRight(view_scope.getRight()-x);
			view_scope.setBottom(view_scope.getBottom()+y);
			view_scope.setTop(view_scope.getTop()+y);
		}

		
//        map_view->m_vector_map->setViewScope(map_view->m_view_scope);
//		map_view->m_vector_map->getLayerDataInScope();
//		map_view->m_vector_map->m_need_redraw=true;
		MapFrame *map_frame = map_view->getMapFrame();
		TERR_CTRL->change(view_scope,map_frame);
		RenderFlagAdjuster::adjust(*map_frame,map_view->getScaleRank());
		map_view->centerSubView(map_view->m_vector_map);
	}

	void MapOpration::moveX(MapView* map_view,double x)
	{
		Scope& view_scope = map_view->m_view_scope;
		view_scope.setLeft(view_scope.getLeft()+x);
		view_scope.setRight(view_scope.getRight()+x);
		MapFrame *map_frame = map_view->getMapFrame();
		TERR_CTRL->change(view_scope,map_frame);
		RenderFlagAdjuster::adjust(*map_frame,map_view->getScaleRank());
// 		map_view->getLayerDataInScope();
	}

	void MapOpration::moveY(MapView* map_view,double y)
	{
		Scope& view_scope = map_view->m_view_scope;
		view_scope.setBottom(view_scope.getBottom()+y);
		view_scope.setTop(view_scope.getTop()+y);
		MapFrame *map_frame = map_view->getMapFrame();	
		TERR_CTRL->change(view_scope,map_frame);
		RenderFlagAdjuster::adjust(*map_frame,map_view->getScaleRank());
// 		map_view->getLayerDataInScope();
	}
// 	//现在框选直接可以在mapview里做
// 	void MapOpration::rectSelect(MapView* map_view,Scope &scope,vector<Pos2DEx>& points,vector<std::string>& names,bool flag)
// 		//flag用于确定是否框选所有图层或者只是可显示图层，现在未实现
// 	{
// 		map_view->rectSelect(scope,points,names,flag);
// 	}

}