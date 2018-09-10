#include "ScreenPixMap.h"
#include "Retiguide/Common/Types/basic_types.h"
#include <cstring>

namespace GIS_Embedded
{

	ScreenPixMap::ScreenPixMap()
	{

	}
	void ScreenPixMap::setScreenSizeUpToDown(int scr_width,int scr_height,int unit_width,int unit_height)
	{
		m_screen_rect.left = 0;//
		m_screen_rect.right = scr_width;
		m_screen_rect.top = 0;
		m_screen_rect.bottom = scr_height;

		int rows = scr_height / unit_height;
		int cols = scr_width / unit_width;
		int sum = rows*cols;

		_map.resize(sum);
		clear();

		//for(int i=0;i < sum ;i++)
		//	_map.push_back('1');
		_rows = rows;
		_cols = cols;
		_length = rows * cols;
		_unit_width = unit_width;
		_unit_height = unit_height;
		_dirty = (unsigned char)1;

	}
	void ScreenPixMap::setScreenSize(int scr_width, int scr_height, int unit_width, int unit_height)
	{
		m_screen_rect.left = 0;
		m_screen_rect.right = scr_width;
		m_screen_rect.top = 0;
		m_screen_rect.bottom = scr_height;

		int rows = scr_height / unit_height;
		int cols = scr_width / unit_width;
		int sum = rows*cols;

		_map.resize(sum);
		clear();

		//for(int i=0;i < sum ;i++)
		//	_map.push_back('1');
		_rows = rows;
		_cols = cols;
		_length = rows * cols;
		_unit_width = unit_width;
		_unit_height = unit_height;
		_dirty = (unsigned char)1;

	}

	void ScreenPixMap::clear()
	{
		memset(&_map[0], 0, _map.size());
		//for(int i = 0; i < _length; i++)
		//    _map[i] = '1';

	}

	bool ScreenPixMap::testDirty(UserScreenRect& screen_rect)
	{
		int lt_unit_col = screen_rect._left / _unit_width;
		int lt_unit_row = screen_rect._top / _unit_height;
		int rb_unit_col = screen_rect._right / _unit_width;
		int rb_unit_row = screen_rect._bottom / _unit_height;
		if(lt_unit_col < 0)
			lt_unit_col = 0;
		if(lt_unit_col >= _cols)
			lt_unit_col = _cols - 1;
		if(lt_unit_row < 0)
			lt_unit_row = 0;
		if(lt_unit_row >= _rows)
			lt_unit_row = _rows - 1;
		if(rb_unit_col < 0)
			rb_unit_col = 0;
		if(rb_unit_col >= _cols)
			rb_unit_col = _cols - 1;
		if(rb_unit_row < 0)
			rb_unit_row = 0;
		if(rb_unit_row >= _rows)
			rb_unit_row = _rows - 1;
		for(int i = lt_unit_row; i <= rb_unit_row; i++)
		{
			for(int j = lt_unit_col; j <= rb_unit_col; j++)
				if(_map[i * _cols + j] == _dirty)
					return true;

		}

		return false;
	}

	bool ScreenPixMap::testDirty(z11::MGPoint& left_top,z11::MGSize& size)
	{
		static UserScreenRect rect;
		rect._left = left_top.x;
		rect._top = left_top.y;
		rect._right = left_top.x + size.width;
		rect._bottom = left_top.y + size.height;
		return testDirty(rect);
	}

	bool ScreenPixMap::testDirty(/*vector<pair<int,int> >* test,*/z11::MGPoint& from,z11::MGPoint& to,z11::MGSize& labelSize)
	{

		if(from.y == to.y)
		{
			static UserScreenRect rect;
			rect._left = from.x;
			rect._top = from.y - labelSize.height/2;
			rect._right = from.x + labelSize.width;
			rect._bottom = from.y + labelSize.height/2;

			return testDirty(rect);
		}
		else if(from.x == to.x)
		{
			static UserScreenRect rect;
			if(from.y < to.y)
			{
				rect._left = from.x - labelSize.width/2;
				rect._top = from.y;
				rect._right = from.x + labelSize.width/2;
				rect._bottom = from.y + labelSize.height;
			}
			else
			{
				rect._left = from.x - labelSize.width/2;
				rect._top = to.y;
				rect._right = from.x + labelSize.width/2;
				rect._bottom = to.y + labelSize.height;
			}

			return testDirty(rect);
		}

		int hWidthj2 = labelSize.height/(_unit_width+_unit_height);
		if(hWidthj2 < 1)
			hWidthj2 = 1;

		int a = 1,b = 1;
		if(from.x > to.x)
			a = -1;
		if(from.y > to.y)
			b = -1;

		int x = from.x/_unit_width;
		int y = from.y/_unit_height;
		int endX = to.x/_unit_width;
		int endY = to.y/_unit_height;
		int dx = to.x/_unit_width - from.x/_unit_width;
		int dy = to.y/_unit_height - from.y/_unit_height;
		unsigned long p = _cols * y + x;

		int dxa = dx * a;
		int dyb = dy * b;
		int twodxa = 2 * dxa;
		int twodyb = 2 * dyb;
		int pitchb = _cols * b;

		int pk;

		if(dxa < dyb)
		{
			int twodxa_dyb = twodxa - twodyb;
			pk = twodxa - dyb;

			do
			{
				for(int flagI =p-hWidthj2+1,flagSum=p+hWidthj2;flagI<=flagSum;flagI++)
				{
					if(flagI > _cols*_rows - 1 || flagI <0)
						continue;

					//test->push_back(make_pair(flagI%_cols,flagI/_cols));
					if(_map[flagI] == _dirty)
						return true;
				}

				y += b;
				if(pk < 0)
				{
					pk += twodxa;

					p += pitchb;
				}
				else
				{
					x += a;
					pk += twodxa_dyb;

					p += pitchb + a;
				}

			}while(y != endY);

		}
		else
		{
			int twodyb_dxa = twodyb - twodxa;
			pk = twodyb - dxa;

			do
			{
				for(int flagI =-hWidthj2+1,flagSum=hWidthj2;flagI<=flagSum;flagI++)
				{
					int index = p + flagI * _cols;
					if(index > _cols*_rows - 1 || index <0)
						continue;

					//test->push_back(make_pair(index%_cols,index/_cols));
					if(_map[index] == _dirty)
						return true;
				}

				x += a;
				if(pk < 0)
				{
					pk += twodyb;
					p += a;
				}
				else
				{
					y += b;
					pk += twodyb_dxa;
					p += pitchb + a;

				}
			}while(x != endX);
		}

		return false;
	}

