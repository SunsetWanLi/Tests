#include "MISearchBar.h"
namespace z11
{
	MISearchBar::MISearchBar(void)
	{
        class_name = "MISearchBar";
	}

	MISearchBar::~MISearchBar(void)
	{
	}

	bool MISearchBar::searchResultsButtonSelected()
	{
		return false;
	}

	MGImage * MISearchBar::imageForSearchBarIcon( MISearchBarIcon icon,MIControlState state )
	{
		return NULL;
	}

	void MISearchBar::setImageForSearchBarIcon( MGImage *image,MISearchBarIcon icon,MIControlState state )
	{

	}

	z11::MGOffset MISearchBar::positionAdjustmentForSearchBarIcon( MISearchBarIcon icon )
	{
		return MGOffset();
	}

	void MISearchBar::setPositionAdjustmentForSearchBarIcon( const MGOffset &adjustment,MISearchBarIcon icon )
	{

	}

	MGImage * MISearchBar::scopeBarButtonBackgroundImageForState( MIControlState state )
	{
		return NULL;
	}

	void MISearchBar::setScopeBarButtonBackgroundImageForState( MGImage *backgroundImage,MIControlState state )
	{

	}

	MGImage * MISearchBar::scopeBarButtonDividerImage( MIControlState leftstate,MIControlState rightstate )
	{
		return NULL;
	}

	MGImage * MISearchBar::searchFieldBackgroundImageForState( MIControlState state )
	{
		return NULL;
	}

	void MISearchBar::setSearchFieldBackgroundImageForState( MGImage *backgroundImage ,MIControlState state )
	{

	}

	void MISearchBar::setScopeBarButtonDividerImage( MGImage *dividerImage,MIControlState leftState,MIControlState rightstate )
	{

	}

}