/*
 * =====================================================================================
 *
 *       Filename:  MIActivityIndicatorView.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/06/2012 11:19:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
 #include "MIActivityIndicatorView.h"
#include "../../System/IO/MGBundle.h"
#include <math.h>
#include "../../GraphicCore/MGCanvas.h"
#include "../../System/IO/MGFileManager.h"
namespace z11 {
	MIActivityIndicatorView *z11::MIActivityIndicatorView::initWithActivityIndicatorStyle(MIActivityIndicatorViewStyle style) {
		MIActivityIndicatorView *ret=new MIActivityIndicatorView();
        ret->_view_flags.user_interaction_enabled = 0;
		string path=MGBundle::mainBundle()->tmpPathWithFileName("controls/ActivityIndicator.png");
		if (!MGFileManager::defaultManager()->fileExistsAtPath(path))
		{
			MGFileManager::defaultManager()->createDirectoryAtPath(MGBundle::mainBundle()->tmpPathWithFileName("controls/"));
			MGCanvas canvas(64,64,32);
			canvas.drawSweep(MGRectMake(0,0,64,64),0);

			//MGImage *back=MGPersistenceImage::fromBlank(32,64 , 64 );
			//MGCanvas canvas(NULLREF);
			//canvas.setBufferImage(MGImageRef(back));
			//ret->setBackgroundColor(MGColor::fromImage(back));


			canvas.writeToFile(MGBundle::mainBundle()->tmpPathWithFileName("controls/ActivityIndicator.png"));
		}
		ret->setBackgroundColor(MGColor::fromImage(path));
        ret->m_ticks = -2;
		return ret;
	}
	void z11::MIActivityIndicatorView::startAnimating() {
		m_is_animating=true;
	}
	void z11::MIActivityIndicatorView::stopAnimating() {
		m_is_animating=false;
	}
	bool z11::MIActivityIndicatorView::isAnimating() {
		return m_is_animating;
	}
	z11::MIActivityIndicatorView::MIActivityIndicatorView():m_is_animating(false),hidesWhenStopped(true) {class_name = "MIActivityIndicatorView";}

	void z11::MIActivityIndicatorView::drawself() {
		if(m_is_animating) {
			if(m_ticks==-2)
				m_ticks=0;
			m_ticks++;
			if(m_ticks==24)
			{
				m_ticks=0;
			}
		}
		else
		{
			m_ticks=-2;
			if(hidesWhenStopped)
				return ;
		}
		MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();

		//engine->drawRoundRect(m_bounds,RGBAColorMake(255,255,0));

        //{
        //    MGRect rect=m_bounds;
        //    rect.width*=2;
        //    rect.x-=rect.width/4;
        //    rect.height*=2;
        //    rect.y-=rect.height/4;
        //    engine->drawFilledRoundRect(rect,RGBAColorMake(128,128,128,192),15);
        //}
		if (m_background_color==NULL||m_background_color->m_image==NULL)
		{
			for(int i=0;i<12;i++)
			{
				double r=m_bounds.width/2;
				double sinx=sin(i*2*3.14/12);
				double cosx=cos(i*2*3.14/12);
				if(m_ticks/2==i)
					engine->drawLine(MGPointMake(m_bounds.x+r*sinx+r,m_bounds.y+r+r*cosx),MGPointMake(m_bounds.x+r+r/2*sinx,m_bounds.y+r+r*cosx/2),RGBAColorMake(255,255,255),3);
				else
					engine->drawLine(MGPointMake(m_bounds.x+r*sinx+r,m_bounds.y+r+r*cosx),MGPointMake(m_bounds.x+r+r/2*sinx,m_bounds.y+r+r*cosx/2),RGBAColorMake(128,128,128),3);

			}
		}
        else {
            MIView::drawself();
            m_rotate=MGRectRotateMake(0, 0, 1, -m_ticks/4*60);
        }
		
	}
}
