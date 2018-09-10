//
//  MGGraphicEngine.cpp
//  GraphicCore
//
//  Created by Wang ZhiPeng on 11-6-20.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MGGraphicEngine.h"
#include "../CommonBase/MGStructure.h"
#include "MGFont.h"
#include "MGImage.h"
#include <math.h>
#include "FreeTypeDefinition.h"
#include "../System/Shared/Utility.h"
#include "FreeTypeFont.h"
#include "../System/Shared/MGLog.h"
#include "../System/Device/MGDevice.h"
#include "../System/IO/MGBundle.h"
#include "MGPersistenceImage.h"
#include "MGFTGLESFont.h"
#include "../ExternalLib/ftgles/FTGL/ftgles.h"
#ifndef _ROUND_RECT_PARAM_
#define _ROUND_RECT_PARAM_ 16
#endif //_ROUND_RECT_PARAM_
#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif
#ifndef M_PI_2
#define M_PI_2      1.57079632679489661923132169163975144   /* pi/2           */
#endif
#include "MGModel.h"
#include <bitset>
#include <algorithm>
//#include "../ExternalLib/GLUES/glues.h"
template<class T>
inline void * get_address_of_vector(T t)
{
    return &t[0];
}

const float eps=1e-6;
namespace z11 {
    
#pragma mark - MGGraphicEngine Constructor -
    // MGGraphicEngine
    MGGraphicEngine* MGGraphicEngine::singleton = NULL;
    
    MGGraphicEngine::MGGraphicEngine()
    {
        clip_stack.clear();
        vtContext.clear();
    }
    MGGraphicEngine::~MGGraphicEngine()
    {
        singleton = NULL;
    }
    MGGraphicEngine* MGGraphicEngine::sharedGraphicEngine()
    {
        if (singleton == NULL) {
            singleton = new MGGraphicEngine();
        }
        return singleton;
    }
    
#pragma mark - Utility tools Function -
    
    void DebugOpenGLES()
    {
        GLenum err = glGetError();
        if (err != 0) {
            //MGLog(MGLog::MGERROR, "OpenGLES got an error : %u.", err);
        }
    }
    
    float dotproduct(const MGPoint& a,const MGPoint &b)
	{
		return a.x*b.y-a.y*b.x;
	}
	
	void addTriangle(vector<float> & triangles,const MGPoint & pt)
	{
        float height = MGDevice::getScreenHeight();
		//MGLogD("add triangle line %f %f",pt.x,pt.y);
		triangles.push_back(pt.x);
		triangles.push_back(height-pt.y);
		triangles.push_back(0.0f);
	}
	MGPoint pointVectorMake(const MGPoint &a,const MGPoint&pt)
	{
		return MGPointMake(pt.x-a.x,pt.y-a.y);
        
	}
	bool insideTriangle(const MGPoint&a,const MGPoint&b,const MGPoint&c,const MGPoint&pt)
	{
		MGPoint pa=pointVectorMake(a,pt);
		MGPoint pb=pointVectorMake(b,pt);
		MGPoint pc=pointVectorMake(c,pt);
        
		return dotproduct(pa,pb)<0&&dotproduct(pb,pc)<0&&dotproduct(pc,pa)<0;
	}
    
	const vector<MGPoint> *g_this_points=NULL;
	bool cmp(size_t a,size_t b)
	{
		//const MGPoint& pt1=g_this_points->at(a);
		//const MGPoint& pt2=g_this_points->at(b);
		//if(pt1.y!=pt2.y)
		return g_this_points->at(a).y<=g_this_points->at(b).y;
	}
    
#pragma mark - Test Drawing Function -
    
