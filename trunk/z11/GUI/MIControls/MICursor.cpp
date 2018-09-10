#include "MICursor.h"
#include <stdio.h>
namespace z11
{
    MICursor::MICursor()
    {
        class_name = "MICursor";
        m_location = 0;
        // important !!!
        _view_flags.user_interaction_enabled = 0;
    }
    
    MICursor::~MICursor()
    {
        
    }
    
    void MICursor::moveToPos(const MGPoint &point)
    {
        
    }
    
    void MICursor::moveToFirst()
    {
        
    }
    
    void MICursor::moveToUp()
    {
        
    }
    
    void MICursor::moveToDown()
    {
        
    }
    
    void MICursor::moveToLeft()
    {
        
    }
    
    void MICursor::moveToRight()
    {
        
    }
    
    void MICursor::animate(bool animated)
    {
        MICursor *m_cursor=this;
        char animation_name[256];
        sprintf(animation_name, "Input View Cursor. %p", this);
        if (animated)
        {
            m_cursor->show();
            m_cursor->setAlpha(0.0f);
            MIView::beginAnimation(animation_name);
            MIView::setAnimationRepeatCount(-1);
            MIView::setAnimationDuration(1.2f);
            MIView::setAnimationCurve(MIViewAnimationOptionCurveCursor);
            m_cursor->setAlpha(1.0f);
            MIView::commitAnimation();
        }
        else
        {
            MIView::stopAnimation(animation_name);
            m_cursor->setAlpha(1.0f);
            m_cursor->hide();
        }
    }
}