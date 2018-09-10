#include "MISegmentedControl.h"
#include "../../GraphicCore/MGFont.h"
#include "../../System/Shared/MGLog.h"
namespace z11
{

	static MGFont *sharedWhiteFont()
	{
		//static MGFont * grey_font=MGFont::create(17,RGBAColorRef::white());

		MGFont *shared_grey_font=MGFont::shareFont();
		shared_grey_font->setFaceSize(17);
		shared_grey_font->setColor(RGBAColorRef::white());

		return shared_grey_font;

	}

	static MGFont *sharedGreyFont()
	{
		//static MGFont * grey_font=MGFont::create(17,RGBAColorRef::darkGray());
		//return grey_font;
		MGFont *shared_grey_font=MGFont::shareFont();
		shared_grey_font->setFaceSize(17);
		shared_grey_font->setColor(RGBAColorRef::lightBlack());

		return shared_grey_font;
	}
	MISegmentedControl::MISegmentedControl(void)
	{
        class_name = "MISegmentedControl";
		m_background_color=MGColor::fromColorRef(RGBAColorRef::lightGray());
		m_selected_segment_index=0;
		m_is_clicked=false;
		m_font_size=0;
	}

	MISegmentedControl::~MISegmentedControl(void)
	{
	}

	void MISegmentedControl::initWithFrame(MGRect frame)
	{
		//frame.height=44;
		MIControl::initWithFrame(frame);
        if(m_font_size == 0)
            m_font_size = 0.5f*frame.height;
	}
	extern void TestDrawFilledRoundRect2(const MGRect& rect, RGBAColorRef color , RGBAColorRef color2,float round_rate , RGBAColorRef blend, DrawRoundRectMode round_mode);
	void MISegmentedControl::drawself()
	{
		MGGraphicEngine* graphic_engine = MGGraphicEngine::sharedGraphicEngine();

		RGBAColorRef color_ref;
		if (m_background_color != NULL) {
			color_ref = m_background_color->m_color;
			color_ref.m_alpha_component = (byte_8)(m_alpha * color_ref.m_alpha_component);
		}else{
			color_ref = RGBAColorMake(0, 0, 0, (byte_8)(m_alpha * 255));
		}
		MGRect rect=m_bounds;
		graphic_engine->drawFilledLightedRect(rect, RGBAColorRef::white(),90,40);
		//TestDrawFilledRoundRect2(rect,RGBAColorRef::white(),RGBAColorRef::lightGray(),10,RGBAColorRef::white(),RoundRectAll);
		const float size=rect.width/titles.size();
		rect.width=size;
		rect.x+=m_selected_segment_index*size;
		graphic_engine->drawFilledLightedRect(rect,RGBAColorMake(100,157,255),20,90);
		if (titles.size()>0)
		{
			rect=m_bounds;
			rect.width=size;
			if (m_selected_segment_index!=0) {
				MGFont *font = sharedGreyFont();
				font->setFaceSize(m_font_size);
				graphic_engine->drawText(titles[0],font,rect,MILineBreakModeClip,MITextAlignmentCenterMiddle);
			} else {
				MGFont *font = sharedWhiteFont();
				font->setFaceSize(m_font_size);
				graphic_engine->drawText(titles[m_selected_segment_index],font,rect,MILineBreakModeClip,MITextAlignmentCenterMiddle);
			}

			for (size_t i=1;i<titles.size();i++)
			{
				rect.x+=size;
				if (m_selected_segment_index!=i) {
					MGFont *font = sharedGreyFont();
					font->setFaceSize(m_font_size);
					graphic_engine->drawText(titles[i],font,rect,MILineBreakModeClip,MITextAlignmentCenterMiddle);
				} else {
					MGFont *font = sharedWhiteFont();
					font->setFaceSize(m_font_size);
					graphic_engine->drawText(titles[m_selected_segment_index],font,rect,MILineBreakModeClip,MITextAlignmentCenterMiddle);
				}
				graphic_engine->drawLine( MGPointMake(rect.x,m_bounds.y), MGPointMake(rect.x,m_bounds.y+m_bounds.height), RGBAColorRef::darkGray());
			}
		}
	}

