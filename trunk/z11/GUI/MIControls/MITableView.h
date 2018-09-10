//
//  MITableView.h
//  Gui
//
//  Created by zhuli on 11-6-10.
//  Modified by wangzhipeng on 11-8-16.
//  Copyright 2011å¹?Zhejiang University. All rights reserved.
//
#pragma once

#include "MIScrollView.h"
#include "MITableViewCell.h"
#include <vector>
#include <map>
#include "../Base/MIMultiReusableQueue.h"
using namespace std;

const float DEFAULT_TABLECELL_HEIGHT	= 44.0f;
const float DEFAULT_TABLE_HEADER_HEIGHT = 44.0f;
const float DEFAULT_TABLE_FOOTER_HEIGHT = 44.0f;
const float DEFAULT_TABLE_SPACE_BETWEEN_SECTIONS = 25.0f;
const float DEFAULT_TABLE_TOP_SPACE = 10.0f;
const float DEFAULT_TABLE_LEFT_SPACE = 10.0f;
const float DEFAULT_TABLE_RIGHT_SPACE = 10.0f;

namespace z11 
{
	class MGIndexPath
	{
	public :
		static MGIndexPath indexPathForRowInSection(int_32 _row,int_32 _section){ return  MGIndexPath(_row,_section); }
		int_32 row() const{ return m_row;}
		int_32 section() const{ return m_section;}
        MGIndexPath(){m_row = 0; m_section = 0;}
        int_32 m_row;
		int_32 m_section;
	private:
		MGIndexPath (int_32 _row,int_32 _section):m_row(_row),m_section(_section){}
        friend class MITableView;

	};
    
	class MITableViewDelegate;
    class MITableViewDataSource;

	typedef enum {
		MITableViewStylePlain,
		MITableViewStyleGrouped
	} MITableViewStyle;

    class MITableView : public MIScrollView
    {

	//Initializing a UITableView Object
    public:
        MITableView();
        virtual ~MITableView();
		virtual void initWithFrame(z11::MGRect frame ,MITableViewStyle style = MITableViewStylePlain);
	private:
		void init();

	//Configuring a Table View
	public:  
		//usually identifier == CellClassName
		MITableViewCell *dequeueReusableCellWithIdentifier(const string &identifier);
		MITableViewStyle style();
		int numberOfRowsInSection(int _section);
		int numberOfSections();
		float m_rowHeight;
		MITableViewCellSeparatorStyle m_separator_style;
		RGBAColorRef m_separatorColor ;						//Not implement
		MIView *m_backgroundView;
		MIView *m_tableHeaderView;
		MIView *m_tableFooterView; 
		float m_sectionHeaderHeight;
		float m_sectionFooterHeight;
		//int sectionIndexMinimumDisplayRowCount;

	//Accessing Cells and Sections
	public:
		MITableViewCell *cellForRowAtIndexPath(const MGIndexPath&indexPath);
		MGIndexPath indexPathForCell(MITableViewCell *cell);
		MGIndexPath indexPathForRowAtPoint(const MGPoint &point);
		//â€?indexPathsForRowsInRect:
		//â€?visibleCells
		//â€?indexPathsForVisibleRows


	//Scrolling the Table View
	public:
		//â€?scrollToRowAtIndexPath:atScrollPosition:animated:
		//â€?scrollToNearestSelectedRowAtScrollPosition:animated:

	//Managing Selections
	public:
		MGIndexPath* indexPathForSelectedRow();
		vector<MGIndexPath*> indexPathsForSelectedRows();
		void selectRowAtIndexPath(MGIndexPath *indexPath);
		void deselectRowAtIndexPath(MGIndexPath *indexPath);
		bool m_allowsSelection;
		bool m_allowsMultipleSelection;
		bool m_allowsSelectionDuringEditing;
		bool m_allowsMultipleSelectionDuringEditing;