	void ScreenPixMap::setDirty(z11::MGPoint& from,z11::MGPoint& to,z11::MGSize& labelSize)
	{
		if(from.y == to.y)
		{
			static UserScreenRect rect;
			rect._left = from.x;
			rect._top = from.y - labelSize.height/2;
			rect._right = from.x + labelSize.width;
			rect._bottom = from.y + labelSize.height/2;

			return setDirty(rect);
		}
		else if(from.x == to.x)
		{
			static UserScreenRect rect;
			if(from.y < to.y)
			{
				rect._left = from.x - labelSize.width/2;
				rect._top = from.y;
				rect._right = from.x + labelSize.width/2;
				rect._bottom = from.y + labelSize.height;
			}
			else
			{
				rect._left = from.x - labelSize.width/2;
				rect._top = to.y;
				rect._right = from.x + labelSize.width/2;
				rect._bottom = to.y + labelSize.height;
			}

			return setDirty(rect);
		}

		int hWidthj2 = labelSize.height/(_unit_width+_unit_height);
		if(hWidthj2 < 1)
			hWidthj2 = 1;

		int a = 1,b = 1;
		if(from.x > to.x)
			a = -1;
		if(from.y > to.y)
			b = -1;

		int x = from.x/_unit_width;
		int y = from.y/_unit_height;
		int endX = to.x/_unit_width;
		int endY = to.y/_unit_height;
		int dx = to.x/_unit_width - from.x/_unit_width;
		int dy = to.y/_unit_height - from.y/_unit_height;
		unsigned long p = _cols * y + x;

		int dxa = dx * a;
		int dyb = dy * b;
		int twodxa = 2 * dxa;
		int twodyb = 2 * dyb;
		int pitchb = _cols * b;

		int pk;

		if(dxa < dyb)
		{
			int twodxa_dyb = twodxa - twodyb;
			pk = twodxa - dyb;

			do
			{
				for(int flagI =p-hWidthj2+1,flagSum=p+hWidthj2;flagI<=flagSum;flagI++)
				{
					if(flagI > _cols*_rows - 1 || flagI <0)
						continue;

					_map[flagI] = _dirty;
				}

				y += b;
				if(pk < 0)
				{
					pk += twodxa;

					p += pitchb;
				}
				else
				{
					x += a;
					pk += twodxa_dyb;

					p += pitchb + a;
				}

			}while(y != endY);

		}
		else
		{
			int twodyb_dxa = twodyb - twodxa;
			pk = twodyb - dxa;

			do
			{
				for(int flagI =-hWidthj2+1,flagSum=hWidthj2;flagI<=flagSum;flagI++)
				{
					int index = p + flagI * _cols;
					if(index > _cols*_rows - 1 || index <0)
						continue;

					_map[index] = _dirty;
				}

				x += a;
				if(pk < 0)
				{
					pk += twodyb;
					p += a;
				}
				else
				{
					y += b;
					pk += twodyb_dxa;
					p += pitchb + a;

				}
			}while(x != endX);
		}

		return;
	}

	void ScreenPixMap::setDirty(UserScreenRect& screen_rect)
	{
		int lt_unit_col = screen_rect._left / _unit_width;
		int lt_unit_row = screen_rect._top / _unit_height;
		int rb_unit_col = screen_rect._right / _unit_width;
		int rb_unit_row = screen_rect._bottom / _unit_height;
		if(lt_unit_col < 0)
			lt_unit_col = 0;
		if(lt_unit_col >= _cols)
			lt_unit_col = _cols - 1;
		if(lt_unit_row < 0)
			lt_unit_row = 0;
		if(lt_unit_row >= _rows)
			lt_unit_row = _rows - 1;
		if(rb_unit_col < 0)
			rb_unit_col = 0;
		if(rb_unit_col >= _cols)
			rb_unit_col = _cols - 1;
		if(rb_unit_row < 0)
			rb_unit_row = 0;
		if(rb_unit_row >= _rows)
			rb_unit_row = _rows - 1;
		for(int i = lt_unit_row; i <= rb_unit_row; i++)
		{
			for(int j = lt_unit_col; j <= rb_unit_col; j++)
				_map[i * _cols + j] = _dirty;

		}

	}

	void ScreenPixMap::setDirty(z11::MGPoint& left_top,z11::MGSize& size)
	{
		static UserScreenRect rect;
		rect._left = left_top.x;
		rect._top = left_top.y;
		rect._right = left_top.x + size.width;
		rect._bottom = left_top.y + size.height;
		return setDirty(rect);
	}

}
