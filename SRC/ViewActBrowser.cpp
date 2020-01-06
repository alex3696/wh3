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
	mTable = nullptr;
	mDvModel = nullptr;
	
	mTable = new wxDataViewCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, wxDV_ROW_LINES
		| wxDV_VERT_RULES
		//| wxDV_HORIZ_RULES
		//| wxDV_MULTIPLE
	);
	
	wxDataViewCtrl* table = mTable;

	mDvModel = new wxDVTableActBrowser();
	table->AssociateModel(mDvModel);
	mDvModel->DecRef();

	#define ICON_HEIGHT 24+2
	int row_height = table->GetCharHeight() + 2;// + 1px in bottom and top 
	if (ICON_HEIGHT > row_height)
		row_height = ICON_HEIGHT;
	table->SetRowHeight(row_height);
	ResetColumns();

	table->Bind(wxEVT_DATAVIEW_COLUMN_HEADER_CLICK
		, [this](wxDataViewEvent& evt) { StoreSelect(); evt.Skip(); });
	table->Bind(wxEVT_DATAVIEW_COLUMN_SORTED
		, [this](wxDataViewEvent& evt) { RestoreSelect(); });
	table->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED
		, &ViewActBrowser::OnCmd_Activate, this);
	table->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED
		, &ViewActBrowser::OnCmd_SelectionChanged, this);
	table->GetTargetWindow()->Bind(wxEVT_MOTION
		, &ViewActBrowser::OnCmd_MouseMove, this);
	table->Bind(wxEVT_COMMAND_MENU_SELECTED
		, [this](wxCommandEvent&) { sigRefresh(); }, wxID_REFRESH);

	mToolTipTimer.Bind(wxEVT_TIMER
		, [this](wxTimerEvent& evt) { ShowToolTip(); });

	table->Bind(wxEVT_DESTROY, [this](wxWindowDestroyEvent& evt)
	{
		if (evt.GetWindow() != mTable)
			return;
		mTable = nullptr;
		mDvModel = nullptr;
	});
	
	wxAcceleratorEntry entries[2];
	char i = 0;
	entries[i++].Set(wxACCEL_CTRL, (int) 'R', wxID_REFRESH);
	entries[i++].Set(wxACCEL_NORMAL, WXK_F5, wxID_REFRESH);
	wxAcceleratorTable accel(i + 1, entries);
	table->SetAcceleratorTable(accel);

}
//-----------------------------------------------------------------------------
ViewActBrowser::~ViewActBrowser()
{
	//mToolTipTimer.Stop();
}
//-----------------------------------------------------------------------------
void ViewActBrowser::StoreSelect()
{
}
//-----------------------------------------------------------------------------
void ViewActBrowser::RestoreSelect()
{
}
//-----------------------------------------------------------------------------
void ViewActBrowser::AutosizeColumns()
{
	if (!mColAutosize)
		return;
	TEST_FUNC_TIME;
	wxBusyCursor busyCursor;
	for (size_t i = 0; i < mTable->GetColumnCount(); i++)
	{
		auto col_pos = mTable->GetModelColumnIndex(i);
		auto col = mTable->GetColumn(col_pos);
		if (col)
		{
			auto bs = mTable->GetBestColumnWidth(i);
			if (bs > 300)
				bs = 300;
			col->SetWidth(bs);
		}
	}
}
//-----------------------------------------------------------------------------
void ViewActBrowser::ResetColumns()
{
	wxWindowUpdateLocker lock(mTable);

	mTable->ClearColumns();
	//auto renderer1 = new wxDataViewIconTextRenderer();

	auto col0 = mTable->AppendTextColumn("Имя", 0, wxDATAVIEW_CELL_INERT, -1
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	auto col1 = mTable->AppendTextColumn("Описание", 1, wxDATAVIEW_CELL_INERT, 150
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	/*
	auto col2 = mTable->AppendTextColumn("Цвет", 2, wxDATAVIEW_CELL_INERT, -1
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	auto col3 = mTable->AppendTextColumn("#", 3, wxDATAVIEW_CELL_INERT, -1
		, wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
	*/
}
//-----------------------------------------------------------------------------
void ViewActBrowser::RebuildColumns()
{
}
//-----------------------------------------------------------------------------
wxDataViewColumn * ViewActBrowser::AppendTableColumn(const wxString & title, int model_id)
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
int ViewActBrowser::GetTitleWidth(const wxString& title)const
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
bool ViewActBrowser::IsSelectedItem(const wxDataViewItem& item)const
{
	if (!item.IsOk())
		return false;
	const IIdent64* ident = static_cast<const IIdent64*> (item.GetID());
	if (!ident)
		return false;
	const auto& act = dynamic_cast<const IAct64*>(ident);
	if (act)
	{
		return act->IsSelected();
	}
	return false;
}
//-----------------------------------------------------------------------------
void ViewActBrowser::SetSelected(const wxDataViewItem & item, bool select) const
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
void ViewActBrowser::SetSelected() const
{
	bool select = !IsSelectedItem(mTable->GetCurrentItem());
	SetSelected(mTable->GetCurrentItem(), select);
}
//-----------------------------------------------------------------------------
void ViewActBrowser::OnCmd_Activate(wxDataViewEvent & evt)
{
	sigActivate();
}
//-----------------------------------------------------------------------------
void ViewActBrowser::OnCmd_SelectionChanged(wxDataViewEvent & evt)
{
}
//-----------------------------------------------------------------------------
void ViewActBrowser::SetBeforeRefresh(std::shared_ptr<const ModelActTable> table)
{

}
//-----------------------------------------------------------------------------
void ViewActBrowser::SetAfterRefresh(std::shared_ptr<const ModelActTable> table)
{
	if (!mDvModel)
		return;
	TEST_FUNC_TIME;
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mTable);

	mDvModel->SetData(table);
	AutosizeColumns();
	RestoreSelect();
}
//-----------------------------------------------------------------------------
void ViewActBrowser::GetSelection(std::set<int64_t>& sel)const
{
	auto item = mTable->GetCurrentItem();
	if (!item.IsOk())
		return;
	const IIdent64* ident = static_cast<const IIdent64*> (item.GetID());
	if (!ident)
		return;
	const auto& act = dynamic_cast<const IAct64*>(ident);
	if (!act)
		return;
	sel.emplace(act->GetId());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ViewActBrowser::ShowToolTip()
{
	if (!mTable || !mTable->GetMainWindow()->IsMouseInWindow())
		return;

	wxPoint pos = wxGetMousePosition();
	pos = mTable->ScreenToClient(pos);

	wxDataViewItem item(nullptr);
	wxDataViewColumn* col = nullptr;

	mTable->HitTest(pos, item, col);
	if (!col || !item.IsOk())
		return;

	wxString val;
	wxVariant var;
	mDvModel->GetValue(var, item, col->GetModelColumn());
	val = var.GetString();

	const auto* ident = static_cast<const IAct64*> (item.GetID());
	if (!ident)
		return;

	wxString item_str = wxString::Format("#[%s]\t%s"
		, ident->GetIdAsString(), ident->GetColour());

	mTable->GetTargetWindow()->SetToolTip(val + "\n\n" + item_str);
}
//-----------------------------------------------------------------------------
void ViewActBrowser::OnCmd_MouseMove(wxMouseEvent& evt)
{
	mTable->GetTargetWindow()->SetToolTip(wxEmptyString);
	mToolTipTimer.StartOnce(15000);
}

