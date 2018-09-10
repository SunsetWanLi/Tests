/*
 * =====================================================================================
 *
 *       Filename:  MIBarButtonItem.cpp
 *
 *    Description:  A bar button item is a button specialized for placement on a UIToolbar
 *    or UINavigationBar object.
 *
 *        Version:  1.0
 *        Created:  02/08/2012 11:18:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MIBarButtonItem.h"
namespace z11 {
	MIBarButtonItem* z11::MIBarButtonItem::initWithBarButtonSystemItem(MIBarButtonItem systemItem, SELHandler handler) {
		MIBarButtonItem *ret=new MIBarButtonItem();
		return ret;
	}
	MIBarButtonItem* z11::MIBarButtonItem::initWithCustomView(MIView* customView) {
		MIBarButtonItem *ret=new MIBarButtonItem();
		return ret;
	}
	MIBarButtonItem* z11::MIBarButtonItem::initWithImage(MGImage* image, MIBarButtonItemStyle style, SELHandler handler) {
		MIBarButtonItem *ret=new MIBarButtonItem();
		return ret;
	}
	MIBarButtonItem* z11::MIBarButtonItem::initWithTitle(const std::string& title, MIBarButtonItemStyle style, SELHandler handler) {
		MIBarButtonItem *ret=new MIBarButtonItem();
		return ret;
	}
	z11::MIBarButtonItem::MIBarButtonItem()
		:style(MIBarButtonItemStylePlain) {}
	MIView* z11::MIBarButtonItem::customView() {
		return NULL;
	}
}
