/*
 * =====================================================================================
 *
 *       Filename:  MKShapes.h
 *
 *    Description:  The MKShape class is an abstract class that defines the basic 
 *    properties for all shape-based annotation objects
 *
 *        Version:  1.0
 *        Created:  04/12/2012 11:50:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once
#include "MapKit/Protocols/MKAnnotation.h"
#include "MapKit/Protocols/MKOverlay.h"
#include <assert.h>
namespace z11
{
	class MKShape: public MKOverlay
	{
	public:
		virtual ~MKShape()=0;
		virtual std::string subtitle();
		virtual std::string title();
		void setTitle(const std::string &_title);
		void setSubtitle(const std::string &_subtitle);
	private:
		std::string m_title;
		std::string m_subtitle;

	};
}
