#include "MISwitch.h"
#include "MIButton.h"
#include "../../GraphicCore/MGFont.h"
#include "../../GraphicCore/MGCanvas.h"
#include <stdio.h>
using namespace std;
namespace z11
{
	//54+40+54
	class MISwitchImpl:public MIButton
	{
	public:
		MISwitchImpl()
		{
            class_name = "MISwitchImpl";
		}
		~MISwitchImpl()
		{

		}
		virtual MIEventHandleState touchesEnded(const std::set<MITouch*>& touches,const MIEvent* const event)
		{
			MIEventHandleState state=MIButton::touchesEnded(touches,event);
			MISwitch *view=(MISwitch*)(parent());
			view->setOn(!view->isOn());
			return state;
		}
	public:
		MIVIEW_DECLARE;

	};
	void MISwitchImpl::initWithFrame(MGRect frame)
	{
		MIButton::initWithFrame(frame);
	}
	void MISwitchImpl::drawself()
	{
		MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();
		MGRect parent_rect=parent()->getBounds();
		MGRect rect=m_bounds;
		float halfWidth=54.0*m_bounds.width/148;
		float antherHalfWidth=parent_rect.width-halfWidth;

#if FONT_SHARED
		MGFont * white_font=MGFont::shareFont();
		white_font->setFaceSize(15/148.0*m_bounds.width);
		white_font->setColor(RGBAColorRef::white());
#else
		static MGFont *white_font=MGFont::create(15/148.0*m_bounds.width,RGBAColorRef::white());
#endif

		rect.width=halfWidth;
		engine->drawFilledLightedRect(rect,RGBAColorMake(100,157,255),20,90);
		engine->drawText("ON",white_font,rect,MILineBreakModeClip,MITextAlignmentCenterMiddle);

#if FONT_SHARED
		MGFont * light_Black_font=MGFont::shareFont();
		light_Black_font->setFaceSize(15/148.0*m_bounds.width);
		light_Black_font->setColor(RGBAColorRef::lightBlack());
#else
		static MGFont *light_Black_font=MGFont::create(15/148.0*m_bounds.width,RGBAColorRef::lightBlack());
#endif

		rect.width=halfWidth;
		rect.x=m_bounds.x+m_bounds.width-halfWidth;
		engine->drawFilledLightedRect(rect,RGBAColorRef::white(),40,90);
		engine->drawText("OFF",light_Black_font,rect,MILineBreakModeClip,MITextAlignmentCenterMiddle);

		rect.width=antherHalfWidth+1;
		rect.x=m_bounds.x+parent_rect.width-antherHalfWidth;
		engine->drawFilledLightedRect(rect,RGBAColorRef::white(),80,20);
	}

	MISwitch::MISwitch(void)
	{
        class_name = "MISwitch";
		_view_flags.clip_subviews = 1;
		impl	=	new MISwitchImpl();
        m_on = true;
	}

	MISwitch::~MISwitch(void)
	{
	}

	bool MISwitch::isOn()
	{
		return m_on;
	}

	void MISwitch::setOn( bool on,bool animated/*=true*/ )
	{
        float width=54.0*m_bounds.width/94;
        float tran_x = 0;
        if (!on) {
            tran_x = -width;
        }
		if (m_on == on)
		{
            impl->setRectTransition(MGRectTransitionMake(tran_x,0,0));
			return ;
		}
		m_on=on;
		if (animated)
		{
            char ptr[128];
			sprintf(ptr, "%p", this);
			string s_ptr = ptr;
			MIView::beginAnimation("Transition Views for Foundation Call Only." + s_ptr);
			MIView::setAnimationCurve(MIViewAnimationOptionTransitionFlipFromRight);
			MIView::setAnimationDuration(0.2f);
			impl->setRectTransition(MGRectTransitionMake(tran_x,0,0));
			MIView::commitAnimation();
		}
		else
		{
			impl->setRectTransition(MGRectTransitionMake(tran_x,0,0));
		}
	}
	void MISwitch::initWithFrame(MGRect frame)
	{
		frame.x=(float)((int)(frame.x+0.5));
		frame.y=(float)((int)(frame.y+0.5));
		frame.width=(float)((int)(frame.width+0.5));
		frame.height=(float)((int)(frame.height+0.5));
		//if(false)
		//{
		//	MGCanvas canvas(2*frame.width-frame.height,frame.height,32);
		//	{
		//		MGRect on_bounds=MGRectMake(0,0,frame.width,frame.height);
		//		MGRect off_bounds=MGRectMake(frame.width-frame.height,0,frame.width,frame.height);
		//		MGRect thumb_bounds=MGRectMake(frame.width-frame.height,0,frame.height,frame.height);
		//		float radius=frame.height/2;
		//		MGPoint pts[2]=
		//		{
		//			MGPointMake(0,0),
		//			MGPointMake(0,on_bounds.height)
		//		};
		//		vector<RGBAColorRef> on_colors;
		//		{
		//			on_colors.push_back(RGBAColorRef::blue());
		//			on_colors.push_back(RGBAColorRef::white());
		//		}
		//		vector<RGBAColorRef> off_colors;
		//		{
		//			off_colors.push_back(RGBAColorRef::lightGray());
		//			off_colors.push_back(RGBAColorRef::white());
		//		}
		//		vector<RGBAColorRef> thumb_colors;
		//		{
		//			thumb_colors.push_back(RGBAColorRef::darkGray());
		//			thumb_colors.push_back(RGBAColorRef::white());
		//		}
		//		vector<float> pos;
		//		pos.push_back(0.0f);
		//		pos.push_back(1.0f);
		//		canvas.drawGradientRoundedRect(on_bounds,radius,radius,pts,on_colors,pos);
		//		canvas.drawGradientRoundedRect(off_bounds,radius,radius,pts,off_colors,pos);
		//		canvas.drawGradientRoundedRect(thumb_bounds,radius,radius,pts,thumb_colors,pos);
		//		canvas.writeToFile(MGBundle::mainBundle()->tmpPathWithFileName("Controls/MISwitch.png"));
		//		//canvas.drawText("ON",)
		//	}

		//}
		MIControl::initWithFrame(frame);
		impl->initWithFrame(MGRectMake(-1,-1,(int)(148.0/94*frame.width+2),(frame.height+2)));
		addSubview(impl);
	}
	void MISwitch::drawself()
	{
		//MIControl::drawself();
	}
}
