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
	mAuiMgr.AddPane(mToolBar, wxAuiPaneInfo().
		Name("ToolBarPane")
		.ToolbarPane().Top().Floatable(false)
		.PaneBorder(false)
		//.BestSize(wxSize(600,50))
		);

	// Create Filter Panel
	mFilterEditor = new FilterArrayEditor(this);
	mAuiMgr.AddPane(mFilterEditor, wxAuiPaneInfo().
		Name("FilterPane").Caption("Фильтр")
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

	mCtrl.SetModel(model);
	mCtrl.SetAuiMgr(&mAuiMgr);
	mCtrl.SetViewTable(mTableView);
	mCtrl.SetViewToolBar(mToolBar);
	mCtrl.SetViewFilter(mFilterEditor);


	mTableView->SetFocus();
	mAuiMgr.Update();
}

