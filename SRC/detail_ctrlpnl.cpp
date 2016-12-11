#include "_pch.h"
#include "detail_ctrlpnl.h"

#include "MainFrame.h"
#include "dlg_move_view_Frame.h"
#include "dlg_act_view_Frame.h"


using namespace wh;
using namespace wh::detail::view;

//-----------------------------------------------------------------------------
CtrlPnl::CtrlPnl(wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxPanel(parent, id, pos, size, style, name)
{
	mAuiMgr.SetManagedWindow(this);

	// Создаём тулбар действий
	mActToolBar = new DetailActToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT);;
	mAuiMgr.AddPane(mActToolBar, wxAuiPaneInfo().
		Name("ActToolBarPane")
		.ToolbarPane().Top().Floatable(false)
		.PaneBorder(false)
		);

	// Создаём тулбар истории
	mLogToolBar = new VTableToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT);
	mAuiMgr.AddPane(mLogToolBar, wxAuiPaneInfo().
		Name("ToolBarPane")
		.ToolbarPane().Top().Floatable(false)
		.PaneBorder(false)
		);

	// Создаём панель свойств
	mObjView = new ObjDetailPGView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	mAuiMgr.AddPane(mObjView, wxAuiPaneInfo().
		Name("ClsPropGrid").Caption("Свойства")
		.CaptionVisible(true)
		//.ToolbarPane()
		.Left()
		.CloseButton(false)
		.MinSize(300, 400)
		//.Fixed()
		.Dockable(true)
		.PaneBorder(false)
		.Layer(2)
		//.Position(1)
		);

	// Создаём панель фильтров истории
	mLogTableFilter = new FilterArrayEditor(this);
	mAuiMgr.AddPane(mLogTableFilter, wxAuiPaneInfo().
		Name("FilterPane").Caption("Фильтр")
		.Left().Layer(1)//.Position(2)
		.MinSize(250, 200)
		.CloseButton(false).Dockable(false).Floatable(false)
		.Hide()
		.PaneBorder(false)
		);

	// Создаём панель таблицы истории
	mLogTable = new wh::VTable(this);
	//mLogTable->SetRowHeight(32);
	mAuiMgr.AddPane(mLogTable, wxAuiPaneInfo().
		Name("LogPane").Caption("LogPane").
		CenterPane().Layer(1).Position(1)
		.CloseButton(true).MaximizeButton(true).PaneBorder(false));
	
	//mCtrl.SetModel(mLogModel);
	mCtrl.SetAuiMgr(&mAuiMgr);
	mCtrl.SetViewTable(mLogTable);
	mCtrl.SetViewToolBar(mLogToolBar);
	mCtrl.SetViewFilter(mLogTableFilter);

	mCtrl.SetObjModel(mObj);
	mCtrl.SetObjView(mObjView);
	mCtrl.SetActToolbar(mActToolBar);

	mAuiMgr.Update();
}
//-----------------------------------------------------------------------------
CtrlPnl::~CtrlPnl()
{
	mAuiMgr.UnInit();
}
//-----------------------------------------------------------------------------
void CtrlPnl::SetModel(const SptrIModel& model)
{
	auto mdl = std::dynamic_pointer_cast<model::Obj>(model);
	mObj = mdl;
	if (!mObj)
		return;
	mCtrl.SetObjModel(mObj);
	mCtrl.SetObjView(mObjView);
	mCtrl.SetActToolbar(mActToolBar);
	
	mCtrl.SetModel(mObj->GetObjHistory());
	mCtrl.SetViewTable(mLogTable);
	mCtrl.SetViewToolBar(mLogToolBar);
	mCtrl.SetViewFilter(mLogTableFilter);

}
