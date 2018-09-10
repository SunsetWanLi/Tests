/*
 * =====================================================================================
 *
 *       Filename:  MIActivityIndicator.h
 *
 *    Description:  The UIActivityIndicatorView class creates andmanages an indicator 
 *    showing the indeterminate progress of a task. Visually, this indicator is a “gear” 
 *    that is animated to spin.
 *
 *        Version:  1.0
 *        Created:  02/06/2012 07:33:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once
#include "../Base/MIView.h"
namespace z11
{
	typedef enum {

		    MIActivityIndicatorViewStyleWhiteLarge,
			MIActivityIndicatorViewStyleWhite,
			MIActivityIndicatorViewStyleGray,

	} MIActivityIndicatorViewStyle;
    
	class MIActivityIndicatorView: public MIView
	{
		public :
			/**
			 * @brief initWithActivityIndicatorStyle : Initializes and returns 
			 * an activity-indicator object.
			 *
			 * @param style : A constant that specifies the style of the object to be created
			 *
			 * @return An initialized UIActivityIndicatorView object 
			 * or nil if the object couldn’t be created.
			 */
			static MIActivityIndicatorView *initWithActivityIndicatorStyle(
					MIActivityIndicatorViewStyle style);

			/**
			 * @brief startAnimating : Starts the animation of the progress indicator.
			 */
			void startAnimating();

			/**
			 * @brief stopAnimating : Stops the animation of the progress indicator.
			 */
			void stopAnimating();

			/**
			 * @brief isAnimating : Returns whether the receiver is animating.
			 *
			 * @return YES if the receiver is animating, otherwise NO.
			 */
			bool isAnimating();

			/**
			 * @brief A Boolean value that controls whether the receiver is hidden 
			 * when the animation is stopped.
			 */
			MGPROPERTY(bool,hidesWhenStopped,HidesWhenStopped);

			/**
			 * @brief The style of the indeterminate progress indicator.
			 */
			MGPROPERTY(MIActivityIndicatorViewStyle,activityIndicatorViewStyle,ActivityIndicatorViewStyle);	
			
        virtual void drawself();

    private:
        MIActivityIndicatorView();
        bool m_is_animating;
        int m_ticks;
	};
}
