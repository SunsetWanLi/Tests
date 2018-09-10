#include "../../CommonBase/MGStructure.h"
#include "../../System/Shared/MGLog.h"
#include "MIGridLayout.h"
#include "../Base/MIView.h"

#include <algorithm>
#include <utility>
#include "LayoutConstants.h"

namespace z11
{

	// LayoutElement ------------------------------------------------------

	// A LayoutElement has a size and location along one axis. It contains
	// methods that are used along both axis.
	class LayoutElement
	{
	public:
		// Invokes ResetSize on all the layout elements.
		template<class T>
		static void ResetSizes(std::vector<T*>* elements)
		{
			// Reset the layout width of each column.
			for(typename std::vector<T*>::iterator i=elements->begin();
				i!=elements->end(); ++i)
			{
				(*i)->ResetSize();
			}
		}

		// Sets the location of each element to be the sum of the sizes of the
		// preceding elements.
		template<class T>
		static void CalculateLocationsFromSize(std::vector<T*>* elements)
		{
			// Reset the layout width of each column.
			int location = 0;
			for(typename std::vector<T*>::iterator i=elements->begin();
				i!=elements->end(); ++i)
			{
				(*i)->SetLocation(location);
				location += (*i)->Size();
			}
		}

		// Distributes delta among the resizable elements.
		// Each resizable element is given ResizePercent / total_percent * delta
		// pixels extra of space.
		template<class T>
		static void DistributeDelta(int delta, std::vector<T*>* elements)
		{
			if(delta == 0)
			{
				return;
			}

			float total_percent = 0;
			int resize_count = 0;
			for(typename std::vector<T*>::iterator i=elements->begin();
				i!=elements->end(); ++i)
			{
				total_percent += (*i)->ResizePercent();
				resize_count++;
			}
			if(total_percent == 0)
			{
				// None of the elements are resizable, return.
				return;
			}
			int remaining = delta;
			int resized = resize_count;
			for(typename std::vector<T*>::iterator i=elements->begin();
				i!=elements->end(); ++i)
			{
				T* element = *i;
				if(element->ResizePercent() > 0)
				{
					int to_give;
					if(--resized == 0)
					{
						to_give = remaining;
					}
					else
					{
						to_give = static_cast<int>(delta *
							(element->resize_percent_ / total_percent));
						remaining -= to_give;
					}
					element->SetSize(element->Size() + to_give);
				}
			}
		}

		// Returns the sum of the size of the elements from start to start + length.
		template<class T>
		static int TotalSize(int start, int length, std::vector<T*>* elements)
		{
			DCHECK(start>=0 && length>0 &&
				start+length<=static_cast<int>(elements->size()));
			int size = 0;
			for(int i=start,max=start+length; i<max; ++i)
			{
				size += (*elements)[i]->Size();
			}
			return size;
		}

		explicit LayoutElement(float resize_percent)
			: resize_percent_(resize_percent)
		{
			DCHECK(resize_percent >= 0);
		}

		virtual ~LayoutElement() {}

		void SetLocation(int location)
		{
			location_ = location;
		}

		int Location()
		{
			return location_;
		}

		// Adjusts the size of this LayoutElement to be the max of the current size
		// and the specified size.
		virtual void AdjustSize(int size)
		{
			size_ = std::max(size_, size);
		}

		// Resets the size to the initial size. This sets the size to 0, but
		// subclasses that have a different initial size should override.
		virtual void ResetSize()
		{
			SetSize(0);
		}

		void SetSize(int size)
		{
			size_ = size;
		}

		int Size()
		{
			return size_;
		}

		void SetResizePercent(float percent)
		{
			resize_percent_ = percent;
		}

		float ResizePercent()
		{
			return resize_percent_;
		}

		bool IsResizable()
		{
			return resize_percent_ > 0;
		}

	private:
		float resize_percent_;
		int location_;
		int size_;

		DISALLOW_COPY_AND_ASSIGN(LayoutElement);
	};

	// Column -------------------------------------------------------------

	// As the name implies, this represents a Column. Column contains default
	// values for views originating in this column.
	class Column : public LayoutElement
	{
	public:
		Column(MIGridLayout::Alignment h_align,
			MIGridLayout::Alignment v_align,
			float resize_percent,
			MIGridLayout::SizeType size_type,
			int fixed_width,
			int min_width,
			size_t index,
			bool is_padding)
			: LayoutElement(resize_percent),
			h_align_(h_align),
			v_align_(v_align),
			size_type_(size_type),
			same_size_column_(-1),
			fixed_width_(fixed_width),
			min_width_(min_width),
			index_(index),
			is_padding_(is_padding),
			master_column_(NULL) {}

