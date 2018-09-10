#include "Scope.h"
#include <cmath>
using std::sqrt;


namespace GIS_Embedded
{

	//鉴于scope的长宽比例与屏幕长宽比例基本一致，采用简化处理,处理后为一正方形
	void Scope::rotateAdjust(bool isRotate,int screenWidth,int screenHeight)
	{
		if(!isRotate)
			return ;

		int center_x = (m_right + m_left)/2;
		int center_y = (m_top + m_bottom)/2;

		int half_square_length;
		if(screenWidth > screenHeight)
		{
			double ratio = sqrt(0.0+screenWidth*screenWidth+screenHeight*screenHeight)/screenWidth;
			half_square_length = (int)((m_right-m_left)*ratio)/2;
		}
		else
		{
			double ratio = sqrt(0.0+screenWidth*screenWidth+screenHeight*screenHeight)/screenHeight;
			half_square_length = (int)((m_top-m_bottom)*ratio)/2;
		}

		m_top = (center_y + half_square_length);
		m_bottom = (center_y - half_square_length);
		m_left = (center_x - half_square_length); 
		m_right = (center_x + half_square_length);

	}

	////对应rotateAdjust(),将Scope缩小，并根据屏幕长宽比例予以修正
	//void Scope::antiRotateAdjust(bool isRotate/*,int screenWidth,int screenHeight*/)
	//{
	//	if(!isRotate)
	//		return ;

	//	int width = m_right - m_left;
	//	int height = m_top - m_bottom;

	//	int center_x = (m_right + m_left)/2;
	//	int center_y = (m_top + m_bottom)/2;

	//	double ratio = height/sqrt(width*width+height*height)-0.1;

	//	int new_width,new_height;			
	//	if(width > height)
	//	{
	//		new_height = height*ratio;
	//		new_width = new_height*width/height;
	//	}
	//	else
	//	{
	//		new_height = width*ratio;
	//		new_width = new_height*width/height;
	//	}

	//	m_top = (center_y + new_height/2);
	//	m_bottom = (center_y - new_height/2);
	//	m_left = (center_x - new_width/2); 
	//	m_right = (center_x + new_width/2);
	//}

}


