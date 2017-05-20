#include "_pch.h"
#include "ViewDetail.h"
#include "ViewHistory.h"
#include "ViewFilterList.h"
#include "ViewObjPropList.h"


using namespace wh;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewToolbarDetail::ViewToolbarDetail(wxWindow* parent)
{
	auto mgr = ResMgr::GetInstance();

	long style = wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND
		| wxAUI_TB_TEXT 
		//| wxAUI_TB_HORZ_TEXT
		//| wxAUI_TB_OVERFLOW
		;
	auto tool_bar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);


	tool_bar->AddTool(wxID_FORWARD, "Переместить", mgr->m_ico_move24, "Переместить(F6)");
	tool_bar->AddTool(wxID_EXECUTE, "Выполнить", mgr->m_ico_act24, "Выполнить(F7)");
	tool_bar->AddTool(wxID_UNDO, "Откатить", wxArtProvider::GetBitmap(wxART_UNDO, wxART_TOOLBAR),
		"Отменить последнее действие/перемещение(CTRL+Z)");

	tool_bar->Realize();

	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarDetail::OnCmd_Move, this, wxID_REPLACE);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarDetail::OnCmd_Act, this, wxID_EXECUTE);
	tool_bar->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarDetail::OnCmd_Redo, this, wxID_UNDO);

	wxAcceleratorEntry entries[3];
	entries[0].Set(wxACCEL_NORMAL, WXK_F6, wxID_REPLACE);
	entries[1].Set(wxACCEL_NORMAL, WXK_F7, wxID_EXECUTE);
	entries[2].Set(wxACCEL_CTRL, (int) 'Z', wxID_UNDO);
	wxAcceleratorTable accel(3, entries);
	tool_bar->SetAcceleratorTable(accel);


	mToolbar = tool_bar;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewToolbarDetail::SetObj(const rec::ObjInfo& rt) 
{
}
//-----------------------------------------------------------------------------

void ViewToolbarDetail::OnCmd_Move(wxCommandEvent& evt)
{
	sigMove();
}
//-----------------------------------------------------------------------------

void ViewToolbarDetail::OnCmd_Act(wxCommandEvent& evt)
{
	sigAct();
}//-----------------------------------------------------------------------------

void ViewToolbarDetail::OnCmd_Redo(wxCommandEvent& evt)
{
	sigRedo();
}











