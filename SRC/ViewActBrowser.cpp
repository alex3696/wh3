#include "_pch.h"
#include "globaldata.h"
#include "ViewActBrowser.h"
//#include "wxDataViewIconMLTextRenderer.h"
//#include "wxComboBtn.h"
//#include "config.h"
//#include <wx/uiaction.h>

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewActBrowser::ViewActBrowser(const std::shared_ptr<IViewWindow>& parent)
	:ViewActBrowser(parent->GetWnd())
{}
//-----------------------------------------------------------------------------
ViewActBrowser::ViewActBrowser(wxWindow* parent)
{
	auto table = new wxDataViewCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, wxDV_ROW_LINES
		| wxDV_VERT_RULES
		//| wxDV_HORIZ_RULES
		//| wxDV_MULTIPLE
	);
	mTable = table;

	//mDvModel = new wxDataViewModel();
	//table->AssociateModel(mDvModel);
	//mDvModel->DecRef();

	#define ICON_HEIGHT 24+2
	int row_height = table->GetCharHeight() + 2;// + 1px in bottom and top 
	if (ICON_HEIGHT > row_height)
		row_height = ICON_HEIGHT;
	table->SetRowHeight(row_height);
	ResetColumns();


	table->Bind(wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, [this](wxDataViewEvent& evt)
	{ StoreSelect(); evt.Skip(); });
	table->Bind(wxEVT_DATAVIEW_COLUMN_SORTED, [this](wxDataViewEvent& evt)
	{ RestoreSelect(); });

	table->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED
		, &ViewActBrowser::OnCmd_Activate, this);

	table->Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent&) 
		{ sigRefresh(); }, wxID_REFRESH);


	table->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED
		, &ViewActBrowser::OnCmd_SelectionChanged, this);


	wxAcceleratorEntry entries[2];
	char i = 0;
	entries[i++].Set(wxACCEL_CTRL, (int) 'R', wxID_REFRESH);
	entries[i++].Set(wxACCEL_NORMAL, WXK_F5, wxID_REFRESH);

	wxAcceleratorTable accel(i + 1, entries);
	table->SetAcceleratorTable(accel);

}
//-----------------------------------------------------------------------------

void wh::ViewActBrowser::StoreSelect()
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::RestoreSelect()
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::AutosizeColumns()
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::ResetColumns()
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::RebuildColumns()
{
}
//-----------------------------------------------------------------------------
wxDataViewColumn * wh::ViewActBrowser::AppendTableColumn(const wxString & title, int model_id)
{
	auto col = mTable->AppendTextColumn(title, model_id
		, wxDATAVIEW_CELL_INERT
		, GetTitleWidth(title)
		, wxALIGN_NOT
		, wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	col->SetMinWidth(80);
	return col;
}
//-----------------------------------------------------------------------------
int wh::ViewActBrowser::GetTitleWidth(const wxString& title)const
{
	const int spw = mTable->GetTextExtent(" ").GetWidth();
	int hw = mTable->GetTextExtent(title).GetWidth() + spw * 4 + 24;
	if (hw < 80)
		hw = -1; // default width
	else if (hw > 300)
		hw = 300;
	return hw;
}
//-----------------------------------------------------------------------------
bool wh::ViewActBrowser::IsSelectedItem(const wxDataViewItem& item)const
{
	if (!item.IsOk())
		return false;
	const IIdent64* ident = static_cast<const IIdent64*> (item.GetID());
	if (!ident)
		return false;
	const auto& act = dynamic_cast<const IAct64*>(ident);
	if (act)
	{
		//return act->IsSelected();
	}
	return false;
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::SetSelected(const wxDataViewItem & item, bool select) const
{
	if (!item.IsOk())
		return;
	const IIdent64* ident = static_cast<const IIdent64*> (item.GetID());
	if (!ident)
		return;
	const auto& act = dynamic_cast<const IAct64*>(ident);
	if (act)
	{
		int64_t aid = act->GetId();
		sigSelect(aid, select);
	}
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::SetSelected() const
{
	bool select = !IsSelectedItem(mTable->GetCurrentItem());
	SetSelected(mTable->GetCurrentItem(), select);
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::OnCmd_Activate(wxDataViewEvent & evt)
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::OnCmd_SelectionChanged(wxDataViewEvent & evt)
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::SetBeforeRefresh()
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::SetAfterRefresh()
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::SetOperation(Operation, const std::vector<const IIdent64*>&)
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::SetInsert()const
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::SetDelete()const
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::SetUpdate()const
{
}
//-----------------------------------------------------------------------------
void wh::ViewActBrowser::SetSelectCurrent()const
{
	auto item = mTable->GetCurrentItem();
	SetSelected(item, true);
}
//-----------------------------------------------------------------------------