    void drawFilledPolyLoopLines2(const vector<MGPoint> &points,RGBAColorRef color, float size, RGBAColorRef blend)
    {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glLineWidth(size);
        
		vector<float> triangles;
		vector<size_t> pos;
		for(size_t i=0;i<points.size();i++)
			pos.push_back(i);
		bitset< 1024 > isConcavPoints;
		vector<size_t> concav_points_pos;
		int i;
		for(i=0;i<(int)pos.size()-2;i++)
		{
			MGPoint ab =pointVectorMake(points[i],points[i+1]);
			MGPoint bc =pointVectorMake(points[i+1],points[i+2]);
			if(dotproduct(ab,bc)>0)
			{
				isConcavPoints.set(i+1);
				concav_points_pos.push_back(i+1);
			}
		}
		for(;i<(int)pos.size();i++)
		{
			int i1=(i+1)%(pos.size());
			int i2=(i+2)%(pos.size());
			MGPoint ab =pointVectorMake(points[pos[i]],points[pos[i1]]);
			MGPoint bc =pointVectorMake(points[pos[i1]],points[pos[i2]]);
			if(dotproduct(ab,bc)>0)
			{
				isConcavPoints.set(i1);
				concav_points_pos.push_back(i1);
			}
		}
		
		while(pos.size()>3)
		{
			bool change=false;
			for(int i=0;i<(int)pos.size();)
			{
				int i1=(i+1)%(pos.size());
				int i2=(i+2)%(pos.size());
				MGPoint ab =pointVectorMake(points[pos[i]],points[pos[i1]]);
				MGPoint bc =pointVectorMake(points[pos[i1]],points[pos[i2]]);
				bool flag=true;
				//if(!isConcavPoints.test(pos[i+1]))
				if(dotproduct(ab,bc)<0)
				{
					for(size_t j=(i+3)%pos.size();j<pos.size();j++)
					{
						if(insideTriangle(points[pos[i]],points[pos[i1]],points[pos[i2]],points[pos[j]]))
						{
							flag=false;
							break;
						}
					}
				}
				else
					flag=false;
				if(flag)
				{
					for(int j=0;j<3;j++)
						addTriangle(triangles,points[pos[(i+j)%pos.size()]]);
					pos.erase(pos.begin()+(i+1)%pos.size());
					change=true;
                    
				}
				else
				{
					i++;
				}
			}
			if (!change) //prevent from die loop
			{
				break;
			}
			
		}
		
		if(pos.size()==3)
		{
			int i=0;
			for(int j=0;j<3;j++)
				addTriangle(triangles,points[pos[i+j]]);
		}
        
        float *vertics = new float[triangles.size()];
        uint_16 count = 0;
        for (vector<float>::const_iterator it = triangles.begin(); it != triangles.end(); ++it) {
            vertics[count++] = *it;
        }
        
        glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)(vertics));
        glEnableClientState(GL_VERTEX_ARRAY);
        
        DebugOpenGLES();
        glDrawArrays(GL_TRIANGLES, 0, count/3);
        
        glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_BLEND);
        delete[] vertics;
    }
    
    void drawFilledPolyLoopLines3(const vector<MGPoint> &points,RGBAColorRef color, float size, RGBAColorRef blend)
    {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
        glLineWidth(size);
        
		vector<size_t> reorder;
		for(size_t i=0;i<points.size();i++)
			reorder.push_back(i);
		g_this_points=&points;
		sort(reorder.begin(),reorder.end(),cmp);
		for(size_t i=0;i<reorder.size();i++)
			MGLogD("points %d = %f ,%f",i,points[reorder[i]].x,points[reorder[i]].y);
		vector<float> triangles;
		for(size_t i=0;i<reorder.size();i++)
		{
			//for(size_t j=0;j<3;j++)
            addTriangle(triangles,points[reorder[i]]);
		}
        //float *vertics = new float[points.size()*3];
        float *vertics = new float[triangles.size()];
		uint_16 i = 0;
        //for(int  i=0;i<triangles.size();++i)
        for(int  i=0;i<9;++i)
		{
			vertics[i]=triangles[i];
		}
        
		glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)(vertics));
        glEnableClientState(GL_VERTEX_ARRAY);
        
        DebugOpenGLES();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, i/3);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_BLEND);
        delete[] vertics;
    }
    
    RGBAColorRef TestIncreaseColor(const RGBAColorRef& color,float alpha)
    {
		if (cos(alpha)<0)
		{
			return RGBAColorMake(0,0,0,color.m_alpha_component);
		}
        return RGBAColorMake(color.m_red_component*cos(alpha)*cos(alpha),
                             color.m_green_component*cos(alpha)*cos(alpha),
                             color.m_blue_component*cos(alpha)*cos(alpha),
                             color.m_alpha_component);
	}
    
    void testOffsetColor(GLfloat colors[4],float dist,const RGBAColorRef &origin,GLfloat offset[])
    {
        colors[0]=(origin.m_red_component+offset[0]*dist)/255.0f;
        colors[1]=(origin.m_green_component+offset[1]*dist)/255.0f;
        colors[2]=(origin.m_blue_component+offset[2]*dist)/255.0f;
        colors[3]=(origin.m_alpha_component+offset[3]*dist)/255.0f;
    }
    
    void TestDrawImage(const MGImage *const image, const MGRect &rect, RGBAColorRef blend, DrawImageMode mode, const MGRect& image_rect)
    {
        if (image == NULL) {
            return;
        }
        if (image->content_info.width_in_pixel <= 0 || image->content_info.height_in_pixel <= 0) {
            return;
        }
        
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(blend.m_red_component / 255.0f, blend.m_green_component / 255.0f, blend.m_blue_component / 255.0f, blend.m_alpha_component / 255.0f);
        MGGraphicEngineContext context = image->context;
        glBindTexture(GL_TEXTURE_2D, context.texture_id);
        float rate_x = 1.0f;
        float rate_y = 1.0f;
        float start_x = 0.0f;
        float start_y = 0.0f;
        MGRect dst_rect = rect;
        switch(mode)
        {
            case ImageScaleToFill:
            {
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x =1.0f;
                    rate_y =1.0f;
                }else{
                    rate_x =1.0f* image->getWidth() / image->getAdapterWidth();
                    rate_y =1.0f* image->getHeight() / image->getAdapterHeight();
                }
                break;
            }
            case ImageAspectFill:
            {
                rate_x = rect.width / image->content_info.width_in_pixel;
                rate_y = rect.height / image->content_info.height_in_pixel;
                float rate = (rate_x < rate_y) ? rate_y : rate_x;
                if (rate == rate_x) {
                    start_y = (rate * image->content_info.height_in_pixel - rect.height) / 2;
                    start_y /= image->content_info.height_in_pixel;
                } else if (rate == rate_y)
                {
                    start_x = (rate * image->content_info.width_in_pixel - rect.width) / 2;
                    start_x /= image->content_info.width_in_pixel;
                }
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x =1.0f;
                    rate_y =1.0f;
                }else{
                    rate_x =1.0f* image->getWidth() / image->getAdapterWidth();
                    rate_y =1.0f* image->getHeight() / image->getAdapterHeight();
                }
                break;
            }
            case ImageAspectFit:
            {
                rate_x = rect.width / image->content_info.width_in_pixel;
                rate_y = rect.height / image->content_info.height_in_pixel;
                float rate = (rate_x < rate_y) ? rate_x : rate_y;
                if (rate == rate_x) {
                    dst_rect.height = rate * image->content_info.height_in_pixel;
                    dst_rect.y += (rect.height - dst_rect.height) / 2;
                } else if (rate == rate_y)
                {
                    dst_rect.width = rate * image->content_info.width_in_pixel;
                    dst_rect.x += (rect.width - dst_rect.width) / 2;
                }
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x =1.0f;
                    rate_y =1.0f;
                }else{
                    rate_x =1.0f* image->getWidth() / image->getAdapterWidth();
                    rate_y =1.0f* image->getHeight() / image->getAdapterHeight();
                }
                break;
            }
            case ImageCustomFit:
            {
                start_x = image_rect.x;
                start_y = image_rect.y;
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x =1.0f;
                    rate_y =1.0f;
                }else{
                    rate_x =1.0f* image->getWidth() / image->getAdapterWidth();
                    rate_y =1.0f* image->getHeight() / image->getAdapterHeight();
                }
                rate_x *= image_rect.width / rect.width;
                rate_y *= image_rect.height / rect.height;
                break;
            }
            default:
            {
            }
        }
        MGRect rounded_rect=dst_rect;
        MGRect rounded_texture_rect=MGRectMake(start_x,rate_y,rate_x,start_y-rate_y);
        //MGLogD("rounded_rect=%f,%f,%f,%f",rounded_rect.x,rounded_rect.y,                rounded_rect.width,rounded_rect.height);
        //MGLogD("IMAGE=%d,%d",image->getWidth(),image->getHeight());
        //MGLogD("rounded_texture_rect=%f,%f,%f,%f",start_x,start_y,rate_x,rate_y);
        MGModel *rounded_model=MGModel::createRoundedRect(rounded_rect,10);
        //MGModel *rounded_texture_model=MGModel::createRoundedRect(rounded_texture_rect, 10.0f/image->getAdapterWidth()*(rate_x-start_x),10.0f/image->getAdapterHeight()*(rate_y-start_y));
        MGModel *rounded_texture_model=MGModel::createRoundedRect(rounded_texture_rect, 10.0f/image->getAdapterWidth(),10.0f/image->getAdapterHeight());
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_FLOAT, 0, rounded_texture_model->getVertics());
        rounded_model->draw();
        delete rounded_model;
        delete rounded_texture_model;
        
        
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        
    }
    
    void TestDrawFilledRoundRect2(const MGRect& rect, RGBAColorRef color , RGBAColorRef color2,float round_rate , RGBAColorRef blend, DrawRoundRectMode round_mode)
    {
        if (rect.height < (round_rate + round_rate)) {
            round_rate = rect.height / 2.0f;
        }
        if (round_rate <= 0.1) {
            return;
        }
        float height = MGDevice::getScreenHeight();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if (blend == RGBAColorRef::white()) {
			glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        } else {
			color=blend;
			color2=blend;
            //glColor4f(blend.m_red_component/255.0f, blend.m_green_component/255.0f, blend.m_blue_component/255.0f, blend.m_alpha_component/255.0f);
        }
        MGPoint centers[4]=
        {
            MGPointMake(rect.x + round_rate, rect.y + round_rate),
            MGPointMake(rect.x + round_rate, rect.y + rect.height - round_rate),
            MGPointMake(rect.x + rect.width - round_rate, rect.y + rect.height - round_rate),
            MGPointMake(rect.x + rect.width - round_rate, rect.y + round_rate),
        };
        float step = M_PI/2 / _ROUND_RECT_PARAM_;
        GLfloat     roundColor[_ROUND_RECT_PARAM_+2][4];
        float coloroffset[] = {static_cast<float>(color2.m_red_component-color.m_red_component),
            static_cast<float>(color2.m_green_component-color.m_green_component),
            static_cast<float>(color2.m_blue_component-color.m_blue_component),
            static_cast<float>(color2.m_alpha_component-color.m_alpha_component)};
        for (int j=0;j<4;j++)
        {
            MGVertex    roundVertics[_ROUND_RECT_PARAM_+2];
            float alpha = M_PI/2*(1+j);
			bool isRound = false;
            MGPoint center = centers[j];
			if ((round_mode & RoundRectLeftTop) && j == 0) {
				isRound = true;
			}
			else if ((round_mode & RoundRectLeftBottom) && j == 1) {
				isRound = true;
			}
			else if ((round_mode & RoundRectRightBottom) && j == 2) {
				isRound = true;
			}
			else if ((round_mode & RoundRectRightTop) && j == 3) {
				isRound = true;
			}
            roundVertics[0]=MGVertexMake(center.x,height-center.y);
            testOffsetColor(roundColor[0],(roundVertics[0].y-rect.y)/rect.height,color,coloroffset);
            //除去起始和结束的两个点不需要计算，需要计算的点数量是_ROUND_RECT_PARAM_ - 1 //
            for (int i = 1; i < _ROUND_RECT_PARAM_+2; ++i, alpha += step) {
				float tmp_alpha = alpha;
				if (!isRound) {
					if (alpha > (M_PI/2*(2+j) - M_PI*2/_ROUND_RECT_PARAM_)) {
						tmp_alpha = (M_PI/2*(2+j));
						roundVertics[i]=(MGVertexMake(center.x+round_rate*cos(tmp_alpha), height-(center.y-round_rate*sin(tmp_alpha))));
					} else if (alpha < (M_PI/2*(1+j) + M_PI*2/_ROUND_RECT_PARAM_)) {
						tmp_alpha = (M_PI/2*(1+j));
						roundVertics[i]=(MGVertexMake(center.x+round_rate*cos(tmp_alpha), height-(center.y-round_rate*sin(tmp_alpha))));
					} else {
						roundVertics[i]=(MGVertexMake(center.x+round_rate*(cos(tmp_alpha)/fabs(cos(tmp_alpha))), height-(center.y-round_rate*sin(tmp_alpha)/fabs(sin(tmp_alpha)))));
					}
				} else {
					roundVertics[i]=(MGVertexMake(center.x+round_rate*cos(tmp_alpha), height-(center.y-round_rate*sin(tmp_alpha))));
				}
				//roundVertics[i]=(MGVertexMake(center.x+round_rate*cos(tmp_alpha), center.y-round_rate*sin(tmp_alpha)));
                testOffsetColor(roundColor[i],((height-roundVertics[i].y)-rect.y)/rect.height,color,coloroffset);
            }
            glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)roundVertics);
            glEnableClientState(GL_VERTEX_ARRAY);
            
            glColorPointer(4,GL_FLOAT,0,(void *)roundColor);
            glEnableClientState(GL_COLOR_ARRAY);
            DebugOpenGLES();
            glDrawArrays(GL_TRIANGLE_FAN, 0, _ROUND_RECT_PARAM_+2);
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
        
        MGVertex vertics[12];
        vertics[0] = MGVertexMake(rect.x + round_rate, height-rect.y);
        vertics[1] = MGVertexMake(rect.x + round_rate, height-(rect.y + round_rate));
        vertics[2] = MGVertexMake(rect.x + rect.width - round_rate, height-(rect.y + round_rate));
        vertics[3] = MGVertexMake(rect.x + rect.width - round_rate, height-rect.y);
        vertics[4] = MGVertexMake(rect.x, height-(rect.y + round_rate));
        vertics[5] = MGVertexMake(rect.x, height-(rect.y + rect.height - round_rate));
        vertics[6] = MGVertexMake(rect.x + rect.width, height-(rect.y + rect.height - round_rate));
        vertics[7] = MGVertexMake(rect.x + rect.width, height-(rect.y + round_rate));
        vertics[8] = MGVertexMake(rect.x + round_rate, height-(rect.y + rect.height - round_rate));
        vertics[9] = MGVertexMake(rect.x + round_rate, height-(rect.y + rect.height ));
        vertics[10] = MGVertexMake(rect.x + rect.width - round_rate, height-(rect.y + rect.height ));
        vertics[11] = MGVertexMake(rect.x + rect.width - round_rate, height-(rect.y + rect.height - round_rate));
        
        
        glShadeModel (GL_SMOOTH);
        GLfloat colors[12][4];
        for(int i=0;i<12;i++)
        {
            testOffsetColor(colors[i],((height-vertics[i].y)-rect.y)/rect.height,color,coloroffset);
        }
        glColorPointer(4,GL_FLOAT,0,colors);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vertics);
        
        DebugOpenGLES();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
        glColorPointer(4,GL_FLOAT,0,&colors[4]);
        
        glVertexPointer(3, GL_FLOAT, 0, &vertics[4]);
        
        DebugOpenGLES();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
        
        glColorPointer(4,GL_FLOAT,0,&colors[8]);
        glVertexPointer(3, GL_FLOAT, 0, &vertics[8]);
        
        DebugOpenGLES();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        
        
        glDisable(GL_BLEND);
        
		MGGraphicEngine::sharedGraphicEngine()->drawRoundRect(rect,color,round_rate,1.0f,blend,round_mode);
    }
	
	void TestDrawImageCurve(float update_time,const MGImage *const image, const MGRect &rect, RGBAColorRef blend, DrawImageMode mode, const MGRect& image_rect,int order)
	{
		//TestDrawImage(image,rect,blend,mode,image_rect);
		//return ;
		if (image == NULL) {
			return;
		}
		if (image->content_info.width_in_pixel <= 0 || image->content_info.height_in_pixel <= 0) {
			return;
		}
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(blend.m_red_component / 255.0f, blend.m_green_component / 255.0f, blend.m_blue_component / 255.0f, blend.m_alpha_component / 255.0f);
		MGGraphicEngineContext context = image->context;
		GLint activeTextureID;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &activeTextureID);
		glBindTexture(GL_TEXTURE_2D, context.texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (glGetError()!=0)
		{
			MGLogD("draw image glDrawArrays");
		}
		float rate_x = 1.0f;
		float rate_y = 1.0f;
		float start_x = 0.0f;
		float start_y = 0.0f;
		MGRect dst_rect = rect;
		switch(mode)
		{
            case ImageScaleToFill:
			{
				if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
					rate_x =1.0f;
					rate_y =1.0f;
				}else{
					rate_x =1.0f* image->getWidth() / image->getAdapterWidth();
					rate_y =1.0f* image->getHeight() / image->getAdapterHeight();
				}
				break;
			}
            case ImageAspectFill:
			{
				rate_x = rect.width / image->content_info.width_in_pixel;
				rate_y = rect.height / image->content_info.height_in_pixel;
				float rate = (rate_x < rate_y) ? rate_y : rate_x;
				if (rate == rate_x) {
					start_y = (rate * image->content_info.height_in_pixel - rect.height) / 2;
					start_y /= image->content_info.height_in_pixel;
				} else if (rate == rate_y)
				{
					start_x = (rate * image->content_info.width_in_pixel - rect.width) / 2;
					start_x /= image->content_info.width_in_pixel;
				}
				if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
					rate_x =1.0f;
					rate_y =1.0f;
				}else{
					rate_x =1.0f* image->getWidth() / image->getAdapterWidth();
					rate_y =1.0f* image->getHeight() / image->getAdapterHeight();
				}
				break;
			}
            case ImageAspectFit:
			{
				rate_x = rect.width / image->content_info.width_in_pixel;
				rate_y = rect.height / image->content_info.height_in_pixel;
				float rate = (rate_x < rate_y) ? rate_x : rate_y;
				if (rate == rate_x) {
					dst_rect.height = rate * image->content_info.height_in_pixel;
					dst_rect.y += (rect.height - dst_rect.height) / 2;
				} else if (rate == rate_y)
				{
					dst_rect.width = rate * image->content_info.width_in_pixel;
					dst_rect.x += (rect.width - dst_rect.width) / 2;
				}
				if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
					rate_x =1.0f;
					rate_y =1.0f;
				}else{
					rate_x =1.0f* image->getWidth() / image->getAdapterWidth();
					rate_y =1.0f* image->getHeight() / image->getAdapterHeight();
				}
				break;
			}
            case ImageCustomFit:
			{
				start_x = image_rect.x;
				start_y = image_rect.y;
				if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
					rate_x =1.0f;
					rate_y =1.0f;
				}else{
					rate_x =1.0f* image->getWidth() / image->getAdapterWidth();
					rate_y =1.0f* image->getHeight() / image->getAdapterHeight();
				}
				rate_x *= image_rect.width / rect.width;
				rate_y *= image_rect.height / rect.height;
				break;
			}
            default:
			{
			}
		}
        float height = MGDevice::getScreenHeight();
		MGVertex vertics[4];
		vertics[0] = MGVertexMake(dst_rect.x, height-dst_rect.y);
		vertics[1] = MGVertexMake(dst_rect.x, height-(dst_rect.y + dst_rect.height));
		vertics[2] = MGVertexMake(dst_rect.x + dst_rect.width, height-(dst_rect.y + dst_rect.height));
		vertics[3] = MGVertexMake(dst_rect.x + dst_rect.width, height-dst_rect.y);
		MGPoint texture_coor[4];
		if (order == 0)
		{
			texture_coor[0] = MGPointMake(start_x, rate_y);
			texture_coor[1] = MGPointMake(start_x, start_y);
			texture_coor[2] = MGPointMake(rate_x, start_y);
			texture_coor[3] = MGPointMake(rate_x, rate_y);
		}
		else if (order == 1)
		{
			texture_coor[1] = MGPointMake(start_x, rate_y);
			texture_coor[0] = MGPointMake(start_x, start_y);
			texture_coor[3] = MGPointMake(rate_x, start_y);
			texture_coor[2] = MGPointMake(rate_x, rate_y);
            
		}
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertics);
        
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texture_coor);
        
        DebugOpenGLES();
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindTexture(GL_TEXTURE_2D, activeTextureID);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}

