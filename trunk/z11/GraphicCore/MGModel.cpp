#include "MGModel.h"
#include "../System/Device/MGDevice.h"
#include <math.h>
#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif
#include "../System/Shared/MGLog.h"
#include "../Platform/MGGL.h"
namespace z11
{

MGModel::MGModel()
{
    //ctor
}

MGModel::~MGModel()
{
    //dtor
}

void MGModel::scale(float scale_x,float scale_y,float scale_z)
{
    for(std::vector<MGVertex>::iterator it=m_vertics.begin();it!=m_vertics.end();it++)
    {
        it->x*=scale_x;
        it->y*=scale_y;
        it->z*=scale_z;
    }
}

void MGModel::translate(float x,float y,float z)
{
    for(std::vector<MGVertex>::iterator it=m_vertics.begin();it!=m_vertics.end();it++)
    {
        it->x+=x;
        it->y+=y;
        it->z+=z;
    }
}
MGModel* MGModel::createRoundedRect(const MGRect &rect,float round_rate_x,float round_rate_y,int segments)
{
    float height = MGDevice::getScreenHeight();
    MGModel *ret=new MGModel;
    ret->m_type=ModelTypeTriangleFan;
    int _ROUND_RECT_PARAM_=(segments-2)/4-1;
    float bUp=rect.height>0?1:-1;
    MGPoint centers[4]=
    {
        MGPointMake(rect.x + rect.width - round_rate_x, rect.y + bUp*round_rate_y),
        MGPointMake(rect.x + round_rate_x, rect.y + bUp*round_rate_y),
        MGPointMake(rect.x + round_rate_x, rect.y + rect.height - bUp*round_rate_y),
        MGPointMake(rect.x + rect.width - round_rate_x, rect.y + rect.height - bUp*round_rate_y),
    };
    float step = (rect.height>0)?(M_PI/2 / _ROUND_RECT_PARAM_):(-M_PI/2 / _ROUND_RECT_PARAM_);

    ret->m_vertics.push_back(MGVertexMake(rect.x+rect.width/2,height-(rect.y+rect.height/2)));
    float alpha = 0;
    for (int j=0;j<4;j++)
    {
        MGPoint center = centers[j];
        //除去起始和结束的两个点不需要计算，需要计算的点数量是_ROUND_RECT_PARAM_ - 1 //
        for (int i = 0; i < _ROUND_RECT_PARAM_+1; ++i, alpha += step) {
            ret->m_vertics.push_back(MGVertexMake(center.x+round_rate_x*cos(alpha), center.y-round_rate_y*sin(alpha)));
        }
    }
    ret->m_vertics.push_back(MGVertexMake(rect.x + rect.width, height-(rect.y+bUp*round_rate_y )));
    return ret;
}
void MGModel::scale(float scale)
{
    for(std::vector<MGVertex>::iterator it=m_vertics.begin();it!=m_vertics.end();it++)
    {
        it->x*=scale;
        it->y*=scale;
        it->z*=scale;
    }
}
MGModel* MGModel::clone()
{
    MGModel *ret=new MGModel();
    *ret=*this;
    return ret;
}
void MGModel::draw()
{
    //glPushMatrix();
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &m_vertics[0]);
    glDrawArrays(m_type, 0, m_vertics.size());
    glDisableClientState(GL_VERTEX_ARRAY);
    //glPopMatrix();
}
MGVertex *MGModel::getVertics()
{
    return &m_vertics[0];
}
MGModel* MGModel::createRoundedRect(const MGRect &rect,float round_rate,int segments)
{
    MGModel *ret=new MGModel;
    ret->m_type=ModelTypeTriangleFan;
    int _ROUND_RECT_PARAM_=(segments-2)/4-1;
    float bUp=rect.height>0?1:-1;
    MGPoint centers[4]=
    {
        MGPointMake(rect.x + rect.width - round_rate, rect.y + bUp*round_rate),
        MGPointMake(rect.x + round_rate, rect.y + bUp*round_rate),
        MGPointMake(rect.x + round_rate, rect.y + rect.height - bUp*round_rate),
        MGPointMake(rect.x + rect.width - round_rate, rect.y + rect.height - bUp*round_rate),
    };
    float step = (rect.height>0)?(M_PI/2 / _ROUND_RECT_PARAM_):(-M_PI/2 / _ROUND_RECT_PARAM_);

    ret->m_vertics.push_back(MGVertexMake(rect.x+rect.width/2,rect.y+rect.height/2));
    float alpha = 0;
    for (int j=0;j<4;j++)
    {
        MGPoint center = centers[j];
        //除去起始和结束的两个点不需要计算，需要计算的点数量是_ROUND_RECT_PARAM_ - 1 //
        for (int i = 0; i < _ROUND_RECT_PARAM_+1; ++i, alpha += step) {
            ret->m_vertics.push_back(MGVertexMake(center.x+round_rate*cos(alpha), center.y-round_rate*sin(alpha)));
        }
    }
    ret->m_vertics.push_back(MGVertexMake(rect.x + rect.width, rect.y+bUp*round_rate ));
    return ret;
}
MGModel* MGModel::createRect(const MGRect &rect)
{
    MGModel *ret=new MGModel;
    ret->m_type=ModelTypeTriangleFan;
    ret->m_vertics.push_back(MGVertexMake(rect.x,rect.y));
    ret->m_vertics.push_back(MGVertexMake(rect.x,rect.y+rect.height));
    ret->m_vertics.push_back(MGVertexMake(rect.x+rect.width,rect.y+rect.height));
    ret->m_vertics.push_back(MGVertexMake(rect.x+rect.width,rect.y));
    return ret;
}
MGModel* MGModel::createSphere(float r,int m,int n)
{
    MGModel *ret=new MGModel();
    ret->m_type=ModelTypeTriangleFan;
    std::vector<MGVertex> points;//m*(n+1)
    float glpi = 2.0 * asin(1.0);
    //int k = 0;
    for (int j = 0; j <= n; ++ j)
    {
        float y = r * cos(glpi*j/n);
        for (int i = 0; i < m; ++ i)
        {
            float x = r * sin(glpi*j/n) * cos(i*2*glpi/m);
            float z = r * sin(glpi*j/n) * sin(i*2*glpi/m);
            points.push_back(MGVertexMake(x,y,z));
        }
    }
    std::vector<MGVertex> &rect=ret->m_vertics;//m*n*4
    for (int i = 0; i < n; ++ i)
    {
        //i 纬线
        int j = 0;
        while(j < m - 1)
        {
            //j 经线
            rect.push_back(points[(m * i + j)]);

            rect.push_back(points[(m * (i + 1) + j)]);

            rect.push_back(points[(m * (i + 1) + j + 1)]);

            rect.push_back(points[(m * i + j + 1)]);

            ++ j;
        }
            rect.push_back(points[(m * i + j)]);

            rect.push_back(points[(m * (i + 1) + j)]);

            rect.push_back(points[(m * (i + 1) + 0 + 1)]);

            rect.push_back(points[(m * i + 0 + 0)]);



    }
    return ret;
}
}
