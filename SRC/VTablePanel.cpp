#include "_pch.h"
#include "VTablePanel.h"

using namespace wh;

//-----------------------------------------------------------------------------
VTablePanel::VTablePanel(wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	:wxPanel(parent, id, pos, size, style, name)
{
	// Create Aui
	mAuiMgr.SetManagedWindow(this);

	// Create AuiToolbar
	mToolBar = new VTableToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, 0 | wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT);

	// Create Filter Panel
	mFilterEditor = new FilterArrayEditor(this);
	mAuiMgr.AddPane(mFilterEditor, wxAuiPaneInfo().
		Name("FilterPane").Caption("Ôèëüòð")
		.Left().Layer(1).Position(1)
		.MinSize(250, 200)
		.CloseButton(false).Dockable(false).Floatable(false)
		.Hide()
		.PaneBorder(false)
		);

	// Create Table Panel
	mTableView = new VTable(this);
	mAuiMgr.AddPane(mTableView, wxAuiPaneInfo().
		Name("TablePane")
		.CenterPane().Layer(1).Position(1)
		.CloseButton(true).MaximizeButton(true)
		.PaneBorder(false));

	mAuiMgr.Update();

	mCtrl.fnOnCmdFilter = [this](wxCommandEvent& evt)
	{
		wxAuiPaneInfo& pi = mAuiMgr.GetPane("FilterPane");
		if (pi.IsOk())
		{
			bool visible = !pi.IsShown();
			pi.Show(visible);

			wxAuiToolBarItem* tool = mToolBar->FindTool(wxID_FIND);
			if (tool)
				tool->SetState(visible ? wxAUI_BUTTON_STATE_CHECKED : wxAUI_BUTTON_STATE_NORMAL);

			mAuiMgr.Update();
		}//if(!pi.IsOk())	
	};

}
//-----------------------------------------------------------------------------
VTablePanel::~VTablePanel()
{
	mAuiMgr.UnInit();
}
//-----------------------------------------------------------------------------
void VTablePanel::SetModel(std::shared_ptr<ITable> model)
{
	wxWindowUpdateLocker	wndLockUpdater(this);

	mMTable = model;
	mTableView->SetModel(model);
	mFilterEditor->SetModel(model);

	mAuiMgr.DetachPane(mToolBar);
	mToolBar->SetModel(model);
	mAuiMgr.AddPane(mToolBar, wxAuiPaneInfo().
		Name("ToolBarPane")
		.ToolbarPane().Top().Floatable(false)
		.PaneBorder(false)
		);
	mAuiMgr.Update();

	mCtrl.SetTableViewModel(mMTable, mTableView);
}