		virtual ~Column() {}

		MIGridLayout::Alignment h_align() { return h_align_; }
		MIGridLayout::Alignment v_align() { return v_align_; }

		virtual void ResetSize();

	private:
		friend class MIColumnSet;
		friend class MIGridLayout;

		Column* GetLastMasterColumn();

		// Determines the max size of all linked columns, and sets each column
		// to that size. This should only be used for the master column.
		void UnifySameSizedColumnSizes();

		virtual void AdjustSize(int size);

		const MIGridLayout::Alignment h_align_;
		const MIGridLayout::Alignment v_align_;
		const MIGridLayout::SizeType size_type_;
		int same_size_column_;
		const int fixed_width_;
		const int min_width_;

		// Index of this column in the ColumnSet.
		const size_t index_;

		const bool is_padding_;

		// If multiple columns have their sizes linked, one is the
		// master column. The master column is identified by the
		// master_column field being equal to itself. The master columns
		// same_size_columns field contains the set of Columns with the
		// the same size. Columns who are linked to other columns, but
		// are not the master column have their master_column pointing to
		// one of the other linked columns. Use the method GetLastMasterColumn
		// to resolve the true master column.
		std::vector<Column*> same_size_columns_;
		Column* master_column_;

		DISALLOW_COPY_AND_ASSIGN(Column);
	};

	void Column::ResetSize()
	{
		if(size_type_ == MIGridLayout::FIXED)
		{
			SetSize(fixed_width_);
		}
		else
		{
			SetSize(min_width_);
		}
	}

	Column* Column::GetLastMasterColumn()
	{
		if(master_column_ == NULL)
		{
			return NULL;
		}
		if(master_column_ == this)
		{
			return this;
		}
		return master_column_->GetLastMasterColumn();
	}

	void Column::UnifySameSizedColumnSizes()
	{
		DCHECK(master_column_ == this);

		// Accumulate the size first.
		int size = 0;
		for(std::vector<Column*>::iterator i=same_size_columns_.begin();
			i!=same_size_columns_.end(); ++i)
		{
			size = std::max(size, (*i)->Size());
		}

		// Then apply it.
		for(std::vector<Column*>::iterator i=same_size_columns_.begin();
			i!=same_size_columns_.end(); ++i)
		{
			(*i)->SetSize(size);
		}
	}

	void Column::AdjustSize(int size)
	{
		if(size_type_ == MIGridLayout::USE_PREF)
		{
			LayoutElement::AdjustSize(size);
		}
	}

	// Row -------------------------------------------------------------

	class Row : public LayoutElement
	{
	public:
		Row(bool fixed_height, int height, float resize_percent,
			MIColumnSet* column_set)
			: LayoutElement(resize_percent),
			fixed_height_(fixed_height),
			height_(height),
			column_set_(column_set),
			max_ascent_(0),
			max_descent_(0) {}

		virtual ~Row() {}

		virtual void ResetSize()
		{
			max_ascent_ = max_descent_ = 0;
			SetSize(height_);
		}

		MIColumnSet* column_set()
		{
			return column_set_;
		}

		// Adjusts the size to accomodate the specified ascent/descent.
		void AdjustSizeForBaseline(int ascent, int descent)
		{
			max_ascent_ = std::max(ascent, max_ascent_);
			max_descent_ = std::max(descent, max_descent_);
			AdjustSize(max_ascent_ + max_descent_);
		}

		int max_ascent() const
		{
			return max_ascent_;
		}

		int max_descent() const
		{
			return max_descent_;
		}

	private:
		const bool fixed_height_;
		const int height_;
		// The column set used for this row; null for padding rows.
		MIColumnSet* column_set_;

		int max_ascent_;
		int max_descent_;

		DISALLOW_COPY_AND_ASSIGN(Row);
	};

	// ViewState -------------------------------------------------------------

	// Identifies the location in the grid of a particular view, along with
	// placement information and size information.
	struct ViewState
	{
		ViewState(MIColumnSet* column_set, MIView* view, int start_col, int start_row,
			int col_span, int row_span, MIGridLayout::Alignment h_align,
			MIGridLayout::Alignment v_align, int pref_width, int pref_height)
			: column_set(column_set),
			view(view),
			start_col(start_col),
			start_row(start_row),
			col_span(col_span),
			row_span(row_span),
			h_align(h_align),
			v_align(v_align),
			pref_width_fixed(pref_width > 0),
			pref_height_fixed(pref_height > 0),
			pref_width(pref_width),
			pref_height(pref_height),
			remaining_width(0),
			remaining_height(0),
			baseline(-1)
		{
			DCHECK(view && start_col>=0 && start_row>=0 && col_span>0 &&
				row_span>0 && start_col<column_set->num_columns() &&
				(start_col+col_span)<=column_set->num_columns());
		}

