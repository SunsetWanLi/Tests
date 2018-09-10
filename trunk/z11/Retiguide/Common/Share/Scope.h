#ifndef _CORE_SHARE_SCOPE_H
#define _CORE_SHARE_SCOPE_H

#include "../Types/basic_types.h"

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

namespace GIS_Embedded
{
	class Scope
	{
	public:

		Scope(int left, int top, int right, int bottom)
		{
			m_left = left;
			m_right = right;
			m_top = top;
			m_bottom = bottom;
		}

		Scope()
		{
    		m_left   =  INT_MAX;
    		m_top    = -INT_MAX;
    		m_right  = -INT_MAX;
    		m_bottom =  INT_MAX;
		}

		int getWidth()const
		{
			return m_right - m_left;
		}

		int getHeight()const
		{
			return m_top - m_bottom;
		}

		int getLeft()const
		{
			return m_left;
		}

		int getRight()const
		{
			return m_right;
		}

		int getTop()const
		{
			return m_top;
		}

		int getBottom()const
		{
			return m_bottom;
		}

		void setLeft(int left)
		{
			m_left = left;
		}

		void setTop(int top)
		{
			m_top = top;
		}

		void setRight(int right)
		{
			m_right = right;
		}

		void setBottom(int bottom)
		{
			m_bottom = bottom;
		}

		void setScope(int left, int top, int right, int bottom)
		{
			m_left = left;
			m_right = right;
			m_top = top;
			m_bottom = bottom;
		}

		void setScope(const Scope& new_scope)
		{
			m_left = new_scope.m_left;
			m_top = new_scope.m_top;
			m_right = new_scope.m_right;
			m_bottom = new_scope.m_bottom;
		}

		bool isIntersect(const Scope& scope)const
		{
			return scope.getLeft() <= m_right && //zgr, 当scope是一个点且刚好在this的边上时，视为intersect
				scope.getRight() >= m_left &&
				scope.getBottom() <= m_top && 
				scope.getTop() >= m_bottom;
		}

		bool isContain(double x, double y)const
		{
			return x >= m_left && x <= m_right && y <= m_top && y >= m_bottom; // zgr, 当x,y刚好在this的边上时，视为contain
		}

		bool isContain(int x, int y)const
		{
			return x >= m_left && x <= m_right && y <= m_top && y >= m_bottom; // zgr, 当x,y刚好在this的边上时，视为contain
		}

		bool isContain(const Scope& scope)const
		{
			return scope.m_left >= m_left && scope.m_right <= m_right
				&& scope.m_bottom >= m_bottom && scope.m_top <= m_top; 
		}

		double getArear()const
		{
			return (double)((m_right - m_left) * (m_top - m_bottom));
		}

		bool equal(const Scope& scope)const
		{
			return scope.m_left == m_left && scope.m_top == m_top && scope.m_right == m_right && scope.m_bottom == m_bottom;
		}

		void resizeScope(double ratio)
		{
			double x = (double)(m_left + m_right) / 2;
			double y = (double)(m_top + m_bottom) / 2;
			double new_width = (m_right - m_left) * ratio;
			double new_height = (m_top - m_bottom) * ratio;
			m_left = (int)(x - new_width / 2);
			m_right = (int)(x + new_width / 2);
			m_top = (int)(y + new_height / 2);
			m_bottom = (int)(y - new_height / 2);
		}

		void extend(Scope& scope)
		{
    		if (scope.m_left<m_left)
    			m_left=scope.m_left;
    		if (scope.m_top>m_top)
    			m_top=scope.m_top;
    		if (scope.m_right>m_right)
    			m_right=scope.m_right;
    		if (scope.m_bottom<m_bottom)
    			m_bottom=scope.m_bottom;
		}
		void extend(Pos2DEx& pos2d)
		{
			if (pos2d.d0<m_left)
    			m_left=(int)pos2d.d0;
    		if (pos2d.d1>m_top)
    			m_top=(int)pos2d.d1;
    		if (pos2d.d0>m_right)
    			m_right=(int)pos2d.d0;
			if (pos2d.d1<m_bottom)
    			m_bottom=(int)pos2d.d1;
		}

		//void moveCenter(int offsetX,int offsetY)
		//{
		//	m_left += offsetX;
		//	m_right += offsetX;
		//	m_top += offsetY;
		//	m_bottom += offsetY;
		//}

		//防止旋转后边缘产生空白，将Scope放大
		void rotateAdjust(bool isRotate,int screenWidth,int screenHeight);

		////对应rotateAdjust(),将Scope缩小
		//void antiRotateAdjust(bool isRotate/*,int screenWidth,int screenHeight*/);

	public:
		int m_left;
		int m_right;
		int m_top;
		int m_bottom;
	};
}

#endif