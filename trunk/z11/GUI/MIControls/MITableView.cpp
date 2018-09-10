//
//  MITableView.cpp
//  Gui
//
//  Created by zhuli on 11-6-10.
//  Modified by wangzhipeng on 11-8-17.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#include "MITableView.h"
#include "../../System/Shared/MGLog.h"
#include <iostream>
#include <math.h>
#include <assert.h>
#include <limits>
#include <float.h>


namespace z11
{

	void MITableView::init()
	{
		//MIView
		m_background_color = MGColor::fromColorRef(RGBAColorRef::lightGray());
        _view_flags.sub_scale_enable = 0;

		//MIScrollView
		_scroll_view_flags.horizontal_bounce_enabled = 0;
		_scroll_view_flags.horizontal_indicator_show = 0;
        _scroll_view_flags.zoom_enable = 0;

		//MITable
		m_separator_style=MITableViewCellSeparatorStyleSingleLine;
		m_rowHeight=DEFAULT_TABLECELL_HEIGHT;
		m_separatorColor=RGBAColorRef::darkGray();
		m_backgroundView=NULL;
		m_tableHeaderView=NULL;
		m_tableFooterView=NULL;
		m_sectionHeaderHeight=DEFAULT_TABLE_HEADER_HEIGHT;
		m_sectionFooterHeight=DEFAULT_TABLE_FOOTER_HEIGHT;

		m_allowsSelection=true;
		m_allowsMultipleSelection=false;
		m_allowsSelectionDuringEditing=false;
		m_allowsMultipleSelectionDuringEditing=false;


		m_editing=false;
		//current_row=-1;
        current_index_path.m_row = 0;
        current_index_path.m_section = 0;
		m_style=MITableViewStylePlain;
	}
    MITableView::MITableView()
	{
        class_name = "MITableView";
		current_index_path = MGIndexPath::indexPathForRowInSection(0, 0);
        m_tableview_delegate=NULL;
		m_tableview_datasource=NULL;
		init();
    }
    
    MITableView::~MITableView()
    {
		//if (m_tableview_datasource!=NULL)
		//{
		//	m_tableview_datasource->release();
		//	//if (m_tableview_datasource->isAutoClean())
		//	//{
		//	//	delete_and_set_null(m_tableview_datasource);
		//	//}
		//	
		//}
		//if (m_tableview_delegate!=NULL)
		//{
		//	m_tableview_delegate->release();
		//}
		
    }
    
    void MITableView::drawself()
    {
        MIScrollView::drawself();
    }
    
    void MITableView::initWithFrame(MGRect frame,MITableViewStyle style)
    {
        MIScrollView::initWithFrame(frame);
		m_style=style;
        reloadData();
    }
    
    MIEventHandleState MITableView::touchesBegan(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
		MIEventHandleState ret = MIScrollView::touchesBegan(touches, event);
        return ret;
    }
    MIEventHandleState MITableView::touchesMoved(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        MIEventHandleState ret = MIScrollView::touchesMoved(touches, event);
        return ret;
    }
    
    MIEventHandleState MITableView::touchesEnded(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        MIEventHandleState ret = MIScrollView::touchesEnded(touches, event);
        return ret;
    }
    MIEventHandleState MITableView::touchesCancelled(const std::set<MITouch*>& touches, const MIEvent* const event)
    {
        MIEventHandleState ret =  MIScrollView::touchesCancelled(touches, event);
        return ret;
    }

    MIEventHandleState MITableView::handleEvent(const MIEvent* const event)
	{
        MIEventHandleState ret = MIScrollView::handleEvent(event);
        return ret;
    }
    