		MIColumnSet* const column_set;
		MIView* const view;
		const int start_col;
		const int start_row;
		const int col_span;
		const int row_span;
		const MIGridLayout::Alignment h_align;
		const MIGridLayout::Alignment v_align;

		// If true, the pref_width/pref_height were explicitly set and the view's
		// preferred size is ignored.
		const bool pref_width_fixed;
		const bool pref_height_fixed;

		// The preferred width/height. These are reset during the layout process.
		int pref_width;
		int pref_height;

		// Used during layout. Gives how much width/height has not yet been
		// distributed to the columns/rows the view is in.
		int remaining_width;
		int remaining_height;

		// The baseline. Only used if the view is vertically aligned along the
		// baseline.
		int baseline;
	};

	static bool CompareByColumnSpan(const ViewState* v1, const ViewState* v2)
	{
		return v1->col_span < v2->col_span;
	}

	static bool CompareByRowSpan(const ViewState* v1, const ViewState* v2)
	{
		return v1->row_span < v2->row_span;
	}

	// ColumnSet -------------------------------------------------------------

	MIColumnSet::MIColumnSet(int id) : id_(id) {}

	MIColumnSet::~MIColumnSet()
	{
		STLDeleteElements(&columns_);
	}

	void MIColumnSet::AddPaddingColumn(float resize_percent, int width)
	{
		AddColumn(MIGridLayout::FILL, MIGridLayout::FILL, resize_percent,
			MIGridLayout::FIXED, width, width, true);
	}

	void MIColumnSet::AddColumn(MIGridLayout::Alignment h_align,
		MIGridLayout::Alignment v_align,
		float resize_percent,
		MIGridLayout::SizeType size_type,
		int fixed_width,
		int min_width)
	{
		AddColumn(h_align, v_align, resize_percent, size_type, fixed_width,
			min_width, false);
	}


	void MIColumnSet::LinkColumnSizes(int first, ...)
	{
		va_list marker;
		va_start(marker, first);
		DCHECK(first>=0 && first<num_columns());
		for(int last=first,next=va_arg(marker, int); next!=-1;
			next=va_arg(marker, int))
		{
			DCHECK(next>=0 && next<num_columns());
			columns_[last]->same_size_column_ = next;
			last = next;
		}
		va_end(marker);
	}

	void MIColumnSet::AddColumn(MIGridLayout::Alignment h_align,
		MIGridLayout::Alignment v_align,
		float resize_percent,
		MIGridLayout::SizeType size_type,
		int fixed_width,
		int min_width,
		bool is_padding)
	{
		Column* column = new Column(h_align, v_align, resize_percent, size_type,
			fixed_width, min_width, columns_.size(), is_padding);
		columns_.push_back(column);
	}

	void MIColumnSet::AddViewState(ViewState* view_state)
	{
		// view_states are ordered by column_span (in ascending order).
		std::vector<ViewState*>::iterator i = lower_bound(view_states_.begin(),
			view_states_.end(),
			view_state,
			CompareByColumnSpan);
		view_states_.insert(i, view_state);
	}

