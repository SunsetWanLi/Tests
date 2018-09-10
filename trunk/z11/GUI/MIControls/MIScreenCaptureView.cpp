#include "MIScreenCaptureView.h"
#include "../../GraphicCore/MGImage.h"
#include "../GuiContainer/GuiContainer.h"
#include "../LightGraphic/LightGraphic.h"
#include "../../System/Shared/MGLog.h"
#define PAGE_SAVE_VIEW_EVENT USER_CUSTOMER_EVENT+568//random number

namespace z11
{
	MIPageTurnView::MIPageTurnView(bool up_order)
	{
        class_name = "MIPageTurnView";
		m_from_view=NULL;
		m_to_view=NULL;
		m_over=true;
		tick_count_total=20;
		tick_count=tick_count_total;
		m_reverse_order = !up_order;
		m_bGridInit=false;
		did_stop_responder=NULL;
		did_stop_handler.sender = NULL;
		did_stop_handler.sel_func =NULL;
		will_start_responder=NULL;
		m_level=1.0f;
		m_grid=NULL;
	}

	MIPageTurnView::~MIPageTurnView(void)
	{

		delete_and_set_null(m_grid);
	}

	void MIPageTurnView::readyForCapture()
	{
		MIView *parent_view=m_from_view->parent();
		assert(parent_view!=NULL);
		if (m_reverse_order)
		{
			//parent_view->addSubview(m_to_view);
			parent_view->addSubview(this);

		}
		else
		{
			parent_view->addSubview(this);
		}
		this->m_bGridInit=false;

	}
	void MIPageTurnView::reorderViewForPerform(MGObject* sender)
	{
		MIView *parent_view=parent();
		if (m_reverse_order)
		{
			//m_to_view->detachFromSuperview();
		}
		else
		{
			m_from_view->detachFromSuperview();
			this->detachFromSuperview();
			parent_view->addSubview(m_to_view);
			parent_view->addSubview(this);

		}

		if(will_start_responder!=NULL)
		{
		    SELFunc sel_func=will_start_handler.sel_func;
			(will_start_responder->*sel_func)(will_start_handler.sender);
		}
	}
	void MIPageTurnView::reorderViewForEnd(MGObject* sender)
	{
		MIView *parent_view=parent();
		if (m_reverse_order)
		{
			parent_view->addSubview(m_to_view);
			m_from_view->detachFromSuperview();
			this->detachFromSuperview();
		}
		else
		{
			//m_from_view->detachFromSuperview();
			//parent_view->addSubview(m_to_view);
			this->detachFromSuperview();
		}
		if(did_stop_responder!=NULL)
		{
		    SELFunc sel_func=did_stop_handler.sel_func;
			(did_stop_responder->*sel_func)(did_stop_handler.sender);
		}
	}
	void MIPageTurnView::drawself()
	{
		if(!m_bGridInit)
		{
			initGrid();
			m_bGridInit=true;
			m_over=false;

			//===================animation start==============================//
			MIEvent *event=new MIEvent();
			event->_event_type = PAGE_SAVE_VIEW_EVENT;
			event->_responder =this;
			event->_content = NULL;
			SELHandler selector;
			selector.sender = this;
			selector.sel_func = (SELFunc)&MIPageTurnView::reorderViewForPerform;
			event->_handler = selector;

			GuiContainer::container->preserveEvent(event);
			//=================animation start over============================//
			if(update_time_sequence.empty() )
			{
				if (m_reverse_order)
				{
					for (int i=0;i<=tick_count_total;i++)
					{
						update_time_sequence.push_back(i*m_level/tick_count_total);
					}
				}
				else
				{
					for (int i=0;i<=tick_count_total;i++)
					{
						update_time_sequence.push_back((1-i*1.0/tick_count_total)*m_level);
					}
				}

			}
		}
		if(!m_over)
		{
			if (tick_count>=0)
			{
				m_grid->update(update_time_sequence[tick_count]);
				//m_grid->blit();
			}
			else
			{
				//tick_count=tick_count_total;
				m_over=true;

				//===================animation end==============================//
				MIEvent *event=new MIEvent();
				event->_event_type = PAGE_SAVE_VIEW_EVENT;
				event->_responder =this;
				event->_content = NULL;
				SELHandler selector;
				selector.sender = this;
				selector.sel_func = (SELFunc)&MIPageTurnView::reorderViewForEnd;
				event->_handler = selector;

				GuiContainer::container->preserveEvent(event);
				//===================animation end over======================//
			}
			tick_count--;
		}
		//else
         m_grid->blit();
	}