#pragma mark - Interface for GUI -
    
#pragma mark - Base Drawing Function -
    // Interface for GUI
    void MGGraphicEngine::drawLine(const MGPoint &first, const MGPoint &second,RGBAColorRef color, float size, RGBAColorRef blend)
    {
        float height = MGDevice::getScreenHeight();
        glEnable(GL_BLEND);
        glLineWidth(size);
        const float line_points[6] =
        {
            first.x, height - first.y, 0.0f,
            second.x, height - second.y, 0.0f
        };
        glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        glVertexPointer(3, GL_FLOAT, 0, line_points);
        glEnableClientState(GL_VERTEX_ARRAY);

        DebugOpenGLES();
        glDrawArrays(GL_LINES, 0, 2);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_BLEND);
    }

    void MGGraphicEngine::drawPolyLines(const vector<MGPoint>& points,RGBAColorRef color, float size, RGBAColorRef blend)
    {
        float height = MGDevice::getScreenHeight();
		glEnable(GL_BLEND);
        glLineWidth(size);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        float *line_points = new float[points.size()*3];
        uint_16 i = 0;
        for (vector<MGPoint>::const_iterator it = points.begin(); it != points.end(); ++it) {
            line_points[i] = it->x;
            line_points[i+1] = height-it->y;
            line_points[i+2] = 0.0;
            i += 3;
        }

        glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)line_points);
        glEnableClientState(GL_VERTEX_ARRAY);

        DebugOpenGLES();
		glDrawArrays(GL_LINE_STRIP, 0, i/3);
        
        glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_BLEND);
        delete[] line_points;
    }

    void MGGraphicEngine::drawPolyLoopLines(const vector<MGPoint> &points,RGBAColorRef color, float size, RGBAColorRef blend)
    {
        float height = MGDevice::getScreenHeight();
        glLineWidth(size);
        float *vertics = new float[points.size()*3];
        uint_16 i = 0;
        for (vector<MGPoint>::const_iterator it = points.begin(); it != points.end(); ++it) {
            vertics[i] = it->x;
            vertics[i+1] = height-it->y;
            vertics[i+2] = 0;
			i += 3;
        }
		glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)(vertics));
        glEnableClientState(GL_VERTEX_ARRAY);

        DebugOpenGLES();
        glDrawArrays(GL_LINE_LOOP, 0, i/3);
        glDisableClientState(GL_VERTEX_ARRAY);
        delete[] vertics;
    }

    void MGGraphicEngine::drawFilledPolyLoopLines(const vector<MGPoint> &points,RGBAColorRef color, float size, RGBAColorRef blend)
    {
        float height = MGDevice::getScreenHeight();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
        glLineWidth(size);
        float *vertics = new float[points.size()*3];
        uint_16 i = 0;
        for (vector<MGPoint>::const_iterator it = points.begin(); it != points.end(); ++it) {
            vertics[i] = it->x;
            vertics[i+1] = height-it->y;
            vertics[i+2] = 0;
            i += 3;
        }
        glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)(vertics));
        glEnableClientState(GL_VERTEX_ARRAY);

        DebugOpenGLES();
        glDrawArrays(GL_TRIANGLE_FAN, 0, i/3);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        delete[] vertics;
		glDisable(GL_BLEND);
    }

    void MGGraphicEngine::drawPoint(const MGPoint &point,RGBAColorRef color,float size, RGBAColorRef blend)
    {
        float height = MGDevice::getScreenHeight();
        glEnable(GL_BLEND);
        glDisable(GL_POINT_SMOOTH);
        //glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);

        glPointSize(size);
        MGVertex vertex = MGVertexMake(point.x, height-point.y);
        glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        glVertexPointer(3, GL_FLOAT, 0, &vertex);
        glEnableClientState(GL_VERTEX_ARRAY);

        DebugOpenGLES();
        glDrawArrays(GL_POINTS, 0, 1);

        glDisableClientState(GL_VERTEX_ARRAY);
        //glDisable(GL_POINT_SMOOTH_HINT);
        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_BLEND);
        
    }

    void MGGraphicEngine::drawPoints(const vector<MGPoint> &points,RGBAColorRef color,float size, RGBAColorRef blend)
    {
        float height = MGDevice::getScreenHeight();
        glEnable(GL_POINT_SMOOTH);
        glPointSize(size);
        vector<MGVertex> vertics(points.size());
        uint_16 i = 0;
        for (vector<MGPoint>::const_iterator it = points.begin(); it != points.end(); ++it, ++i) {
            vertics[i] = MGVertexMake(it->x, height-it->y);
        }
        glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)get_address_of_vector(vertics));
        glEnableClientState(GL_VERTEX_ARRAY);

        DebugOpenGLES();
        glDrawArrays(GL_POINTS, 0, i);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_POINT_SMOOTH);
        
    }

    void MGGraphicEngine::drawRect(const MGRect &rect,RGBAColorRef color, float size, RGBAColorRef blend)
    {
        float height = MGDevice::getScreenHeight();
        MGVertex vertics[4];
        vertics[0] = MGVertexMake(rect.x, height-rect.y);
        vertics[1] = MGVertexMake(rect.x, height-(rect.y + rect.height));
        vertics[2] = MGVertexMake(rect.x + rect.width, height-(rect.y + rect.height));
        vertics[3] = MGVertexMake(rect.x + rect.width, height-rect.y);
		glLineWidth(size);
        glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        glVertexPointer(3, GL_FLOAT, 0, vertics);
        glEnableClientState(GL_VERTEX_ARRAY);

        DebugOpenGLES();
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    void MGGraphicEngine::drawRoundRect(const MGRect &rect,RGBAColorRef color, float round_rate, float size, RGBAColorRef blend, DrawRoundRectMode round_mode)
    {
        float height = MGDevice::getScreenHeight();
        if (rect.width < (round_rate + round_rate)) {
            round_rate = rect.width / 2.0f;
        }
        if (round_rate <= 0.1) {
            drawRect(rect, color);
            return;
        }
        
        glEnable(GL_BLEND);
        //DebugOpenGLES();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //DebugOpenGLES();
		glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
        //DebugOpenGLES();
		glLineWidth(size);
        glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);

        MGPoint centers[4]=
		{
			MGPointMake(rect.x + round_rate, rect.y + round_rate),
			MGPointMake(rect.x + round_rate, rect.y + rect.height - round_rate),
			MGPointMake(rect.x + rect.width - round_rate, rect.y + rect.height - round_rate),
			MGPointMake(rect.x + rect.width - round_rate, rect.y + round_rate),

        };
        float step = M_PI_2 / _ROUND_RECT_PARAM_;
        MGVertex vertics[4*(_ROUND_RECT_PARAM_+1)+1];
		int start=0;
        for(int j=0;j<4;j++)
		{
			bool isRound = false;
			MGPoint center = centers[j];
            float alpha = M_PI_2*(j+1);
			if ((round_mode & RoundRectLeftTop) && j == 0) {
				isRound = true;
			}
			if ((round_mode & RoundRectLeftBottom) && j == 1) {
				isRound = true;
			}
			if ((round_mode & RoundRectRightBottom) && j == 2) {
				isRound = true;
			}
			if ((round_mode & RoundRectRightTop) && j == 3) {
				isRound = true;
			}
			for (int i = 0; i <= _ROUND_RECT_PARAM_ ; ++i, alpha += step) {
				float tmp_alpha = alpha;
				if (!isRound) {
					if (alpha > (M_PI_2*(2+j) - M_PI*2/_ROUND_RECT_PARAM_)) {
						tmp_alpha = (M_PI_2*(2+j));
						vertics[start++]=(MGVertexMake(center.x+round_rate*cos(tmp_alpha), height-(center.y-round_rate*sin(tmp_alpha))));
					} else if (alpha < (M_PI_2*(1+j) + M_PI*2/_ROUND_RECT_PARAM_)) {
						tmp_alpha = (M_PI_2*(1+j));
						vertics[start++]=(MGVertexMake(center.x+round_rate*cos(tmp_alpha), height-(center.y-round_rate*sin(tmp_alpha))));
					} else {
						vertics[start++]=(MGVertexMake(center.x+round_rate*(cos(tmp_alpha)/fabs(cos(tmp_alpha))), height-(center.y-round_rate*sin(tmp_alpha)/fabs(sin(tmp_alpha)))));
					}
				} else {
					vertics[start++]=(MGVertexMake(center.x+round_rate*cos(tmp_alpha), height-(center.y-round_rate*sin(tmp_alpha))));
				}
				//vertics[start++]=(MGVertexMake(center.x+round_rate*cos(alpha), center.y-round_rate*sin(alpha)));
			}
		}
		vertics[start]=vertics[0];
		glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)get_address_of_vector(vertics));
		glEnableClientState(GL_VERTEX_ARRAY);
        
        DebugOpenGLES();
		glDrawArrays(GL_LINE_STRIP, 0, start  + 1 );

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_BLEND);
    }

	void MGGraphicEngine::drawRoundAbovedRect(const MGRect &rect,RGBAColorRef color, float round_rate, float size, RGBAColorRef blend)
	{
		drawRoundRect(rect, color, round_rate, size, blend, RoundRectTop);
	}

	void MGGraphicEngine::drawRoundBelowedRect(const MGRect &rect,RGBAColorRef color, float round_rate, float size, RGBAColorRef blend)
	{
		drawRoundRect(rect, color, round_rate, size, blend, RoundRectBottom);
	}

    void MGGraphicEngine::drawFilledRect(const MGRect &rect,RGBAColorRef color, RGBAColorRef blend)
    {
        float height = MGDevice::getScreenHeight();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        MGVertex vertics[4];
        vertics[0] = MGVertexMake(rect.x, height-rect.y);
        vertics[1] = MGVertexMake(rect.x, height-(rect.y + rect.height));
        vertics[2] = MGVertexMake(rect.x + rect.width, height-(rect.y + rect.height));
        vertics[3] = MGVertexMake(rect.x + rect.width, height-rect.y);
        if (blend == RGBAColorRef::white()) {
            glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        } else {
            glColor4f(blend.m_red_component/255.0f, blend.m_green_component/255.0f, blend.m_blue_component/255.0f, blend.m_alpha_component/255.0f);
        }
        glVertexPointer(3, GL_FLOAT, 0, vertics);
        glEnableClientState(GL_VERTEX_ARRAY);

        DebugOpenGLES();
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_BLEND);
    }

	void MGGraphicEngine::drawFilledRoundAbovedRect(const MGRect &rect,RGBAColorRef color, float round_rate, RGBAColorRef blend)
	{
		TestDrawFilledRoundRect2(rect,color,color,round_rate,blend, RoundRectTop);
	}

	void MGGraphicEngine::drawFilledRoundBelowedRect(const MGRect &rect,RGBAColorRef color, float round_rate, RGBAColorRef blend)
	{
		TestDrawFilledRoundRect2(rect,color,color,round_rate,blend, RoundRectBottom);
	}

	void MGGraphicEngine::drawFilledRoundRect(const MGRect &rect,RGBAColorRef color, float round_rate, RGBAColorRef blend, DrawRoundRectMode round_mode)
    {
        if(round_rate < 0.1)
        {
            drawFilledRect(rect, color, blend);
            return;
        }
        
		TestDrawFilledRoundRect2(rect,color,color,round_rate,blend, round_mode);
		return ;

        float height = MGDevice::getScreenHeight();
        if (rect.height < (round_rate + round_rate)) {
            round_rate = rect.height / 2.0f;
        }
        if (round_rate <= 0.1) {
            drawFilledRect(rect, color);
            return;
        }
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if (blend == RGBAColorRef::white()) {
            glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        } else {
            glColor4f(blend.m_red_component/255.0f, blend.m_green_component/255.0f, blend.m_blue_component/255.0f, blend.m_alpha_component/255.0f);
        }

        float step = round_rate / _ROUND_RECT_PARAM_;
        { //vector left_top
            MGVertex *vertics=new MGVertex[3*_ROUND_RECT_PARAM_];
            float count = step;
            MGPoint tmp_point = MGPointMake(rect.x + round_rate, rect.y + round_rate);
            vertics[0]=(MGVertexMake(tmp_point.x, height-tmp_point.y));
            vertics[1]=(MGVertexMake(tmp_point.x, height-rect.y));
            //除去起始和结束的两个点不需要计算，需要计算的点数量是_ROUND_RECT_PARAM_ - 1 //
            for (int i = 0; i < _ROUND_RECT_PARAM_ - 1; ++i, count += step) {
                float r_h = sqrtf((round_rate * round_rate) - (count * count));
                vertics[2+i*3]=(MGVertexMake(tmp_point.x - count, height-(tmp_point.y - r_h)));
                vertics[3+i*3]=(MGVertexMake(tmp_point.x - count, height-(tmp_point.y - r_h)));
                vertics[4+i*3]=(MGVertexMake(tmp_point.x, height-tmp_point.y));
            }
            vertics[3*_ROUND_RECT_PARAM_-1]=(MGVertexMake(rect.x, height-tmp_point.y));

            glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)vertics);
            glEnableClientState(GL_VERTEX_ARRAY);
            
            DebugOpenGLES();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 3*_ROUND_RECT_PARAM_);
            glDisableClientState(GL_VERTEX_ARRAY);
            delete_array_and_set_null(vertics);
        }
        { //vector left_bottom
            MGVertex *vertics=new MGVertex[3*_ROUND_RECT_PARAM_];
            float count = round_rate - step;
            MGPoint tmp_point = MGPointMake(rect.x + round_rate, rect.y + rect.height - round_rate);
            vertics[0]=(MGVertexMake(tmp_point.x, height-tmp_point.y));
            vertics[1]=(MGVertexMake(rect.x, height-tmp_point.y));
            //除去起始和结束的两个点不需要计算，需要计算的点数量是_ROUND_RECT_PARAM_ - 1 //
            for (int i = 0; i < _ROUND_RECT_PARAM_ - 1; ++i, count -= step) {
                float r_h = sqrtf((round_rate * round_rate) - (count * count));
                vertics[2+i*3]=(MGVertexMake(tmp_point.x - count, height-(tmp_point.y + r_h)));
                vertics[3+i*3]=(MGVertexMake(tmp_point.x - count, height-(tmp_point.y + r_h)));
                vertics[4+i*3]=(MGVertexMake(tmp_point.x, height-tmp_point.y));
            }
            vertics[3*_ROUND_RECT_PARAM_-1]=(MGVertexMake(tmp_point.x, height-(rect.y + rect.height)));

            glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)vertics);
            glEnableClientState(GL_VERTEX_ARRAY);
            
            DebugOpenGLES();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, _ROUND_RECT_PARAM_ * 3);
            glDisableClientState(GL_VERTEX_ARRAY);
            delete_array_and_set_null(vertics);
        }
        { //vector right_bottom
            MGVertex *vertics=new MGVertex[3*_ROUND_RECT_PARAM_];
            float count = step;
            MGPoint tmp_point = MGPointMake(rect.x + rect.width - round_rate, rect.y + rect.height - round_rate);
            vertics[0]=(MGVertexMake(tmp_point.x, height-tmp_point.y));
            vertics[1]=(MGVertexMake(tmp_point.x, height-(rect.y + rect.height)));
            //除去起始和结束的两个点不需要计算，需要计算的点数量是_ROUND_RECT_PARAM_ - 1 //
            for (int i = 0; i < _ROUND_RECT_PARAM_ - 1; ++i, count += step) {
                float r_h = sqrtf((round_rate * round_rate) - (count * count));
                vertics[2+i*3]=(MGVertexMake(tmp_point.x + count, height-(tmp_point.y + r_h)));
                vertics[3+i*3]=(MGVertexMake(tmp_point.x + count, height-(tmp_point.y + r_h)));
                vertics[4+i*3]=(MGVertexMake(tmp_point.x, height-tmp_point.y));
            }
            vertics[3*_ROUND_RECT_PARAM_-1]=(MGVertexMake(rect.x + rect.width, height-tmp_point.y));

            glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)vertics);
            glEnableClientState(GL_VERTEX_ARRAY);
            
            DebugOpenGLES();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, _ROUND_RECT_PARAM_ * 3);
            glDisableClientState(GL_VERTEX_ARRAY);
            delete_array_and_set_null(vertics);
        }
        { //vector right_top
            MGVertex *vertics=new MGVertex[3*_ROUND_RECT_PARAM_];
            float count = round_rate - step;
            MGPoint tmp_point = MGPointMake(rect.x + rect.width - round_rate, rect.y + round_rate);
            vertics[0]=(MGVertexMake(tmp_point.x, height-tmp_point.y));
            vertics[1]=(MGVertexMake(rect.x + rect.width, height-tmp_point.y));
            //除去起始和结束的两个点不需要计算，需要计算的点数量是_ROUND_RECT_PARAM_ - 1 //
            for (int i = 0; i < _ROUND_RECT_PARAM_ - 1; ++i, count -= step) {
                float r_h = sqrtf((round_rate * round_rate) - (count * count));
                vertics[2+i*3]=(MGVertexMake(tmp_point.x + count, height-(tmp_point.y - r_h)));
                vertics[3+i*3]=(MGVertexMake(tmp_point.x + count, height-(tmp_point.y - r_h)));
                vertics[4+i*3]=(MGVertexMake(tmp_point.x, height-tmp_point.y));
            }
            vertics[3*_ROUND_RECT_PARAM_-1]=(MGVertexMake(tmp_point.x, height-rect.y));

            glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)vertics);
            glEnableClientState(GL_VERTEX_ARRAY);
            
            DebugOpenGLES();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, _ROUND_RECT_PARAM_ * 3);
            glDisableClientState(GL_VERTEX_ARRAY);
            delete_array_and_set_null(vertics);
        }
        MGVertex vertics[12];
        vertics[0] = MGVertexMake(rect.x + rect.width - round_rate, height-rect.y);
        vertics[1] = MGVertexMake(rect.x + round_rate, height-rect.y);
        vertics[2] = MGVertexMake(rect.x + round_rate, height-(rect.y + rect.height));
        vertics[3] = MGVertexMake(rect.x + rect.width - round_rate, height-(rect.y + rect.height));
        vertics[4] = MGVertexMake(rect.x, height-(rect.y + round_rate));
        vertics[5] = MGVertexMake(rect.x, height-(rect.y + rect.height - round_rate));
        vertics[6] = MGVertexMake(rect.x + round_rate, height-(rect.y + rect.height - round_rate));
        vertics[7] = MGVertexMake(rect.x + round_rate, height-(rect.y + round_rate));
        vertics[8] = MGVertexMake(rect.x + rect.width - round_rate, height-(rect.y + round_rate));
        vertics[9] = MGVertexMake(rect.x + rect.width - round_rate, height-(rect.y + rect.height - round_rate));
        vertics[10] = MGVertexMake(rect.x + rect.width, height-(rect.y + rect.height - round_rate));
        vertics[11] = MGVertexMake(rect.x + rect.width, height-(rect.y + round_rate));
        glVertexPointer(3, GL_FLOAT, 0, vertics);
        glEnableClientState(GL_VERTEX_ARRAY);
        
        DebugOpenGLES();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glVertexPointer(3, GL_FLOAT, 0, &vertics[4]);
        
        DebugOpenGLES();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glVertexPointer(3, GL_FLOAT, 0, &vertics[8]);
        
        DebugOpenGLES();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_BLEND);


		drawRoundRect(rect,color,round_rate,1.0f,blend);
    }
    
    void MGGraphicEngine::drawImage(const MGImage *const image, const MGRect &rect, RGBAColorRef blend, DrawImageMode mode, const MGRect& image_rect,int order)
    {
        //TestDrawImage(image,rect,blend,mode,image_rect);
        //return ;
        if (image == NULL) {
            return;
        }
        if (image->content_info.width_in_pixel <= 0 || image->content_info.height_in_pixel <= 0) {
            return;
        }
        float height = MGDevice::getScreenHeight();
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        if (blend.valid_src)
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        else
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(blend.m_red_component / 255.0f, blend.m_green_component / 255.0f, blend.m_blue_component / 255.0f, blend.m_alpha_component / 255.0f);
        MGGraphicEngineContext context = image->context;
		GLint activeTextureID;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &activeTextureID);
		glBindTexture(GL_TEXTURE_2D, context.texture_id);
        
        // start 和 rate 分别是图片坐标系， 以左下角为(0,0)
        float rate_x = 1.0f;
        float rate_y = 1.0f;
        float start_x = 0.0f;
        float start_y = 0.0f;
        MGRect dst_rect = rect;
        switch(mode)
        {
        case ImageScaleToFill:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x =1.0f;
                    rate_y =1.0f;
                }else{
                    rate_x =1.0f* image->getWidth() / image->getAdapterWidth();
                    rate_y =1.0f* image->getHeight() / image->getAdapterHeight();
                }
                break;
            }
        case ImageAspectFill:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                rate_x = rect.width / image->content_info.width_in_pixel;
                rate_y = rect.height / image->content_info.height_in_pixel;
                float rate = (rate_x < rate_y) ? rate_y : rate_x;
                if (rate == rate_x)
                {
                    start_y = (image->content_info.height_in_pixel - rect.height / rate) / 2;
                    start_y /= image->getAdapterHeight();
                }
                else if (rate == rate_y)
                {
                    start_x = (image->content_info.width_in_pixel - rect.width / rate) / 2;
                    start_x /= image->getAdapterWidth();
                }
                
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x = 1.0f;
                    rate_y = 1.0f;
                }else{
                    rate_x = 1.0f * image->getWidth() / image->getAdapterWidth() - start_x;
                    rate_y = 1.0f * image->getHeight() / image->getAdapterHeight() - start_y;
                }
                break;
            }
        case ImageAspectFit:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                rate_x = rect.width / image->content_info.width_in_pixel;
                rate_y = rect.height / image->content_info.height_in_pixel;
                float rate = (rate_x < rate_y) ? rate_x : rate_y;
                if (rate == rate_x) {
                    dst_rect.height = rate * image->content_info.height_in_pixel;
                    dst_rect.y += (rect.height - dst_rect.height) / 2;
                } else if (rate == rate_y)
                {
                    dst_rect.width = rate * image->content_info.width_in_pixel;
                    dst_rect.x += (rect.width - dst_rect.width) / 2;
                }
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x =1.0f;
                    rate_y =1.0f;
                }else{
                    rate_x =1.0f* image->getWidth() / image->getAdapterWidth();
                    rate_y =1.0f* image->getHeight() / image->getAdapterHeight();
                }
                break;
            }
        case ImageCustomFit:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x =1.0f;
                    rate_y =1.0f;
                }else{

                    start_x = image_rect.x / image->getAdapterWidth();
                    //start_y = 1 - ((image_rect.y + image_rect.height) / image->getAdapterHeight());
					start_y = (image->getHeight() - image_rect.height - image_rect.y) / image->getAdapterHeight();
                    rate_x = (image_rect.x + image_rect.width) / image->getAdapterWidth();
                    //rate_y = 1 - (image_rect.y / image->getAdapterHeight());
					rate_y = start_y + image_rect.height / image->getAdapterHeight();
                }
                break;
            }
        case ImageCustomOriginalCenter:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x =1.0f;
                    rate_y =1.0f;
                }else{
                    start_x = image_rect.x / image->getAdapterWidth();
                    //start_y = 1 - ((image_rect.y + image_rect.height) / image->getAdapterHeight());
					start_y = (image->getHeight() - image_rect.height - image_rect.y) / image->getAdapterHeight();
                    rate_x = (image_rect.x + image_rect.width) / image->getAdapterWidth();
                    //rate_y = 1 - (image_rect.y / image->getAdapterHeight());
					rate_y = start_y + image_rect.height / image->getAdapterHeight();
                    MGPoint center;
                    center.x = dst_rect.x + dst_rect.width / 2.0f;
                    center.y = dst_rect.y + dst_rect.height / 2.0f;
                    dst_rect.x = center.x - image_rect.width / 2.0f;
                    dst_rect.y = center.y - image_rect.height / 2.0f;
                    dst_rect.width = image_rect.width;
                    dst_rect.height = image_rect.height;
                }
                break;
            }
        case ImageRepeat:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x =1.0f;
                    rate_y =1.0f;
                }else{
                    rate_x =1.0f* rect.width / image->getAdapterWidth();
                    rate_y =1.0f* rect.height / image->getAdapterHeight();
                }
                break;
            }
        case ImageMirrorRepeat:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x =1.0f;
                    rate_y =1.0f;
                }else{
                    rate_x =1.0f* rect.width / image->getAdapterWidth();
                    rate_y =1.0f* rect.height / image->getAdapterHeight();
                }
                break;
            }
        case ImageClamp:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                if (image->getAdapterWidth() == 0 || image->getAdapterHeight() == 0) {
                    rate_x =1.0f;
                    rate_y =1.0f;
                }else{
                    rate_x =1.0f* rect.width / image->getAdapterWidth();
                    rate_y =1.0f* rect.height / image->getAdapterHeight();
                }
                break;
            }
        default:
            {
            }
        }
        MGVertex vertics[4];
        vertics[0] = MGVertexMake(dst_rect.x, height-dst_rect.y);
        vertics[1] = MGVertexMake(dst_rect.x, height-(dst_rect.y + dst_rect.height));
        vertics[2] = MGVertexMake(dst_rect.x + dst_rect.width, height-(dst_rect.y + dst_rect.height));
        vertics[3] = MGVertexMake(dst_rect.x + dst_rect.width, height-(dst_rect.y));
        MGPoint texture_coor[4];
		if (order == 0)
		{
			texture_coor[0] = MGPointMake(start_x, rate_y);
			texture_coor[1] = MGPointMake(start_x, start_y);
			texture_coor[2] = MGPointMake(rate_x, start_y);
			texture_coor[3] = MGPointMake(rate_x, rate_y);
		}
		else if (order == 1)
		{
			texture_coor[1] = MGPointMake(start_x, rate_y);
			texture_coor[0] = MGPointMake(start_x, start_y);
			texture_coor[3] = MGPointMake(rate_x, start_y);
			texture_coor[2] = MGPointMake(rate_x, rate_y);

		}
		glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vertics);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, texture_coor);

        DebugOpenGLES();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindTexture(GL_TEXTURE_2D, activeTextureID);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }
    