//Scope::Scope(double left, double top, double right, double bottom)
//    {
//        _left = left;
//        _right = right;
//        _top = top;
//        _bottom = bottom;
//    }
//
//Scope::Scope()
//    {
//    	_left   =  DOUBLE_MAX;
//    	_top    = -DOUBLE_MAX;
//    	_right  = -DOUBLE_MAX;
//    	_bottom =  DOUBLE_MAX;
//    }
//
//double Scope::getWidth()const
//    {
//        return _right - _left;
//    }
//
//double Scope::getHeight()const
//    {
//        return _top - _bottom;
//    }
//
//double Scope::getLeft()const
//    {
//        return _left;
//    }
//
//double Scope::getRight()const
//    {
//        return _right;
//    }
//
//double Scope::getTop()const
//    {
//        return _top;
//    }
//
//double Scope::getBottom()const
//    {
//        return _bottom;
//    }
//
//void Scope::setLeft(double left)
//    {
//        _left = left;
//    }
//
//void Scope::setTop(double top)
//    {
//        _top = top;
//    }
//
//void Scope::setRight(double right)
//    {
//        _right = right;
//    }
//
//void Scope::setBottom(double bottom)
//    {
//        _bottom = bottom;
//    }
//
//void Scope::setScope(double left, double top, double right, double bottom)
//    {
//        _left = left;
//        _right = right;
//        _top = top;
//        _bottom = bottom;
//    }
//
//bool Scope::isIntersect(const Scope& scope)const
//    {
//        //int bit = 0;
//        //if(scope.getLeft() >= _right)
//        //    bit |= 0x1;
//        //if(scope.getRight() <= _left)
//        //    bit |= 0x2;
//        //if(scope.getBottom() >= _top)
//        //    bit |= 0x4;
//        //if(scope.getTop() <= _bottom)
//        //    bit |= 0x8;
//        //return bit == 0;
//		return scope.getLeft() <= _right && //zgr, 当scope是一个点且刚好在this的边上时，视为intersect
//			scope.getRight() >= _left &&
//			scope.getBottom() <= _top && 
//			scope.getTop() >= _bottom;
//    }
//
//bool Scope::isContain(double x, double y)const
//    {
//        return x >= _left && x <= _right && y <= _top && y >= _bottom; // zgr, 当x,y刚好在this的边上时，视为contain
//    }
//
//bool Scope::isContain(const Scope& scope)const
//    {
//        //int bit_test = 0;
//        //if(scope.getLeft() < _left)
//        //    bit_test |= 0x1;
//        //if(scope.getRight() > _right)
//        //    bit_test |= 0x2;
//        //if(scope.getTop() > _top)
//        //    bit_test |= 0x4;
//        //if(scope.getBottom() < _bottom)
//        //    bit_test |= 0x8;
//        //return bit_test == 0;
//
//		//08.8.6 by my
//		//return scope.getLeft() >= _left && scope.getRight() <= _left
//		//	&& scope.getBottom() >= _bottom && scope.getTop() <= _top; 
//		return scope.getLeft() >= _left && scope.getRight() <= _right
//			&& scope.getBottom() >= _bottom && scope.getTop() <= _top; 
//    }
//
//double Scope::getArear()const
//    {
//        return (_right - _left) * (_top - _bottom);
//    }
//
//void Scope::setScope(const Scope& new_scope)
//    {
//        _left = new_scope.getLeft();
//        _top = new_scope.getTop();
//        _right = new_scope.getRight();
//        _bottom = new_scope.getBottom();
//    }
//
//bool Scope::equal(const Scope& scope)const
//    {
//        return scope.getLeft() == _left && scope.getTop() == _top && scope.getRight() == _right && scope.getBottom() == _bottom;
//    }
//
//Scope* Scope::computeIntersect(const Scope& other_scope)
//    {
//		if(this->isIntersect(other_scope))
//		{
//			double left = _left >= other_scope.getLeft() ? _left : other_scope.getLeft();
//			double top = _top <= other_scope.getTop() ? _top : other_scope.getTop();
//			double right = _right <= other_scope.getRight() ? _right : other_scope.getRight();
//			double bottom = _bottom >= other_scope.getBottom() ? _bottom : other_scope.getBottom();
//			return new Scope(left,top,right,bottom);
//		}
//		else
//			return NULL;
//    }
//
//void Scope::resizeScope(double ratio)
//    {
//        double x = (_left + _right) / 2;
//        double y = (_top + _bottom) / 2;
//        double new_width = (_right - _left) * ratio;
//        double new_height = (_top - _bottom) * ratio;
//        _left = x - new_width / 2;
//        _right = x + new_width / 2;
//        _top = y + new_height / 2;
//        _bottom = y - new_height / 2;
//    }
//    
//void Scope::extend(Scope& scope)
//    {
//    	if (scope._left<_left)
//    		_left=scope._left;
//    	if (scope._top>_top)
//    		_top=scope._top;
//    	if (scope._right>_right)
//    		_right=scope._right;
//    	if (scope._bottom<_bottom)
//    		_bottom=scope._bottom;
//    }
//
//void Scope::rotateAdjust()
//{
//	double width = _right - _left;
//	double height = _top - _bottom;
//	double center_x = (_right + _left)/2;
//	double center_y = (_top + _bottom)/2;
//
//	if(width > height)
//	{
//		double new_half_width = width*1.4/2;
//		_top = center_y + new_half_width;
//		_bottom = center_y - new_half_width;
//		_left = center_x - new_half_width; 
//		_right = center_x + new_half_width;
//	}
//	else
//	{
//		double new_half_height = height*1.4/2;
//		_left = center_x - new_half_height;
//		_right = center_x + new_half_height;
//		_top = center_y + new_half_height;
//		_bottom = center_y - new_half_height;
//	}
//}