	void MIColumnSet::CalculateMasterColumns()
	{
		for(std::vector<Column*>::iterator i=columns_.begin();
			i!=columns_.end(); ++i)
		{
			Column* column = *i;
			int same_size_column_index = column->same_size_column_;
			if(same_size_column_index != -1)
			{
				DCHECK(same_size_column_index>=0 &&
					same_size_column_index<static_cast<int>(columns_.size()));
				Column* master_column = column->master_column_;
				Column* same_size_column = columns_[same_size_column_index];
				Column* same_size_column_master = same_size_column->master_column_;
				if(master_column == NULL)
				{
					// Current column is not linked to any other column.
					if(same_size_column_master == NULL)
					{
						// Both columns are not linked.
						column->master_column_ = column;
						same_size_column->master_column_ = column;
						column->same_size_columns_.push_back(same_size_column);
						column->same_size_columns_.push_back(column);
					}
					else
					{
						// Column to link to is linked with other columns.
						// Add current column to list of linked columns in other columns
						// master column.
						same_size_column->GetLastMasterColumn()->
							same_size_columns_.push_back(column);
						// And update the master column for the current column to that
						// of the same sized column.
						column->master_column_ = same_size_column;
					}
				}
				else
				{
					// Current column is already linked with another column.
					if(same_size_column_master == NULL)
					{
						// Column to link with is not linked to any other columns.
						// Update it's master_column.
						same_size_column->master_column_ = column;
						// Add linked column to list of linked column.
						column->GetLastMasterColumn()->same_size_columns_.
							push_back(same_size_column);
					}
					else if(column->GetLastMasterColumn() !=
						same_size_column->GetLastMasterColumn())
					{
						// The two columns are already linked with other columns.
						std::vector<Column*>* same_size_columns =
							&(column->GetLastMasterColumn()->same_size_columns_);
						std::vector<Column*>* other_same_size_columns =
							&(same_size_column->GetLastMasterColumn()->same_size_columns_);
						// Add all the columns from the others master to current columns
						// master.
						same_size_columns->insert(same_size_columns->end(),
							other_same_size_columns->begin(),
							other_same_size_columns->end());
						// The other master is no longer a master, clear its vector of
						// linked columns, and reset its master_column.
						other_same_size_columns->clear();
						same_size_column->GetLastMasterColumn()->master_column_ = column;
					}
				}
			}
		}
		AccumulateMasterColumns();
	}

	void MIColumnSet::AccumulateMasterColumns()
	{
		DCHECK(master_columns_.empty());
		for(std::vector<Column*>::iterator i=columns_.begin();
			i!=columns_.end(); ++i)
		{
			Column* column = *i;
			Column* master_column = column->GetLastMasterColumn();
			if(master_column && find(master_columns_.begin(),
				master_columns_.end(), master_column)==master_columns_.end())
			{
				master_columns_.push_back(master_column);
			}
			// At this point, GetLastMasterColumn may not == master_column
			// (may have to go through a few Columns)_. Reset master_column to
			// avoid hops.
			column->master_column_ = master_column;
		}
	}

	void MIColumnSet::UnifySameSizedColumnSizes()
	{
		for(std::vector<Column*>::iterator i=master_columns_.begin();
			i!=master_columns_.end(); ++i)
		{
			(*i)->UnifySameSizedColumnSizes();
		}
	}

	void MIColumnSet::UpdateRemainingWidth(ViewState* view_state)
	{
		for(int i=view_state->start_col; i<view_state->col_span; ++i)
		{
			view_state->remaining_width -= columns_[i]->Size();
		}
	}

	void MIColumnSet::DistributeRemainingWidth(ViewState* view_state)
	{
		// This is nearly the same as that for rows, but differs in so far as how
		// Rows and Columns are treated. Rows have two states, resizable or not.
		// Columns have three, resizable, USE_PREF or not resizable. This results
		// in slightly different handling for distributing unaccounted size.
		int width = view_state->remaining_width;
		if(width <= 0)
		{
			// The columns this view is in are big enough to accommodate it.
			return;
		}

		// Determine which columns are resizable, and which have a size type
		// of USE_PREF.
		int resizable_columns = 0;
		int pref_size_columns = 0;
		int start_col = view_state->start_col;
		int max_col = view_state->start_col + view_state->col_span;
		float total_resize = 0;
		for(int i=start_col; i<max_col; ++i)
		{
			if(columns_[i]->IsResizable())
			{
				total_resize += columns_[i]->ResizePercent();
				resizable_columns++;
			}
			else if(columns_[i]->size_type_ == MIGridLayout::USE_PREF)
			{
				pref_size_columns++;
			}
		}

		if(resizable_columns > 0)
		{
			// There are resizable columns, give them the remaining width. The extra
			// width is distributed using the resize values of each column.
			int remaining_width = width;
			for(int i=start_col,resize_i=0; i<max_col; ++i)
			{
				if(columns_[i]->IsResizable())
				{
					resize_i++;
					int delta = (resize_i==resizable_columns) ? remaining_width :
						static_cast<int>(width*columns_[i]->ResizePercent()/total_resize);
					remaining_width -= delta;
					columns_[i]->SetSize(columns_[i]->Size() + delta);
				}
			}
		}
		else if(pref_size_columns > 0)
		{
			// None of the columns are resizable, distribute the width among those
			// that use the preferred size.
			int to_distribute = width / pref_size_columns;
			for(int i=start_col; i<max_col; ++i)
			{
				if(columns_[i]->size_type_ == MIGridLayout::USE_PREF)
				{
					width -= to_distribute;
					if(width < to_distribute)
					{
						to_distribute += width;
					}
					columns_[i]->SetSize(columns_[i]->Size() + to_distribute);
				}
			}
		}
	}