    void MITableView::reloadData()
    {
        if (m_tableview_datasource == NULL /*|| m_tableview_delegate == NULL*/) {
            return;
        }
        removeAllSubviews();
		sub_transition=MGRectTransitionMake(0,0,0);
        AUTO_LOCK_THIS;
        int_32 number_of_sections = m_tableview_datasource->numberOfSections(this);
		float top_space = 0.0, left_space = 0.0, right_space = 0.0;
		if (style() == MITableViewStyleGrouped) {
			top_space = DEFAULT_TABLE_TOP_SPACE;
			left_space = DEFAULT_TABLE_LEFT_SPACE;
			right_space = DEFAULT_TABLE_RIGHT_SPACE;
		}
		float table_length = top_space;
        for(int section_idx = 0; section_idx < number_of_sections; ++section_idx)
        {
			//title cell
			MGIndexPath path = MGIndexPath::indexPathForRowInSection(-1, section_idx);
			MITableViewCell* cell = m_tableview_datasource->cellForRowAtIndexPath(this, path);
			float length = 0;
			if (m_tableview_delegate!=NULL) {
				length = m_tableview_delegate->heightForRowAtIndexPath(this, path);
			} else {
				length = m_rowHeight;
			}
			if (cell != NULL) {
				cell->setPositionInSection(MITableViewCellPositionTitle);
				cell->disableClick();
				cell->autoClean();
				cell->setFrame(MGRectMake(left_space, table_length, cell->getFrame().width-left_space-right_space, length));
				current_index_path.m_section = section_idx;
				current_index_path.m_row = -1;
				addSubview(cell);
				table_length += (length);
			} else {
				if (section_idx != 0)
					table_length += DEFAULT_TABLE_SPACE_BETWEEN_SECTIONS;
			}

            int_32 number_of_rows = m_tableview_datasource->numberOfRowsInSection(this, section_idx);
            for (int row_idx = 0; row_idx < number_of_rows; ++row_idx) {
                MGIndexPath path = MGIndexPath::indexPathForRowInSection(row_idx, section_idx);
                MITableViewCell* cell = m_tableview_datasource->cellForRowAtIndexPath(this, path);
				if(cell == NULL)
                {
					continue ;
				}
                if (table_length<m_frame.height)
				{
				}
				else
				{
					cell->hide();
				}
				
				if (number_of_rows == 1) {
					cell->setPositionInSection(MITableViewCellPositionHeadAndTail);
				} else {
					if (row_idx == 0) {
						cell->setPositionInSection(MITableViewCellPositionHead);
					} else if (row_idx == number_of_rows - 1) {
						cell->setPositionInSection(MITableViewCellPositionTail);
					} else {
						cell->setPositionInSection(MITableViewCellPositionMiddle);
					}
				}
                float length = 0;
                if (m_tableview_delegate!=NULL) {
                    length = m_tableview_delegate->heightForRowAtIndexPath(this, path);
				} else {
					length = m_rowHeight;
				}
                cell->autoClean();
                cell->setFrame(MGRectMake(left_space, table_length, cell->getFrame().width-left_space-right_space, length));
                //current_row = i;
                current_index_path.m_section = section_idx;
                current_index_path.m_row = row_idx;
                addSubview(cell);
                if(m_separator_style==MITableViewCellSeparatorStyleSingleLine)
                    table_length += (length + 1);
                else if(m_separator_style==MITableViewCellSeparatorStyleNone)
                    table_length += (length);
                
                if (m_tableview_delegate!=NULL) {
                    float m_gap = m_tableview_delegate->cellGapForRowAtIndexPath(this, path);
                    table_length += m_gap;
                }
                
            }
        }
        adjustIndicatorInVertical();
        adjustIndicatorInHorizontal();
    }
    
    void MITableView::removeAllSubviews()
    {
        MIScrollView::removeAllSubviews();
        subview_index_map.clear();
    }
    
    void MITableView::includeNewSubview(z11::MIView *subview)
    {
        MIScrollView::includeNewSubview(subview);
        subview_index_map[subview] = current_index_path;
    }
    
    void MITableView::excludeOldSubview(z11::MIView *subview)
    {
        MIScrollView::excludeOldSubview(subview);
        SubviewIndexMapIt it = subview_index_map.find(subview);
        if (it != subview_index_map.end()) {
            subview_index_map.erase(it);
        }
    }

	MGIndexPath MITableView::indexPathForCell( MITableViewCell *cell )
	{
		MGIndexPath ret ;
        SubviewIndexMapIt it = subview_index_map.find(cell);
        if (it != subview_index_map.end()) {
            ret = MGIndexPath::indexPathForRowInSection(it->second.m_row, it->second.m_section);
        }
        return ret;
	}


	void MITableView::setTableViewDelegate( MITableViewDelegate* delegate )
	{
		m_tableview_delegate = delegate;
	}

	void MITableView::setTableViewDataSource( MITableViewDataSource* datasource )
	{
		m_tableview_datasource = datasource;
	}

	z11::MITableViewStyle MITableView::style()
	{
		return m_style;
	}