#pragma mark - Advanced Drawing Function -
    //************************************
    // Method:    drawText
    // FullName:  z11::MGGraphicEngine::drawText
    // Access:    virtual public
    // Returns:   void
    // Qualifier:
    // Parameter: const string & _text
    // Parameter: const z11::MGFont * _font
    // Parameter: const MGRect & _rect
    // Parameter: MILineBreakMode mode
    // Parameter: TextAlignment text_alignment
    // Parameter: RGBAColorRef color
    // Parameter: RGBAColorRef blend
    // drawText 首先判断是否在MGTextContextMannager已经有了，如果有了直接执行第三步，如果没有执行第一步
    // 第一步根据MILineBreakMode确定绘制字数和区域，将要绘制的文字转换为bitmap，并保存起来
    // 第二步将第一步生成出来的bitmap组合起来绘制到一个MGImage(TextTextureRef)上，并把bitmap释放掉
    // 第三步绘制TextTextureRef
    //************************************


    void MGGraphicEngine::drawText(const string &_text, const z11::MGFont * _font, const MGRect& _rect, MILineBreakMode mode, MITextAlignment text_alignment, RGBAColorRef _color, RGBAColorRef blend,MGImage *cachedImage)
    {
        if (_text=="")
        {
            return ;
        }
		glColor4f(blend.m_red_component / 255.0f, blend.m_green_component / 255.0f, blend.m_blue_component / 255.0f, blend.m_alpha_component / 255.0f);
        

        if (_font == NULL)
        {
			MGFont *shared_font=MGFont::shareFont();
			shared_font->setFaceSize(17);
			shared_font->setColor(_color);
			shared_font->render(_text,_rect,cachedImage,mode,text_alignment,blend);
        }
        else
			_font->render(_text,_rect,cachedImage,mode,text_alignment,blend);
    }
    
    void MGGraphicEngine::drawFilledLightedRect( const MGRect& rect,RGBAColorRef color /*= RGBAColorMake(255,255,255)*/,float start_alpha/*=0*/,float end_alpha/*=180*/)
	{
		const float M = 16;
		MGRect rect2=rect;
		rect2.height/=M;
		float alpha=start_alpha*M_PI/180-M_PI/2;
		float step=(end_alpha-start_alpha)*M_PI/180/M;
		for(int i=0;i<16;i++,alpha+=step)
		{
			vector<RGBAColorRef> colors;
            colors.push_back(TestIncreaseColor(color,alpha));
            colors.push_back(TestIncreaseColor(color,alpha+step));
            
			MGGraphicEngine::sharedGraphicEngine()->drawFilledGradientRect(rect2,colors);
			rect2.y+=rect2.height;
		}
        
	}
    
	void MGGraphicEngine::drawFilledLightedRoundRect( const MGRect& rect,RGBAColorRef color,float round_rate ,float start_alpha,float end_alpha, DrawRoundRectMode round_mode, RGBAColorRef blend )
	{
        
	}
    
    void MGGraphicEngine::drawFilledCylinderRect( const MGRect& rect,RGBAColorRef color, RGBAColorRef light, bool vertical)
	{
        vector<RGBAColorRef> colors;
        MGRect draw_rect = rect;
        if(vertical)
        {
            colors.push_back(light);
            colors.push_back(color);
            colors.push_back(color);
            colors.push_back(light);
            draw_rect.width = rect.width * 0.5f;
        }else{
            colors.push_back(color);
            colors.push_back(color);
            colors.push_back(light);
            colors.push_back(light);
            draw_rect.height = rect.height * 0.5f;
        }
        MGGraphicEngine::sharedGraphicEngine()->drawFilledGradientRect(draw_rect,colors);
        colors.clear();
        if(vertical)
        {
            draw_rect.x = rect.x + draw_rect.width;
            colors.push_back(color);
            colors.push_back(light);
            colors.push_back(light);
            colors.push_back(color);
        }else{
            draw_rect.y = rect.y + draw_rect.height;
            colors.push_back(light);
            colors.push_back(light);
            colors.push_back(color);
            colors.push_back(color);
        }
        MGGraphicEngine::sharedGraphicEngine()->drawFilledGradientRect(draw_rect,colors);
	}
    
	void MGGraphicEngine::drawFilledGradientRect( const MGRect &rect,const std::vector<RGBAColorRef>& colors,RGBAColorRef blend/*=RGBAColorMake(255,255,255)*/ )
	{
		float height = MGDevice::getScreenHeight();
        
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glShadeModel (GL_SMOOTH);
		MGVertex vertics[4]
		={
			MGVertexMake(rect.x + rect.width, height-rect.y),
			MGVertexMake(rect.x, height-rect.y),
			MGVertexMake(rect.x, height-(rect.y + rect.height)),
			MGVertexMake(rect.x + rect.width, height-(rect.y + rect.height))
		};
        
		GLfloat _colors[4][4];
		if(colors.size()==4)
		{
			for (int i=0;i<4;i++)
			{
				_colors[i][0]=colors[i].m_red_component/255.0f;
				_colors[i][1]=colors[i].m_green_component/255.0f;
				_colors[i][2]=colors[i].m_blue_component/255.0f;
				_colors[i][3]=colors[i].m_alpha_component/255.0f;
			}
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4,GL_FLOAT,0,_colors);
		}
		else if(colors.size()==2)
		{
			for (int i=0;i<4;i++)
			{
				_colors[i][0]=colors[i/2].m_red_component/255.0f;
				_colors[i][1]=colors[i/2].m_green_component/255.0f;
				_colors[i][2]=colors[i/2].m_blue_component/255.0f;
				_colors[i][3]=colors[i/2].m_alpha_component/255.0f;
			}
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4,GL_FLOAT,0,_colors);
		}
		else if(colors.size()==1)
		{
			if (blend == RGBAColorRef::white()) {
				glColor4f(colors[0].m_red_component/255.0f, colors[0].m_green_component/255.0f, colors[0].m_blue_component/255.0f, colors[0].m_alpha_component/255.0f);
			} else {
				glColor4f(blend.m_red_component/255.0f, blend.m_green_component/255.0f, blend.m_blue_component/255.0f, blend.m_alpha_component/255.0f);
			}
		}
        
        
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertics);
        
        DebugOpenGLES();
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDisableClientState(GL_VERTEX_ARRAY);
        if (colors.size() == 2 || colors.size() == 4) {
            glDisableClientState(GL_COLOR_ARRAY);
        }
        glDisable(GL_BLEND);
		
	}
    
    void MGGraphicEngine::drawTriangles(vector<MGVertex> vertics,int segments,RGBAColorRef color,RGBAColorRef blend)
    {
        float height = MGDevice::getScreenHeight();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if (blend == RGBAColorRef::white()) {
            glColor4f(color.m_red_component/255.0f, color.m_green_component/255.0f, color.m_blue_component/255.0f, color.m_alpha_component/255.0f);
        } else {
            glColor4f(blend.m_red_component/255.0f, blend.m_green_component/255.0f, blend.m_blue_component/255.0f, blend.m_alpha_component/255.0f);
        }
        for (vector<MGVertex>::iterator it = vertics.begin(); it != vertics.end(); ++it) {
            it->y = height - it->y;
        }
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, &vertics.at(0));
        
        DebugOpenGLES();
        glDrawArrays(GL_TRIANGLE_FAN, 0, segments);
        glDisableClientState(GL_VERTEX_ARRAY);
        
    }
    
	void MGGraphicEngine::drawDashLine( const MGPoint& first, const MGPoint& second, RGBAColorRef color /*= RGBAColorMake(255,255,255)*/, float size /*= 1.0f*/,const vector<float>&lengths, float dash_phase/*=0.0f*/,RGBAColorRef blend /*= RGBAColorMake(255,255,255,255)*/ )
	{
		if (lengths.size()<2)
		{
			drawLine(first,second,color,size,blend);
			return ;
		}
		//now ,only support 2 length points;
		//y=ax+b;
		//(y1-y2)=a(x1-x2)==>a=(y1-y2)/(x1-x2);
		//==>b=(x1y1-2*x2y1+x2y2)/(x1-x2);
		float x1=first.x;
		float y1=first.y;
		float x2=second.x;
		float y2=second.y;
		if (x1>x2+eps||x1<x2-eps)
		{
			float a=(y1-y2)/(x1-x2);
			float b=(x1*y2-x2*y1)/(x1-x2);
            
			float startx;
			float x_step=lengths[0]/sqrt(a*a+1);
			float all_step=(lengths[0]+lengths[1])/sqrt(a*a+1);
			if (x1<x2)
			{
				for (startx=x1;startx<x2;startx+=all_step)
				{
					float endx=startx+x_step;
					endx=MINVALUE(endx,x2);
					drawLine(MGPointMake(startx,a*startx+b),MGPointMake(endx,a*endx+b),color,size,blend);
				}
			}
			else
			{
				for (startx=x1;startx>x2;startx-=all_step)
				{
					float endx=startx-x_step;
					endx=MAXVALUE(endx,x2);
					drawLine(MGPointMake(startx,a*startx+b),MGPointMake(endx,a*endx+b),color,size,blend);
				}
                
			}
		}
		else if(y1>y2+eps||y1<y2-eps)
		{
			float starty;
			float y_step=lengths[0];
			float all_step=(lengths[0]+lengths[1]);
			if (y1<y2)
			{
				for (starty=y1;starty<y2;starty+=all_step)
				{
					float endy=starty+y_step;
					endy=MINVALUE(endy,y2);
					drawLine(MGPointMake(x1,starty),MGPointMake(x1,endy),color,size,blend);
				}
			}
			else
			{
				for (starty=y1;starty>y2;starty-=all_step)
				{
					float endy=starty-y_step;
					endy=MAXVALUE(endy,y2);
					drawLine(MGPointMake(x1,starty),MGPointMake(x1,endy),color,size,blend);
				}
                
			}
            
		}
	}
    
	void MGGraphicEngine::drawDashPolyLines( const vector<MGPoint>& points,RGBAColorRef color /*= RGBAColorMake(255,255,255)*/, float size /*= 1.0f*/,const vector<float>&lengths/*=vector<float>()*/,float dash_phase/*=0.0f*/, RGBAColorRef blend /*= RGBAColorMake(255,255,255,255)*/ )
	{
		if(points.empty())
			return;
        
		if (lengths.empty())
		{
            drawPolyLines(points,color,size,blend);
            return;
		}
        
		for (vector<MGPoint>::const_iterator it=points.begin();(it+1)!=points.end();it++)
		{
			drawDashLine(*it,*(it+1),color,size,lengths,dash_phase,blend);
		}
	}