	int MIColumnSet::LayoutWidth()
	{
		int width = 0;
		for(std::vector<Column*>::iterator i=columns_.begin();
			i!=columns_.end(); ++i)
		{
			width += (*i)->Size();
		}
		return width;
	}

	int MIColumnSet::GetColumnWidth(int start_col, int col_span)
	{
		return LayoutElement::TotalSize(start_col, col_span, &columns_);
	}

	void MIColumnSet::ResetColumnXCoordinates()
	{
		LayoutElement::CalculateLocationsFromSize(&columns_);
	}

	void MIColumnSet::CalculateSize()
	{
		z11::MGSize pref;
		// Reset the preferred and remaining sizes.
		for(std::vector<ViewState*>::iterator i = view_states_.begin();
			i!=view_states_.end(); ++i)
		{
			ViewState* view_state = *i;
			if(!view_state->pref_width_fixed || !view_state->pref_height_fixed)
			{
				pref = view_state->view->GetPreferredSize();
				if(!view_state->pref_width_fixed)
				{
					view_state->pref_width = pref.width;
				}
				if(!view_state->pref_height_fixed)
				{
					view_state->pref_height = pref.height;
				}
			}
			view_state->remaining_width = pref.width;
			view_state->remaining_height = pref.height;
		}

		// Let layout element reset the sizes for us.
		LayoutElement::ResetSizes(&columns_);

		// Distribute the size of each view with a col span == 1.
		std::vector<ViewState*>::iterator view_state_iterator =
			view_states_.begin();
		for(; view_state_iterator!=view_states_.end()&&
			(*view_state_iterator)->col_span==1; ++view_state_iterator)
		{
			ViewState* view_state = *view_state_iterator;
			Column* column = columns_[view_state->start_col];
			column->AdjustSize(view_state->pref_width);
			view_state->remaining_width -= column->Size();
		}

		// Make sure all linked columns have the same size.
		UnifySameSizedColumnSizes();

		// Distribute the size of each view with a column span > 1.
		for(; view_state_iterator!=view_states_.end(); ++view_state_iterator)
		{
			ViewState* view_state = *view_state_iterator;

			// Update the remaining_width from columns this view_state touches.
			UpdateRemainingWidth(view_state);

			// Distribute the remaining width.
			DistributeRemainingWidth(view_state);

			// Update the size of linked columns.
			// This may need to be combined with previous step.
			UnifySameSizedColumnSizes();
		}
	}

	void MIColumnSet::Resize(int delta)
	{
		LayoutElement::DistributeDelta(delta, &columns_);
	}

	// GridLayout -------------------------------------------------------------

	MIGridLayout::MIGridLayout(MIView* host)
		: host_(host),
		calculated_master_columns_(false),
		remaining_row_span_(0),
		current_row_(-1),
		next_column_(0),
		current_row_col_set_(NULL),
		top_inset_(0),
		bottom_inset_(0),
		left_inset_(0),
		right_inset_(0),
		adding_view_(false)
	{
		DCHECK(host);
	}

	MIGridLayout::~MIGridLayout()
	{
		STLDeleteElements(&column_sets_);
		STLDeleteElements(&view_states_);
		STLDeleteElements(&rows_);
	}

	// static
	MIGridLayout* MIGridLayout::CreatePanel(MIView* host)
	{
		MIGridLayout* layout = new MIGridLayout(host);
		layout->SetInsets(kPanelVertMargin, kPanelHorizMargin,
			kPanelVertMargin, kPanelHorizMargin);
		return layout;
	}

	void MIGridLayout::SetInsets(int top, int left, int bottom, int right)
	{
		top_inset_ = top;
		bottom_inset_ = bottom;
		left_inset_ = left;
		right_inset_ = right;
	}

	void MIGridLayout::SetInsets(const gfx::Insets& insets)
	{
		SetInsets(insets.top(), insets.left(), insets.bottom(), insets.right());
	}

	MIColumnSet* MIGridLayout::AddColumnSet(int id)
	{
		DCHECK(GetColumnSet(id) == NULL);
		MIColumnSet* column_set = new MIColumnSet(id);
		column_sets_.push_back(column_set);
		return column_set;
	}

	void MIGridLayout::StartRowWithPadding(float vertical_resize, int column_set_id,
		float padding_resize, int padding)
	{
		AddPaddingRow(padding_resize, padding);
		StartRow(vertical_resize, column_set_id);
	}

