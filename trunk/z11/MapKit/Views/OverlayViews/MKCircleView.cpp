/*
 * =====================================================================================
 *
 *       Filename:  MKCircleView.cpp
 *
 *    Description:  The MKCircleView class provides the visual 
 *    representation for an MKCircle annotation object
 *
 *        Version:  1.0
 *        Created:  04/12/2012 03:03:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MKCircleView.h"
namespace z11
{
    
    MKCircleView::MKCircleView()
    {
        class_name = "MKCircleView";
        _view_flags.clip_subviews = 0;
    }
    
	MKCircle * MKCircleView::circle()
	{
		return m_circle;
	}

	MKCircleView* MKCircleView::initWithCircle( MKCircle *_circle )
	{
		MKCircleView *ret=new MKCircleView();
		ret->m_circle=_circle;
		return ret;
	}

	void MKCircleView::createPath()
	{

	}

}