	void MIPageTurnView::initGrid()
	{
		m_grid=new MGGrid();
		assert (m_background_color!=NULL && m_background_color->m_image!=NULL);
		MGImage *grid_image = MGImage::imageWithMGImage(m_background_color->m_image) ;
		m_grid->setScale(MGRectScaleMake(m_bounds.width/grid_image->getWidth(),m_bounds.height/grid_image->getHeight(),1));
		m_grid->initWithSize(MGIntSize(grid_image->getWidth()/32,grid_image->getHeight()/32),grid_image,false);
	}

	void MIScreenCaptureView::initWithFromViewAndToView( MIView *from_view,MIView *to_view )
	{
		this->initWithFrame(from_view->getBounds());
		this->m_from_view=from_view;
		this->m_to_view=to_view;
		readyForCapture();
	}

	void MIPageTurnView::initWithFromViewAndToView( MIView *from_view,MIView *to_view )
	{
		this->initWithFrame(from_view->getBounds());
		this->m_from_view=from_view;
		this->m_to_view=to_view;
		if (m_reverse_order)
		{
			assert(to_view->getBackgroundColor()!=NULL&&to_view->getBackgroundColor()->m_image!=NULL);
			MGImage *image=MGImage::imageWithMGImage(to_view->getBackgroundColor()->m_image);
			m_background_color=MGColor::fromImage(image);

		}
		readyForCapture();
	}

	void MIPageTurnView::setDuration( float duration )
	{
		tick_count=tick_count_total=duration*GuiContainer::container->light_graphic->getRealFps();
	}


	void MIScreenCaptureView::initGrid()
	{
		delete_and_set_null(m_grid);
		m_grid=new MGScreenGrid();
		static_cast<MGScreenGrid *>(m_grid)->initWithScreen();
	}

	MIView* MIScreenCaptureView::hitDetect( MITouch* touch, MIEvent *event, MITouchEventType& type )
	{
		return NULL;
	}

	MIScreenCaptureView::MIScreenCaptureView( bool up_order ) :MIPageTurnView(up_order)
	{
        class_name = "MIScreenCaptureView";
	}

	MIScreenCaptureView::~MIScreenCaptureView()
	{

	}
	void MICurlView::up()
	{
		m_bGridInit=false;
		state=MICurlViewStateUp;
	}

	void MICurlView::down()
	{
		m_from_view->parent()->_view_flags.user_interaction_enabled=0;
		tick_count=tick_count_total;
		update_time_sequence.clear();
		for (int i=0;i<=tick_count_total;i++)
		{
			update_time_sequence.push_back(i*m_level/tick_count_total);
		}
		m_over=false;
		state=MICurlViewStateDown;
	}

