#include "MIDatePicker.h"
#include "MIPickerView.h"
#include <sstream>
#include "../../System/Shared/MGLog.h"
namespace z11
{
	class DatePickerDelegate:public MIPickerViewDelegate,public MIPickerViewDataSource
	{
	private:
		vector< vector<string> > m_titles;
	public:
		DatePickerDelegate()
		{
			vector<string> strings;
			for (int i=0;i<10;i++)
			{
				stringstream ss;
				ss<<2000+i<<"yy";
				strings.push_back(ss.str());
			}
			m_titles.push_back(strings);

			strings.clear();
			for (int i=1;i<=12;i++)
			{
				stringstream ss;
				ss<<i<<"mm";
				strings.push_back(ss.str());
			}
			m_titles.push_back(strings);


			strings.clear();
			for (int i=1;i<=30;i++)
			{
				stringstream ss;
				ss<<i<<"dd";
				strings.push_back(ss.str());
			}
			m_titles.push_back(strings);
		}
		virtual float rowHeightForComponent(MIPickerView *picker_view,int component)
		{
			return 40;
		}
		virtual float widthForComponent(MIPickerView *picker_view,int component)
		{
			if (component!=0)
			{
				return 75;
			}
			return 150;
		}

		//Setting the Content of Component Rows
		virtual string titleForRowForComponent(MIPickerView *picker_view,int row,int component)
		{

			return m_titles[component][row];
		}
		//virtual MIView* viewForRowForComponentReusingView(MIPickerView *picker_view,int row,int component,MIView *view)=0;

		virtual void didSelectRowInComponent(MIPickerView *picker_view,int row,int component)
		{
			MGLogD("didSelectRowInComponent %d",row);
		}


		virtual int numberOfComponentsInPickerView(MIPickerView *view)
		{
			return 3;
		}
		virtual int numberOfRowsInComponent(MIPickerView *view,int component)
		{
			return 10;
		}
	};
	MIDatePicker::MIDatePicker(void)
	{
        class_name = "MIDatePicker";
		impl = NULL;
		delegate= new DatePickerDelegate();
		time = MGTime::Now();
		time.explode(date, true);
	}

	MIDatePicker::~MIDatePicker(void)
	{
		delete_and_set_null(delegate) ;
	}

	void MIDatePicker::setDate( const MGTime::Exploded& _date )
	{
		date=_date;
	}

	void MIDatePicker::initWithFrame( MGRect frame )
	{
		MIControl::initWithFrame(frame);
		frame.x=0;
		frame.y=0;
		setBackgroundColor(MGColor::fromRGBA(100,100,100,100));
		impl=new MIPickerView();
		impl->setDelegate(delegate);
		impl->setDataSource(delegate);
		impl->initWithFrame(frame);
		addSubview(impl);

	}

	MGTime::Exploded MIDatePicker::getDate()
	{
		return date;
	}

}