	void MIGridLayout::StartRow(float vertical_resize, int column_set_id)
	{
		MIColumnSet* column_set = GetColumnSet(column_set_id);
		DCHECK(column_set);
		AddRow(new Row(false, 0, vertical_resize, column_set));
	}

	void MIGridLayout::AddPaddingRow(float vertical_resize, int pixel_count)
	{
		AddRow(new Row(true, pixel_count, vertical_resize, NULL));
	}

	void MIGridLayout::SkipColumns(int col_count)
	{
		DCHECK(col_count > 0);
		next_column_ += col_count;
		DCHECK(current_row_col_set_ &&
			next_column_<=current_row_col_set_->num_columns());
		SkipPaddingColumns();
	}

	void MIGridLayout::AddView(MIView* view)
	{
		AddView(view, 1, 1);
	}

	void MIGridLayout::AddView(MIView* view, int col_span, int row_span)
	{
		DCHECK(current_row_col_set_ &&
			next_column_<current_row_col_set_->num_columns());
		Column* column = current_row_col_set_->columns_[next_column_];
		AddView(view, col_span, row_span, column->h_align(), column->v_align());
	}

	void MIGridLayout::AddView(MIView* view, int col_span, int row_span,
		Alignment h_align, Alignment v_align)
	{
		AddView(view, col_span, row_span, h_align, v_align, 0, 0);
	}

	void MIGridLayout::AddView(MIView* view, int col_span, int row_span,
		Alignment h_align, Alignment v_align,
		int pref_width, int pref_height)
	{
		DCHECK(current_row_col_set_ && col_span>0 && row_span>0 &&
			(next_column_+col_span)<=current_row_col_set_->num_columns());
		// We don't support baseline alignment of views spanning rows. Please add if
		// you need it.
		DCHECK(v_align!=BASELINE || row_span==1);
		ViewState* state = new ViewState(current_row_col_set_, view, next_column_,
			current_row_, col_span, row_span, h_align, v_align, pref_width, pref_height);
		AddViewState(state);
	}

	static void CalculateSize(int pref_size, MIGridLayout::Alignment alignment,
		int* location, int* size)
	{
		if(alignment != MIGridLayout::FILL)
		{
			int available_size = *size;
			*size = std::min(*size, pref_size);
			switch(alignment)
			{
			case MIGridLayout::LEADING:
				// Nothing to do, location already points to start.
				break;
			case MIGridLayout::BASELINE: // If we were asked to align on baseline, but
				// the view doesn't have a baseline, fall back
				// to center.
			case MIGridLayout::CENTER:
				*location += (available_size - *size) / 2;
				break;
			case MIGridLayout::TRAILING:
				*location = *location + available_size - *size;
				break;
			default:
				NOTREACHED();
			}
		}
	}

	void MIGridLayout::Installed(MIView* host)
	{
		DCHECK(host_ == host);
	}

	void MIGridLayout::Uninstalled(MIView* host)
	{
		DCHECK(host_ == host);
	}

	void MIGridLayout::ViewAdded(MIView* host, MIView* view)
	{
		DCHECK(host_==host && adding_view_);
	}

	void MIGridLayout::ViewRemoved(MIView* host, MIView* view)
	{
		DCHECK(host_ == host);
	}

	void MIGridLayout::Layout(MIView* host)
	{
		DCHECK(host_ == host);
		// SizeRowsAndColumns sets the size and location of each row/column, but
		// not of the views.
		z11::MGSize pref;
		SizeRowsAndColumns(true, host_->width, host_->height, &pref);

		// Size each view.
		for(std::vector<ViewState*>::iterator i=view_states_.begin();
			i!=view_states_.end(); ++i)
		{
			ViewState* view_state = *i;
			MIColumnSet* column_set = view_state->column_set;
			MIView* view = (*i)->view;
			DCHECK(view);
			int x = column_set->columns_[view_state->start_col]->Location() +
				left_inset_;
			int width = column_set->GetColumnWidth(view_state->start_col,
				view_state->col_span);
			CalculateSize(view_state->pref_width, view_state->h_align,
				&x, &width);
			int y = rows_[view_state->start_row]->Location() + top_inset_;
			int height = LayoutElement::TotalSize(view_state->start_row,
				view_state->row_span, &rows_);
			if(view_state->v_align==BASELINE && view_state->baseline!=-1)
			{
				y += rows_[view_state->start_row]->max_ascent() - view_state->baseline;
				height = view_state->pref_height;
			}
			else
			{
				CalculateSize(view_state->pref_height, view_state->v_align, &y, &height);
			}
			view->SetBounds(x, y, width, height);
		}
	}