	//Inserting, Deleting, and Moving Rows and Sections
	public:
		void beginUpdates();
		void endUpdates();
		void insertRowsAtIndexPaths(const std::vector<MGIndexPath*> &indexPaths );
		void deleteRowsAtIndexPaths(const std::vector<MGIndexPath*> &indexPaths );
		void moveRowAtIndexPathToIndexPath(const MGIndexPath *indexPath,const MGIndexPath*newIndexPath);
		//â€?insertSections:withRowAnimation:
		//â€?deleteSections:withRowAnimation:
		//â€?moveSection:toSection:

	//Managing the Editing of Table Cells
	public:
		bool editing();
		void setEditing(bool _edit);

	//Reloading the Table View
	public:    
		void reloadData();
		void reloadRowsAtIndexPaths(const vector<MGIndexPath *> &indexPaths);
		//void reloadSections()
		void reloadSectionIndexTitles();

	//Managing the Delegate and the Data Source
    public:
        void setTableViewDelegate(MITableViewDelegate* delegate);
        void setTableViewDataSource(MITableViewDataSource* datasource);
    protected:
        MITableViewDelegate * m_tableview_delegate;
        MITableViewDataSource * m_tableview_datasource;

    public:
        //int_32 indexForCell(MITableViewCell *cell);
        virtual void removeAllSubviews();
        virtual void includeNewSubview(MIView* subview);
        virtual void excludeOldSubview(MIView* subview);

	private:
		MITableViewStyle m_style;
		std::vector< std::vector<int> > m_section_number;
		bool m_editing;
		map<MIView*, MGIndexPath> subview_index_map;
		//int_32 current_row;
        MGIndexPath current_index_path;
		typedef map<MIView*, MGIndexPath>::iterator SubviewIndexMapIt;

	protected:        
		virtual void drawself();
		MIMultiReusableQueueBasedAccessTime m_reuseable_cell_queue;
		virtual void scrollViewDraggingWithContentOffset(const MGPoint &targetContentOffset,const MGPoint &velocity);
		//MGRange m_current_show_range;//only available in same cell
	public:
		MIRESPONDER_TOUCH_DECLARE
		friend class MITableViewCell;
    };
        
	class MITableViewDelegate
	{
	public:
		MITableViewDelegate(){}//fix me? maybe should't retain ;
        virtual MGIndexPath willSelectRowAtIndexPath(MITableView * table_view,const MGIndexPath &indexPath){return MGIndexPath();}
        virtual void didSelectRowAtIndexPath(MITableView * table_view, const MGIndexPath &indexPath) = 0;
		virtual float heightForRowAtIndexPath(MITableView * table_view,const MGIndexPath &indexPath){return DEFAULT_TABLECELL_HEIGHT;}
        virtual float cellGapForRowAtIndexPath(MITableView * table_view,const MGIndexPath &indexPath){return 0;}
    };
#define MITABLEVIEW_DELEGAGE_DECLARE \
        virtual MGIndexPath willSelectRowAtIndexPath(MITableView * table_view,const MGIndexPath &indexPath); \
        virtual void didSelectRowAtIndexPath(MITableView * table_view, const MGIndexPath &indexPath); \
        virtual float heightForRowAtIndexPath(MITableView * table_view,const MGIndexPath &indexPath);
    
	class MITableViewDataSource
    {
    public:
		MITableViewDataSource(){}//fix me? maybe should't retain ;
        //virtual int_32 numberOfRows(MITableView * table_view) = 0;
		virtual int_32 numberOfRowsInSection(MITableView * table_view,int_32 section) = 0;
		virtual int numberOfSections(MITableView * table_view) = 0;
        virtual MITableViewCell* cellForRowAtIndexPath(MITableView * table_view, const MGIndexPath& path) = 0;
    public:
        virtual bool canEditRowAtIndexPath(MITableView * table_view,const MGIndexPath& path) {return true;}

    }; 
#define MITABLEVIEW_DATASOURCE_DECLARE \
        virtual int_32 numberOfRowsInSection(MITableView * table_view, int_32 section); \
        virtual int numberOfSections(MITableView* table_view); \
        virtual MITableViewCell* cellForRowAtIndexPath(MITableView * table_view, const MGIndexPath& path); \
		virtual bool canEditRowAtIndexPath(MITableView * table_view, const MGIndexPath& path); 
}//namespace z11