#pragma mark - Matrix Transfer -
    
    /*  旋转矩阵变换 ax,ay,az 代表单位矢量轴，base_angle为旋转角度
     
         ax*ax ax*ay ax*az            0   -az   ay
    A^ = ay*ax ay*ay ay*az      A* =  az   0   -ax
         az*ax az*ay az*az           -ay   ax   0
     
     M = A^ + cos(base_angle)(I - A^) + sin(base_angle)*(A*)
     
     P' = P * MT    MT代表M矩阵的转秩矩阵
     
    */
    void MGGraphicEngine::rotate(const MGPoint& point, MGRectRotate angle)
    {
        // MGRectRotate的参数正好与OpenGLES的glRotate相反，MGRectRotate是矢量三参数在前，角度在后，glRotate是角度在前，矢量三参数在后 //
        if (angle == MGRectRotateMake(0.0f, 0.0f, 0.0f, 0.0f)) {
            return;
        }
        float base_axis = sqrtf(powf(angle.axis_x, 2) + powf(angle.axis_y, 2) + powf(angle.axis_z, 2));
        
        if (base_axis > eps) {
            float height = MGDevice::getScreenHeight();
            
            float ax = angle.axis_x / base_axis;
            float ax2 = powf(ax, 2);
            float ay = angle.axis_y / base_axis;
            float ay2 = powf(ay, 2);
            float az = angle.axis_z / base_axis;
            //float az2 = powf(az, 2);
            float cos_a = cosf(angle.angle * M_PI / 180);
            float sin_a = sinf(angle.angle * M_PI / 180);
            if (fabs(cos_a) < eps) {
                cos_a = 0;
            }
            if (fabs(sin_a) < eps) {
                sin_a = 0;
            }
            float axay = ax * ay;
            float axaz = ax * az;
            float ayaz = ay * az;
            float sinax = sin_a * ax;
            float sinay = sin_a * ay;
            float sinaz = sin_a * az;
            
            float new_x = point.x*(ax2+cos_a*(1-ax2)) + (height-point.y)*(axay+cos_a*(-axay)-sinaz);
            float new_y = point.x*(axay+cos_a*(-axay)+sinaz) + (height-point.y)*(ay2+cos_a*(1-ay2));
            float new_z = point.x*(axaz+cos_a*(-axaz)-sinay) + (height-point.y)*(ayaz+cos_a*(-ayaz)+sinax);
            
            float dst_width = point.x - new_x;
            float dst_height = (height-point.y) - new_y;
            float dst_depth = -new_z;
            
            glTranslatef(dst_width, dst_height, dst_depth);
            
            glRotatef(angle.angle, ax, ay, az);
        }
    }
    
    void MGGraphicEngine::transition(MGRectTransition transition)
    {
        if (transition == MGRectTransitionMake(0.0f, 0.0f, 0.0f)) {
            return;
        }
        glTranslatef(transition.axis_x, -transition.axis_y, transition.axis_z);
    }
    void MGGraphicEngine::scale(const MGPoint& point, MGRectScale scale)
    {
        if (scale == MGRectScaleMake(1, 1, 1)) {
            return;
        }
        float height = MGDevice::getScreenHeight();
        float dst_width = (point.x) * (1 - scale.axis_x);
        float dst_height = (height-point.y) * (1 - scale.axis_y);
        glTranslatef(dst_width, dst_height, 0);
        glScalef(scale.axis_x, scale.axis_y, scale.axis_z);
    }