//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class wxAuiPanel : public wxPanel
{
public:
	wxAuiPanel(wxWindow* wnd)
		:wxPanel(wnd)
	{
		mAuiMgr.SetManagedWindow(this);
	}
	~wxAuiPanel()
	{
		mAuiMgr.UnInit();
	}
	wxAuiManager	mAuiMgr;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewPageDetail::ViewPageDetail(std::shared_ptr<IViewWindow> parent)
{
	auto panel = new wxAuiPanel(parent->GetWnd());

	mAuiMgr = &panel->mAuiMgr;
	mPanel = panel;

	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	//auto face_colour = *wxRED;
	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);

	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);


	mViewToolbarHistory = std::make_shared<ViewToolbarHistory>(panel);
	panel->mAuiMgr.AddPane(mViewToolbarHistory->GetWnd(), wxAuiPaneInfo().
		Name(wxT("HistoryToolBar"))
		.CaptionVisible(false)
		.ToolbarPane().Top().Row(1).Position(1)
		//.Fixed()
		//.Dockable(false)
		.PaneBorder(false)
		//.Gripper(false)
		);

	mViewToolbarDetail = std::make_shared<ViewToolbarDetail>(panel);
	panel->mAuiMgr.AddPane(mViewToolbarDetail->GetWnd(), wxAuiPaneInfo().
		Name(wxT("ActToolBar"))
		.CaptionVisible(false)
		.ToolbarPane().Top().Row(1).Position(0)
		//.Fixed()
		//.Dockable(false)
		.PaneBorder(false)
		//.Gripper(false)
		);

	mViewTableHistory = std::make_shared<ViewTableHistory>(panel);
	panel->mAuiMgr.AddPane(mViewTableHistory->GetWnd(), wxAuiPaneInfo().
		Name(wxT("HistoryTable")).CenterPane()
		.PaneBorder(false)
		);

	mViewObjPropList = std::make_shared<ViewObjPropList>(panel);
	panel->mAuiMgr.AddPane(mViewObjPropList->GetWnd(), wxAuiPaneInfo().
		Name("ViewObjPropListPane").Caption("Свойства")
		.Left()
		.PaneBorder(false)
		.Hide()
		.CloseButton(false)
		.MinSize(300, 200)
		);

	mViewFilterList = std::make_shared<ViewFilterList>(panel);
	panel->mAuiMgr.AddPane(mViewFilterList->GetWnd(), wxAuiPaneInfo().
		Name("ViewFilterListPane").Caption("Фильтры")
		.Left()//Right()
		.PaneBorder(false)
		.Hide()
		.CloseButton(false)
		.MinSize(250, 200)
		);

	auto toolbar_wnd = std::dynamic_pointer_cast<ViewToolbarHistory>(mViewToolbarHistory).get();
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Update, toolbar_wnd, wxID_REFRESH);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Backward, toolbar_wnd, wxID_BACKWARD);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Forward, toolbar_wnd, wxID_FORWARD);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Filter, toolbar_wnd, wxID_FIND);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_PropList, toolbar_wnd, wxID_PREFERENCES);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_Convert, toolbar_wnd, wxID_CONVERT);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarHistory::OnCmd_ShowSetup, toolbar_wnd, wxID_SETUP);

	auto toolbar_act = std::dynamic_pointer_cast<ViewToolbarDetail>(mViewToolbarDetail).get();
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarDetail::OnCmd_Move, toolbar_act, wxID_REPLACE);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarDetail::OnCmd_Act, toolbar_act, wxID_EXECUTE);
	panel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewToolbarDetail::OnCmd_Redo, toolbar_act, wxID_UNDO);

	wxAcceleratorEntry entries[10];
	entries[0].Set(wxACCEL_NORMAL, WXK_F5, wxID_REFRESH);
	entries[1].Set(wxACCEL_CTRL, WXK_PAGEUP, wxID_BACKWARD);
	entries[2].Set(wxACCEL_CTRL, WXK_PAGEDOWN, wxID_FORWARD);
	entries[3].Set(wxACCEL_CTRL, (int) 'F', wxID_FIND);
	entries[4].Set(wxACCEL_CTRL, (int) 'P', wxID_PREFERENCES);
	entries[5].Set(wxACCEL_CTRL, (int) 'E', wxID_CONVERT);
	entries[6].Set(wxACCEL_CTRL, (int) 'N', wxID_SETUP);
	entries[7].Set(wxACCEL_NORMAL, WXK_F6, wxID_REPLACE);
	entries[8].Set(wxACCEL_NORMAL, WXK_F7, wxID_EXECUTE);
	entries[9].Set(wxACCEL_CTRL, (int) 'Z', wxID_UNDO);

	wxAcceleratorTable accel(10, entries);
	//mViewTableHistory->GetWnd()->SetAcceleratorTable(accel);
	//mViewToolbarHistory->GetWnd()->SetAcceleratorTable(accel);
	panel->SetAcceleratorTable(accel);

	mViewTableHistory->GetWnd()->SetFocus();


	panel->mAuiMgr.Update();

}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewToolbarDetail>	ViewPageDetail::GetViewToolbarDetail()const //override;
{
	return mViewToolbarDetail;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewToolbarHistory> ViewPageDetail::GetViewToolbarHistory()const
{
	return mViewToolbarHistory;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewTableHistory> ViewPageDetail::GetViewTableHistory()const//override 
{
	return mViewTableHistory;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewFilterList> ViewPageDetail::GetViewFilterList()const//override 
{
	return mViewFilterList;
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<IViewObjPropList> ViewPageDetail::GetViewObjPropList()const //override;
{
	return mViewObjPropList;
}
//-----------------------------------------------------------------------------
void ViewPageDetail::ShowFilterList(bool show)
{
	auto& pane = mAuiMgr->GetPane("ViewFilterListPane");
	if (pane.IsOk())
	{
		bool pane_shown = pane.IsShown();
		if (pane_shown != show)
		{
			wxWindowUpdateLocker lock(mPanel);
			pane.Show(show);
			mAuiMgr->Update();
		}
	}
}
//-----------------------------------------------------------------------------
void ViewPageDetail::ShowObjPropList(bool show)
{
	auto& pane = mAuiMgr->GetPane("ViewObjPropListPane");
	if (pane.IsOk())
	{
		bool pane_shown = pane.IsShown();
		if (pane_shown != show)
		{
			wxWindowUpdateLocker lock(mPanel);
			pane.Show(show);
			mAuiMgr->Update();
		}
	}//if (pane.IsOk())
}
//-----------------------------------------------------------------------------
//virtual 
void ViewPageDetail::SetCfg(const rec::PageHistory& cfg) //override;
{
	mCfg = cfg;
	ShowObjPropList(cfg.mShowPropertyList);
	ShowFilterList(cfg.mShowFilterList);

}