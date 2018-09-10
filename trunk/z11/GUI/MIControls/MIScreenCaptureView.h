#pragma once

#include "../Base/MIView.h"
#include "../../GraphicCore/MGGrid.h"
#include <vector>
namespace z11
{
	class MIPageTurnView:public z11::MIView
	{
	public:
		float m_level;
		MIPageTurnView(bool up_order);
		~MIPageTurnView(void);
		virtual void initWithFromViewAndToView(MIView *from_view,MIView *to_view);
		virtual void drawself();
		void setDuration(float duration);
		void setAnimationStopSelector(MIResponder *responder,SELHandler selector)
		{
			did_stop_responder = responder;
			did_stop_handler = selector;
		}
		void setAnimationStartSelector(MIResponder *responder,SELHandler selector)
		{
            will_start_responder= responder;
            will_start_handler = selector;
		}
	protected:
		MIResponder *did_stop_responder;
		SELHandler did_stop_handler;
		MIResponder *will_start_responder;
		SELHandler will_start_handler;
		//should be called after initWithFrame
		virtual void initGrid();
		void readyForCapture();
		void reorderViewForPerform(MGObject* sender);
		void reorderViewForEnd(MGObject* sender);
		int tick_count;
		int tick_count_total;
		MIView *m_from_view;
		MIView *m_to_view;
		bool m_reverse_order;
		bool m_bGridInit;
		bool m_over;
		MGGrid *m_grid;
		std::vector<float> update_time_sequence;

	};

	class MIScreenCaptureView :public z11::MIPageTurnView
	{
	public:
		virtual void initGrid();
		virtual void initWithFromViewAndToView(MIView *from_view,MIView *to_view);
		MIScreenCaptureView(bool up_order);
		virtual ~MIScreenCaptureView();
		virtual MIView* hitDetect(MITouch* touch, MIEvent *event, MITouchEventType& type);
	};
	enum MICurlViewState
	{
		MICurlViewStateUninit,
		MICurlViewStateUp,
		MICurlViewStateInit,
		MICurlViewStateDown,
		MICurlViewStateFinish
	};
	class MICurlView:public MIScreenCaptureView
	{
	public:
		MICurlViewState state;
		MICurlView();
		virtual void initWithFromViewAndToView( MIView *from_view,MIView *to_view );
		void reorderViewForPerform(MGObject* sender);
		void performUpSequenceFinish(MGObject* sender);
		void reorderViewForEnd(MGObject* sender);
		void performDownSequenceFinish(MGObject *sender);
		void up();
		void down();
		virtual void drawself();
	};
};
