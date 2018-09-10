
#include "ScopeResizer.h"
#include <math.h>

using namespace GIS_Embedded;


ScopeResizer::ScopeResizer()
{

}

void ScopeResizer::setSize(double precision,double max_width,double max_height,double min_width,double min_height)
{

	_precision=precision;
	_s_max=max_width*max_height;
	_max_width=max_width;
	_max_height=max_height;
	_s_min=min_width*min_height;
	_min_width=min_width;
	_min_height=min_height;
	_a = (_s_max / _s_min -_s_max / _s_max) / (_precision*_precision) ;
	_b = _s_max / _s_min - _a * (_precision*_precision) ; //han...


}


Scope* ScopeResizer::resizeScope(Scope& view_scope, Scope& scope)
{
	double width,height,new_width,new_height;
	double new_s;
	Scope* ret;
	width=view_scope.getWidth();
	height=view_scope.getHeight();
	new_width=scope.getWidth();
	new_height=scope.getHeight();
	double zoom_level;
	//Smax/S=a*x^2+b
	
	
	if (new_width/_min_width>new_height/_min_width)
	{
		new_s=new_width*new_width*_min_height/_min_width;
	}
	else
	{
		new_s=new_height*new_height*_min_width/_min_height;
	}
	
	
	if (new_s<_s_min)
		new_s=_s_min;
	if (new_s>_s_max)
		new_s=_s_max;
	
	
	
	zoom_level = sqrt((_s_max/new_s-_b)/_a);
	ret = resizeScope(scope,zoom_level/_precision);
	return ret;


}


Scope* ScopeResizer::resizeScope(Scope& screen_scope, double zoom_level)
{

	if (zoom_level<0||zoom_level>1)
	 	return NULL;
	  
	zoom_level=_precision*zoom_level;
		
	double new_s = _s_max / (_a * pow(zoom_level, 2) + _b);
	double ratio = (double)_max_width / (double)_max_height;
	double new_height = sqrt(new_s / ratio);
	double new_width = new_height * ratio;
	double c_x,c_y;
	double left,top,right,bottom;
	c_x=(screen_scope.getLeft()+screen_scope.getRight())/2;
	c_y=(screen_scope.getTop()+screen_scope.getBottom())/2;
	left=c_x-new_width/2;
	right=c_x+new_width/2;
	top=c_y+new_height/2;
	bottom=c_y-new_height/2;
	Scope* new_screen_scope = new Scope(static_cast<int>(left),static_cast<int>(top),static_cast<int>(right),static_cast<int>(bottom));
	return new_screen_scope;

}