	z11::MGSize MIGridLayout::GetPreferredSize(MIView* host)
	{
		DCHECK(host_ == host);
		z11::MGSize out;
		SizeRowsAndColumns(false, 0, 0, &out);
		return out;
	}

	int MIGridLayout::GetPreferredHeightForWidth(MIView* host, int width)
	{
		DCHECK(host_ == host);
		z11::MGSize pref;
		SizeRowsAndColumns(false, width, 0, &pref);
		return pref.height;
	}

	void MIGridLayout::SizeRowsAndColumns(bool layout, int width, int height,
		z11::MGSize* pref)
	{
		// Make sure the master columns have been calculated.
		CalculateMasterColumnsIfNecessary();
		pref->SetSize(0, 0);
		if(rows_.empty())
		{
			return;
		}

		// Calculate the preferred width of each of the columns. Some views'
		// preferred heights are derived from their width, as such we need to
		// calculate the size of the columns first.
		for(std::vector<MIColumnSet*>::iterator i=column_sets_.begin();
			i!=column_sets_.end(); ++i)
		{
			(*i)->CalculateSize();
			pref->set_width(std::max(pref->width, (*i)->LayoutWidth()));
		}
		pref->set_width(pref->width + left_inset_ + right_inset_);

		// Go over the columns again and set them all to the size we settled for.
		width = width ? width : pref->width;
		for(std::vector<MIColumnSet*>::iterator i=column_sets_.begin();
			i!=column_sets_.end(); ++i)
		{
			// We're doing a layout, divy up any extra space.
			(*i)->Resize(width - (*i)->LayoutWidth() - left_inset_ - right_inset_);
			// And reset the x coordinates.
			(*i)->ResetColumnXCoordinates();
		}

		// Reset the height of each row.
		LayoutElement::ResetSizes(&rows_);

		// Do the following:
		// . If the view is aligned along it's baseline, obtain the baseline from the
		//   view and update the rows ascent/descent.
		// . Reset the remaining_height of each view state.
		// . If the width the view will be given is different than it's pref, ask
		//   for the height given a particularly width.
		for(std::vector<ViewState*>::iterator i=view_states_.begin();
			i!=view_states_.end(); ++i)
		{
			ViewState* view_state = *i;
			view_state->remaining_height = view_state->pref_height;

			if(view_state->v_align == BASELINE)
			{
				view_state->baseline = view_state->view->GetBaseline();
			}

			if(view_state->h_align == FILL)
			{
				// The view is resizable. As the pref height may vary with the width,
				// ask for the pref again.
				int actual_width = view_state->column_set->GetColumnWidth(
					view_state->start_col, view_state->col_span);
				if(actual_width!=view_state->pref_width &&
					!view_state->pref_height_fixed)
				{
					// The width this view will get differs from it's preferred. Some Views
					// pref height varies with it's width; ask for the preferred again.
					view_state->pref_height =
						view_state->view->GetHeightForWidth(actual_width);
					view_state->remaining_height = view_state->pref_height;
				}
			}
		}

		// Update the height/ascent/descent of each row from the views.
		std::vector<ViewState*>::iterator view_states_iterator = view_states_.begin();
		for(; view_states_iterator!=view_states_.end() &&
			(*view_states_iterator)->row_span==1; ++view_states_iterator)
		{
			ViewState* view_state = *view_states_iterator;
			Row* row = rows_[view_state->start_row];
			row->AdjustSize(view_state->remaining_height);
			if(view_state->baseline!=-1 &&
				view_state->baseline<=view_state->pref_height)
			{
				row->AdjustSizeForBaseline(view_state->baseline,
					view_state->pref_height - view_state->baseline);
			}
			view_state->remaining_height = 0;
		}

		// Distribute the height of each view with a row span > 1.
		for(; view_states_iterator!=view_states_.end(); ++view_states_iterator)
		{
			ViewState* view_state = *view_states_iterator;

			// Update the remaining_width from columns this view_state touches.
			UpdateRemainingHeightFromRows(view_state);

			// Distribute the remaining height.
			DistributeRemainingHeight(view_state);
		}

		// Update the location of each of the rows.
		LayoutElement::CalculateLocationsFromSize(&rows_);

		// We now know the preferred height, set it here.
		pref->set_height(rows_[rows_.size()-1]->Location() +
			rows_[rows_.size()-1]->Size() + top_inset_ + bottom_inset_);

		if(layout && height!=pref->height)
		{
			// We're doing a layout, and the height differs from the preferred height,
			// divy up the extra space.
			LayoutElement::DistributeDelta(height-pref->height, &rows_);

			// Reset y locations.
			LayoutElement::CalculateLocationsFromSize(&rows_);
		}
	}

