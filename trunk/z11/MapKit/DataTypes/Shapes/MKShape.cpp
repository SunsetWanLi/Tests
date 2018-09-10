/*
 * =====================================================================================
 *
 *       Filename:  MKShapes.cpp
 *
 *    Description:  The MKShape class is an abstract class that defines the basic 
 *    properties for all shape-based annotation objects
 *
 *        Version:  1.0
 *        Created:  04/12/2012 11:48:33 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKShape.h"
namespace z11
{
	z11::MKShape::~MKShape()
	{

	}


	std::string MKShape::subtitle()
	{
		return m_subtitle;
	}

	std::string MKShape::title()
	{
		return m_title;
	}

	void MKShape::setTitle( const std::string &_title )
	{
		m_title=_title;
	}

	void MKShape::setSubtitle( const std::string &_subtitle )
	{
		m_subtitle=_subtitle;
	}
}