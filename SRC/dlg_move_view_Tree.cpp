#include "_pch.h"
#include "dlg_move_view_Tree.h"




using namespace wh;
using namespace wh::dlg_move::view;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Tree::Tree(wxWindow*		parent,
	wxWindowID		id,
	const wxPoint&	pos,
	const wxSize &	size,
	long			style)
	: wxDataViewCtrl(parent, id, pos, size, style
	/*| wxDV_ROW_LINES | wxDV_VERT_RULES */ | wxDV_HORIZ_RULES)
{
	this->SetRowHeight(24);

	mDvModel = new DvModel();
	wxDataViewCtrl::AssociateModel(mDvModel);
	mDvModel->DecRef();


	Bind(wxEVT_SIZE, &Tree::OnResize, this);


	AppendIconTextColumn("Имя", 1, wxDATAVIEW_CELL_INERT, 200,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn("Местоположение", 2, wxDATAVIEW_CELL_INERT, -1,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn("ID", 3, wxDATAVIEW_CELL_INERT, 50,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);// ->SetHidden(true);
	//AppendTextColumn("LastLogId", 4, wxDATAVIEW_CELL_INERT, 100,
	//	wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);


	EnableAutosizeColumn(1);
}

//---------------------------------------------------------------------------
void Tree::SetModel(std::shared_ptr<IModel> model)
{
	if (model && mMovable != model)
	{
		namespace sph = std::placeholders;

		mMovable = std::dynamic_pointer_cast<model::MovableObj>(model);
		mDvModel->SetModel(model);

		//auto typeQty = mMovable->GetDstTypes()->GetChildQty();
		//std::vector<unsigned int> addedItems(typeQty);
		//for (unsigned int i = 0; i < typeQty; i++)
		//	addedItems[i] = i;

		//mDvModel->OnAppend(*model, addedItems);

	}
}


//---------------------------------------------------------------------------
void Tree::GetSelected(std::vector<unsigned int>& selected)
{
	wxDataViewItemArray viewSelArr;
	this->GetSelections(viewSelArr);
	for (const wxDataViewItem& viewItem : viewSelArr)
	{
		if (viewItem.IsOk())
		{
			unsigned int pos = this->GetRowByItem(viewItem);
			selected.emplace_back(pos);
		}
	}
}


//---------------------------------------------------------------------------
void Tree::ExpandAll()
{
	wxDataViewItemArray itemArray;
	mDvModel->GetChildren(wxDataViewItem(NULL), itemArray);

	//std::for_each(itemArray.begin(), itemArray.end(),
	//	std::bind(&Tree::Expand, this, std::placeholders::_1) );
	for (const auto& item : itemArray)
		Expand(item);

}

//---------------------------------------------------------------------------
void Tree::OnResize(wxSizeEvent& evt)
{
	if (!mAutosizeColumn.empty())
	{
		wxWindowUpdateLocker	wndDisabler(this);
		int sum = 0, asum = 0;
		for (unsigned int i = 0; i<GetColumnCount(); i++)
			if (mAutosizeColumn.end() == mAutosizeColumn.find(i))
				sum += GetColumn(i)->IsHidden() ? 0 : GetColumn(i)->GetWidth();
			else
				asum = wxDVC_DEFAULT_WIDTH;

		int ctrl_width = this->GetClientSize().GetWidth();
		int auto_width = (ctrl_width >(sum + asum)) ? ((ctrl_width - sum) / mAutosizeColumn.size()) : wxDVC_DEFAULT_WIDTH;

		for (auto it = mAutosizeColumn.begin(); it != mAutosizeColumn.end(); ++it) {
			if (*it < GetColumnCount())
				GetColumn(*it)->SetWidth(auto_width);
		}
	}//if ( mAutosizeColumn.size() )
	evt.Skip();
}//virtual void OnResize

//---------------------------------------------------------------------------
void Tree::EnableAutosizeColumn(unsigned int columnNo)
{
	mAutosizeColumn.insert(columnNo);
}
//---------------------------------------------------------------------------
void Tree::DisableAutosizeColumn(unsigned int columnNo)
{
	mAutosizeColumn.erase(columnNo);
}
//---------------------------------------------------------------------------
void Tree::SetMultiselect(bool enabled)
{
	long currentFlags = this->GetWindowStyleFlag();
	currentFlags = enabled ? currentFlags | wxDV_MULTIPLE : currentFlags & ~wxDV_MULTIPLE;
	this->SetWindowStyleFlag(currentFlags);
}
//---------------------------------------------------------------------------
std::shared_ptr<wh::dlg_move::model::DstObj> Tree::GetSelected()const
{
	wxDataViewItem selectedItem = this->GetSelection();

	if (selectedItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		if (modelInterface)
		{
			auto objItem = dynamic_cast<model::DstObj*> (modelInterface);
			if (objItem)
				return objItem->shared_from_this();
		}
	}
	return std::shared_ptr<wh::dlg_move::model::DstObj>();
}