	int MITableView::numberOfRowsInSection( int _section )
	{
		if (m_tableview_datasource!=NULL)
			return m_tableview_datasource->numberOfRowsInSection(this,_section);
		else
			return m_section_number.at(_section).size();
	}

	int MITableView::numberOfSections()
	{
		if (m_tableview_datasource!=NULL)
			return m_tableview_datasource->numberOfSections(this);
		else
			return m_section_number.size();
	}

	MITableViewCell * MITableView::cellForRowAtIndexPath( const MGIndexPath&indexPath )
	{
		MITableViewCell* ret = NULL;
		SubviewIndexMapIt it = subview_index_map.begin();
		for (; it!=subview_index_map.end(); it++) {
			if (it->second.section() == indexPath.section()
				&& it->second.row() == indexPath.row())
			{
				ret = static_cast<MITableViewCell*>(it->first);
			}
		}
		return ret;
	}

	MGIndexPath  MITableView::indexPathForRowAtPoint( const MGPoint &point )
	{
		return MGIndexPath();
	}

	MGIndexPath * MITableView::indexPathForSelectedRow()
	{
		return NULL;
	}

	vector<MGIndexPath *> MITableView::indexPathsForSelectedRows()
	{
		vector<MGIndexPath *> selecteds;
		return selecteds;
	}

	void MITableView::selectRowAtIndexPath( MGIndexPath *indexPath )
	{

	}

	void MITableView::deselectRowAtIndexPath( MGIndexPath *indexPath )
	{

	}

	void MITableView::beginUpdates()
	{

	}

	void MITableView::endUpdates()
	{

	}

	void MITableView::insertRowsAtIndexPaths( const std::vector<MGIndexPath*> &indexPaths )
	{

	}

	void MITableView::deleteRowsAtIndexPaths( const std::vector<MGIndexPath*> &indexPaths )
	{

	}

	void MITableView::moveRowAtIndexPathToIndexPath( const MGIndexPath *indexPath,const MGIndexPath*newIndexPath )
	{

	}

	bool MITableView::editing()
	{
		return m_editing;
	}

	void MITableView::setEditing( bool _edit )
	{
		m_editing=_edit;
	}

	void MITableView::reloadRowsAtIndexPaths( const vector<MGIndexPath *> &indexPaths )
	{

	}

	void MITableView::reloadSectionIndexTitles()
	{

	}

	MITableViewCell * MITableView::dequeueReusableCellWithIdentifier( const string &identifier )
	{
		return dynamic_cast<MITableViewCell *>(m_reuseable_cell_queue.dequeueReusableCellWithIdentifier(identifier));
	}

	void MITableView::scrollViewDraggingWithContentOffset( const MGPoint &targetContentOffset,const MGPoint &velocity)
	{
	//	MGLogD("MITabelView scroll %f,%f",targetContentOffset.x,targetContentOffset.y);
		MGPoint content_offset=MGPointMake(sub_transition.axis_x,sub_transition.axis_y);
		float scroll_offset_up=0;
		float scroll_offset_down=0;
		if (velocity.y>1e-6)
		{
			scroll_offset_up=velocity.y*5;
		}
		else if(velocity.y<-1e-6)
		{
			scroll_offset_down=velocity.y*5;
		}
		else
		{
			content_offset=targetContentOffset;
		}
		
		//LogStart();
		for (size_t i=0;i<m_subviews.size();i++)
		{
			MGRect sub_frame=m_subviews[i]->getFrame();
			MGRectTransition sub_view_trans=m_subviews[i]->getRectTransition();
			if (sub_frame.y+content_offset.y+sub_view_trans.axis_y>-sub_frame.height-scroll_offset_up&&sub_frame.y+sub_frame.height+content_offset.y+sub_view_trans.axis_y<=m_frame.height+sub_frame.height-scroll_offset_down)
			{
				m_subviews[i]->show();
			}
			else
			{
				m_subviews[i]->hide();
			}
			
		}
		//LogEnd("MITableView::scrollViewDraggingWithContentOffset");
		
	}


	int_32 MITableViewDataSource::numberOfRowsInSection( MITableView * table_view,int_32 section )
	{
		return 1;
	}

	int MITableViewDataSource::numberOfSections( MITableView * table_view )
	{
		return 1;
	}

} //namespace z11