#pragma mark - Clip Operation -
    void MGGraphicEngine::enableClip(const MGRect &rect)
    {
        // OpenGLES的坐标垂直方向是从下向上逐渐变大 //
        glEnable(GL_SCISSOR_TEST);
        int_32 height = MGDevice::getScreenHeight();
        MGRect clip_rect = MGRectMake(rect.x, height - rect.y - rect.height, rect.width, rect.height);
        // 需要把裁剪的区域转成正区域 //
        if (clip_rect.width < 0) {
            clip_rect.x = clip_rect.x + clip_rect.width;
            clip_rect.width = -clip_rect.width;
        }
        if (clip_rect.height < 0) {
            clip_rect.y = clip_rect.y + clip_rect.height;
            clip_rect.height = -clip_rect.height;
        }
        if (clip_stack.size() > 0)
        {
            MGRect last_clip = clip_stack.back();
            rectClip(clip_rect, last_clip);
        }
//        MGLogD("push and enable clip rect : (%f, %f, %f, %f).", clip_rect.x, clip_rect.y, clip_rect.width, clip_rect.height);
        glScissor(clip_rect.x, clip_rect.y, clip_rect.width, clip_rect.height);
        clip_stack.push_back(clip_rect);
    }
    void MGGraphicEngine::disableClip()
    {
        clip_stack.pop_back();
        if ((int)clip_stack.size() > 0) {
            MGRect clip_rect = clip_stack.back();
            glScissor(clip_rect.x, clip_rect.y, clip_rect.width, clip_rect.height);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }
    }
    
    void MGGraphicEngine::rectClip(MGRect &clip_rect, const MGRect &last_clip)
    {
        // 修正裁剪效果，clip_stack要确保最上面的clip区域都要比下面的区域小，否则就是逻辑错误 //
        if (clip_rect.x >= last_clip.x + last_clip.width) {
            clip_rect.width = 0;
            return;
        }
        if (clip_rect.y >= last_clip.y + last_clip.height) {
            clip_rect.height = 0;
            return;
        }
        float right = clip_rect.x + clip_rect.width;
        float top = clip_rect.y + clip_rect.height;
        if (clip_rect.x < last_clip.x) {
            if (right <= last_clip.x) {
                clip_rect.width = 0;
                return;
            }
            clip_rect.width -= (last_clip.x - clip_rect.x);
            clip_rect.x = last_clip.x;
        }
        right = clip_rect.x + clip_rect.width;
        if (right > last_clip.x + last_clip.width) {
            clip_rect.width = last_clip.x + last_clip.width - clip_rect.x;
        }
        if (clip_rect.y < last_clip.y) {
            if (top <= last_clip.y) {
                clip_rect.height = 0;
                return;
            }
            clip_rect.height -= (last_clip.y - clip_rect.y);
            clip_rect.y = last_clip.y;
        }
        top = clip_rect.y + clip_rect.height;
        if (top > last_clip.y + last_clip.height) {
            clip_rect.height = last_clip.y + last_clip.height - clip_rect.y;
        }
    }

#pragma mark - Context handle -
    //
    void MGGraphicEngine::beginDraw()
    {
        glPushMatrix();
    }
    void MGGraphicEngine::endDraw()
    {
        glPopMatrix();
    }

    void MGGraphicEngine::cleanContext()
    {
        AUTO_LOCK_THIS;
        if (!vtContext.empty())
        {
            vector<MGGraphicEngineContext>::iterator it;
            for (it=vtContext.begin();it!=vtContext.end();it++)
            {
                it->releaseContext();
            }
            vtContext.clear();
        }

    }
    
    void MGGraphicEngine::initContext()
    {
        AUTO_LOCK_THIS;
        vtContext.clear();
    }

    void MGGraphicEngine::pushContext( const MGGraphicEngineContext & _context )
    {
        AUTO_LOCK_THIS;
        vtContext.push_back(_context);
    }
	
}