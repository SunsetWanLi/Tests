//
//  MITouch.cpp
//  Gui
//
//  Created by Wang ZhiPeng on 11-5-22.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MITouch.h"
#include <vector>
#include "../Base/MIView.h"
#include <math.h>

using namespace std;

namespace z11
{
    MITouch::MITouch()
    {
        tag = INVALID_TOUCH_TAG;
        timestamp = MGTimeTicks::Now();
        point = MGPointMake(0.0f, 0.0f);
        screen_point = point;
        maintained_view = NULL;
        touched_view = NULL;
    }
    MITouch::MITouch(MITouch* touch)
    {
        timestamp = touch->timestamp;
        point = touch->point;
        screen_point = touch->screen_point;
        saved_point = touch->saved_point;
        tag = touch->tag;
        phase = touch->phase;
        maintained_view = touch->maintained_view;
        touched_view = touch->touched_view;
    }
    MITouch::~MITouch()
    {
        
    }
    MGPoint MITouch::locationInView(z11::MIView *view) const
    {
        MGPoint ret = screen_point;
        do {
            if (view == NULL) {
                ret = point;
                break;
            }
            vector<MIView*> view_tree;
            while (view != NULL) {
                view_tree.push_back(view);
                view = view->parent();
            }
            while (view_tree.size() > 0) {
                view = view_tree.back();
                view_tree.pop_back();
                view->translatePoint(ret);
                if(view_tree.size() > 0)
                    view->translateSubPoint(ret);
            }
        } while (0);
        return ret;
    }
    
    MITouch* MITouch::nearestTouch(const std::set<MITouch*> touches)
    {
        MITouch* ret = NULL;
        std::set<MITouch*>::const_iterator it;
        float distance = 0.0f;
        bool func_flag = true;
        for (it = touches.begin(); it != touches.end(); ++it) {
            float tmp_dis = sqrtf(pow((this->screen_point.x - (*it)->screen_point.x) , 2) + pow((this->screen_point.y - (*it)->screen_point.y), 2));
            if (tmp_dis < distance || func_flag) {
                func_flag = false;
                distance = tmp_dis;
                ret = *it;
            }
        }
        return ret;
    }
    
    float MITouch::nearestDis(const std::set<MITouch*> touches)
    {
        float ret = -1;
        std::set<MITouch*>::const_iterator it;
        float distance = 0.0f;
        bool func_flag = true;
        for (it = touches.begin(); it != touches.end(); ++it) {
            float tmp_dis = sqrtf(pow((this->screen_point.x - (*it)->screen_point.x) , 2) + pow((this->screen_point.y - (*it)->screen_point.y), 2));
            if (tmp_dis < distance || func_flag) {
                func_flag = false;
                distance = tmp_dis;
                ret = distance;
            }
        }
        return ret;
    }
}