	void MICurlView::drawself()
	{
		if (state==MICurlViewStateUninit)
		{
			return ;
		}
		if(!m_bGridInit)
		{
			initGrid();
			m_bGridInit=true;
			//===================animation start==============================//
			MIEvent *event=new MIEvent();
			event->_event_type = PAGE_SAVE_VIEW_EVENT;
			event->_responder =this;
			event->_content = NULL;
			SELHandler selector;
			selector.sender = this;
			selector.sel_func = (SELFunc)&MICurlView::reorderViewForPerform;
			event->_handler = selector;

			GuiContainer::container->preserveEvent(event);
			//=================animation start over============================//
		}
		if (state==MICurlViewStateUp)
		{
			return ;
		}

		if(!m_over)
		{
			if (tick_count>=0)
			{
				m_grid->update(update_time_sequence[tick_count]);
				//m_grid->blit();
			}
			else
			{
				//tick_count=tick_count_total;
				m_over=true;
				if (state==MICurlViewStateDown)
				{
					//===================animation end==============================//
					MIEvent *event=new MIEvent();
					event->_event_type = PAGE_SAVE_VIEW_EVENT;
					event->_responder =this;
					event->_content = NULL;
					SELHandler selector;
					selector.sender = this;
					selector.sel_func = (SELFunc)&MICurlView::reorderViewForEnd;
					event->_handler = selector;

					GuiContainer::container->preserveEvent(event);
					//===================animation end over======================//
				}
				else if(state==MICurlViewStateInit)
				{
					//===================animation end==============================//
					MIEvent *event=new MIEvent();
					event->_event_type = PAGE_SAVE_VIEW_EVENT;
					event->_responder =this;
					event->_content = NULL;
					SELHandler selector;
					selector.sender = this;
					selector.sel_func = (SELFunc)&MICurlView::performUpSequenceFinish;
					event->_handler = selector;

					GuiContainer::container->preserveEvent(event);
					//===================animation end over======================//

				}
				else if(state==MICurlViewStateFinish)
				{
					//===================animation end==============================//
					MIEvent *event=new MIEvent();
					event->_event_type = PAGE_SAVE_VIEW_EVENT;
					event->_responder =this;
					event->_content = NULL;
					SELHandler selector;
					selector.sender = this;
					selector.sel_func = (SELFunc)&MICurlView::performDownSequenceFinish;
					event->_handler = selector;

					GuiContainer::container->preserveEvent(event);
					//===================animation end over======================//

				}

			}
			tick_count--;
		}
		m_grid->blit();
	}

	void MICurlView::initWithFromViewAndToView( MIView *from_view,MIView *to_view )
	{
		if (_view_flags.view_initialized==0)
		{
			this->initWithFrame(from_view->getBounds());
		}
		this->m_from_view=from_view;
		this->m_to_view=to_view;
		readyForCapture();
	}

	void MICurlView::reorderViewForPerform( MGObject* sender )
	{
		m_from_view->parent()->_view_flags.user_interaction_enabled=0;
		m_from_view->hide();
		m_to_view->detachFromSuperview();
		this->detachFromSuperview();
		m_from_view->parent()->addSubview(m_to_view);
		m_from_view->parent()->addSubview(this);


		tick_count=tick_count_total;

		update_time_sequence.clear();
		for (int i=0;i<=tick_count_total;i++)
		{
			update_time_sequence.push_back((1-i*1.0/tick_count_total)*m_level);
		}

		m_over=false;
		MGLogD("reorderViewForPerform");
		if(will_start_responder!=NULL)
		{
			SELFunc sel_func=will_start_handler.sel_func;
			(will_start_responder->*sel_func)(will_start_handler.sender);
		}

		state=MICurlViewStateInit;
	}

	void MICurlView::reorderViewForEnd( MGObject* sender )
	{
		m_from_view->parent()->_view_flags.user_interaction_enabled=1;
		MGLogD("reorderViewForEnd");
		if(did_stop_responder!=NULL)
		{
			SELFunc sel_func=did_stop_handler.sel_func;
			(did_stop_responder->*sel_func)(did_stop_handler.sender);
		}
		state=MICurlViewStateFinish;
		m_to_view->detachFromSuperview();
		this->detachFromSuperview();
		m_from_view->show();
	}

	MICurlView::MICurlView() :MIScreenCaptureView(true)
	{
        class_name = "MICurlView";
		m_level=0.5;
		state=MICurlViewStateUninit;
	}

	void MICurlView::performUpSequenceFinish( MGObject* sender )
	{
		MGLogD("MICurlView::performUpSequenceFinish");
		m_from_view->parent()->_view_flags.user_interaction_enabled=1;
	}

	void MICurlView::performDownSequenceFinish( MGObject *sender )
	{
		MGLogD("MICurlView::performDownSequenceFinish");
		m_from_view->parent()->_view_flags.user_interaction_enabled=1;

	}
};