	void MIGridLayout::CalculateMasterColumnsIfNecessary()
	{
		if(!calculated_master_columns_)
		{
			calculated_master_columns_ = true;
			for(std::vector<MIColumnSet*>::iterator i=column_sets_.begin();
				i!=column_sets_.end(); ++i)
			{
				(*i)->CalculateMasterColumns();
			}
		}
	}

	void MIGridLayout::AddViewState(ViewState* view_state)
	{
		DCHECK(view_state->view && (view_state->view->parent()==NULL ||
			view_state->view->parent()==host_));
		if(!view_state->view->parent())
		{
			adding_view_ = true;
			host_->addSubview(view_state->view);
			adding_view_ = false;
		}
		remaining_row_span_ = std::max(remaining_row_span_, view_state->row_span);
		next_column_ += view_state->col_span;
		current_row_col_set_->AddViewState(view_state);
		// view_states are ordered by row_span (in ascending order).
		std::vector<ViewState*>::iterator i = lower_bound(view_states_.begin(),
			view_states_.end(), view_state, CompareByRowSpan);
		view_states_.insert(i, view_state);
		SkipPaddingColumns();
	}

	MIColumnSet* MIGridLayout::GetColumnSet(int id)
	{
		for(std::vector<MIColumnSet*>::iterator i=column_sets_.begin();
			i!=column_sets_.end(); ++i)
		{
			if((*i)->id_ == id)
			{
				return *i;
			}
		}
		return NULL;
	}

	void MIGridLayout::AddRow(Row* row)
	{
		current_row_++;
		remaining_row_span_--;
		DCHECK(remaining_row_span_<=0 || row->column_set()==NULL ||
			row->column_set()==GetLastValidColumnSet());
		next_column_ = 0;
		rows_.push_back(row);
		current_row_col_set_ = row->column_set();
		SkipPaddingColumns();
	}

	void MIGridLayout::UpdateRemainingHeightFromRows(ViewState* view_state)
	{
		for(int i=0,start_row=view_state->start_row;
			i<view_state->row_span; ++i)
		{
			view_state->remaining_height -= rows_[i+start_row]->Size();
		}
	}

	void MIGridLayout::DistributeRemainingHeight(ViewState* view_state)
	{
		int height = view_state->remaining_height;
		if(height <= 0)
		{
			return;
		}

		// Determine the number of resizable rows the view touches.
		int resizable_rows = 0;
		int start_row = view_state->start_row;
		int max_row = view_state->start_row + view_state->row_span;
		for(int i=start_row; i<max_row; ++i)
		{
			if(rows_[i]->IsResizable())
			{
				resizable_rows++;
			}
		}

		if(resizable_rows > 0)
		{
			// There are resizable rows, give the remaining height to them.
			int to_distribute = height / resizable_rows;
			for(int i=start_row; i<max_row; ++i)
			{
				if(rows_[i]->IsResizable())
				{
					height -= to_distribute;
					if(height < to_distribute)
					{
						// Give all slop to the last column.
						to_distribute += height;
					}
					rows_[i]->SetSize(rows_[i]->Size() + to_distribute);
				}
			}
		}
		else
		{
			// None of the rows are resizable, divy the remaining height up equally
			// among all rows the view touches.
			int each_row_height = height / view_state->row_span;
			for(int i=start_row; i<max_row; ++i)
			{
				height -= each_row_height;
				if(height < each_row_height)
				{
					each_row_height += height;
				}
				rows_[i]->SetSize(rows_[i]->Size() + each_row_height);
			}
			view_state->remaining_height = 0;
		}
	}

	void MIGridLayout::SkipPaddingColumns()
	{
		if(!current_row_col_set_)
		{
			return;
		}
		while(next_column_<current_row_col_set_->num_columns() &&
			current_row_col_set_->columns_[next_column_]->is_padding_)
		{
			next_column_++;
		}
	}

	MIColumnSet* MIGridLayout::GetLastValidColumnSet()
	{
		for(int i=current_row_-1; i>=0; --i)
		{
			if(rows_[i]->column_set())
			{
				return rows_[i]->column_set();
			}
		}
		return NULL;
	}

} //namespace view