	void MISegmentedControl::setImageForSegmentAtIndex( MGImage *image,uint_32 segment )
	{

	}

	const MGImage* MISegmentedControl::imageForSegmentAtIndex( uint_32 segment )
	{
		return NULL;
	}

	void MISegmentedControl::setTitleForSegmentAtIndex( const string &title,uint_32 segment )
	{
		if (segment < titles.size())
		{
			titles[segment]=title;
		}
	}

	const string & MISegmentedControl::titleForSegmentAtIndex( uint_32 segment )
	{
		if (segment < titles.size())
		{
			return titles[segment];
		}
		static string str;
		return str;
	}

	void MISegmentedControl::initWithItems( const std::vector<MGImage *>& items )
	{

	}

	void MISegmentedControl::initWithItems( const std::vector<std::string>& items )
	{
		titles=items;
	}

	void MISegmentedControl::insertSegmentWithImage( MGImage *image,uint_32 segment,bool animated/*=true*/ )
	{

	}

	void MISegmentedControl::insertSegmentWithTitle( const std::string &title,uint_32 segment,bool animated/*=true*/ )
	{
		if (segment <= titles.size())
		{
			titles.insert(titles.begin()+segment,title);
		}
	}

	void MISegmentedControl::removeAllSegments()
	{
		titles.clear();
		images.clear();
	}

	void MISegmentedControl::removeSegmentAtIndex( uint_32 segment,bool animated/*=true*/ )
	{
		if (segment < titles.size())
		{
			titles.erase(titles.begin()+segment);
		}
	}

	void MISegmentedControl::setEnabledForSegmentAtIndex( bool enabled ,uint_32 segment )
	{

	}

	bool MISegmentedControl::isEnabledForSegmentAtIndex( uint_32 segment )
	{
		return false;
	}

	void MISegmentedControl::setContentOffsetForSegmentAtIndex( const MGSize &offset,uint_32 segment )
	{

	}

	const MGSize & MISegmentedControl::contentOffsetForSegmentAtIndex( uint_32 segment )
	{
		static MGSize size;
		return size;
	}

	void MISegmentedControl::setWidthForSegmentAtIndex( float width ,uint_32 segment )
	{

	}

	float MISegmentedControl::widthForSegmentAtIndex( uint_32 segment )
	{
		return 0.0f;
	}

	z11::uint_32 MISegmentedControl::numberOfSegments()
	{
		return titles.size();
	}

	z11::MIEventHandleState MISegmentedControl::touchesBegan( const std::set<MITouch*>& touches, const MIEvent* const event )
	{
		m_is_clicked=true;
		return MIEventHandleStateDone;
	}

	z11::MIEventHandleState MISegmentedControl::touchesEnded( const std::set<MITouch*>& touches, const MIEvent* const event )
	{
		if (!pointInside(((*touches.begin())->locationInView(this)), event))
			return MIEventHandleStatePass;
		
		if (m_is_clicked)
		{
			const int size=m_bounds.width/titles.size();
			m_selected_segment_index=(int)(((*touches.begin())->point.x-m_bounds.x)/size);
			{
				EventsMapIterator it = events_dispatch_map.find(MIControlEventValueChanged);
				if (it != events_dispatch_map.end()) {
					assert(it->second.view_controller != NULL);
					assert(it->second.event_func != NULL);
					EventFunc func = it->second.event_func;
					((it->second.view_controller)->*func)(this);
				}

			}
			m_is_clicked=false;
		}
		return MIEventHandleStateDone;
	}

	z11::MIEventHandleState MISegmentedControl::touchesCancelled( const std::set<MITouch*>& touches, const MIEvent* const event )
	{
		m_is_clicked=false;
		return MIEventHandleStateDone;

	}

}
