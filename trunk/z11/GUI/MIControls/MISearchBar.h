//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MISearchBar.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-11-11)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MISEARCHBAR_H_
#define _MISEARCHBAR_H_
#pragma once
#include "../Base/MIView.h"
#include "MIControl.h"
namespace z11
{
	typedef enum {
		MIBarStyleDefault          = 0,
		MIBarStyleBlack            = 1,
	} MIBarStyle;

	//MITextInputTraits 
	typedef enum {
		MITextAutocapitalizationTypeNone,
		MITextAutocapitalizationTypeWords,
		MITextAutocapitalizationTypeSentences,
		MITextAutocapitalizationTypeAllCharacters,
	} MITextAutocapitalizationType;

	typedef enum {
		MITextAutocorrectionTypeDefault,
		MITextAutocorrectionTypeNo,
		MITextAutocorrectionTypeYes,
	} MITextAutocorrectionType;

	typedef enum {
		MIKeyboardTypeDefault,
		MIKeyboardTypeASCIICapable,
		MIKeyboardTypeNumbersAndPunctuation,
		MIKeyboardTypeURL,
		MIKeyboardTypeNumberPad,
		MIKeyboardTypePhonePad,
		MIKeyboardTypeNamePhonePad,
		MIKeyboardTypeEmailAddress,
		MIKeyboardTypeDecimalPad,
		MIKeyboardTypeTwitter,
		MIKeyboardTypeAlphabet = MIKeyboardTypeASCIICapable
	} MIKeyboardType;

	typedef enum {
		MITextSpellCheckingTypeDefault,
		MITextSpellCheckingTypeNo,
		MITextSpellCheckingTypeYes,
	} MITextSpellCheckingType;

	//search bar
	typedef enum {
		MISearchBarIconSearch,
		MISearchBarIconClear,
		MISearchBarIconBookmark,
		MISearchBarIconResultsList,
	} MISearchBarIcon;

	class MISearchBarDelegate ;
	class MISearchBar:public MIView
	{
	public:
		MISearchBar(void);
		~MISearchBar(void);

		//Text Content
		MGPROPERTY(std::string , m_placeholder,Placeholder);
		MGPROPERTY(std::string , m_prompt,Prompt);
		MGPROPERTY(std::string , m_text,Text);

		//Display Attributes
		MGPROPERTY(MIBarStyle,m_bar_style,BarStyle);
		MGPROPERTY(RGBAColorRef,m_tintColor,TintColor);
		MGPROPERTY(bool,m_translucent,Translucent);

		//Text Input Properties
		MGPROPERTY(MITextAutocapitalizationType,m_autocapitalizationType,AutocapitalizationType);
		MGPROPERTY(MITextAutocorrectionType,m_autocorrectionType,AutocorrectionType);
		MGPROPERTY(MIKeyboardType,m_keyboardType,KeyboardType);
		MGPROPERTY(MITextSpellCheckingType,m_spellCheckingType,SpellCheckingType);

		//Button Configuration
		MGPROPERTY(bool,m_showsBookmarkButton,ShowsBookmarkButton);
		MGPROPERTY(bool,m_showsCancelButton,ShowsCancelButton);
		MGPROPERTY(bool,m_showsSearchResultsButton,ShowsSearchResultsButton);
		bool searchResultsButtonSelected();

		//Scope Buttons
		//scopeButtonTitles  property
		//selectedScopeButtonIndex  property
		//showsScopeBar  property

		//Delegate
		MISearchBarDelegate *delegate;

		//Customizing Appearance
		MGPROPERTY(MGImage *,m_backgroundImage,BackgroundImage);
		MGImage *imageForSearchBarIcon(MISearchBarIcon icon,MIControlState state);
		void setImageForSearchBarIcon(MGImage *image,MISearchBarIcon icon,MIControlState state);
		MGOffset positionAdjustmentForSearchBarIcon(MISearchBarIcon icon);
		void setPositionAdjustmentForSearchBarIcon(const MGOffset &adjustment,MISearchBarIcon icon);
		MGPROPERTY(MGImage *,m_scopeBarBackgroundImage,ScopeBarBackgroundImage);
		MGImage *scopeBarButtonBackgroundImageForState(MIControlState state);
		void setScopeBarButtonBackgroundImageForState(MGImage *backgroundImage,MIControlState state);
		MGImage *scopeBarButtonDividerImage(MIControlState leftstate,MIControlState rightstate);
		void setScopeBarButtonDividerImage(MGImage *dividerImage,MIControlState leftState,MIControlState rightstate);
		//- (NSDictionary *)scopeBarButtonTitleTextAttributesForState:(MIControlState)state
		//- (void)setScopeBarButtonTitleTextAttributes:(NSDictionary *)attributes forState:(MIControlState)state
		MGImage *searchFieldBackgroundImageForState(MIControlState state);
		void setSearchFieldBackgroundImageForState(MGImage *backgroundImage ,MIControlState state);
		MGPROPERTY(MGOffset,m_searchFieldBackgroundPositionAdjustment,SearchFieldBackgroundPositionAdjustment);
		MGPROPERTY(MGOffset,m_searchTextPositionAdjustment,SearchTextPositionAdjustment);

	};

	class MISearchBarDelegate
	{
		//Editing Text
		virtual void textDidChange(MISearchBar *searchBar ,const string &searchText)=0;
		virtual bool shouldChangeTextInRange(MISearchBar *searchBar,const MGRange& range,const string &text)=0;
		virtual bool shouldBeginEditing(MISearchBar *searchBar)=0;
		virtual void textDidBeginEditing(MISearchBar *searchBar)=0;
		virtual bool shouldEndEditing(MISearchBar *searchBar)=0;
		virtual void textDidEndEditing(MISearchBar *searchBar)=0;

		//Clicking Buttons
		virtual void bookmarkButtonClicked(MISearchBar *searchBar)=0;
		virtual void cancelButtonClicked(MISearchBar *searchBar)=0;
		virtual void searchButtonClicked(MISearchBar *searchBar)=0;
		virtual void barResultsListButtonClicked(MISearchBar *searchBar)=0;

		//Scope Button
		//searchBar:selectedScopeButtonIndexDidChange:
	};
}

#endif //_MISEARCHBAR_H_
