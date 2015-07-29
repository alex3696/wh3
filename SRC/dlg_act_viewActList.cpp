#include "_pch.h"
#include "dlg_act_view_ActList.h"




using namespace wh;
using namespace wh::dlg_act::view;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ActList::ActList(wxWindow*		parent,
	wxWindowID		id,
	const wxPoint&	pos,
	const wxSize &	size,
	long			style)
	: wxDataViewCtrl(parent, id, pos, size, style
	/*| wxDV_ROW_LINES | wxDV_VERT_RULES */ | wxDV_HORIZ_RULES)
{
	this->SetRowHeight(24);

	mDvModel = new ActListDvModel();
	wxDataViewCtrl::AssociateModel(mDvModel);
	mDvModel->DecRef();


	Bind(wxEVT_SIZE, &ActList::OnResize, this);

	//AppendIconTextColumn
	AppendTextColumn("Действие", 1, wxDATAVIEW_CELL_INERT, 200,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn("Описание", 2, wxDATAVIEW_CELL_INERT, -1,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn("ID", 3, wxDATAVIEW_CELL_INERT, 50,
		wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);// ->SetHidden(true);

	EnableAutosizeColumn(1);
}

//---------------------------------------------------------------------------
void ActList::SetModel(std::shared_ptr<dlg_act::model::ActArray> model)
{
	if (model && mActArray != model)
	{
		namespace sph = std::placeholders;

		mActArray = model;
		mDvModel->SetModel(model);
	}
}


//---------------------------------------------------------------------------
void ActList::GetSelected(std::vector<unsigned int>& selected)
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
void ActList::OnResize(wxSizeEvent& evt)
{
	if (!mAutosizeColumn.empty())
	{
		wxWindowDisabler	wndDisabler(this);

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
void ActList::EnableAutosizeColumn(unsigned int columnNo)
{
	mAutosizeColumn.insert(columnNo);
}
//---------------------------------------------------------------------------
void ActList::DisableAutosizeColumn(unsigned int columnNo)
{
	mAutosizeColumn.erase(columnNo);
}
//---------------------------------------------------------------------------
void ActList::SetMultiselect(bool enabled)
{
	long currentFlags = this->GetWindowStyleFlag();
	currentFlags = enabled ? currentFlags | wxDV_MULTIPLE : currentFlags & ~wxDV_MULTIPLE;
	this->SetWindowStyleFlag(currentFlags);
}
//---------------------------------------------------------------------------
std::shared_ptr<dlg_act::model::Act> ActList::GetSelected()const
{
	wxDataViewItem selectedItem = this->GetSelection();

	if (selectedItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		if (modelInterface)
		{
			auto actItem = dynamic_cast<model::Act*> (modelInterface);
			if (actItem)
				return actItem->shared_from_this();
		}
	}
	return std::shared_ptr<dlg_act::model::Act>();
}